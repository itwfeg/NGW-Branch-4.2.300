// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef _CMSG_INTBYTE_STREAM_UTIL_HEADER_
#define _CMSG_INTBYTE_STREAM_UTIL_HEADER_

/*******************************************************************************


TYPE:			Derived Class
					
PURPOSE:		Takes the byte stream from above an adds the functionality to 
				allow the streaming of objects for DADF internal communications....

FUNCTION:		see above


INTERFACES:		The class will be initialised
				
				Data will then be read and written to the class using the 
				overloaded >> and << operators 
				

RESOURCES:		Heap - potentially  > 2K

REFERENCES:		


PROCESSING:		See Above


DATA:			See 'DATA:' in the functions below.

*******************************************************************************/

#include "CMSGByteStream.h"


namespace Util
{


class CMSGIntByteStream : public CMSGByteStream
{

	enum e_CMSGIntByteStream
	{
		E_STREAM_DEFAULT_INITIAL_SIZE = 1000
	};

public:

	// Description
	//		Constructor
	// Returns
	//
	CMSGIntByteStream
	(
		unsigned int i_Size = E_STREAM_DEFAULT_INITIAL_SIZE	// The initial size of the stream.
	)
	: Util::CMSGByteStream(i_Size)
	{
	}
	
	// Description
	//		Constructor		
	// Returns
	//	
	CMSGIntByteStream
	(
		const BYTE*	i_T,		// The array of bytes to initialise the stream with.
		DWORD		l_TotalSize	// The size of the array.
	)
	: Util::CMSGByteStream(i_T,l_TotalSize) 
	{
	}

	// Description		
	//		Construct an internal byte stream with an existing readwrite object
	// Returns 
	//		
	CMSGIntByteStream(Util::CSmartPtr<Util::CReadWriteObject> i_Object)
	: Util::CMSGByteStream(i_Object) 
	{
	}

	// Description
	//		destructor
	// Returns
	//	
	virtual ~CMSGIntByteStream() {}

	// A range of streaming operators.
	// - Insertion operators receive as argument the data to insert,
	//   and return the stream with the inserted data.
	// - Extraction operators receive as argument a reference to where
	//   the extracted data shall be written,
	//   and return the stream after the data extraction.

	virtual Util::CMSGByteStream& operator << (const bool i_Val)
	{
		unsigned char l_Val;
		l_Val = i_Val ? 1 : 0;
		WriteExactly(&l_Val,sizeof(char));
		return *this;
	}

	virtual Util::CMSGByteStream& operator >> (bool& o_Val)
	{
		unsigned char l_Val;
		ReadExactly(&l_Val,sizeof(char));
		o_Val = (l_Val != 0);
		return *this;
	}

	Util::CMSGByteStream& operator << (const std::string& i_String)
	{
		*this << (unsigned __int32 )(i_String.length());

		// Place the actual text onto the stream.
		WriteExactly((const unsigned char*) i_String.c_str(),i_String.length());
		return *this;
	}

	Util::CMSGByteStream& operator >> (std::string& i_String)
	{
		unsigned __int32 l_Size;
		//
		// Pull the size of the string off the stream.
		// 
		*this >> l_Size ; 
				
		i_String.resize(l_Size,0);
#if _MSC_VER > 1100 && _MSC_VER < 1300 //for VS6 C++
		ReadExactly((unsigned char*)i_String.begin().base(),l_Size);
#else                                  //for C++ .NET2003
		ReadExactly((unsigned char*)i_String.data(),l_Size);
#endif
		return *this;
	}



	Util::CMSGByteStream& operator << (const std::wstring& i_String)
	{
		*this << (unsigned __int32 )(i_String.length()*sizeof(wchar_t));

		// Place the actual text onto the stream.
		WriteExactly((const unsigned char*) i_String.c_str(),i_String.length()*sizeof(wchar_t));
		return *this;
	}

	Util::CMSGByteStream& operator >> (std::wstring& i_String)
	{
		unsigned __int32 l_Size;
		//
		// Pull the size of the string off the stream.
		// 
		*this >> l_Size ; 
		// We are streaming bytes into a wide string
		
		i_String.resize(l_Size/sizeof(wchar_t),0);
#if _MSC_VER > 1100 && _MSC_VER < 1300 //for VS6 C++
		ReadExactly((unsigned char*)i_String.begin().base(),l_Size);
#else                                  //for C++ .NET2003
		ReadExactly((unsigned char*)i_String.data(),l_Size);
#endif
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const char i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (char& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

#if _MSC_VER > 1100 && _MSC_VER < 1300
	virtual Util::CMSGByteStream& operator << (const __int8 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (__int8& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	virtual Util::CMSGByteStream& operator << (const unsigned char i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (unsigned char& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

#if _MSC_VER > 1100 && _MSC_VER < 1300
	virtual Util::CMSGByteStream& operator << (const unsigned __int8 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (unsigned __int8& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	virtual Util::CMSGByteStream& operator << (const __int16 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (__int16& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

#if _MSC_VER > 1100 && _MSC_VER < 1300
	virtual Util::CMSGByteStream& operator << (const unsigned __int16 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (unsigned __int16& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	virtual Util::CMSGByteStream& operator << (const unsigned short i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (unsigned short& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const __int32 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (__int32& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const UINT32 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (UINT32& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

#if _MSC_VER > 1100 && _MSC_VER < 1300
	virtual Util::CMSGByteStream& operator << (const unsigned __int32 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (unsigned __int32& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	virtual Util::CMSGByteStream& operator << (const unsigned long i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (unsigned long& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const __int64 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator >> (__int64& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

#if _MSC_VER > 1100 && _MSC_VER < 1300
	virtual Util::CMSGByteStream& operator << (const unsigned __int64 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator >> (unsigned __int64& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	virtual Util::CMSGByteStream& operator << (const float i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator >> (float& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const double i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator >> (double& o_Val)
	{
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const Util::CMSGTime& i_Val)
	{
		::FILETIME l_FileTime = i_Val.GetNTFileTime();
		WriteExactly((BYTE*)&l_FileTime,sizeof(l_FileTime));
		return *this;
	}

	virtual Util::CMSGByteStream& operator >> (Util::CMSGTime& o_Val)
	{
		::FILETIME l_FileTime;
		ReadExactly((BYTE*)&l_FileTime,sizeof(l_FileTime));
		o_Val = l_FileTime;
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const Util::CMSGTimeSpan& i_Val)
	{
		return *this << i_Val.GetTimeSpan();
	}

	virtual Util::CMSGByteStream& operator >> (Util::CMSGTimeSpan& o_Val)
	{
		__int64 timeSpan;
		*this >> timeSpan;
		o_Val = Util::CMSGTimeSpan(timeSpan);
		return *this;
	}

};


} // end of the namespace


#endif
