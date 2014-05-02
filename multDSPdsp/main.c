/*  ----------------------------------- DSP/BIOS Headers            */
#include "multDSPcfg.h"
#include <sys.h>
#include <sem.h>
#include <msgq.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>

/*  ----------------------------------- Sample Headers              */
#include <tskMessage.h>

/*  ----------------------------------- BSL Headers                 */
 
#ifdef __cplusplus
extern "C" {
#endif


/* FILEID is used by SET_FAILURE_REASON macro. */
#define FILEID  FID_APP_C


/* Number of iterations message transfers to be done by the application. */
Uint8 Size;



/** ----------------------------------------------------------------------------
 *  @func   tskMessage
 *
 *  @desc   Task for TSK based TSKMESSAGE application.
 */
static Int tskMessage();

/** ============================================================================
 *  @func   atoi
 *
 *  @desc   Converts character string to integer value.
 *
 *  ============================================================================
 */
extern int atoi(const char* str);

/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry function.
 *
 *  @modif  None
 *  ============================================================================
 */
Void main(Int argc, Char* argv [])
{
    /* Task handler for TSK_create */
    TSK_Handle tskMessageTask;

#if !defined (DSP_BOOTMODE_NOBOOT)
    /* Get the number of transfers to be done by the application */
    Size = atoi(argv [0]);
    /* Initialize DSP/BIOS LINK. */
    DSPLINK_init();
#endif

    /* Creating task for TSKMESSAGE application */
    tskMessageTask = TSK_create(tskMessage, NULL, 0);
    if (tskMessageTask == NULL)
    {
        SET_FAILURE_REASON(SYS_EALLOC);
        LOG_printf(&trace, "Create TSKMESSAGE: Failed.\n");
    }
}

/** ----------------------------------------------------------------------------
 *  @func   tskMessage
 *
 *  @desc   Task for TSK based TSKMESSAGE application.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Int tskMessage()
{
    Int status = SYS_OK;
    TSKMESSAGE_TransferInfo* info;

    /* Create Phase */
    status = TSKMESSAGE_create(&info);

    /* Execute Phase */
    if (status == SYS_OK)
    {
        /* Start the execution phase. */
        status = TSKMESSAGE_execute(info);
        if (status != SYS_OK)
        {
            SET_FAILURE_REASON(status);
        }
    }

    /* Delete Phase */
    status = TSKMESSAGE_delete(info);
    if (status != SYS_OK)
    {
        SET_FAILURE_REASON(status);
    }
    return status;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
