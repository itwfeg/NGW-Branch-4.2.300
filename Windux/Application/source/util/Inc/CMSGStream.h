// Header file included by ANTLR UniLexer


#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif

#ifndef _CMSG_STREAM_HEADER_
#define _CMSG_STREAM_HEADER_

/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Base Class
		
PURPOSE:		This is the base streaming class
				
				It can be used as a message stream, in which case the deque is 
				used to be have better memory management of potentially complex T classes.


FUNCTION:		To allow us the concept of streaming complex objects onto a data storage 
				area using the >> << operators


INTERFACES:		The class will be constructed.
				
				the << and >> operators can then be called.
				
				the methods push() and pop() also carry out the same functionality

				The Length and Clear methods can be used at any time.
	

RESOURCES:		Heap - potentially  > 2K

REFERENCES:		


PROCESSING:		See Above


DATA:			See 'DATA:' in the functions below.

****  END MODULE HEADER   ***
**************************************************************************/

#ifdef PAL_WIN32_RUN 
#ifdef XTRA_COM_INFO
	#pragma message( "Compiling " __FILE__ ) 
#endif
#endif//PAL_WIN32_RUN

#include "PAL_platformheader.h"

#ifdef PAL_WIN32_RUN
#pragma warning(disable: 4786)
#endif


#include <deque>
#include <iostream>






namespace Util
{
// MSVC2010
#if ( defined (_MSC_VER) && (_MSC_VER < 1600) ) || ( defined( __linux__ ) )
	template <class T, class Store> class CMSGStream; 
	template<typename T, class Store> 
	bool operator== (Util::CMSGStream<T,Store> &i_1, Util::CMSGStream<T,Store> &i_2);
#endif


	template <class T, class Store = std::deque<T> >
	class CMSGStream 
	{
	public:
		Store m_DataStore;    //HZ May31,2005 A temporary workaround -- make it public so as to avoid the friend operator from being called.
	protected:
	public:
		enum E_CMSGSTREAM_ENUMS 
		{
			E_STREAM_DEFAULT_INITIAL_SIZE = 1000
		};

		// define some types which can be used.....
#if _MSC_VER > 1100 && _MSC_VER < 1300 //for VS6 C++
		typedef Store::iterator iterator_type ;
		typedef Store::value_type value_type ;
#else
		typedef typename Store::iterator iterator_type ;
		typedef typename Store::value_type value_type ;
#endif
		// Description
		//		Constructor
		// Arguments
		//
		// Returns
		//
		CMSGStream(){}

		// Description
		//		Destructor
		virtual ~CMSGStream();

		// Description
		//		operator <<
		// Parameter
		//		i_T				The T to place onto the stream
		// Return
		//		CMSGStream<T,Store>&	The modified current object with the T now on it
		virtual Util::CMSGStream<T,Store>& operator << (const T& i_T);

		// Description
		//		operator <<
		// Parameter
		//		o_T	- The T to place the head of the stream onto
		// Return
		//		Util::CMSGStream<T,Store>& - The modified current object with the T now removed from it
		virtual  Util::CMSGStream<T,Store>& operator >> (T& o_T);
		

		// Description
		//		operator <<
		// Parameter
		//		i_T				The T to place onto the stream
		// Return
		//
		virtual void push(const T& i_T);
		
		// Description
		//		pop something off the stream
		// Parameter
		//		o_T				The T to place the head of the stream onto
		// Return
		//		int - did the operation work
		virtual int pop(T& o_T);
		
		
		// Description
		//		make this stream = to another.
		// Parameter
		//		i_Stream		The incoming stream
		//	return
		//		CMSGStream<T,Store>&	The modified outgoing stream
		virtual Util::CMSGStream<T,Store>& operator = (CMSGStream<T,Store>& i_Stream);

		// Description
		//		The length of the stream
		//	Arguments
		//
		// Returns
		//		DWORD - The length of the stream
		DWORD Length();

		// Description
		//		Clear the stream
		// Arguments
		//
		// Returns
		//
		virtual void Clear();

// MSVC2010
#if ( defined (_MSC_VER) && (_MSC_VER < 1600) ) || ( defined( __linux__ ) )

		// Description
		//		Equivalence Operator
		// Arguments
		//
		// Returns
		//
//		friend bool operator == (Util::CMSGStream<T,Store>& i_1, 
		friend bool operator == <>(Util::CMSGStream<T,Store>& i_1, 
								 Util::CMSGStream<T,Store>& i_2);
		//HZ Jun01,2005 Explicity provide the template in friend method
#endif
	};
} // end of the namespace

#include "CMSGStream_impl.h"
#endif

