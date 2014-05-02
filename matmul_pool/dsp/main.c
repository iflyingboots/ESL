/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <tsk.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <mpcs.h>

/*  ----------------------------------- Sample Headers              */
#include <task.h>
#include <pool_notify_config.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID             FID_APP_C


/** ============================================================================
 *  @name   MPCSXFER_BufferSize
 *
 *  @desc   Size of buffer to be used for data transfer.
 *  ============================================================================
 */
Uint16 MPCSXFER_BufferSize ;

/** ============================================================================
 *  @name   MPCSXFER_NumIterations
 *
 *  @desc   Iterations of data transfer to be done by the application.
 *  ============================================================================
 */
Uint32 MPCSXFER_NumIterations ;

#if defined (DSP_BOOTMODE_NOBOOT)
/** ============================================================================
 *  @name   DSPLINK_initFlag
 *
 *  @desc   Flag to indicate to the DSPLINK_init.
 *  ============================================================================
 */
volatile Uint32 DSPLINK_initFlag = 0xBABAC0C0 ;

#if defined (DM6446GEM)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (DM6467GEM )
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (OMAP3530)

/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (5u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (55u)

#elif defined (OMAP2530)

/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (5u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (55u)

#elif defined (DM6437)

/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (56u)

#elif defined (DM648)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (15u)

#elif defined (DRA44XGEM )
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (DRX416GEM)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (16u)

#elif defined (DA8XXGEM)
/** ============================================================================
 *  @name   DSPLINK_INT_VEC_ID
 *
 *  @desc   Interrupt vectorId to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_VEC_ID         (4u)

/** ============================================================================
 *  @name   DSPLINK_INT_ID
 *
 *  @desc   Interrupt ID to be used in HAL_initIsr. Value Corresponds to
 *          IPS table 0 in CFG file.
 *  ============================================================================
 */
#define  DSPLINK_INT_ID             (5u)

#endif
#endif

/** ============================================================================
 *  @name   trace
 *
 *  @desc   trace LOG_Obj used to do LOG_printf
 *  ============================================================================
 */
extern LOG_Obj trace ;

/** ============================================================================
 *  @func   atoi
 *
 *  @desc   Converts character string to integer value.
 *
 *  @arg    str
 *              Pointer to character string.
 *
 *  @ret    <integer value>
 *              Converted integer value.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern int atoi (const char * str) ;


#if defined (DSP_BOOTMODE_NOBOOT)
/** ----------------------------------------------------------------------------
 *  @func   HAL_initIsr
 *
 *  @desc   ISR which is executed to tell DSP, now it is time to rum DSPLINK_init
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static void HAL_initIsr (Ptr arg) ;
#endif


/** ----------------------------------------------------------------------------
 *  @func   Task
 *
 *  @desc   Task for TSK based MPCSXFER application.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static Int Task () ;

/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry function.
 *
 *  @modif  None
 *  ============================================================================
 */
Void main (Int argc, Char *argv [])
{

    /* Initialize DSP/BIOS LINK. */
    DSPLINK_init () ;

    /* Transfer size for data buffer given by GPP side */
    MPCSXFER_BufferSize = DSPLINK_ALIGN (atoi (argv[0]),
                                         DSPLINK_BUF_ALIGN) ;

    /* Creating task for MPCSXFER application */
    TSK_create (Task, NULL, 0) ;
}


/** ----------------------------------------------------------------------------
 *  @func   Task
 *
 *  @desc   Task for TSK based MPCSXFER application.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Int Task ()
{
    Int                        status = SYS_OK ;
    Task_TransferInfo * info ;

    /* Create Phase */
    status = Task_create (&info) ;

    /* Execute Phase */
    if (status == SYS_OK) {
        status = Task_execute (info) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Delete Phase */
    status = Task_delete (info) ;
    if (status != SYS_OK) {
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}

