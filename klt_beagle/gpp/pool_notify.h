#if !defined (pool_H)
#define pool_H

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/** ============================================================================
 *  @const  ID_PROCESSOR
 *
 *  @desc   The processor id of the processor being used.
 *  ============================================================================
 */
#define ID_PROCESSOR       0


/** ============================================================================
 *  @func   pool_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    strBufferSize
 *              String representation of buffer size to be used
 *              for data transfer.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *  @arg    processorId
 *             Id of the DSP Processor.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Resource allocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    pool_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
pool_Create (IN Char8 *dspExecutable,
                    IN Char8 *strBufferSize,
                    IN Uint8   processorId) ;


/** ============================================================================
 *  @func   pool_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of times to send the message to the DSP.
 *  @arg    processorId
 *             Id of the DSP Processor.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              MESSAGE execution failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    pool_Delete , pool_Create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
pool_Execute (IN Uint32 numIterations, IN Uint8 processorId) ;


/** ============================================================================
 *  @func   pool_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          pool_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @arg    processorId
 *             Id of the DSP Processor.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Resource deallocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    pool_Create
 *  ============================================================================
 */
NORMAL_API
Void
pool_Delete (IN Uint8 processorId) ;


/** ============================================================================
 *  @func   pool_Main
 *
 *  @desc   The OS independent driver function for the MESSAGE sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strBufferSize
 *              Buffer size to be used for data-transfer in string format.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *  @arg    strProcessorId
 *             ID of the DSP Processor in string format.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    pool_Create, pool_Execute, pool_Delete
 *  ============================================================================
 */
NORMAL_API
Void
pool_Main (IN Char8 *dspExecutable,
           IN Char8 *strBufferSize,
           IN float *gradx_data,
           IN float *grady_data) ;


#endif /* !defined (pool_H) */
