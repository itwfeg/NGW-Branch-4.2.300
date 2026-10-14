// Header file included by ANTLR UniLexer
#if defined ( _WIN32 )
#include <tchar.h>
#endif
#ifndef CRelativeTimer_h
#define CRelativeTimer_h

/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Concrete Class

PURPOSE:		This is a class for a timer defined by an relative time.

FUNCTION:		The CRelativeTimer class takes a value that contains
				a time span when the timer should get signalled. A SU
				can wait for this to happen and react at a specified
				moment.

INTERFACES:		The class could be either created without value or with
				a time value (either a DWORD expressing the span in
				milliseconds or a CMSGTimeSpan that stores the span in
				units of 100 nanoseconds. The difference is that with a 
				time value the timer starts immediatly while the other
				alternative starts first when the Reset() function is 
				called. The Reset() function takes two parameter: the time
				span, either expressed as milliseconds or as CMSGTimeSpan, 
				and a periodic timer interval (in milliseconds). If this 
				periodic timer interval is 0 (default), the timer is signaled 
				once; if it is greater than 0, the timer is periodic: it 
				automatically reactivates each time the period elapses.
				The periodic timer interval can specified in the constructor
				and defaults to 0 if left out.
				The calling SU has to get the handle for the timer and wait 
				for it with one of the wait functions.

RESOURCES:		DADF Libraries: Util

REFERENCES:		none

PROCESSING:		The processing logic for all functions is 'inline' and hence
				no processing description is provided.

DATA:			See 'DATA:' in the class header below.

****  END MODULE HEADER   ***
**************************************************************************/

#include "BaseNT.h"
#include "ErrorHandling.h"
#include "SmartHandle.h"
#include "MsgTime.h"

namespace Util
{
    class CRelativeTimer
    {
        private:
            //	DATA:
            // the handle where the calling SU must wait on
            Util::CSmartHandle	m_WaitableTimer;

        public:
            //	INTERFACES:
            CRelativeTimer (void)
            {
                try
                {
                    m_WaitableTimer = PAL_absclass::PAL_createWaitableTimer (NULL, FALSE, NULL);
                #if defined(LINUX_FIX)
                    Assert(m_WaitableTimer != NULL, Util::CPlatformException ());
                #else
                    Assert(m_WaitableTimer != NULL, Util::CNTException ());
                #endif
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This constructor creates a timer but does not start it.
            //	Arguments:				none
            //	Returns:				none	
            //	Exceptions:				CNTException

            CRelativeTimer (const CRelativeTimer & UNUSED(i))
            {
                try
                {
                    m_WaitableTimer = PAL_absclass::PAL_createWaitableTimer (NULL, FALSE, NULL);
#if defined(LINUX_FIX)
                    Assert(m_WaitableTimer != NULL, Util::CPlatformException ());
#else
                    Assert(m_WaitableTimer != NULL, Util::CNTException ());
#endif
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This copy constructor creates a timer, but does not start it.
            //	Arguments:				none
            //	Returns:				none	
            //	Exceptions:				CNTException

            CRelativeTimer (const DWORD i_dwMilliseconds, const DWORD i_dwPeriod = 0)
            {
                try
                {
                    m_WaitableTimer = PAL_absclass::PAL_createWaitableTimer (NULL, FALSE, NULL);
                #if defined(LINUX_FIX)
                    Assert(m_WaitableTimer != NULL, Util::CPlatformException ());
                #else
                    Assert(m_WaitableTimer != NULL, Util::CNTException ());
                #endif

                    Reset (i_dwMilliseconds, i_dwPeriod);
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This constructor creates a timer and starts it with the given
            //		time value and a given periodic timer interval.
            //	Arguments:
            //		i_dwMilliseconds	time span after the timer gets signalled
            //		idwPeriod			period after the timer gets signalled again (if != 0)
            //	Returns:				none	
            //	Exceptions:				CNTException

            CRelativeTimer (const Util::CMSGTimeSpan &i_timeSpan, const DWORD i_dwPeriod = 0)
            {
                try
                {
                    m_WaitableTimer = PAL_absclass::PAL_createWaitableTimer (NULL, FALSE, NULL);
                #if defined(LINUX_FIX)
                    Assert(m_WaitableTimer != NULL, Util::CPlatformException ());
                #else
                    Assert(m_WaitableTimer != NULL, Util::CNTException ());
                #endif

                    Reset (i_timeSpan, i_dwPeriod);
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This constructor creates a timer and starts it with the given
            //		time value and a given periodic timer interval.
            //	Arguments:
            //		i_timeSpan			time span after the timer gets signalled
            //		idwPeriod			period after the timer gets signalled again (if != 0)
            //	Returns:				none	
            //	Exceptions:				CNTException

            virtual ~CRelativeTimer ()
            {
                try
                {
                    PAL_absclass::PAL_cancelWaitableTimer (m_WaitableTimer.GetHandle());
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		In the destructor the timer object gets stopped.
            //	Arguments:				none
            //	Returns:				none	
            //	Exceptions:				CNTException

            bool Reset (const DWORD i_dwMilliseconds, const DWORD i_dwPeriod = 0)
            {
                try
                {
                    DWORD		ret;
                    __int64		iTics = -10000;	// Factor to convert ms to negative 100ns units

                    iTics = iTics * i_dwMilliseconds;

                    ret = PAL_absclass::PAL_setWaitableTimer (m_WaitableTimer.GetHandle(), (LARGE_INTEGER *) &iTics, i_dwPeriod, NULL, NULL, FALSE);
                #if defined(LINUX_FIX)
                    Assert(ret==TRUE, Util::CPlatformException ());
                #else
                    Assert(ret==TRUE, Util::CNTException ());
                #endif

                    return TRUE;
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This function (re)starts the timer with the given time span and the new
            //		maximum period.
            //	Arguments:
            //		i_dwMilliseconds	time span after the timer gets signalled
            //		idwPeriod			period after the timer gets signalled again (if != 0)
            //	Returns:				none	
            //	Exceptions:				CNTException

            bool Reset (const Util::CMSGTimeSpan &i_timeSpan, const DWORD i_dwPeriod = 0)
            {
                try
                {
                    DWORD		ret;
                    __int64		iTics;

                    iTics = (__int64) -(i_timeSpan.GetTimeSpan());

                    ret = PAL_absclass::PAL_setWaitableTimer (m_WaitableTimer.GetHandle(), (LARGE_INTEGER *) &iTics, i_dwPeriod, NULL, NULL, FALSE);
                #if defined(LINUX_FIX)
                    Assert(ret==TRUE, Util::CPlatformException ());
                #else
                    Assert(ret==TRUE, Util::CNTException ());
                #endif

                    return TRUE;
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This function (re)starts the timer with the given time span and the new
            //		maximum period.
            //	Arguments:
            //		i_timeSpan			time span after the timer gets signalled
            //							(expressed in units of 100 nanoseconds)
            //		idwPeriod			period after the timer gets signalled again (if != 0)
            //	Returns:				none	
            //	Exceptions:				CNTException

            void Cancel ()
            {
                try
                {
                    PAL_absclass::PAL_cancelWaitableTimer (m_WaitableTimer.GetHandle());
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		Aborts the timer function.
            //	Arguments:				none
            //	Returns:				none	
            //	Exceptions:				none

            operator HANDLE () 
            { 
                try
                {
                    return m_WaitableTimer.GetHandle();
                }
                catch(...)
                {
                    LogCatchAndThrow;
                }
            }
            //	Description:			
            //		This operator returns the handle of the private waitable
            //		timer object.
            //	Arguments:				none
            //	Returns:				
            //		m_WaitableTimer		the handle of the waitable timer
            //	Exceptions:				none
    };

} // end namespace Util

#endif
