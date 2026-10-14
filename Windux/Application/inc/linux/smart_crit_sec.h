//
// File : smart_crit_sec.h
//
// Holds generic specific critical section classes used to initialize 
// and use critical section.
//

#ifndef _SMART_CRIT_SEC_H_
#define _SMART_CRIT_SEC_H_

#include "PAL_platformheader.h"

#include <mutex>

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
		std::recursive_mutex m_CritSec;

	public:

		// Description
		//              Constructor
		CSmartCriticalSection()
		{
#ifdef MON_CRITSEC
			PAL_absclass::PAL_InterlockedIncrement( (LONG *)&ms_Stamp);
			m_Stamp = ms_Stamp;
#endif
		}

		// Description
		//              Destructor
		~CSmartCriticalSection()
		{
		}

		// Do not use this function, unless you know what you are doing.
		std::recursive_mutex & GetCritSec(){ return m_CritSec;}

		// Description
		//              Enter the critical section
		void Enter()
		{
#ifdef MON_CRITSEC
			CCritSecMonitor::Instance()->BeforeEnter(m_Stamp);
#endif

			m_CritSec.lock();

#ifdef MON_CRITSEC
			CCritSecMonitor::Instance()->Enter(m_Stamp);
#endif
		}

		// Description
		//              Leave the critical section
		void Leave()
		{
			m_CritSec.unlock();

#ifdef MON_CRITSEC
			CCritSecMonitor::Instance()->Leave(m_Stamp);
#endif
		}

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

#endif //_SMART_CRIT_SEC_H_
