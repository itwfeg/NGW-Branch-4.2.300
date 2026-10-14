// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef _CMSG_BYTE_STREAM_HEADER_
#define _CMSG_BYTE_STREAM_HEADER_

/*******************************************************************************

TYPE:			Base Class
					
PURPOSE:		This is the base byte streaming class. 

				
FUNCTION:		There are two derivations of this Class
				Util::CMSGIntByteStream  - For internal communications.
				Util::CMSGExtByteStream	 - For external communications.

				This class is an interface class that allows us to deal 
				with the communication of data and specifically converting 
				data into a serialised form without caring wether or not 
				the data will need to be byte swapped.

INTERFACES:		The class will be initialised
				
				Data will then be read and written to the class using the 
				overloaded >> and << operators 


RESOURCES:		Heap - potentially  > 2K

REFERENCES:		

PROCESSING:		See Above


DATA:			See 'DATA:' in the functions below.

*******************************************************************************/

#pragma warning(disable: 4786)

#include <list>
#include <vector>
#include <deque>

#include "CSmartPtr.h"
#include "MsgTime.h"
#include "CRawByteStream.h"


namespace Util
{


	class CMSGByteStream : public Util::CReadWriteObject
	{

	private:

		enum e_CMSGByteStream
		{
			E_STREAM_DEFAULT_INITIAL_SIZE = 1000
		};

	public: 
 
		// Description:	Constructor
		// Returns:		Nothing.
		CMSGByteStream
		(
			unsigned int i_Size = E_STREAM_DEFAULT_INITIAL_SIZE	// The initial size of the stream.
		)
		{
			m_DataStore = Util::CSmartPtr<Util::CReadWriteObject>(new Util::CRawByteStream(i_Size));
			Assert(m_DataStore.Get(), Util::CCLibException());
		} 

		// Description:	Constructor
		// Returns:		Nothing.
		CMSGByteStream
		(
			Util::CSmartPtr<Util::CReadWriteObject> i_DataSource	// A constructed stream.
		)
		: m_DataStore(i_DataSource)
		{
		}

		// Description:	Constructor
		// Returns:		Nothing.
		CMSGByteStream
		(
			const BYTE*	i_T,		// The array of bytes to initialise the stream with.
			DWORD		i_TotalSize	// The size of the array.
		)
		{
			int l_AmountWritten;
			m_DataStore = Util::CSmartPtr<Util::CReadWriteObject>(new Util::CRawByteStream(i_TotalSize)); 
			Assert(m_DataStore.Get(), Util::CCLibException());
			l_AmountWritten = m_DataStore->Write(i_T,i_TotalSize);
			Assert((DWORD)l_AmountWritten == i_TotalSize,Util::CBadObject());
		} 

		// Description:	Destructor
		// Returns:		Nothing.
		virtual ~CMSGByteStream()
		{
		}

		size_t size() { return ((Util::CRawByteStream *)(m_DataStore.get()))->Length();}	
        const std::basic_string<BYTE>
            & GetByteString() { return ((Util::CRawByteStream *)(m_DataStore.get()))->GetDataStore();}

		// Description:	Some streaming operators which are pure and need
		//				to be defined by sub classes.
		//				For each type there is an insertion operator (<<)
		//				and an extraction operator (>>).
		
		virtual CMSGByteStream& operator << (const bool  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      bool& o_Val) = 0;

		virtual CMSGByteStream& operator << (const std::wstring& i_string) = 0;
		virtual CMSGByteStream& operator >> (      std::wstring& i_String) = 0;

		virtual CMSGByteStream& operator << (const std::string& i_string) = 0;
		virtual CMSGByteStream& operator >> (      std::string& i_String) = 0;

		// Description:	Read from the front of the stream.
		// Returns:		The amount of information read.
		virtual int Read
		(
			BYTE*	o_Buffer,	// The array to Read into.
			int		i_Length	// The amount of information to Read off.
		)
		{
			return m_DataStore->Read(o_Buffer, i_Length);
		}

		// Description:	Peek from the front of the stream.
		// Returns:		The amount of information peeked.
		virtual int Peek
		(
			BYTE*	o_Buffer,	// The array to Read into.
			int		i_Length	// The amount of information to Read off.
		)
		{
			return m_DataStore->Peek(o_Buffer,i_Length);
		}

		// Description:	Write to the back of the stream 
		// Returns:		The amount of information written
		virtual int Write
		(
			const BYTE*	i_Buffer,	// The array to Write.
			int			i_Length	// The amount of information to Write.
		)
		{
			return m_DataStore->Write(i_Buffer,i_Length);
		}

		// Description:	Return a pointer to the start of the data.
		// Returns:		A pointer to the start of the data.
		virtual const BYTE* c_str()
		{
			return m_DataStore->c_str();
		}

		// Description:	Extraction operator.
		// Returns:		The stream with the data extracted from it.
		CMSGByteStream& operator >>
		(
			DWORD& o_Val	// The value to extract.
		)
		{
			unsigned __int32 i_int;
			*this >> i_int;
			o_Val = i_int;
			return *this;
		}

		// Description:	Streaming operators for GSDS Volume F specific data types.
		//				- Insertion operators receive as argument the data to insert,
		//				  and return the stream with the data on it.
		//				- Extraction operators receive as argument a reference to
		//				  where to store the extracted data,
		//				  and return the stream after data extraction.

		virtual CMSGByteStream& operator << (const char  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      char& o_Val) = 0;


#if _MSC_VER > 1100 && _MSC_VER < 1300
		virtual CMSGByteStream& operator << (const __int8  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      __int8& o_Val) = 0;
#endif

		virtual CMSGByteStream& operator << (const unsigned char  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      unsigned char& o_Val) = 0;

#if _MSC_VER > 1100 && _MSC_VER < 1300
		virtual CMSGByteStream& operator << (const unsigned __int8  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      unsigned __int8& o_Val) = 0;
#endif

		virtual CMSGByteStream& operator << (const __int16  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      __int16& o_Val) = 0;

#if _MSC_VER > 1100 && _MSC_VER < 1300
		virtual CMSGByteStream& operator << (const unsigned __int16  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      unsigned __int16& o_Val) = 0;
#endif
		virtual CMSGByteStream& operator << (const unsigned short  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      unsigned short& o_Val) = 0;

		virtual CMSGByteStream& operator << (const __int32  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      __int32& o_Val) = 0;

#if _MSC_VER > 1100 && _MSC_VER < 1300
		virtual CMSGByteStream& operator << (const unsigned long  i_Val) = 0;
//		virtual CMSGByteStream& operator >> (      unsigned long& o_Val) = 0;

		virtual CMSGByteStream& operator << (const unsigned __int32  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      unsigned __int32& o_Val) = 0;
#endif

		virtual CMSGByteStream& operator << (const UINT32  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      UINT32& o_Val) = 0;

		virtual CMSGByteStream& operator << (const __int64  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      __int64& o_Val) = 0;

#if _MSC_VER > 1100 && _MSC_VER < 1300
		virtual CMSGByteStream& operator << (const unsigned __int64  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      unsigned __int64& o_Val) = 0;
#endif

		virtual CMSGByteStream& operator << (const float  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      float& o_Val) = 0;

		virtual CMSGByteStream& operator << (const double  i_Val) = 0;
		virtual CMSGByteStream& operator >> (      double& o_Val) = 0;

		virtual CMSGByteStream& operator << (const Util::CMSGTime& i_Val) = 0;
		virtual CMSGByteStream& operator >> (      Util::CMSGTime& o_Val) = 0;

		virtual CMSGByteStream& operator << (const Util::CMSGTimeSpan& i_Val) = 0;
		virtual CMSGByteStream& operator >> (      Util::CMSGTimeSpan& o_Val) = 0;

	private:

		// The holder of the data.
		Util::CSmartPtr<Util::CReadWriteObject> m_DataStore;

	};

	// Streaming methods for lists
	template<class t>
	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, std::list<t>& i_cont)
	{
		std::list<t>::iterator i;
		o_Stream << (unsigned long) m_cont.size();
		for (i = m_cont.begin() ; i != m_cont.end() ; i++)
		{
			o_Stream << * i;
		}
		return o_Stream;
	}

	template<class t>
	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& o_Stream, std::list<t>& i_cont)
	{
		unsigned long i,noElements;
		o_Stream >> noElements;
		t l_t;
		m_cont.clear();
		for (i = 0; i < noElements ; i++)
		{
			o_Stream >> l_t;
			m_cont.push_back(l_t);
		}
		return o_Stream;
	}


	// Streaming methods for vectors
	template<class t>
	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, std::vector<t>& i_cont)
	{
		std::vector<t>::iterator i;
		o_Stream << (unsigned long) m_cont.size();
		for (i = m_cont.begin() ; i != m_cont.end() ; i++)
		{
			o_Stream << * i;
		}
		return o_Stream;
	}

	template<class t>
	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& o_Stream, std::vector<t>& i_cont)
	{
		unsigned long i,noElements;
		o_Stream >> noElements;
		t l_t;
		m_cont.clear();
		for (i = 0; i < noElements ; i++)
		{
			o_Stream >> l_t;
			m_cont.push_back(l_t);
		}
		return o_Stream;
	}

	
	// Streaming methods for deques
	template<class t>
	Util::CMSGByteStream& operator << (Util::CMSGByteStream& o_Stream, std::deque<t>& i_cont)
	{
		std::deque <t>::iterator i;
		o_Stream << (unsigned long) m_cont.size();
		for (i = m_cont.begin() ; i != m_cont.end() ; i++)
		{
			o_Stream << * i;
		}
		return o_Stream;
	}

	template<class t>
	Util::CMSGByteStream& operator >> (Util::CMSGByteStream& o_Stream, std::deque<t>& i_cont)
	{
		unsigned long i,noElements;
		o_Stream >> noElements;
		t l_t;
		m_cont.clear();
		for (i = 0; i < noElements ; i++)
		{
			o_Stream >> l_t;
			m_cont.push_back(l_t);
		}
		return o_Stream;
	}


} //  end namespace


#endif
