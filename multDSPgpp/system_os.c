/*  ----------------------------------- OS Specific Headers           */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <system_os.h>

#if defined (__cplusplus)
extern "C"
{
#endif /* defined (__cplusplus) */

#if defined (PROFILE)
    /** ============================================================================
     *  @name   startUsecTime
     *
     *  @desc   This variable stores the value of start time in microseconds.
     *  ============================================================================
     */
    STATIC Uint32 startUsecTime = 0;

    /** ============================================================================
     *  @name   endUsecTime
     *
     *  @desc   This variable stores the value of end time in microseconds.
     *  ============================================================================
     */
    STATIC Uint32 endUsecTime = 0;

    /** ============================================================================
     *  @name   initialUsecTime
     *
     *  @desc   This variable stores the value of reset time in microsecond.
     *  ============================================================================
     */
    STATIC struct timeval initialUsecTime;

#endif /* if defined (PROFILE) */


    /** ============================================================================
     *  @func   SYSTEM_0Print()
     *
     *  @desc   Print a message without any arguments.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_0Print(Char8* str)
    {
        printf(str);
    }


    /** ============================================================================
     *  @func   SYSTEM_1Print()
     *
     *  @desc   Print a message with one argument.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_1Print(Char8* str, Uint32 arg)
    {
        printf(str, arg);
    }

    /** ============================================================================
     *  @func   SYSTEM_2Print()
     *
     *  @desc   Print a message with two arguments.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_2Print(Char8* str, Uint32 arg1, Uint32 arg2)
    {
        printf(str, arg1, arg2);
    }

    /** ============================================================================
     *  @func   SYSTEM_1Sprint()
     *
     *  @desc   Print a formatted message with one argument to a stream.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_1Sprint(Char8* str, Char8* format, Uint32 arg)
    {
        sprintf(str, format, arg);
    }

    /** ============================================================================
     *  @func   SYSTEM_2Sprint()
     *
     *  @desc   Print a formatted message with two arguments to a stream.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_2Sprint(Char8* str, Char8* format, Uint32 arg1, Uint32 arg2)
    {
        sprintf(str, format, arg1, arg2);
    }

    /** ============================================================================
     *  @func   SYSTEM_Sleep()
     *
     *  @desc   Sleeps for the specified number of microseconds.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_Sleep(Uint32 uSec)
    {
        usleep(uSec);
    }

    /** ============================================================================
     *  @func   SYSTEM_Atoi
     *
     *  @desc   This function converts a string into an integer value.
     *
     *  @arg    str
     *              String representation of the number.
     *
     *  @ret    <valid integer>
     *              If the 'initial' part of the string represents a valid integer
     *          0
     *              If the string passed does not represent an integer or is NULL.
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    Uint32 SYSTEM_Atoi(Char8* str)
    {
        return(atoi(str));
    }

    /** ============================================================================
     *  @func   SYSTEM_Atoll()
     *
     *  @desc   Converts ascii to long int
     *
     *  @ret    <valid integer>
     *              If the 'initial' part of the string represents a valid integer
     *          0
     *              If the string passed does not represent an integer or is NULL.
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Uint32 SYSTEM_Atoll(Char8* str)
    {
        Uint32 val = 0;
        val = strtoll(str, NULL, 16);
        return val;
    }


#if defined (PROFILE)
    /** ============================================================================
     *  @func   SYSTEM_ResetUsecTime()
     *
     *  @desc   This function records the reset time in microseconds.
     *
     *  @modif  initialUsecTime
     *  ============================================================================
     */
    STATIC Void SYSTEM_ResetUsecTime(Void)
    {
        DSP_STATUS status = DSP_SOK;
        Int32 osStatus = 0;

        osStatus = gettimeofday(&initialUsecTime, NULL);
        if (osStatus != 0)
        {
            status = DSP_EFAIL;
            SYSTEM_1Print("System call gettimeofday returned error. Status = 0x%x", status);
        }
    }


    /** ============================================================================
     *  @func   SYSTEM_GetStartTime()
     *
     *  @desc   This function records the reset time in microseconds.
     *
     *  @modif  startUsecTime
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_GetStartTime(Void)
    {
        DSP_STATUS status = DSP_SOK;
        Int32 osStatus = 0;
        struct timeval currentTime;

        SYSTEM_ResetUsecTime();

        osStatus = gettimeofday(&currentTime, NULL);
        if (osStatus != 0)
        {
            status = DSP_EFAIL;
            SYSTEM_1Print("System call gettimeofday returned error. Status = 0x%x", status);
        }
        else
        {
            startUsecTime = (((currentTime.tv_sec - initialUsecTime.tv_sec) * 1000000) + (currentTime.tv_usec - initialUsecTime.tv_usec));
        }
    }


    /** ============================================================================
     *  @func   SYSTEM_GetEndTime()
     *
     *  @desc   This function records the reset time in microseconds.
     *
     *  @modif  endUsecTime
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_GetEndTime(Void)
    {
        DSP_STATUS status = DSP_SOK;
        Int32 osStatus = 0;
        struct timeval currentTime;

        osStatus = gettimeofday(&currentTime, NULL);
        if (osStatus != 0)
        {
            status = DSP_EFAIL;
            SYSTEM_1Print("System call gettimeofday returned error. Status = 0x%x", status);
        }
        else
        {
            endUsecTime = (((currentTime.tv_sec - initialUsecTime.tv_sec) * 1000000) + (currentTime.tv_usec - initialUsecTime.tv_usec));
        }
    }

    /** ============================================================================
     *  @func   SYSTEM_GetProfileInfo
     *
     *  @desc   It converts a time variable to elapsed time structure.
     *
     *  @modif  None
     *  ============================================================================
     */
    NORMAL_API
    Void
    SYSTEM_GetProfileInfo(Char8* name)
    {
        Uint32 usecTimeTaken = 0;
        Real32 numSeconds = 0;

        usecTimeTaken = (endUsecTime - startUsecTime);


        /* To calculate number of seconds in usecTimeTaken */
        numSeconds = (Real32)((Real32) usecTimeTaken / 1000000.0);

        SYSTEM_1Print("%d seconds ", numSeconds);
        SYSTEM_1Print("%d microseconds.\n", (Real32)(usecTimeTaken % 1000000));



        return;
    }

#endif /* if defined (PROFILE) */

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
