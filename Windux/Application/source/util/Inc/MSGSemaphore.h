// Header file included by ANTLR UniLexer


#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif

#ifndef MSGSemaphore_h
#define MSGSemaphore_h

/**************************************************************************
 ****  DADF MODULE HEADER   ***


TYPE:                   Concrete class



PURPOSE:                A semaphore class.


FUNCTION:               see above


INTERFACES:             "See 'INTERFACES:' in the module declaration below"


RESOURCES:      

REFERENCES:             


PROCESSING:             See Above


DATA:                   See 'DATA:' in the class header below.

 ****  END MODULE HEADER   ***
 **************************************************************************/

#ifdef XTRA_COM_INFO
#pragma message( "Compiling " __FILE__ ) 
#endif

#include "DADFResourceUtils.h"
#if defined(LINUX_FIX)
#include <limits.h>
#endif

namespace Util
{
    class CMSGSemaphore : public Util::CSmartHandle
    {
        public:
            // Constructor
            //  Parameters              
            // i_SmartHandle    The smarthandle to duplicate
            CMSGSemaphore(Util::CSmartHandle& i_SmartHandle) :
                Util::CSmartHandle(i_SmartHandle){}

            // Constructor
            //  Parameters
            //  iSemaphoreHandle        The semaphore to duplicate
            CMSGSemaphore(Util::CMSGSemaphore& i_SemaphoreHandle) :
                Util::CSmartHandle((Util::CSmartHandle&)i_SemaphoreHandle){}

            // Constructor
            //  Parameters
            //  TYPE                            Name                            Purpose
            //  DWORD                           i_InitialCount          The initial count of the mutex
            //  DWORD                           i_MaximumCount          The maximum count of the mutex
            //  std::string                     i_Name                          The name of the semaphore(shared across processes)

            CMSGSemaphore(const LONG i_InitialCount = 0, 
#ifdef LINUX_FIX
                    const LONG i_MaximumCount = PAL_SEM_MAX_VAL, 
#else
                    const LONG i_MaximumCount = LONG_MAX, 
#endif
                    const wchar_t * ip_Name = L"")
                //                                        const std::wstring& i_Name = (std::wstring)_T(""))
            {
                // Make sure that the counts are valid.
                Assert(i_InitialCount >= 0, Util::CBadObject());
                Assert(i_MaximumCount >= 0, Util::CBadObject());

#ifdef LINUX_FIX
                m_Handle = PAL_absclass::PAL_createSemaphore(ip_Name, i_InitialCount,
                        i_MaximumCount, NULL);
                Assert(m_Handle != NULL,CPlatformException());
#else
                m_Handle = ::CreateSemaphore(NULL,i_InitialCount,i_MaximumCount,ip_Name);
                Assert(m_Handle != NULL,CNTException());
#endif
            }

            // Destructor
            virtual ~CMSGSemaphore(){}

            // Decrement the semaphore
            //  Parameters
            //  TYPE                            Name                            Purpose
            //  DWORD                           i_Timeout                       The timeout to wait for the semaphore if its
            //                                                                                  count is 0
            DWORD Dec(DWORD i_Timeout = INFINITE)
            {
                return Util::MSGWaitForSingleObject(*this,i_Timeout);
            }

            // Increment the semaphore
            //  Parameters
            //  TYPE                            Name                            Purpose
            //  long                            i_ReleaseCount          The count which should be released - defaulted to 1.
            BOOL Inc(long i_ReleaseCount = 1)
            {
                Assert(i_ReleaseCount >=0, Util::CParamException());
#ifdef LINUX_FIX
                return PAL_absclass::PAL_incSemaphore(m_Handle, i_ReleaseCount, NULL);
#else
                return ::ReleaseSemaphore(m_Handle,i_ReleaseCount,NULL);
#endif
            }
#ifdef LINUX_FIX
            // Copy constructor
            // Parameters
            //  Type                            Name                            Purpose
            //  CMSGSemaphore                   sem                             CMSGSemaphore object to copy                            
            /*************
            //          CMSGSemaphore(CMSGSemaphore &sem):
            //                          Util::CSmartHandle(sem.m_Handle){}
            CMSGSemaphore &operator=(HANDLE sem)
            {
            //
            //take care of self assignment
            //
            //                  if (&sem != this)
            m_Handle = sem; 
            return(*this);
            }
             *************/
#endif
    };
} // end of the namespace

/**************************************************************************
 ****  DADF MODULE HEADER   ***


TYPE:                   Concrete class


PURPOSE:                A semaphore claim class


FUNCTION:               see above


INTERFACES:             "See 'INTERFACES:' in the module declaration below"


RESOURCES:      

REFERENCES:             


PROCESSING:             See Above


DATA:                   See 'DATA:' in the class header below.

 ****  END MODULE HEADER   ***
 **************************************************************************/
namespace Util
{
    class CMSGSemaphoreClaim
    {
        public:
            // Constructor
            //  Parameters
            //  TYPE                            Name                            Purpose
            //  Util::CMSGSemaphore     o                                       The semaphore to be claimed
            //  bool                            i_lock                          Should be lock this object
            CMSGSemaphoreClaim(Util::CMSGSemaphore& o,bool i_lock = true) : m_Object(o) 
        {if (i_lock) m_Object.Dec();}

            // Destructor 
            // also releases the semaphore
            virtual ~CMSGSemaphoreClaim()
            {m_Object.Inc();}
        private:
            // A reference to the semaphore object
            Util::CMSGSemaphore& m_Object;

            // Prevent assignment operator
            CMSGSemaphoreClaim& operator = (const CMSGSemaphoreClaim& rhs);
    };
} // end of the namespace

#endif

