/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EVOLUTION_H
#define EVOLUTION_H


#ifdef PAL_WIN32_RUN
#pragma warning(disable : 4996)
#endif


#include "PAL_platformheader.h"


#define HobRecordNum long

#define HobErrorCode int

#define HobPrice  long
#define HobValue  long
#define HobWeight long
#define HobValue64 long long


/* For RWTimer, make sure that if you ever do a timer.reset(), ALWAYS do a
 * timer.start() afterwards.  If you do a timer.stop() after a timer.reset()
 * without the timer.start(), the timer's start-time will be zero, but the
 * timer's stop-time will be the amount of time that has passes since the
 * timer was started the very first time.  THerefore, the timer.elapsedTime()
 * will return a larger number than expected, since the timer's start-time
 * was not set back up after the call to timer.reset().  lap
 */
/* This was removed from this include file, and is now only included in those
 * classes (source) that uses RWTimers (Session, Messenger). 1/22/02 lap
 */
//#define HobTimer              RWTimer
#define OK 0
#define ERROR_CODE (-1)
typedef int             STATUS;
typedef unsigned long USBD_NODE_ID;
#define INET_ADDR_LEN   18

#define UNKNOWN_EVENT (-1)

#define BrowserFormatSeparator ","
#define UnitSeparator   "\x1F"
#define Sub             "\x1A"

#define RECORD_CORRUPT   -2
#define RECORD_NOT_FOUND -1
#define RECORD_OK         0

#define UNIT_PRICING      1


typedef enum
{
        InvalidLanguage,

        PrimaryLanguage,
        SecondaryLanguage,

        LastLanguage
} LanguageSelection;


//#define RegisterFirstAsPrimary        0                       //      Temporary (CCD). See Larry for explanation at x3221.
#define NeedForTestBrowser              1                       //      Temporary (CCD). Only needed when running with the TestBrowser
//#define JOES_NULL_STRING_PROBLEM                              //      Temporary (CCD) until Joe can handle NULL strings on Browser side.

#define returnToLater   1       //      Temporary (CCD). Stuff I was interrupted on and don't want to lose changes for.
                                //      I will get back to this code when I am done working on the stuff I was switched to.

#include "Debug.h"


#define FileExtensionSeparator "."


#endif //EVOLUTION_H
