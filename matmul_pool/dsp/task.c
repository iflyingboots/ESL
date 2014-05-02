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

extern Uint16 MPCSXFER_BufferSize ;


static Void Task_notify (Uint32 eventNo, Ptr arg, Ptr info) ;

Int Task_create (Task_TransferInfo ** infoPtr)
{
    Int status    = SYS_OK ;
    Task_TransferInfo * info = NULL ;

    /* Allocate Task_TransferInfo structure that will be initialized
     * and passed to other phases of the application */
    if (status == SYS_OK) {
        *infoPtr = MEM_calloc (DSPLINK_SEGID,
                               sizeof (Task_TransferInfo),
                               0) ; /* No alignment restriction */
        if (*infoPtr == NULL) {
            status = SYS_EALLOC ;
        }
        else {
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
    SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
    SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
    SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;

    return status ;
}

Uint16* buf;
int length;
int matrix_size;

void matmul_dsp() {
    Uint32 i, j, k;
    // int *mat1 = buf;
    // mat1 = buf
    // int *mat2 = buf + matrix_size * matrix_size;
    // mat2 = buf[matrix_size * matrix_size]
    // int *prod = buf + matrix_size * matrix_size * 2;

    // for (i = 0; i < matrix_size * matrix_size * 3; i++) {
    //     NOTIFY_notify(ID_GPP,MPCSXFER_IPS_ID,MPCSXFER_IPS_EVENTNO,buf[i]);
    // }
    // return;

    for (i = 0;i < matrix_size; i++)
    {
        for (j = 0; j < matrix_size; j++)
        {
            // prod[i * matrix_size + j] = 0;
            buf[matrix_size * matrix_size * 2 + i * matrix_size + j] = 0;
            for(k = 0; k < matrix_size; k++) {
                buf[matrix_size * matrix_size * 2 + i * matrix_size + j] = buf[matrix_size * matrix_size * 2 + i * matrix_size + j] + buf[i * matrix_size + k] * buf[matrix_size * matrix_size + k * matrix_size + j];
            }
            // NOTIFY_notify(ID_GPP,MPCSXFER_IPS_ID,MPCSXFER_IPS_EVENTNO,(Uint32)buf[i * matrix_size + j + matrix_size * matrix_size * 2]);
            // NOTIFY_notify(ID_GPP,MPCSXFER_IPS_ID,MPCSXFER_IPS_EVENTNO,(Uint32)buf[matrix_size * matrix_size * 2 + i * matrix_size + j]);
            // prod[i][j] = prod[i][j]+mat1[i][k] * mat2[k][j];
        }
    }
}

Int Task_execute (Task_TransferInfo * info)
{
    SEM_pend (&(info->notifySemObj), SYS_FOREVER);

    BCACHE_inv ((Ptr)buf, length, TRUE) ;

    matmul_dsp();

    BCACHE_wb ((Ptr)buf, length, TRUE) ;

    NOTIFY_notify(ID_GPP,MPCSXFER_IPS_ID,MPCSXFER_IPS_EVENTNO,(Uint32)0);
    NOTIFY_notify(ID_GPP,MPCSXFER_IPS_ID,MPCSXFER_IPS_EVENTNO,(Uint32)999);


    // NOTIFY_notify(ID_GPP,MPCSXFER_IPS_ID,MPCSXFER_IPS_EVENTNO,(Uint32)sum);

    return SYS_OK;
}

Int Task_delete (Task_TransferInfo * info)
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
    Task_TransferInfo * mpcsInfo = (Task_TransferInfo *) arg ;

    (Void) eventNo ; /* To avoid compiler warning. */

    count++;
    if (count==1) {
        buf =(Uint16 *)info ;
    }
    if (count==2) {
        length = (int)info;
    }
    if (count==3) {
        matrix_size = (int)info;
    }

    SEM_post(&(mpcsInfo->notifySemObj));
}
