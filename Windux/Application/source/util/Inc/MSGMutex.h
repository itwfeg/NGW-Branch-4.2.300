// Header file included by ANTLR UniLexer



#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif


#ifndef Util_MSGMutex_h
#define Util_MSGMutex_h

/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Concrete class
		


PURPOSE:		A class descended from the base resource class which ensures mutal exclusion.


FUNCTION:		see above


INTERFACES:		"See 'INTERFACES:' in the module declaration below"
	

RESOURCES:	

REFERENCES:		


PROCESSING:		See Above


DATA:			See 'DATA:' in the class header below.

****  END MODULE HEADER   ***
**************************************************************************/

#include "SmartHandle.h"
#include "DADFResourceUtils.h"

namespace Util
{
	class CMSGMutex : public Util::CSmartHandle
	{
	public:
		// Constructor
		//  Parameters
		//	TYPE			Name				Purpose
		//	std::string		i_Name				The name of the mutex to be created.
//		CMSGMutex(const std::wstring& i_Name = (std::wstring) _T(""))
		CMSGMutex( const wchar_t * ip_Name = L"")
		{
#ifdef LINUX_FIX
			m_Handle = PAL_absclass::PAL_createMutex(0, false, ip_Name);
			Assert(m_Handle != NULL,CPlatformException());
#else
			m_Handle = ::CreateMutex(0,false,ip_Name);
			Assert(m_Handle != NULL,CNTException());
#endif
		}

		// Constructor
		//  Parameters
		//	i_SmartHandle			Mutex to be copied
		CMSGMutex(Util::CSmartHandle& i_SmartHandle) :
		  Util::CSmartHandle(i_SmartHandle){}

		 // Constructor
		//  Parameters
		//	i_MutexHandle			Mutex to be copied
		CMSGMutex(Util::CMSGMutex& i_MutexHandle) :
		  Util::CSmartHandle((Util::CSmartHandle&)i_MutexHandle){}

		// destructor
		virtual ~CMSGMutex(){}

		// Lock the mutex
		//  Parameters
		//	TYPE			Name				Purpose
		//	DWORD			i_Timeout			The amount of time to wait for the object before 
		//										returning
		DWORD Lock(const DWORD i_Timeout = INFINITE)
#ifdef LINUX_FIX
		{return PAL_absclass::PAL_waitForSingleObj(*this, i_Timeout);}
#else
		{return ::WaitForSingleObject(*this,i_Timeout);}
#endif
		
		// Release the mutex
		BOOL Unlock()
#ifdef LINUX_FIX
		{return PAL_absclass::PAL_releaseMutex(m_Handle);}
#else
		{return ::ReleaseMutex(m_Handle);}
#endif
	};
} // end of the namespace


/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Concrete class
		


PURPOSE:		A Class to easily lock an instantiated mutex and then release it when the claim
				goes out of scope.


FUNCTION:		see above


INTERFACES:		"See 'INTERFACES:' in the module declaration below"
	

RESOURCES:	

REFERENCES:		


PROCESSING:		See Above


DATA:			See 'DATA:' in the class header below.

****  END MODULE HEADER   ***
**************************************************************************/
namespace Util
{
	class CMSGMutexClaim
	{
	public:
		// Constructor
		//  Parameters
		//	TYPE				Name				Purpose
		//	Util::CMSGMutex		o					The mutex to claim
		//	bool				i_Lock				Should we lock this object ?...
		CMSGMutexClaim(Util::CMSGMutex& o,bool i_lock = true)
				: m_Object(o)
			{if (i_lock) m_Object.Lock();}
		
		// Destructor - also unlock the object
		virtual ~CMSGMutexClaim()
			{m_Object.Unlock();}
	private:
		// The locally stored reference to the Util::CMSGMutex object
		Util::CMSGMutex& m_Object;

		// Prevent assignment operator
		CMSGMutexClaim& operator = (const CMSGMutexClaim& rhs);
	};
}

#endif

