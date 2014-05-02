/** ============================================================================
 *  @file   mpcsxfer_config.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/mpcsxfer/
 *
 *  @desc   Header file containing configuration for the MPCSXFER sample.
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


#if !defined (MPCSXFER_CONFIG_)
#define MPCSXFER_CONFIG_


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the pool used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID                 0

/*  ============================================================================
 *  @const   MPCSXFER_INVALID_ID
 *
 *  @desc   Indicates invalid processor ID within the MPCSXFER_Ctrl structure.
 *  ============================================================================
 */
#define MPCSXFER_INVALID_ID            (Uint32) -1

/** ============================================================================
 *  @const  MPCSXFER_IPS_ID
 *
 *  @desc   The IPS ID to be used for sending notification events to the DSP.
 *  ============================================================================
 */
#define MPCSXFER_IPS_ID                0

/** ============================================================================
 *  @const  MPCSXFER_IPS_EVENTNO
 *
 *  @desc   The IPS event number to be used for sending notification events to
 *          the DSP.
 *  ============================================================================
 */
#define MPCSXFER_IPS_EVENTNO           5

typedef struct MPCSXFER_Ctrl_tag {
    Uint32   procId ;
    Uint32   iterNo ;
    Uint32   dataVerify ;
} MPCSXFER_Ctrl ;

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MPCSXFER_CONFIG_) */
