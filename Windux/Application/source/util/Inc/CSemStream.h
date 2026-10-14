// Header file included by ANTLR UniLexer

#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif

#ifndef _CSEM_STREAM_HEADER_
#define _CSEM_STREAM_HEADER_

/*******************************************************************************

TYPE:                   Derived Class

PURPOSE:                Takes safe stream from Util:CMSGSafeStream and adds the functionality 
to allow the notifiable safe streaming of objects by using a semaphore

FUNCTION:               Allows packets to be streamed to a location and then streamed from a location
with the addition of a semaphore to allow two threads to synchronise their 
reads and writes.

INTERFACES:             The class will intially be constructed.

Generally multiple threads can then read and write classes to the stream 
in a user defined consumer-producer relationship only using the overloaded 
operators >> and <<.

The consumer will also need to call the GetNewMessSemaphore() method to gain 
access to the new message semaphore which will indicate when there is a new 
packet.


RESOURCES:              Heap - potentially  > 2K

REFERENCES:             


PROCESSING:             See Above


DATA:                   See 'DATA:' in the functions below.

 *******************************************************************************/
#ifdef PAL_WIN32_RUN
#pragma warning(disable: 4786)
#endif

#include "CMSGSafeStream.h"
#include "MSGSemaphore.h"
#include "CCriticalSection.h"

#include "Statistic.h"

#ifdef LINUX_FIX
#include "PAL_platformheader.h"
//#include "PAL_typedef.h"
//#include <limits.h>
#endif


namespace Util
{

#ifdef LINUX_FIX
    //template <class T> Util::CSmartCriticalSection<T>;
    //class Util::CMSGSemaphore;
#endif

    template <class T>
        class CSemStream  : public Util::CMSGSafeStream<T>
    {
        public:
            enum e_mode
            {
                BLOCKING,
                PURGING
            } m_mode;


            // Description
            //          Constructor 
            // Returns
            //  
#ifdef LINUX_FIX
            CSemStream(LONG     i_MaxCount = PAL_SEM_MAX_VAL,          // The Max size of the stream.
                    int i_mode = BLOCKING):  
#ifdef PAL_WIN32_RUN
                m_mode((CSemStream<T>::e_mode) i_mode),
#else
                m_mode((typename CSemStream<T>::e_mode)i_mode),
#endif//PAL_WIN32_RUN
                m_Semaphore(0,i_MaxCount,L""), 
                m_InsertSemaphore(i_MaxCount,i_MaxCount,L"")
#else
                    CSemStream(LONG     i_MaxCount = LONG_MAX,          // The Max size of the stream.
                            int i_mode = BLOCKING)
                    :m_Semaphore(0,i_MaxCount,L"")
                     , m_InsertSemaphore(i_MaxCount,i_MaxCount,L"")
                     , m_mode((CSemStream<T>::e_mode)i_mode)
#endif
        {
            m_MaxCount = i_MaxCount;
        }

            // Description
            //          Destructor
            // Returns
            //  
            virtual ~CSemStream(){}

            // Some friendly operators
            // Parameters
            //          i_Q                             The stream to be modified
            //          i_ptr/o_ptr             the T to place/suck from/
            // return
            //          CSemStream<T>   The modified stream
            friend Util::CSemStream<T>& operator << (Util::CSemStream<T>& i_Q, const T& i_ptr)
            {
                i_Q.push(i_ptr);
                if (i_Q.m_mode == CSemStream<T>::BLOCKING)
                {
#if defined(LINUX_FIX)
                    Assert(i_Q.m_InsertSemaphore.Dec() != WAIT_FAILED,Util::CPlatformException());
#else
                    Assert(i_Q.m_InsertSemaphore.Dec() != WAIT_FAILED,Util::CNTException());
#endif
                    Assert(i_Q.m_Semaphore.Inc() == TRUE,
                            Util::CNamedException(
                                _T("The consumer has removed an item from the CSemStream without taking a unit from the new message semaphore")));
                }
                if (i_Q.m_mode == CSemStream<T>::PURGING)
                {
                    if (i_Q.m_InsertSemaphore.Dec(0) == WAIT_TIMEOUT)
                    {
                        T       m_eater;
                        i_Q.pop(m_eater);
                    }
                    else
                    {
#if defined(LINUX_FIX)
                        Assert(i_Q.m_Semaphore.Inc(),Util::CPlatformException());
#else
                        Assert(i_Q.m_Semaphore.Inc(),Util::CNTException());
#endif
                    }
                }
                return i_Q;
            }

            friend Util::CSemStream<T>& operator >> (Util::CSemStream<T>& i_Q, T& o_ptr)
            {
                // Does not dec semaphore assumes that a thread has been waiting on the semaphore which will have been dec 
                // when the continuation of that thread was allowed.
                Assert(i_Q.m_InsertSemaphore.Inc()== TRUE,
                        Util::CNamedException(
                            _T("The consumer has removed more items from the CSemStream than it has taken units from the semaphore") ));
                i_Q.pop(o_ptr);
                return i_Q;
            }

            // Description
            //          Get a reference to the semaphore to allow us to figure out
            //          externally if there is a new message.
            // Returns
            //          Util::CMSGSemaphore - A new semaphore object
            Util::CMSGSemaphore &GetNewMessSemaphore()
            {
                return m_Semaphore;
            }

            // Description
            //          Clear out the current stream
            // Returns
            //          
            virtual void Clear()
            {
                // Lock the stream
                Util::CLockCriticalSection lock(m_CS);

                // Decrement the semaphore and remove the associated item while it it valid to do so.
                DWORD l_Ret = WAIT_OBJECT_0;
                while(l_Ret == WAIT_OBJECT_0)
                {
#ifdef LINUX_FIX
                    l_Ret = PAL_absclass::PAL_waitForSingleObj(m_Semaphore.GetHandle(), 0);
                    Assert(l_Ret!= WAIT_FAILED,Util::CPlatformException());
#else
                    l_Ret = ::WaitForSingleObject(m_Semaphore.GetHandle(),0);
                    Assert(l_Ret!= WAIT_FAILED,Util::CNTException());
#endif
                    if (l_Ret == WAIT_OBJECT_0)
                    {
                        T cleared;
                        *this >> cleared;
                    }
                }
            }

        private:
            // The semaphore to allow blocking 
            Util::CMSGSemaphore m_Semaphore;
            Util::CMSGSemaphore m_InsertSemaphore;

            LONG m_MaxCount;    // Only used for stats

// not sure what this should be
//#ifdef LINUX_FIX
//#ifdef PAL_WIN32_RUN
            mutable Util::CSmartCriticalSection m_CS;
//#else
//            mutable Util::CSmartCriticalSection Util::CMSGSafeStream<T>::m_CS;
//            //#else
//            //          mutable Util::CSmartCriticalSection     m_CS;
//#endif//PAL_WIN32_RUN
//#endif
    };


} // end namespace


#endif

