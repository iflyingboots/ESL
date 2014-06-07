/*  ----------------------------------- OS Specific Headers           */
#include <stdio.h>
#include <stdlib.h>

/* KLT Headers */
#include "pnmio.h"
#include "klt.h"
#include "Timer.h"

#define REPLACE

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <pool_notify.h>

Char8 *dspExecutable = NULL;
/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
int main (int argc, char **argv)
{

	/* START KLT CONFIG */
	Timer appTimer;
	unsigned char *img1, *img2;
	char fnamein[100], fnameout[100];
	KLT_TrackingContext tc;
	KLT_FeatureList fl;
	KLT_FeatureTable ft;
	int ncols, nrows;
	int i;

	// TODO
	int nFeatures = 512;
	int nFrames = 10;
	int rows_dsp = 50;
	Char8 *strBufferSize = "800000";


	if (argc == 2) {
		dspExecutable = argv[1];
	} else {
		printf ("Usage : %s <absolute path of DSP executable> \n", argv [0]) ;
		exit(0);
	}

    initTimer(&appTimer, "APP Time");
	tc = KLTCreateTrackingContext();
	fl = KLTCreateFeatureList(nFeatures);
	ft = KLTCreateFeatureTable(nFrames, nFeatures);

	tc->sequentialMode = TRUE;
	tc->writeInternalImages = FALSE;
	tc->affineConsistencyCheck = -1;  /* set this to 2 to turn on affine consistency check */


	// Just open the pool
	pool_Main(dspExecutable, strBufferSize, tc);

	startTimer(&appTimer);
	img1 = pgmReadFile("img0.pgm", NULL, &ncols, &nrows);
	img2 = (unsigned char *) malloc(ncols * nrows * sizeof(unsigned char));





	KLTSelectGoodFeatures(tc, img1, ncols, nrows, fl, rows_dsp);
	KLTStoreFeatureList(fl, ft, 0);
	KLTWriteFeatureListToPPM(fl, img1, ncols, nrows, "feat0.ppm");

	for (i = 1 ; i < nFrames ; i++) {
		sprintf(fnamein, "img%d.pgm", i);
		pgmReadFile(fnamein, img2, &ncols, &nrows);
		KLTTrackFeatures(tc, img1, img2, ncols, nrows, fl);

#ifdef REPLACE
		KLTReplaceLostFeatures(tc, img2, ncols, nrows, fl, rows_dsp);
#endif

		KLTStoreFeatureList(fl, ft, i);
		sprintf(fnameout, "feat%d.ppm", i);
		KLTWriteFeatureListToPPM(fl, img2, ncols, nrows, fnameout);
	}
	stopTimer(&appTimer);
	printTime(appTimer);
	printf("Frames per second = %4.2f\n", nFrames / getTime(appTimer) * 1000 );

	KLTWriteFeatureTable(ft, "features.txt", "%5.1f");
	KLTWriteFeatureTable(ft, "features.ft", NULL);

	KLTFreeFeatureTable(ft);
	KLTFreeFeatureList(fl);
	KLTFreeTrackingContext(tc);
	free(img1);
	free(img2);
	pool_Delete();

	/* END KLT CONFIG */

	return 0 ;
}
