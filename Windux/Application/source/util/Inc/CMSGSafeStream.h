// Header file included by ANTLR UniLexer

#ifndef REM_LINUX_ERR 
#include <tchar.h>
#endif


#ifndef _CMSG_SAFE_STREAM_HEADER_
#define _CMSG_SAFE_STREAM_HEADER_

/**************************************************************************
****  DADF MODULE HEADER   ***

TYPE:			Derived Class
					
PURPOSE:		Takes base stream from Util:CMSGStream and adds the functionality 
				to allow the safe streaming of objects by using a mutex

FUNCTION:		To allow a generic template streaming class that can be used with 
				multiple threads.


INTERFACES:		The class will be created.

				The push and pop methods can then be used to add and remove messages
				onto the stream.		

RESOURCES:		Heap - potentially  > 2K

REFERENCES:		


PROCESSING:		See Above


DATA:			See 'DATA:' in the functions below.

****  END MODULE HEADER   ***
**************************************************************************/

#ifdef XTRA_COM_INFO
	#pragma message( "Compiling " __FILE__ ) 
#endif

#include "CMSGStream.h"
#include "CCriticalSection.h"
#include "MSGMutex.h"

namespace Util
{
	template <class T, class Store = std::deque<T> >
	class CMSGSafeStream : 	public  Util::CMSGStream<T,Store>
	{
	public:

		// Description
		//		Constructor and Destructor
		// Parameters
		//
		// return
		//
		CMSGSafeStream (){}
		virtual ~CMSGSafeStream (){}	

		// Description
		//		Pop off a T.
		// Parameters
		//		o_T				The T to pop off into
		// return
		//		CMSGStream<T,Store>			The modified popped-off stream
		virtual int pop(T& o_T)
			{Util::CLockCriticalSection lock(m_CS); return CMSGStream<T,Store>::pop(o_T);}

		// Push on a T.
		// Parameters
		//		i_T				The T to push in
		// return
		//		CMSGStream<T,Store>		The modified pushed-in stream
		virtual void  push( const T& i_T)
			{Util::CLockCriticalSection lock(m_CS); CMSGStream<T,Store>::push(i_T);}
	
		// Description
		//		Clear the stream of data
		// Parameters
		//
		// return
		//
		virtual void Clear()
			{Util::CLockCriticalSection lock(m_CS); CMSGStream<T,Store>::Clear();}

	protected:
		//Util::CMSGMutex m_Mutex;
		mutable Util::CSmartCriticalSection	m_CS;

	};

} // end of the namespace

#endif

