// Header file included by ANTLR UniLexer

#ifdef PAL_WIN32_RUN
#pragma warning( disable : 4297 )
#include <tchar.h>
#endif

#include "../Debug.h"

#ifndef SmartHandle_h
#define SmartHandle_h

/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:                   [Base Class]
PURPOSE:                A base class from which all other classes are descended.
FUNCTION:               Deals with the deallocation of the HANDLE of a resource in a safe manner.
INTERFACES:             "See 'INTERFACES:' in the module declaration below"
RESOURCES:

REFERENCES:
PROCESSING:             See Above
DATA:                   See 'DATA:' in the class header below.

****  END MODULE HEADER   ***
**************************************************************************/
#include"PAL_platformheader.h"
#include "ErrorHandling.h"

#pragma warning( disable : 4297 )

namespace Util
{
    class CSmartHandle
    {
        private:

        protected:
            // The handle to the resource
            HANDLE m_Handle;
        public:

            // Description:
            //          Constructors
            CSmartHandle()
            {m_Handle = NULL;}

            // Description:
            //          Copy Constructor
            // Arguments
            //          CSmartHandle the handle to copy
            CSmartHandle(const CSmartHandle& i_Handle)
            {
                Assert(i_Handle != NULL,Util::CBadObject());

                // Duplicate the handle assume local process for now.
                BOOL l_RetVal = PAL_absclass::PAL_dupHandle(PAL_absclass::PAL_getCurrProc(),
                        i_Handle.m_Handle,
                        PAL_absclass::PAL_getCurrProc(),
                        &m_Handle,
                        0,
                        TRUE,
                        DUPLICATE_SAME_ACCESS);

                Assert(l_RetVal,Util::CPlatformException());
            }

            // Description:
            //          Copy Constructor
            // Arguments
            //          HANDLE the handle to copy
            CSmartHandle(HANDLE i_Handle) :
                m_Handle(i_Handle) {}

            // Description:
            //          destructor
            virtual ~CSmartHandle()
            {
                try
                {
                    if (NULL!=m_Handle) PAL_absclass::PAL_closeHandle(m_Handle);
                }
                catch(...)
                {
			        Debug::trace("CSmartHandle.error", "~CSmartHandle():  catch (...)");
                    LogCatchAndThrow;
                }
            }

            // Description
            //          Get the handle within the class
            // Return
            // HANDLE&          The reference to the handle contained within the class.
            HANDLE get() const
            {return m_Handle;}



            // Description
            //          Return a safe copy of the handle
            // Arguments
            // Returns
            //          CSmartHandle
            CSmartHandle Duplicate(HANDLE i_DestinationProcessHandle = PAL_absclass::PAL_getCurrProc())
            {
                HANDLE l_Handle;

                BOOL l_RetVal = PAL_absclass::PAL_dupHandle(PAL_absclass::PAL_getCurrProc(),
                        m_Handle,
                        i_DestinationProcessHandle,
                        &l_Handle,
                        0,
                        TRUE,
                        DUPLICATE_SAME_ACCESS);

                Assert(l_RetVal != FALSE,Util::CPlatformException());
                return (CSmartHandle) l_Handle;
            }

            // Description
            //              Get the handle within the class
            // Return
            // HANDLE&              The reference to the handle contained within the class.
            HANDLE GetHandle()
            {return m_Handle;}

            // Description
            //              Get the handle within the class
            // Return
            // HANDLE&              The reference to the handle contained within the class.
            operator HANDLE ()
            {return m_Handle;}

            HANDLE Release()
            {
                HANDLE oldHandle = m_Handle;
                m_Handle = NULL;
                return oldHandle;
            }

            // Description
            //              Absorb a handle
            // Argument
            //              rhs             the handle to absorb
            CSmartHandle& operator = (HANDLE rhs)
            {
                if (m_Handle != NULL)
                {
                    PAL_absclass::PAL_closeHandle(m_Handle);
                }
                m_Handle = rhs;
                return *this;
            }

            // Description
            //              Absorb a smart handle
            // Argument
            //              rhs             the handle to absorb
            CSmartHandle& operator = (const CSmartHandle& rhs)
            {
                if (this != &rhs)
                {
                    if (m_Handle != NULL)
                    {
                        PAL_absclass::PAL_closeHandle(m_Handle);
                    }
                    // Need to do this to get rid of the const-ness.
                    CSmartHandle& l_TempHandle = const_cast<CSmartHandle&>(rhs);

                    m_Handle = l_TempHandle.Release();

                }
                return *this;
            }

            // Description
            //              compare some a handles.
            // Argument
            //              i_Handle the handle to compare with
            bool operator == (HANDLE i_Handle) const
            {return m_Handle == i_Handle;}

            // Description
            //              compare some a handles.
            // Argument
            //              i_Handle the handle to compare with
            bool operator != (HANDLE i_Handle) const
            {return !(m_Handle == i_Handle);}

            // Description
            //              compare some a handles.
            // Argument
            //              i_Handle the handle to compare with
            bool operator < (const CSmartHandle& i_res) const
            {return !(m_Handle < i_res.m_Handle);}

            // Description
            //              compare some a handles.
            // Argument
            //              i_Handle the handle to compare with
            bool operator > (const CSmartHandle& i_res) const
            {return !(m_Handle > i_res.m_Handle);}

            // Description
            //              compare some a handles.
            // Argument
            //              i_Handle the handle to compare with
            bool operator == (const CSmartHandle& i_res) const
            {return !(m_Handle == i_res.m_Handle);}

            // Description
            //              compare some a handles.
            // Argument
            //              i_Handle the handle to compare with
            bool operator != (const CSmartHandle& i_res) const
            {return !(m_Handle == i_res.m_Handle);}
        };
} // end of the namespace

#endif

