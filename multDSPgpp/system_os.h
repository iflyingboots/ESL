/** ============================================================================
 *  @file   system_os.h
 *
 *  @path
 *
 *  @desc   OS specific definitions for the helloDSP sample.
 *
 *  @ver    1.10
 *  ============================================================================
 *  Copyright (c) Texas Instruments Incorporated 2002-2009
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied or provided.
 *  ============================================================================
 */


#if !defined (SYSTEM_OS_H)
#define SYSTEM_OS_H


#if defined (__cplusplus)
extern "C"
{
#endif /* defined (__cplusplus) */


    /** ============================================================================
     *  @func   atoi
     *
     *  @desc   Extern declaration for function that converts a string into an
     *          integer value.
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
    extern int atoi(const char* str);


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
    Uint32 SYSTEM_Atoi(Char8* str);


    /** ============================================================================
     *  @func   SYSTEM_Atoll
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
    NORMAL_API Uint32 SYSTEM_Atoll(Char8* str);

    /** ============================================================================
     *  @func   SYSTEM_0Print
     *
     *  @desc   Print a message without any arguments.
     *
     *  @arg    str
     *              String message to be printed.
     *
     *  @ret    None
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_0Print(Char8* str);


    /** ============================================================================
     *  @func   SYSTEM_1Print
     *
     *  @desc   Print a message with one argument.
     *
     *  @arg    str
     *              String message to be printed.
     *  @arg    arg
     *              Argument to be printed.
     *
     *  @ret    None
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_1Print(Char8* str, Uint32 arg);

    /** ============================================================================
     *  @func   SYSTEM_2Print
     *
     *  @desc   Print a message with two arguments.
     *
     *  @arg    str
     *              String message to be printed.
     *  @arg    arg1, arg2
     *              Arguments to be printed.
     *
     *  @ret    None
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_2Print(Char8* str, Uint32 arg1, Uint32 arg2);


    /** ============================================================================
     *  @func   SYSTEM_1Sprint
     *
     *  @desc   Print a formatted message with one argument to a stream.
     *
     *  @arg    str
     *              Target stream.
     *  @arg    format
     *              String message to be printed.
     *  @arg    arg
     *              Argument to be printed.
     *
     *  @ret    None
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_1Sprint(Char8* str, Char8* format, Uint32 arg);

    /** ============================================================================
     *  @func   SYSTEM_2Sprint
     *
     *  @desc   Print a formatted message with two arguments to a stream.
     *
     *  @arg    str
     *              Target stream.
     *  @arg    str
     *              String message to be printed.
     *  @arg    arg1, arg2
     *              Arguments to be printed.
     *
     *  @ret    None
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_2Sprint(Char8* str, Char8* format, Uint32 arg1, Uint32 arg2);

    /** ============================================================================
     *  @func   SYSTEM_Sleep
     *
     *  @desc   Sleeps for the specified number of microseconds.
     *
     *  @arg    uSec
     *              Microseconds to sleep.
     *
     *  @ret    None
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_Sleep(Uint32 uSec);


#if defined (PROFILE)
    /** ============================================================================
     *  @func   SYSTEM_GetStartTime ()
     *
     *  @desc   This function records the start time in microseconds.
     *
     *  @arg    None.
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
    NORMAL_API Void SYSTEM_GetStartTime(Void);


    /** ============================================================================
     *  @func   SYSTEM_GetEndTime ()
     *
     *  @desc   This function records the end time in microseconds.
     *
     *  @arg    None.
     *
     *  @ret    None.
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_GetEndTime(Void);


    /** ============================================================================
     *  @func   SYSTEM_GetProfileInfo
     *
     *  @desc   It converts a time variable to elapsed time structure.
     *
     *  @arg    numIterations
     *             Number of iteratiosn for which SYSTEM sample is excuted.
     *
     *  @enter  None
     *
     *  @leave  None
     *
     *  @see    None
     *  ============================================================================
     */
    NORMAL_API Void SYSTEM_GetProfileInfo(Char8* name);

#endif /* if defined (PROFILE) */

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SYSTEM_OS_H) */
