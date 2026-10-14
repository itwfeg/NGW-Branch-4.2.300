//
// File : smart_crit_sec.h
//
// Holds Win32 specific critical section classes used to initialize 
// and use critical section.
//

#ifndef _SMART_CRIT_SEC_H_
#define _SMART_CRIT_SEC_H_

#if defined( _WIN32 )
	#pragma warning(disable:4702)
#endif

#include "PAL_platformheader.h"

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

			InitializeCriticalSection(&m_CritSec);
			cnt = 0;
		}

		// Description
		//              Destructor
		~CSmartCriticalSection()
		{
			try
			{ 
				DeleteCriticalSection(&m_CritSec);
			}
			catch(...)
			{
				PAL_absclass::PAL_OutputDbgStrA( "~CSmartCriticalSection: exception in DeleteCriticalSection\r\n" );
			}
		}

        // Do not use this function, unless you know what you are doing.
        CRITICAL_SECTION & GetCritSec() {return m_CritSec;}

		// Description
		//              Enter the critical section
		void Enter()   {
#ifdef MON_CRITSEC
			CCritSecMonitor::Instance()->BeforeEnter(m_Stamp);
#endif
           
			EnterCriticalSection(&m_CritSec);
			++cnt;
	
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
	            LeaveCriticalSection(&m_CritSec);
			}
#ifdef MON_CRITSEC
            CCritSecMonitor::Instance()->Leave(m_Stamp);
#endif
		}

		// Description
		//              Enter the critical section
		BOOL TryEnter()
		{
#ifdef MON_CRITSEC
			CCritSecMonitor::Instance()->BeforeEnter(m_Stamp);
#endif
			if (TryEnterCriticalSection(&m_CritSec))
			{
				++cnt;
#ifdef MON_CRITSEC
				CCritSecMonitor::Instance()->Enter(m_Stamp);
#endif
				return TRUE;
			}

			return FALSE;
		}

		private:
			int cnt;

 	};

    class CSmartCriticalSectionGuard
    {
    protected:

    	// DATA:
    	CSmartCriticalSection *m_CritSec;

    public:

    	// Description
		//              Constructor
    	CSmartCriticalSectionGuard(CSmartCriticalSection *cs) :
    		m_CritSec( cs )
		{
    		if ( m_CritSec )
    			m_CritSec->Enter();
		}

		// Description
		//              Destructor
		~CSmartCriticalSectionGuard()
		{
    		if ( m_CritSec )
    			m_CritSec->Leave();
		}

    };

#endif//_SMART_CRIT_SEC_H_

#if defined( _WIN32 )
	#pragma warning(default:4702)
#endif