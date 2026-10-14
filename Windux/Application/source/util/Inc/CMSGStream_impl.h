// Header file included by ANTLR UniLexer

#ifndef REM_LINUX_ERR
#include <tchar.h>
#endif


#ifndef _CMSG_STREAM_IMPL_HEADER_
#define _CMSG_STREAM_IMPL_HEADER_

#include "ErrorHandling.h"

///////
// Implementation of the methods for the classes in the headers.
//////
namespace Util
{
	template <class T, class Store>
	inline void CMSGStream<T,Store>::push(const T& i_T) 
	{
		m_DataStore.push_back(i_T);
	}

	template <class T, class Store>
	inline int CMSGStream<T,Store>::pop(T& o_T)
	{
		if (m_DataStore.size()  < 1)
		{
			return 0;
		}
		else
		{
			o_T = m_DataStore.front();
			m_DataStore.pop_front();
			return 1;
		}
	}

	template <class T, class Store>
	inline CMSGStream<T,Store>& CMSGStream<T,Store>::operator = (CMSGStream<T,Store>& i_Stream)
	{
		m_DataStore = i_Stream.m_DataStore;
		return *this;
	}

	template <class T, class Store>
	inline void CMSGStream<T,Store>::Clear()
	{
		m_DataStore.erase(m_DataStore.begin(),m_DataStore.end());
	}

	template <class T, class Store>
	inline DWORD CMSGStream<T,Store>::Length() 
	{ 
		return m_DataStore.size();
	}

	template <class T, class Store>
	inline CMSGStream<T,Store>& CMSGStream<T,Store>::operator >> (T& o_T)
	{
		// Pop off the last elements asserting a true result
		// otherwise throw an exception
		Assert(pop(o_T), Util::CBadObject());
		return *this;
	}

	template <class T, class Store>
		inline CMSGStream<T,Store>& CMSGStream<T,Store>::operator << (const T& i_T) 
	{
		push(i_T);
		return  *this;
	}


	
	template <class T, class Store>
	inline bool operator == (CMSGStream<T,Store>& i_1, CMSGStream<T,Store>& i_2)
	{
		return (i_1.m_DataStore == i_2.m_DataStore);
	}

	template <class T, class Store>
	inline CMSGStream<T,Store>::~CMSGStream()
	{
		Clear();
	}


} // end of the namespace

#endif

