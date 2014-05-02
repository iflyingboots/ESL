/*  ----------------------------------- OS Specific Headers           */
#include <stdio.h>
#include <stdlib.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <gpptypes.h>
#include <dsplink.h>
#include <errbase.h>

/*  ----------------------------------- Application Header            */
#include <system_os.h>
#include <multDSP.h>


#if defined (__cplusplus)
extern "C"
{
#endif /* defined (__cplusplus) */

#define MAXSIZE 128*128
	
	Uint32 mat1[MAXSIZE], mat2[MAXSIZE], prod_gpp[MAXSIZE], prod_dsp[MAXSIZE];

	void matMult(Uint8 Size, Uint32 *mat1, Uint32 *mat2, Uint32 *prod);


    /** ============================================================================
     *  @func   main
     *
     *  @desc   Entry point for the application
     *
     *  @modif  None
     *  ============================================================================
     */
    int main (int argc, char** argv)
    {
        Char8* dspExecutable = NULL;
        Char8* strMatSize = NULL;
        Uint8 i, j, Size;
        Char8* strProcessorId = NULL;
        Uint8 processorId = 0;
        
        int match = 1;
        


        if ((argc != 4) && (argc!=3))
        {
            SYSTEM_1Print("Usage : %s <absolute path of DSP executable> <matrix size> <DSP Processor Id>\n"
                          "\n\t use value of 0  if sample needs to be run on DSP 0 "
                          "\n\t use value of 1  if sample needs to be run on DSP 1"
                          "\n\t For single DSP configuration this is optional argument\n",
                          (int) argv[0]);
        }

        else
        {
            dspExecutable = argv[1];
            strMatSize = argv[2];
            Size = atoi(strMatSize);

            if (argc == 3)
            {
                strProcessorId = "0";
                processorId = 0;
            }
            else
            {
                strProcessorId = argv[3];
                processorId = atoi(argv[3]);
            }

            if (processorId < MAX_PROCESSORS)
            {
				for (j = 0;j < Size; j++)
				{
					for (i = 0; i < Size; i++)
					{
						mat1[i+j*Size] = i+j*2;
					}
				}
	
				for(j = 0; j < Size; j++)
				{
					for (i = 0; i < Size; i++)
					{
						mat2[i+j*Size] = i+j*3;
						//mat2[i+j*Size] = j+i*3;
					}
				}
				

				matMult(Size,mat1,mat2,prod_gpp);



				
                multDSP_Main(dspExecutable, strMatSize, mat1, mat2, prod_dsp, strProcessorId);
                
                            
                match = 1;
                for(j = 0; j < Size; j++)
				{
					for (i = 0; i < Size; i++)
					{
						//printf("%ld %ld   ", prod_gpp[i+j*Size], prod_dsp[i+j*Size]);
						if (prod_gpp[i+j*Size] != prod_dsp[i+j*Size]) match = 0;
					}
					//printf("\n");
				}
				
				//printf("\n%ld %ld\n", prod_gpp[10], prod_dsp[10]);
				
				if (match) printf("Let's party!!!!!!!!!!!!\n");
				else printf("Let's cry\n");
            }
        }

        return 0;
    }
    
void matMult(Uint8 Size, Uint32 *mat1, Uint32 *mat2, Uint32 *prod)
{
	Uint8 i, j, k;
	for (j = 0;j < Size; j++)
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


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
