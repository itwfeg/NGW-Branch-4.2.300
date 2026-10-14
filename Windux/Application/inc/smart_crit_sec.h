//
//
// File : smart_crit_sec.h
//
// Holds generic specific critical section classes used to initialize 
// and use critical section.
//

#ifndef _SMART_CRIT_SEC_H_
#define _SMART_CRIT_SEC_H_

#include "PAL_platformheader.h"

#error Please use platform specific critical section code!

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
        PAL_CRITICAL_SECTION m_CritSec;
        public:

        // Description
        //              Constructor
        CSmartCriticalSection()
        {
#ifdef MON_CRITSEC
            PAL_absclass::PAL_InterlockedIncrement( (LONG *)&ms_Stamp);
            m_Stamp = ms_Stamp;
#endif

            PAL_absclass::PAL_initializeCriticalSection(&m_CritSec);
			cnt = 0;
        }

        // Description
        //              Destructor
        ~CSmartCriticalSection()
        {
            PAL_absclass::PAL_deleteCriticalSection(m_CritSec);
        }
        // Do not use this function, unless you know what you are doing.
        PAL_CRITICAL_SECTION & GetCritSec(){ return m_CritSec;}
        // Description
        //              Enter the critical section
        void Enter()   {
#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->BeforeEnter(m_Stamp);
#endif
            
			PAL_absclass::PAL_enterCriticalSection(m_CritSec);
			cnt++;

#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->Enter(m_Stamp);
#endif
        }

        // Description
        //              Leave the critical section
        void Leave()
        {         			
			if (0 < cnt)
			{
				--cnt;
				PAL_absclass::PAL_leaveCriticalSection(m_CritSec);
			}

#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->Leave(m_Stamp);
#endif
        }
		private:
		int cnt;
    };

#endif//_SMART_CRIT_SEC_H_

