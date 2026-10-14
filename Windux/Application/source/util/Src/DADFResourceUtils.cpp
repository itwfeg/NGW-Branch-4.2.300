// Header file included by ANTLR UniLexer
#include "PAL_platformheader.h"
#if defined ( WIN32 )
#include <tchar.h>
#include <process.h>
#include <strstream>
#endif

#include <string>

#include "DADFResourceUtils.h"
#include "MSGSemaphore.h"
#include "CTLSThread.h"

#include "MsgTime.h"
#include "Hex.h"

// Implementation of the DADFResourceUtils stuff
namespace Util
{
    DWORD MSGWaitForOneOfMultipleObjects(std::vector<HANDLE>& i_Resources,DWORD i_Timeout)
    {
        DWORD result;
        if (CRunnableClass::ExitEvent() == NULL)
        {
            // being called from a non class thread
            PRECONDITION(i_Resources.size() <= PAL_MAXIMUM_WAIT_OBJECTS);
            result = PAL_absclass::PAL_waitForMultipleObjs((DWORD)i_Resources.size(),(HANDLE*)&(i_Resources.front()), false, i_Timeout);
        }
        else
        {
            try
            {
                // Being called from a CTLSThread
                PRECONDITION(i_Resources.size() < PAL_MAXIMUM_WAIT_OBJECTS);

                // The terminate event must always be the first in the array, otherwise a busy queue would not
                // stop when told to.

                // Create a new vector
                std::vector<HANDLE> l_Resources;
                // Add on the terminate event
                l_Resources.push_back(CRunnableClass::ExitEvent());
                // Add on the rest of the events
                l_Resources.insert(l_Resources.begin() + 1,i_Resources.begin(),i_Resources.end());

                // Wait for the event to be signalled
                result = PAL_absclass::PAL_waitForMultipleObjs((DWORD)l_Resources.size(),(HANDLE*)&(l_Resources.front()), false, i_Timeout);

                if (result == WAIT_OBJECT_0 || result == WAIT_ABANDONED_0)
                {
                    throw CTerminateNow();
                }
                else if ((result > WAIT_OBJECT_0 && result <= WAIT_OBJECT_0 + PAL_MAXIMUM_WAIT_OBJECTS)
                        ||  (result > WAIT_ABANDONED_0 && result <= WAIT_ABANDONED_0 + PAL_MAXIMUM_WAIT_OBJECTS))
                {
                    result --;
                }
                // Otherwise the result is not dependant on the position of the matched object in the queue
            }
            catch(CTerminateNow&)
            {
                throw;
            }
            catch(...)
            {
                LogCatchAndThrow;
            }
        }
        return result;
    }
}


//    #include <iostream>
//    #include <sstream>
//    #include <string>

namespace Util
{
    //extern void OutputDebugString__(std::string& s);

    DWORD MSGWaitForSingleObject(HANDLE i_Handle, DWORD i_Timeout )
    {
        DWORD result;
//        DWORD tid = GetCurrentThreadId();
//        ::std::ostringstream s;
//        s << ::std::hex;

        if (CRunnableClass::ExitEvent() != NULL )
        {
            HANDLE handles[2];
            // Put the thread monitor in first
            handles[0] = CRunnableClass::ExitEvent();
            handles[1] = i_Handle;

 //           s.str("");
 //           s << "XXXX wait: waiting. TID=" << tid << ::std::endl;
            //OutputDebugString__(s.str());
            result = PAL_absclass::PAL_waitForMultipleObjs( 2, handles, false, i_Timeout );

            if (result == WAIT_OBJECT_0)
            {
 //               s.str("");
 //               s <<  "XXXX wait: ExitEvent signaled. throwing CTerminateNow. TID=" << tid << ::std::endl;
                //OutputDebugString__(s.str());
                throw CTerminateNow();
            }

            // We are waiting for a single object and it
            // was signalled so the user thinks that
            // we should be returning 0.
            if (result == WAIT_OBJECT_0 +1 || result == WAIT_ABANDONED_0 +1)
            {
 //               s.str("");
 //               s << "XXXX wait: wait satisfied. TID=" << tid << ::std::endl;
                //OutputDebugString__(s.str());
                result --;
            }
        }
        else  // We are being called from a none class thread
        {
 //           s.str("");
 //           s << "XXXX wait: waiting on non-class thread handle. TID=" << tid << ::std::endl;
            //OutputDebugString__(s.str());
            result = PAL_absclass::PAL_waitForSingleObj(i_Handle,i_Timeout );
        }
 //       s.str("");
 //       s << "XXXX wait: returning result= " << result << ", TID=" << tid << ::std::endl;
        //OutputDebugString__(s.str());
        return result;
    }

    DWORD MSGWaitForMultipleObjects(std::vector<HANDLE>& i_Handles,DWORD i_Timeout)
    {
        DWORD result;
        try
        {
            PRECONDITION( i_Handles.size() <= PAL_MAXIMUM_WAIT_OBJECTS );

            if (CRunnableClass::ExitEvent() == NULL )
            {
                // being called from a non class thread

                result = PAL_absclass::PAL_waitForMultipleObjs((DWORD)i_Handles.size(),(HANDLE*)&(i_Handles.front()),true, i_Timeout );
            }
            else
            {
                // This problem is practically impossible to solve, so we'll Poll instead.
                CMSGTime finish = CMSGTime::GetTheCurrentTime() + (CMSGTimeSpan::Millisecond() * i_Timeout);
                CMSGTimeSpan delay = (CMSGTimeSpan::Millisecond() * 10);
                DWORD milliseconds;

                do
                {
                    CMSGTime now = CMSGTime::GetTheCurrentTime();
                    CMSGTimeSpan time_left = finish - now;

                    if (time_left <= delay)
                    {
                        delay = time_left;
                    }

                    milliseconds = static_cast<unsigned long>(delay.GetSpanInMilliseconds());

                    result = PAL_absclass::PAL_waitForMultipleObjs((DWORD)i_Handles.size(),(HANDLE*)&(i_Handles.front()),true, milliseconds );

                    if (result == WAIT_TIMEOUT)
                    {
                        if (WAIT_TIMEOUT != PAL_absclass::PAL_waitForSingleObj( CRunnableClass::ExitEvent(), 0 ))
                            throw CTerminateNow();
                    }
                } while (result == WAIT_TIMEOUT && milliseconds > 0);
            } // End of the else
        } // end of the try block
        catch(Util::CTerminateNow&)
        {
            throw;
        }
        catch(...)
        {
            LogCatchAndThrow ;
        }
        return result;
    } // end of the function.
} // end of the namespace
