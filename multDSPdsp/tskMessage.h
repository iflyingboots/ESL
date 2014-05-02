/** ============================================================================
 *  @file   tskMessage.h
 *
 *  @path   
 *
 *  @desc   This is simple TSK based application that uses MSGQ. It receives
 *          and transmits messages from/to the GPP and runs the DSP
 *          application code (located in an external source file)
 *
 *  @ver    1.10
 *  ============================================================================
 *  Copyright (c) Texas Instruments Incorporated 2002-2009
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied or provided.
 *  ============================================================================
 */


#ifndef TSKMESSAGE_
#define TSKMESSAGE_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>
#include <sem.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @name   TSKMESSAGE_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  sequenceNumber
 *              Expected id from the GPP.
 *  @field  numTransfers
 *              Number of message to be transferred between GPP and DSP.
 *  @field  localMsgq
 *              Handle of opened message queue.
 *  @field  locatedMsgqHandle
 *              Handle to located message queue.
 *  @field  notifySemObj
 *              Semaphore used for message notification.
 *  ============================================================================
 */
typedef struct TSKMESSAGE_TransferInfo_tag {
    Uint16     sequenceNumber;
    Uint8	   Size;
    MSGQ_Queue localMsgq;
    MSGQ_Queue locatedMsgq;
    SEM_Obj    notifySemObj;
} TSKMESSAGE_TransferInfo;


/** ============================================================================
 *  @func   TSKMESSAGE_create
 *
 *  @desc   Create phase function of TSKMESSAGE application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKMESSAGE_create(TSKMESSAGE_TransferInfo** transferInfo);

/** ============================================================================
 *  @func   TSKMESSAGE_execute
 *
 *  @desc   Excecute phase function of TSKMESSAGE application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKMESSAGE_execute(TSKMESSAGE_TransferInfo* transferInfo);

/** ============================================================================
 *  @func   TSKMESSAGE_delete
 *
 *  @desc   Delete phase function of TSKMESSAGE application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKMESSAGE_delete(TSKMESSAGE_TransferInfo* transferInfo);


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* TSKMESSAGE_ */
