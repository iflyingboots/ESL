/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <gbl.h>
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <tsk.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <failure.h>
#include <dsplink.h>
#include <platform.h>
#include <notify.h>
#include <bcache.h>
/*  ----------------------------------- Sample Headers              */
#include <pool_notify_config.h>
#include <math.h>
#include <task.h>

#define SHIFT 10

extern Uint16 MPCSXFER_BufferSize ;

Int32 *buf;
Int32 length;

#define NEXT(n, i)  (((n) + (i)/(n)) >> 1)

int is_initial_loop;

unsigned long isqrt(unsigned long x)
{
    register unsigned long op, res, one;

    op = x;
    res = 0;

    /* "one" starts at the highest power of four <= than the argument. */
    one = 1 << 30;  /* second-to-top bit set */
    while (one > op) one >>= 2;

    while (one != 0) {
        if (op >= res + one) {
            op -= res + one;
            res += one << 1;  // <-- faster than 2 * one
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}

static int _minEigenvalue(Int32 gxx, Int32 gxy, Int32 gyy)
{
	return (Int32) ((gxx + gyy - isqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy)) / 2);
}

void _ComputeTrackability()
{
	register Int32 gx, gy;
	register Int32 gxx, gxy, gyy;
	register Int32 xx, yy;

	Int32 x, y;
	// Int32 gradx_start = 5;
	Int32 gradx_start = 9;
	Int32 result_address;
	Int32 grady_start;

	//Int32 nrows = buf[0];
	Int32 ncols = buf[1];
	Int32 rows_dsp = buf[2];
	Int32 window_hh = buf[3];
	Int32 window_hw = buf[4];
	Int32 borderx = buf[5];
	//Int32 bordery = buf[6];

	grady_start = buf[7];
	result_address = buf[8];

	/* For most of the pixels in the image, do ... */
	for (y = window_hh; y < window_hh + rows_dsp ; y ++)
		for (x = borderx  ; x < ncols - borderx ; x ++)  {
			/* Sum the gradients in the surrounding window */
			gxx = 0;  gxy = 0;  gyy = 0;
			for (yy = y - window_hh ; yy <= y + window_hh ; yy++)
				for (xx = x - window_hw ; xx <= x + window_hw ; xx++)  {
					gx = *(buf + gradx_start + ncols * yy + xx);
					gy = *(buf + grady_start + ncols * yy + xx);

					gxx += (gx * gx) >> SHIFT;
					gxy += (gx * gy) >> SHIFT;
					gyy += (gy * gy) >> SHIFT;
				}
			gxx = gxx / (1 << SHIFT);
			gxy = gxy / (1 << SHIFT);
			gyy = gyy / (1 << SHIFT);

			/* Store the trackability of the pixel as the minimum
			of the two eigenvalues */
			buf[result_address++] = _minEigenvalue(gxx, gxy, gyy);
		}
}

static Void Task_notify (Uint32 eventNo, Ptr arg, Ptr info) ;

Int Task_create (Task_TransferInfo **infoPtr)
{
	Int status    = SYS_OK ;
	Task_TransferInfo *info = NULL ;

	is_initial_loop = 1;

	/* Allocate Task_TransferInfo structure that will be initialized
	 * and passed to other phases of the application */
	if (status == SYS_OK) {
		*infoPtr = MEM_calloc (DSPLINK_SEGID,
		                       sizeof (Task_TransferInfo),
		                       0) ; /* No alignment restriction */
		if (*infoPtr == NULL) {
			status = SYS_EALLOC ;
		} else {
			info = *infoPtr ;
		}
	}

	/* Fill up the transfer info structure */
	if (status == SYS_OK) {
		info->dataBuf       = NULL ; /* Set through notification callback. */
		info->bufferSize    = MPCSXFER_BufferSize ;
		SEM_new (&(info->notifySemObj), 0) ;
	}

	/*
	 *  Register notification for the event callback to get control and data
	 *  buffer pointers from the GPP-side.
	 */
	if (status == SYS_OK) {
		status = NOTIFY_register (ID_GPP,
		                          MPCSXFER_IPS_ID,
		                          MPCSXFER_IPS_EVENTNO,
		                          (FnNotifyCbck) Task_notify,
		                          info) ;
		if (status != SYS_OK) {
			return status;
		}
	}

	/*
	 *  Send notification to the GPP-side that the application has completed its
	 *  setup and is ready for further execution.
	 */
	if (status == SYS_OK) {
		status = NOTIFY_notify (ID_GPP,
		                        MPCSXFER_IPS_ID,
		                        MPCSXFER_IPS_EVENTNO,
		                        (Uint32) 0) ; /* No payload to be sent. */
		if (status != SYS_OK) {
			return status;
		}
	}

	/*
	 *  Wait for the event callback from the GPP-side to post the semaphore
	 *  indicating receipt of the data buffer pointer and image width and height.
	 */

	// Should wait for notifications:
	// buf address, length
	if (is_initial_loop) {
		SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
		is_initial_loop = 0;
	}

	return status ;
}


Int Task_execute (Task_TransferInfo *info)
{
	int i;
	for (i = 0; i < 10; i++) {
		SEM_pend(&(info->notifySemObj), SYS_FOREVER);
		// invalidate cache
		BCACHE_inv ((Ptr)buf, length, TRUE) ;
		_ComputeTrackability();
		// writeback
		BCACHE_wb ((Ptr)buf, length, TRUE) ;
		NOTIFY_notify (ID_GPP, MPCSXFER_IPS_ID, MPCSXFER_IPS_EVENTNO, (Uint32)1) ;
	}

	return SYS_OK;
}

Int Task_delete (Task_TransferInfo *info)
{
	Int    status     = SYS_OK ;
	/*
	 *  Unregister notification for the event callback used to get control and
	 *  data buffer pointers from the GPP-side.
	 */
	status = NOTIFY_unregister (ID_GPP,
	                            MPCSXFER_IPS_ID,
	                            MPCSXFER_IPS_EVENTNO,
	                            (FnNotifyCbck) Task_notify,
	                            info) ;

	/* Free the info structure */
	MEM_free (DSPLINK_SEGID,
	          info,
	          sizeof (Task_TransferInfo)) ;
	info = NULL ;

	return status ;
}


static Void Task_notify (Uint32 eventNo, Ptr arg, Ptr info)
{
	static int count = 0;
	Task_TransferInfo *mpcsInfo = (Task_TransferInfo *) arg ;

	(Void) eventNo ; /* To avoid compiler warning. */

	count++;

	// if the first loop, we need to set up buffer info
	if (is_initial_loop) {
		// first notification: buffer address
		if (count == 1) {
			buf = (Int32 *)info;
		}
		// second: buffer length
		if (count == 2) {
			length = (Int32)info;
			count = 0;
			SEM_post(&(mpcsInfo->notifySemObj));
		}

	} else {
		// ready to fire
		if (count == 1) {
			count = 0;
			SEM_post(&(mpcsInfo->notifySemObj));
		}
	}
}
