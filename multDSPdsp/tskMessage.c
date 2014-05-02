/** ============================================================================
 *  @file   tskMessage.c
 *
 *  @path   
 *
 *  @desc   This is simple TSK based application that uses MSGQ. It receives
 *          and transmits messages from/to the GPP and runs the DSP
 *          application code (located in an external source file)
 *
 *  @ver    1.10
 */


/*  ----------------------------------- DSP/BIOS Headers            */
#include "multDSPcfg.h"
#include <gbl.h>
#include <sys.h>
#include <sem.h>
#include <msgq.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>

/*  ----------------------------------- Sample Headers              */
#include <multDSP_config.h>
#include <tskMessage.h>

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* FILEID is used by SET_FAILURE_REASON macro. */
#define FILEID  FID_APP_C

/* Place holder for the MSGQ name created on DSP */
Uint8 dspMsgQName[DSP_MAX_STRLEN];

#define MAXELEMENTS 128 * 128

/* Number of iterations message transfers to be done by the application. */
extern Uint8 Size;
Uint32 mat1[MAXELEMENTS], mat2[MAXELEMENTS], prod_dsp[MAXELEMENTS];


/** ============================================================================
 *  @func   TSKMESSAGE_create
 *
 *  @desc   Create phase function for the TSKMESSAGE application. Initializes
 *          the TSKMESSAGE_TransferInfo structure with the information that will
 *          be used by the other phases of the application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMESSAGE_create(TSKMESSAGE_TransferInfo** infoPtr)
{
    Int status = SYS_OK;
    MSGQ_Attrs msgqAttrs = MSGQ_ATTRS;
    TSKMESSAGE_TransferInfo* info = NULL;
    MSGQ_LocateAttrs syncLocateAttrs;

    /* Allocate TSKMESSAGE_TransferInfo structure that will be initialized
     * and passed to other phases of the application */
    *infoPtr = MEM_calloc(DSPLINK_SEGID, sizeof(TSKMESSAGE_TransferInfo), DSPLINK_BUF_ALIGN);
    if (*infoPtr == NULL)
    {
        status = SYS_EALLOC;
        SET_FAILURE_REASON(status);
    }
    else
    {
        info = *infoPtr;
        info->Size = Size;
        info->localMsgq = MSGQ_INVALIDMSGQ;
        info->locatedMsgq = MSGQ_INVALIDMSGQ;
    }

    if (status == SYS_OK)
    {
        /* Set the semaphore to a known state. */
        SEM_new(&(info->notifySemObj), 0);

        /* Fill in the attributes for this message queue. */
        msgqAttrs.notifyHandle = &(info->notifySemObj);
        msgqAttrs.pend = (MSGQ_Pend) SEM_pendBinary;
        msgqAttrs.post = (MSGQ_Post) SEM_postBinary;

        SYS_sprintf((Char *)dspMsgQName, "%s%d", DSP_MSGQNAME, GBL_getProcId());

        /* Creating message queue */
        status = MSGQ_open((String)dspMsgQName, &info->localMsgq, &msgqAttrs);
        if (status != SYS_OK)
        {
            SET_FAILURE_REASON(status);
        }
        else
        {
            /* Set the message queue that will receive any async. errors. */
            MSGQ_setErrorHandler(info->localMsgq, SAMPLE_POOL_ID);

            /* Synchronous locate.                           */
            /* Wait for the initial startup message from GPP */
            status = SYS_ENOTFOUND;
            while ((status == SYS_ENOTFOUND) || (status == SYS_ENODEV))
            {
                syncLocateAttrs.timeout = SYS_FOREVER;
                status = MSGQ_locate(GPP_MSGQNAME, &info->locatedMsgq, &syncLocateAttrs);
                if ((status == SYS_ENOTFOUND) || (status == SYS_ENODEV))
                {
                    TSK_sleep(1000);
                }
                else if(status != SYS_OK)
                {
#if !defined (LOG_COMPONENT)
                    LOG_printf(&trace, "MSGQ_locate (msgqOut) failed. Status = 0x%x\n", status);
#endif
                }
            }
        }
       /* Initialize the sequenceNumber */
        info->sequenceNumber = 0;
    }

    return status;
}



    void matMult2(Uint8 Size, Uint32 *mat1, Uint32 *mat2, Uint32 *prod)
	{
		Uint8 i, j, k;
		for (j = Size/2;j < Size; j++)
		{
			for (i = 0; i < Size; i++)
			{
				prod[i+j*Size]=0;
				for(k=0;k<Size;k++)
					prod[i+j*Size] = prod[i+j*Size] + mat1[k+j*Size] * mat2[i+k*Size];
					//prod[i+j*Size] = prod[i+j*Size] + mat1[k+j*Size] * mat2[k+i*Size];
			}
		}
	} 




/** ============================================================================
 *  @func   TSKMESSAGE_execute
 *
 *  @desc   Execute phase function for the TSKMESSAGE application. Application
 *          receives a message, verifies the id and executes the DSP processing.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMESSAGE_execute(TSKMESSAGE_TransferInfo* info)
{
    Int status = SYS_OK;
    ControlMsg* msg;
    Uint16 nrElements = info->Size * info->Size;



	/* Get the first matrix */
	status = MSGQ_get(info->localMsgq,(MSGQ_Msg*) &msg, SYS_FOREVER);
	if (status == SYS_OK)
	{
		/* Check if the message is an asynchronous error message */
		if (MSGQ_getMsgId((MSGQ_Msg) msg) == MSGQ_ASYNCERRORMSGID)
		{
#if !defined (LOG_COMPONENT)
			LOG_printf(&trace, "Transport error Type = %d",((MSGQ_AsyncErrorMsg *) msg)->errorType);
#endif
			/* Must free the message */
			MSGQ_free((MSGQ_Msg) msg);
			status = SYS_EBADIO;
			SET_FAILURE_REASON(status);
		}
		else if (MSGQ_getMsgId ((MSGQ_Msg) msg) != info->sequenceNumber)
		{
#if !defined (LOG_COMPONENT)
			LOG_printf(&trace, "Out of sequence message!");
#endif
			MSGQ_free((MSGQ_Msg) msg);
			status = SYS_EBADIO;
			SET_FAILURE_REASON(status);
		}
		else
		{
			memcpy( mat1, &msg->mat, nrElements *  sizeof(Uint32) );
		}
	}
	else
    {
        SET_FAILURE_REASON(status);
    }
    
    // Send a sync message
    MSGQ_setMsgId((MSGQ_Msg) msg, info->sequenceNumber);
    MSGQ_setSrcQueue((MSGQ_Msg) msg, info->localMsgq);
	status = MSGQ_put(info->locatedMsgq, (MSGQ_Msg) msg);
	if (status != SYS_OK)
	{
		/* Must free the message */
		MSGQ_free ((MSGQ_Msg) msg);
		SET_FAILURE_REASON(status);
	}
    else
    {
        SET_FAILURE_REASON(status);
    }
    info->sequenceNumber++;
	
	
	/* Get the second matrix */
	status = MSGQ_get(info->localMsgq,(MSGQ_Msg*) &msg, SYS_FOREVER);
	if (status == SYS_OK)
	{
		/* Check if the message is an asynchronous error message */
		if (MSGQ_getMsgId((MSGQ_Msg) msg) == MSGQ_ASYNCERRORMSGID)
		{
#if !defined (LOG_COMPONENT)
			LOG_printf(&trace, "Transport error Type = %d",((MSGQ_AsyncErrorMsg *) msg)->errorType);
#endif
			/* Must free the message */
			MSGQ_free((MSGQ_Msg) msg);
			status = SYS_EBADIO;
			SET_FAILURE_REASON(status);
		}
		else if (MSGQ_getMsgId ((MSGQ_Msg) msg) != info->sequenceNumber)
		{
#if !defined (LOG_COMPONENT)
			LOG_printf(&trace, "Out of sequence message!");
#endif
			MSGQ_free((MSGQ_Msg) msg);
			status = SYS_EBADIO;
			SET_FAILURE_REASON(status);
		}
		else
		{
			memcpy( mat2, &msg->mat, nrElements *  sizeof(Uint32) );
		}
	}
	else
    {
        SET_FAILURE_REASON(status);
    }
    
    matMult2(Size,mat1,mat2,prod_dsp);
	
	// Send the result
    MSGQ_setMsgId((MSGQ_Msg) msg, info->sequenceNumber);
    MSGQ_setSrcQueue((MSGQ_Msg) msg, info->localMsgq);
    
    if (Size % 2 == 0) memcpy( &msg->mat+ nrElements/2, prod_dsp+ nrElements/2, nrElements/2 * sizeof(Uint32));
    else memcpy( &msg->mat+ (nrElements-Size)/2, prod_dsp+ (nrElements-Size)/2, (nrElements+Size)/2 * sizeof(Uint32));
    
	status = MSGQ_put(info->locatedMsgq, (MSGQ_Msg) msg);
	if (status != SYS_OK)
	{
		/* Must free the message */
		MSGQ_free ((MSGQ_Msg) msg);
		SET_FAILURE_REASON(status);
	}
    else
    {
        SET_FAILURE_REASON(status);
    }
    info->sequenceNumber++;


    return status;
}


/** ============================================================================
 *  @func   TSKMESSAGE_delete
 *
 *  @desc   Delete phase function for the TSKMESSAGE application. It deallocates
 *          all the resources of allocated during create phase of the
 *          application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMESSAGE_delete(TSKMESSAGE_TransferInfo* info)
{
    Int status = SYS_OK;
    Int tmpStatus = SYS_OK;
    Bool freeStatus = FALSE;

    /* Release the located message queue */
    if (info->locatedMsgq != MSGQ_INVALIDMSGQ)
    {
        status = MSGQ_release(info->locatedMsgq);
        if (status != SYS_OK)
        {
            SET_FAILURE_REASON(status);
        }
    }

     /* Reset the error handler before deleting the MSGQ that receives */
     /* the error messages.                                            */
    MSGQ_setErrorHandler(MSGQ_INVALIDMSGQ, POOL_INVALIDID);

    /* Delete the message queue */
    if (info->localMsgq != MSGQ_INVALIDMSGQ)
    {
        tmpStatus = MSGQ_close(info->localMsgq);
        if ((status == SYS_OK) && (tmpStatus != SYS_OK))
        {
            status = tmpStatus;
            SET_FAILURE_REASON(status);
        }
    }

    /* Free the info structure */
    freeStatus = MEM_free(DSPLINK_SEGID, info, sizeof(TSKMESSAGE_TransferInfo));
    if ((status == SYS_OK) && (freeStatus != TRUE))
    {
        status = SYS_EFREE;
        SET_FAILURE_REASON(status);
    }
    return status;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
