/*  ----------------------------------- OS Specific Headers           */
#include <stdio.h>
#include <stdlib.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <pool_notify.h>

/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
int main (int argc, char ** argv)
{
    Char8 * dspExecutable    = NULL ;
    Char8 * strBufferSize    = NULL ;
    Uint8 matrixSize;

    char strBuffer[80];

    if (argc != 3) {
        printf ("Usage : %s <absolute path of DSP executable> "
           "<size of the matrix>\n",
           argv [0]) ;
    }
    else {
        dspExecutable    = argv [1] ;

        matrixSize = atoi(argv[2]);
        sprintf(strBuffer, "%d", matrixSize * matrixSize * sizeof(int) * 3);
        strBufferSize = strBuffer;

#ifdef DEBUG
        printf("matrixSize:%d strBufferSize:%s\n", matrixSize, strBufferSize);
#endif

        pool_notify_Main (dspExecutable,
                          strBufferSize,
                          matrixSize) ;
    }

    return 0 ;
}
