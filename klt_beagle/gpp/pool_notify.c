#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>   /* fsqrt()           */
#include "klt.h"
#include <arm_neon.h>
#include <sys/time.h>

#include <semaphore.h>
/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <pool.h>
#include <mpcs.h>
#include <notify.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif

#define SHIFT 10

/*  ----------------------------------- Application Header              */
#include <pool_notify.h>
//#include <pool_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS                       1

/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the POOL used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID                 0

/** ============================================================================
 *  @name   NUM_BUF_SIZES
 *
 *  @desc   Number of buffer pools to be configured for the allocator.
 *  ============================================================================
 */
#define NUM_BUF_SIZES                  1

/** ============================================================================
 *  @const  NUM_BUF_POOL0
 *
 *  @desc   Number of buffers in first buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL0                  1

/*  ============================================================================
 *  @const   pool_INVALID_ID
 *
 *  @desc   Indicates invalid processor ID within the pool_Ctrl structure.
 *  ============================================================================
 */
#define pool_INVALID_ID            (Uint32) -1

/** ============================================================================
 *  @const  pool_IPS_ID
 *
 *  @desc   The IPS ID to be used for sending notification events to the DSP.
 *  ============================================================================
 */
#define pool_IPS_ID                0

/** ============================================================================
 *  @const  pool_IPS_EVENTNO
 *
 *  @desc   The IPS event number to be used for sending notification events to
 *          the DSP.
 *  ============================================================================
 */
#define pool_IPS_EVENTNO           5


/*  ============================================================================
 *  @name   pool_BufferSize
 *
 *  @desc   Size of buffer to be used for data transfer.
 *  ============================================================================
 */
STATIC Int32  pool_BufferSize ;

/** ============================================================================
 *  @name   pool_DataBuf
 *
 *  @desc   Pointer to the shared data buffer used by the pool_notify sample
 *          application.
 *  ============================================================================
 */
Uint32 *pool_DataBuf = NULL ;

int is_initial_loop;
long long start;
long long get_usec(void);
/** ============================================================================
 *  @func   pool_Notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive notification indicating that the DSP-
 *          side application has completed its setup phase.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the IPS component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the NOTIFY
 *              component. This information is specific to the IPS being
 *              implemented.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
STATIC
Void
pool_Notify (Uint32 eventNo, Pvoid arg, Pvoid info) ;

sem_t sem;

/** ============================================================================
 *  @func   pool_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
pool_Create (IN Char8 *dspExecutable,
             IN Char8 *strBufferSize,
             IN Uint8   processorId,
             IN KLT_TrackingContext tc)
{
	DSP_STATUS      status     = DSP_SOK  ;
	Uint32          numArgs    = NUM_ARGS ;
	Void           *dspDataBuf = NULL ;
	Uint32          numBufs [NUM_BUF_SIZES] = {NUM_BUF_POOL0,
	                                          } ;
	Uint32          size    [NUM_BUF_SIZES] ;
	SMAPOOL_Attrs   poolAttrs ;
	Char8          *args [NUM_ARGS] ;


#ifdef DEBUG
	printf ("Entered pool_Create ()\n") ;
#endif

	sem_init(&sem, 0, 0);

	is_initial_loop = 1;

	/*
	 *  Create and initialize the proc object.
	 */
	status = PROC_setup (NULL) ;

	/*
	 *  Attach the Dsp with which the transfers have to be done.
	 */
	if (DSP_SUCCEEDED (status)) {
		status = PROC_attach (processorId, NULL) ;
		if (DSP_FAILED (status)) {
			printf ("PROC_attach () failed. Status = [0x%x]\n",
			        (int)status) ;
		}
	} else {
		printf ("PROC_setup () failed. Status = [0x%x]\n", (int)status) ;
	}

	/*
	 *  Open the pool.
	 */
	if (DSP_SUCCEEDED (status)) {
		size [0] = pool_BufferSize ;
		poolAttrs.bufSizes      = (Uint32 *) &size ;
		poolAttrs.numBuffers    = (Uint32 *) &numBufs ;
		poolAttrs.numBufPools   = NUM_BUF_SIZES ;
		poolAttrs.exactMatchReq = TRUE ;
		status = POOL_open (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
		                    &poolAttrs) ;
		if (DSP_FAILED (status)) {
			printf ("POOL_open () failed. Status = [0x%x]\n",
			        (int)status) ;
		}
	}

	/*
	 *  Allocate the data buffer to be used for the application.
	 */
	if (DSP_SUCCEEDED (status)) {
		status = POOL_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
		                     (Void **) &pool_DataBuf,
		                     pool_BufferSize) ;

		/* Get the translated DSP address to be sent to the DSP. */
		if (DSP_SUCCEEDED (status)) {
			status = POOL_translateAddr (
			                 POOL_makePoolId(processorId, SAMPLE_POOL_ID),
			                 &dspDataBuf,
			                 AddrType_Dsp,
			                 (Void *) pool_DataBuf,
			                 AddrType_Usr) ;

			if (DSP_FAILED (status)) {
				printf ("POOL_translateAddr () DataBuf failed."
				        " Status = [0x%x]\n",
				        (int)status) ;
			}
		} else {
			printf ("POOL_alloc () DataBuf failed. Status = [0x%x]\n",
			        (int)status) ;
		}
	}

	/*
	 *  Register for notification that the DSP-side application setup is
	 *  complete.
	 */
	if (DSP_SUCCEEDED (status)) {
		status = NOTIFY_register (processorId,
		                          pool_IPS_ID,
		                          pool_IPS_EVENTNO,
		                          (FnNotifyCbck) pool_Notify,
		                          0/* vladms XFER_SemPtr*/) ;
		if (DSP_FAILED (status)) {
			printf ("NOTIFY_register () failed Status = [0x%x]\n",
			        (int)status) ;
		}
	}

	/*
	 *  Load the executable on the DSP.
	 */
	if (DSP_SUCCEEDED (status)) {
		args [0] = strBufferSize ;
		{
			status = PROC_load (processorId, dspExecutable, numArgs, args) ;
		}

		if (DSP_FAILED (status)) {
			printf ("PROC_load () failed. Status = [0x%x]\n", (int)status) ;
		}
	}

	/*
	 *  Start execution on DSP.
	 */
	if (DSP_SUCCEEDED (status)) {
		status = PROC_start (processorId) ;
		if (DSP_FAILED (status)) {
			printf ("PROC_start () failed. Status = [0x%x]\n",
			        (int)status) ;
		}
	}

	/*
	 *  Wait for the DSP-side application to indicate that it has completed its
	 *  setup. The DSP-side application sends notification of the IPS event
	 *  when it is ready to proceed with further execution of the application.
	 */
	if (DSP_SUCCEEDED (status)) {
		// wait for initialization
		if (is_initial_loop) {
			sem_wait(&sem);
			is_initial_loop = 0;
		}
	}

	/*
	 *  Send notifications to the DSP with information about the address of the
	 *  control structure and data buffer to be used by the application.
	 *
	 */
	status = NOTIFY_notify (processorId,
	                        pool_IPS_ID,
	                        pool_IPS_EVENTNO,
	                        (Uint32) dspDataBuf);
	if (DSP_FAILED (status)) {
		printf ("Notify DataBuf failed."
		        " Status = [0x%x]\n",
		        (int)status) ;
	}

	status = NOTIFY_notify (processorId,
	                        pool_IPS_ID,
	                        pool_IPS_EVENTNO,
	                        (Uint32) pool_BufferSize);
	if (DSP_FAILED (status)) {
		printf ("Notify pool_BufferSize failed."
		        " Status = [0x%x]\n",
		        (int)status) ;
	}



#ifdef DEBUG
	printf ("Leaving pool_Create ()\n") ;
#endif

	return status ;
}

int result_address;

void copy_data(KLT_TrackingContext tc, float *gradx_data, float *grady_data, int nrows, int ncols, int rows_dsp)
{
	int y, x;

	int window_hh = (Uint32) tc->window_height / 2;
	int bordery = tc->bordery;  /* lost by convolution */

	result_address = 0;
#ifdef DEBUG
	printf("\nCopying data\n");
#endif

	// write related variables to pool
	// pool_DataBuf[0]: nrows
	pool_DataBuf[result_address++] = (Int32) nrows;
	// pool_DataBuf[1]: ncols
	pool_DataBuf[result_address++] = (Int32) ncols;
	// pool_DataBuf[2]: rows_dsp
	pool_DataBuf[result_address++] = (Int32) rows_dsp;
	// pool_DataBuf[3]: window_hh
	pool_DataBuf[result_address++] = (Int32) tc->window_height / 2;
	// pool_DataBuf[4]: window_hw
	pool_DataBuf[result_address++] = (Int32) tc->window_width / 2;
	// pool_DataBuf[5]: borderx
	pool_DataBuf[result_address++] = (Int32) tc->borderx;
	// pool_DataBuf[6]: bordery
	pool_DataBuf[result_address++] = (Int32) tc->bordery;
	// pool_DataBuf[7]: the start of grady will be stored
	result_address++;
	// pool_DataBuf[8]: address the result address will be stored
	result_address++;

	// copy data from gradx_data, grady_data to pool_DataBuf
	// converting floating into fixed-point

	// first do gradx_data
	for (y = nrows - rows_dsp - bordery - window_hh; y < nrows; y++)
		for (x = 0; x < ncols; x++)
			pool_DataBuf[result_address++] = (Int32) ( gradx_data[y * ncols + x] * (1 << SHIFT) );

	// store the start of grady
	pool_DataBuf[7] = result_address;

	for (y = nrows - rows_dsp - bordery - window_hh; y < nrows; y++)
		for (x = 0; x < ncols; x++) {
			pool_DataBuf[result_address++] = (Int32) ( grady_data[y * ncols + x] * (1 << SHIFT) );
		}
	// store the addr of result
	pool_DataBuf[8] = result_address;


#ifdef DEBUG
	printf("End copying data\n");
#endif

}

long long get_usec(void)
{
	long long r;
	struct timeval t;
	gettimeofday(&t, NULL);
	r = t.tv_sec * 1000000 + t.tv_usec;
	return r;
}



/** ============================================================================
 *  @func   pool_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
pool_Execute (IN KLT_TrackingContext tc, IN float *gradxData, IN float *gradyData, int nrows, int ncols, int rows_dsp )
{
	Uint8 processorId = 0;
	DSP_STATUS  status    = DSP_SOK ;


#if defined(DSP)
	unsigned char *buf_dsp;
#endif


#ifdef DEBUG
	printf ("\nEntered pool_Execute ()\n") ;
#endif

	copy_data(tc, gradxData, gradyData, nrows, ncols, rows_dsp);


	POOL_writeback (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
	                pool_DataBuf,
	                pool_BufferSize);

	POOL_translateAddr ( POOL_makePoolId(processorId, SAMPLE_POOL_ID),
	                     (void *)&buf_dsp,
	                     AddrType_Dsp,
	                     (Void *) pool_DataBuf,
	                     AddrType_Usr) ;

	// notify DSP that it can start
	NOTIFY_notify (processorId, pool_IPS_ID, pool_IPS_EVENTNO, (Uint32)1);
	start = get_usec();

#ifdef DEBUG
	printf ("\nExiting pool_Execute (), starting DSP\n") ;
#endif

	return status ;
}


/** ============================================================================
 *  @func   pool_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          pool_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
pool_Delete (void)
{
	Uint8 processorId = 0;
	DSP_STATUS status    = DSP_SOK ;
	DSP_STATUS tmpStatus = DSP_SOK ;

#ifdef DEBUG
	printf ("Entered pool_Delete ()\n") ;
#endif

	/*
	 *  Stop execution on DSP.
	 */
	status = PROC_stop (processorId) ;
	if (DSP_FAILED (status)) {
		printf ("PROC_stop () failed. Status = [0x%x]\n", (int)status) ;
	}

	/*
	 *  Unregister for notification of event registered earlier.
	 */
	tmpStatus = NOTIFY_unregister (processorId,
	                               pool_IPS_ID,
	                               pool_IPS_EVENTNO,
	                               (FnNotifyCbck) pool_Notify,
	                               0/* vladms pool_SemPtr*/) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		status = tmpStatus ;
		printf ("NOTIFY_unregister () failed Status = [0x%x]\n",
		        (int)status) ;
	}

	/*
	 *  Free the memory allocated for the data buffer.
	 */
	tmpStatus = POOL_free (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
	                       (Void *) pool_DataBuf,
	                       pool_BufferSize) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		status = tmpStatus ;
		printf ("POOL_free () DataBuf failed. Status = [0x%x]\n",
		        (int)status) ;
	}

	/*
	 *  Close the pool
	 */
	tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		status = tmpStatus ;
		printf ("POOL_close () failed. Status = [0x%x]\n", (int)status) ;
	}

	/*
	 *  Detach from the processor
	 */
	tmpStatus = PROC_detach  (processorId) ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		status = tmpStatus ;
		printf ("PROC_detach () failed. Status = [0x%x]\n", (int)status) ;
	}

	/*
	 *  Destroy the PROC object.
	 */
	tmpStatus = PROC_destroy () ;
	if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
		status = tmpStatus ;
		printf ("PROC_destroy () failed. Status = [0x%x]\n", (int)status) ;
	}

#ifdef DEBUG
	printf ("Leaving pool_Delete ()\n") ;
#endif
}


/** ============================================================================
 *  @func   pool_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
pool_Main (IN Char8 *dspExecutable,
           IN Char8 *strBufferSize,
           IN KLT_TrackingContext tc)
{
	DSP_STATUS status       = DSP_SOK ;
	Uint8      processorId  = 0 ;


#ifdef DEBUG
	printf ("========== Open pool ==========\n") ;
#endif

	if (   (dspExecutable != NULL)
	       && (strBufferSize != NULL)) {
		/*
		 *  Validate the buffer size and number of iterations specified.
		 */
		pool_BufferSize = DSPLINK_ALIGN ( atoi (strBufferSize),
		                                  DSPLINK_BUF_ALIGN) ;
#ifdef DEBUG
		printf(" Allocated a buffer of %d bytes\n", (int)pool_BufferSize );
#endif
		processorId            = 0 ;
		if (pool_BufferSize == 0) {
			status = DSP_EINVALIDARG ;
			printf ("ERROR! Invalid arguments specified for  ");
			printf ("     Buffer size    = %d\n",
			        (int)pool_BufferSize) ;
		}

		/*
		 *  Specify the dsp executable file name and the buffer size for
		 *  pool_notify creation phase.
		 */
		status = pool_Create (dspExecutable,
		                      strBufferSize,
		                      0,
		                      tc) ;

	} else {
		status = DSP_EINVALIDARG ;
		printf ("ERROR! Invalid arguments specified for  "
		        "pool_notify application\n") ;
	}

	printf ("====================================================\n") ;
}

/** ----------------------------------------------------------------------------
 *  @func   pool_Notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive notification indicating that the DSP-
 *          side application has completed its setup phase.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
Void
pool_Notify (Uint32 eventNo, Pvoid arg, Pvoid info)
{
	static int count = 0;
	count++;

#ifdef DEBUG
	printf("Notification %d: %8d \n", count, (int)info);
#endif

	sem_post(&sem);
}


/*********************************************************************
 * _minEigenvalue
 *
 * Given the three distinct elements of the symmetric 2x2 matrix
 *                     [gxx gxy]
 *                     [gxy gyy],
 * Returns the minimum eigenvalue of the matrix.
 */

static float _minEigenvalue(float gxx, float gxy, float gyy)
{
	return (float) ((gxx + gyy - sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy)) / 2.0f);
}


/**
 * This function involves DSPLINK computation
 */
int _ComputeTrackability(KLT_TrackingContext tc, int *pointlist, int nrows, int ncols, _KLT_FloatImage gradx, _KLT_FloatImage grady, int rows_dsp)
{
	register float gxx, gxy, gyy;
	register int xx, yy;
	register int *ptr;
	float val;
	Int32 res;
	int borderx = tc->borderx;  /* Must not touch cols */
	int bordery = tc->bordery;  /* lost by convolution */
	int window_hh = (Uint32) tc->window_height / 2;
	int window_hw = (Uint32) tc->window_width / 2;
	int x, y;
	int npoints = 0;

	if (borderx < window_hw)  borderx = window_hw;
	if (bordery < window_hh)  bordery = window_hh;

	pool_Execute (tc, gradx->data, grady->data, nrows, ncols, rows_dsp);

	/* For most of the pixels in the image, do ... */
	ptr = pointlist;
	for (y = bordery ; y < nrows - bordery - rows_dsp ; y += tc->nSkippedPixels + 1)
		for (x = borderx ; x < ncols - borderx ; x += tc->nSkippedPixels + 1)  {

			/* Sum the gradients in the surrounding window */
			// 2 temp 32 vectors
			float32x2_t vec64a, vec64b;

			// clear accumulators
			float32x4_t gxx128 = vdupq_n_f32(0.0f);
			float32x4_t gxy128 = vdupq_n_f32(0.0f);
			float32x4_t gyy128 = vdupq_n_f32(0.0f);

			for (yy = y - window_hh ; yy <= y + window_hh ; yy++) {
				for (xx = x - window_hw ; xx <= ((x + window_hw + 1) / 4) * 4 ; xx += 4)  {
					// load 4 x 32 bit values
					float32x4_t gx128 = vld1q_f32(&gradx->data[ncols * yy + xx]);
					float32x4_t gy128 = vld1q_f32(&grady->data[ncols * yy + xx]);

					// Vector multiply accumulate: vmla -> Vr[i] := Va[i] + Vb[i] * Vc[i]
					gxx128 = vmlaq_f32(gxx128, gx128, gx128);
					gxy128 = vmlaq_f32(gxy128, gx128, gy128);
					gyy128 = vmlaq_f32(gyy128, gy128, gy128);
				}
			}

			// split 128 bit vector into 2 x 64 bit vector
			vec64a = vget_low_f32(gxx128);
			vec64b = vget_high_f32(gxx128);
			// add 64 bit vectors together
			vec64a = vadd_f32(vec64a, vec64b);
			//  extract lanes and add together scalars
			gxx  = vget_lane_f32(vec64a, 0);
			gxx += vget_lane_f32(vec64a, 1);

			// same for gxy
			vec64a = vget_low_f32(gxy128);
			vec64b = vget_high_f32(gxy128);
			vec64a = vadd_f32(vec64a, vec64b);

			gxy  = vget_lane_f32(vec64a, 0);
			gxy += vget_lane_f32(vec64a, 1);

			// same for gyy
			vec64a = vget_low_f32(gyy128);
			vec64b = vget_high_f32(gyy128);
			vec64a = vadd_f32(vec64a, vec64b);

			gyy  = vget_lane_f32(vec64a, 0);
			gyy += vget_lane_f32(vec64a, 1);

			/* Store the trackability of the pixel as the minimum
			of the two eigenvalues */
			*ptr++ = x;
			*ptr++ = y;
			val = _minEigenvalue(gxx, gxy, gyy);
			*ptr++ = (int) val;
			npoints++;
		}

	// wait for the DSP
	sem_wait(&sem);
#ifdef DEBUG
	printf("DSP time: %lld\n", get_usec() - start);
#endif DEBUG
	// copy the results calculated by DSP to ptr
	for (y = nrows - bordery - rows_dsp ; y < nrows - bordery; y += tc->nSkippedPixels + 1)
		for (x = borderx ; x < ncols - borderx ; x += tc->nSkippedPixels + 1) {
			*ptr++ = x;
			*ptr++ = y;
			res = (Int32) pool_DataBuf[result_address] ;
			result_address++;

			*ptr++ = (int) res ;
			npoints++;

			/////* Sum the gradients in the surrounding window */
			//gxx = 0;  gxy = 0;  gyy = 0;
			//for (yy = y - window_hh ; yy <= y + window_hh ; yy++)
			//for (xx = x - window_hw ; xx <= x + window_hw ; xx++)  {
			//gx = *(gradx->data + ncols * yy + xx);
			//gy = *(grady->data + ncols * yy + xx);

			//gxx += gx * gx;
			//gxy += gx * gy;
			//gyy += gy * gy;
			//}

			//val =  _minEigenvalue(gxx, gxy, gyy);

		}
	return npoints;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
