/** ============================================================================
 *  @file   pool_notify_config.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/pool_notify/
 *
 *  @desc   Source file containing configuration for the pool_notify sample.
 *
 *  @ver    1.63
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#if ((PHYINTERFACE == PCI_INTERFACE) || (PHYINTERFACE == VLYNQ_INTERFACE))
#include <dma_pool.h>
#else
#include <sma_pool.h>
#endif /* if ((PHYINTERFACE == PCI_INTERFACE) || (PHYINTERFACE == VLYNQ_INTERFACE)) */
#include <mpcs.h>

/*  ----------------------------------- Sample Headers              */
#include <pool_notify_config.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  NUM_POOLS
 *
 *  @desc   Number of pools configured in the system.
 *  ============================================================================
 */
#define NUM_POOLS         1

/** ============================================================================
 *  @const  SAMPLEPOOL_PARAMS, SAMPLEPOOL_FXNS, SAMPLEPOOL_init
 *
 *  @desc   Pool configuration definitions
 *  ============================================================================
 */
#define SAMPLEPOOL_PARAMS       SamplePoolParams
#if ((PHYINTERFACE == PCI_INTERFACE) || (PHYINTERFACE == VLYNQ_INTERFACE))
#define SAMPLEPOOL_FXNS         DMAPOOL_FXNS
#define SAMPLEPOOL_init         DMAPOOL_init

DMAPOOL_Params SamplePoolParams =
{
    SAMPLE_POOL_ID, /* Pool ID */
    TRUE            /* Exact Size Match Requirement */
} ;
#else
#define SAMPLEPOOL_FXNS         SMAPOOL_FXNS
#define SAMPLEPOOL_init         SMAPOOL_init

SMAPOOL_Params SamplePoolParams =
{
    SAMPLE_POOL_ID, /* Pool ID */
    TRUE            /* Exact Size Match Requirement */
} ;
#endif /* if ((PHYINTERFACE == PCI_INTERFACE) || (PHYINTERFACE == VLYNQ_INTERFACE)) */

/** ============================================================================
 *  @name   pool_notify_Pools
 *
 *  @desc   Array of pools.
 *  ============================================================================
 */
POOL_Obj pool_notify_Pools [NUM_POOLS] =
{
#if defined (DSP_BOOTMODE_NOBOOT)
    POOL_NOENTRY
#else
    {
        &SAMPLEPOOL_init,               /* Init Function                      */
        (POOL_Fxns *) &SAMPLEPOOL_FXNS, /* Pool interface functions           */
        &SAMPLEPOOL_PARAMS,              /* Pool params                        */
        NULL                            /* Pool object: Set within pool impl. */
    }
#endif
} ;

/** ============================================================================
 *  @name   POOL_config
 *
 *  @desc   POOL configuration information.
 *          POOL_config is a required global variable.
 *  ============================================================================
 */
POOL_Config POOL_config = {pool_notify_Pools, NUM_POOLS} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
