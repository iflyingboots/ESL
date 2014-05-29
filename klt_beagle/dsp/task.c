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
#include <task.h>

#define SHIFT 10

extern Uint16 MPCSXFER_BufferSize ;

int is_initial_loop;

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

// buf type may be very subtle..
Int32 *buf;
int length;
int window_hh;
int window_hw;


void calculate_grad()
{
	/* Sum the gradients in the surrounding window */
	int gxx = 0;
	int gxy = 0;
	int gyy = 0;
	int i, gx, gy;
	int offset = window_hh *  2 * window_hw * 2;
	for (i = 0; i < offset; i++) {
		gx = buf[i];
		gy = buf[i + offset];
		gxx += (gx * gx) >> SHIFT;
		gxy += (gx * gy) >> SHIFT;
		gyy += (gy * gy) >> SHIFT;
	}

	NOTIFY_notify(ID_GPP, MPCSXFER_IPS_ID, MPCSXFER_IPS_EVENTNO, (Int32)gxx);
	NOTIFY_notify(ID_GPP, MPCSXFER_IPS_ID, MPCSXFER_IPS_EVENTNO, (Int32)gxy);
	NOTIFY_notify(ID_GPP, MPCSXFER_IPS_ID, MPCSXFER_IPS_EVENTNO, (Int32)gyy);
}

Int Task_execute (Task_TransferInfo *info)
{
    while (1) {
        SEM_pend(&(info->notifySemObj), SYS_FOREVER);
        BCACHE_inv ((Ptr)buf, length, TRUE) ;
        calculate_grad();
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
		// First notification: buffer address
		if (count == 1) {
			buf = (Int32 *)info;
		}
		// buffer length
		if (count == 2) {
			length = (int)info;
			count = 0;
            SEM_post(&(mpcsInfo->notifySemObj));
		}
	} else {
		// window_hh
        if (count == 1) {
			window_hh = (int)info;
		}
		// window_hw
		if (count == 2) {
			window_hw = (int)info;
			SEM_post(&(mpcsInfo->notifySemObj));
		    count = 0;
		}
	}

}
