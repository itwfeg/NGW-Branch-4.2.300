// Header file included by ANTLR UniLexer

#ifdef PAL_WIN32_RUN
#include <tchar.h>
#endif

#ifndef _CMSGVENT_H_
#define _CMSGVENT_H_

/**************************************************************************
 ****  DADF MODULE HEADER   ***


TYPE:                   Concrete class


PURPOSE:                An event notification class


FUNCTION:               see above


INTERFACES:             "See 'INTERFACES:' in the module declaration below"


RESOURCES:

REFERENCES:


PROCESSING:             See Above


DATA:                   See 'DATA:' in the class header below.

 ****  END MODULE HEADER   ***
 **************************************************************************/
#include "DADFResourceUtils.h"


namespace Util
{
    class CMSGEvent : public Util::CSmartHandle
    {
        public:
            // Constructor
            //  Parameters
            //  TYPE                            Name                    Purpose
            //  bool                            i_ManualReset           should the event be auto or manual reset
            //  bool                            i_InitialState          is the initial state signalled or not
            //  std::string                     i_Name                  The name of the event - for shared across
            //                                                                                  Processes.
            CMSGEvent(bool i_ManualReset = true,
                    bool i_InitialState = false,
                    const std::wstring& i_Name = (std::wstring)_T(""))
            {
                m_Handle = PAL_absclass::PAL_createEvent(i_Name.c_str(), i_ManualReset,
                        i_InitialState, NULL);
                Assert(m_Handle != NULL,CPlatformException());
            }

            CMSGEvent(HANDLE i_Handle) : Util::CSmartHandle(i_Handle) {}

            CMSGEvent(const Util::CSmartHandle& i_SmartHandle) :
                Util::CSmartHandle(i_SmartHandle){}

            CMSGEvent(const Util::CMSGEvent& i_EventHandle) :
                Util::CSmartHandle(i_EventHandle){}

            // Destructor
            virtual ~CMSGEvent(){}

            // Wait for the event to occur.
            //  Parameters
            //  TYPE                            Name                            Purpose
            //  DWORD                           i_Timeout                       The timeout to wait for,,,,
            DWORD Wait(DWORD i_Timeout = INFINITE)
            {return Util::MSGWaitForSingleObject(*this,i_Timeout);}

            // Set the event to signalled. - waiting threads will proceed.
            // Return
            //  BOOL   was the operation successful
            BOOL EventSet()
            {
                return PAL_absclass::PAL_setEvent(m_Handle);
            }

            //  Description
            //          What is the status of the current event.
            //          Please note that this is only applicable to manual reset events and
            //          not to auto reset events.
            //  Arguments
            //
            //  Returns
            //          bool    - is the current event set.
            bool IsEventSet()
            {
                // short cut the Util::MSGWaitForMultipleObjects for objects as we will
                // be able to return immediately
                DWORD l_Ret = PAL_absclass::PAL_waitForSingleObj(m_Handle, 0);
                Assert(l_Ret != WAIT_FAILED,Util::CPlatformException());
                return l_Ret == WAIT_OBJECT_0;
            }

            // Reset the event to an unsignalled state. - threads dependent upon this will block.
            // Return
            // BOOL             was the operation successful
            BOOL EventReset()
            {
                return PAL_absclass::PAL_resetEvent(m_Handle);
            }

            // Pulse the event - i.e. release the correct number of waiting variables then reset to the
            // none signalled state
            // Return
            // BOOL             was the operation successful
            BOOL Pulse()
            {
                return PAL_absclass::PAL_pulseEvent(m_Handle);
            }
    };

    // Reset a manual event, do something, then set it back again.
    class CResetEvent
    {
        private:
            Util::CMSGEvent &m_ManualEvent;

            // Prevent assignment operator
            CResetEvent& operator = (const CResetEvent& rhs);

        public:
            CResetEvent (Util::CMSGEvent &i_ManualEvent) :
                m_ManualEvent (i_ManualEvent)
	        {
	            BOOL        ret;
	            ret = m_ManualEvent.EventReset();
	            Assert(ret == TRUE, CPlatformException ());
	        }

            ~CResetEvent ()
            {
                BOOL ret;
                ret = m_ManualEvent.EventSet();
                Assert(ret == TRUE,CPlatformException());
            }
    };
} // end of the namespace

#endif

