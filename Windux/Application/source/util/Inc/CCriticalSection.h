// Header file included by ANTLR UniLexer


#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif

#ifndef _CCRITICALSECTION_H_
#define _CCRITICALSECTION_H_

/**************************************************************************
 ****  DADF MODULE HEADER   ***


TYPE:                   Concrete classes


PURPOSE:                Lock the critical section


FUNCTION:               To provide easy access to the critical section element


INTERFACES:             

RESOURCES:              Negligable


PROCESSING:             The class is constructed. 
When the class falls out of scope it will release the critical 
section element.                

DATA:                   See Below:


LOGIC:          


 ****  END MODULE HEADER   ***
 **************************************************************************/

#include "PAL_platformheader.h"
#include "smart_crit_sec.h"

/*************************************moved to smart_crit_sec.h***************/
//////////////////////////////////
//minhaj
//secion under #if 0 moved to Application/inc
//folder for appropriate platform
//
#if 0
/*** Use this define to enable Critical Section Monitoring ****/
//#define MON_CRITSEC
/*******/
#ifdef MON_CRITSEC

#include "CSmartPtr.h"
namespace Util
{
    class CMemoryMappedBuffer;
}
class CCritSecMonitor
{
    public:
        static CCritSecMonitor * Instance();
        virtual ~CCritSecMonitor();
        void BeforeEnter(const __int32 & iCritSec);

        void Enter( const __int32 & iCritSec);

        void Leave( const __int32 & iCritSec);

        void Message( const char * iMess );

    protected:
        CCritSecMonitor();
        static Util::CSmartPtr< CCritSecMonitor> mp_CritSecMon;
};

#endif



namespace Util
{
    class CSmartCriticalSection
    {
#ifdef MON_CRITSEC
        friend class CCritSecMonitor;
        __int32 m_Stamp;
        static __int32 ms_Stamp;


#endif
        protected:
        // DATA:
        CRITICAL_SECTION m_CritSec;
        public:

        // Description
        //              Constructor
        CSmartCriticalSection()  
        { 
#ifdef MON_CRITSEC
            PAL_absclass::PAL_InterlockedIncrement( (LONG *)&ms_Stamp);
            m_Stamp = ms_Stamp;

#endif

#ifdef PAL_WIN32_RUN
            InitializeCriticalSection(&m_CritSec); 
#endif
        }

        // Description
        //              Destructor
        ~CSmartCriticalSection() 
        { 
#ifdef PAL_WIN32_RUN
            DeleteCriticalSection(&m_CritSec); 
#endif
        }
        // Do not use this function, unless you know what you are doing.
        CRITICAL_SECTION & GetCritSec(){ return m_CritSec;}
        // Description
        //              Enter the critical section
        void Enter()   { 
#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->BeforeEnter(m_Stamp);
#endif
            EnterCriticalSection(     &m_CritSec); 
#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->Enter(m_Stamp);
#endif
        }

        // Description
        //              Leave the critical section
        void Leave()    
        {
            LeaveCriticalSection(     &m_CritSec); 
#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->Leave(m_Stamp);
#endif
        }
    };


#endif//#if 0




    /******************************************end moved to smart_crit_sec.h************************************/

    class CLockCriticalSection
    {
        private:
            // Data
            CSmartCriticalSection&      m_CriticalSection;

            // Prevent assignment operator
            CLockCriticalSection& operator = (const CLockCriticalSection& rhs);
        public:

            // Description
            //          Constructor
            // Arguments
            //          i_CriticalSection               The Critical section object to lock
            CLockCriticalSection (CSmartCriticalSection& i_CriticalSection) : m_CriticalSection (i_CriticalSection)
            {
                m_CriticalSection.Enter();
            }

            // Description
            //          Destructor
            ~CLockCriticalSection ()
            {
                m_CriticalSection.Leave();
            }
    };
} // end namespace Util



#endif
