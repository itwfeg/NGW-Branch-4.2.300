// Header file included by ANTLR UniLexer
#include <tchar.h>

#ifndef _CMSG_STR_BYTE_STREAM_HEADER_
#define _CMSG_STR_BYTE_STREAM_HEADER_

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


//namespace COMS : note This class was originally in COMS
namespace Util
{


//##ModelId=39FF1ECA025C
class CMSGStrByteStream : public Util::CMSGByteStream
{

	enum e_CMSGStrByteStream
	{
		E_STREAM_DEFAULT_INITIAL_SIZE = 1000
	};

public:

	// Description
	//		Constructor
	// Returns
	//
	//##ModelId=39FF1ECA034F
	CMSGStrByteStream
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
	CMSGStrByteStream
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
	//##ModelId=39FF1ECA0351
	CMSGStrByteStream(Util::CSmartPtr<Util::CReadWriteObject> i_Object)
	: Util::CMSGByteStream(i_Object) 
	{
	}

	// Description
	//		destructor
	// Returns
	//	
	//##ModelId=39FF1ECA034D
	virtual ~CMSGStrByteStream() {}

	// A range of streaming operators.
	// - Insertion operators receive as argument the data to insert,
	//   and return the stream with the inserted data.
	// - Extraction operators receive as argument a reference to where
	//   the extracted data shall be written,
	//   and return the stream after the data extraction.

	//##ModelId=39FF1ECA02F3
	virtual Util::CMSGByteStream& operator << (const bool i_Val)
	{
		unsigned char l_Val;
		l_Val = i_Val ? 1 : 0;
		WriteExactly(&l_Val,sizeof(char));
		return *this;
	}

	//##ModelId=39FF1ECA0267
	virtual Util::CMSGByteStream& operator >> (bool& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		unsigned char l_Val;
		ReadExactly(&l_Val,sizeof(char));
		o_Val = (l_Val != 0);
		return *this;
	}

	//##ModelId=39FF1ECA02F6
	Util::CMSGByteStream& operator << (const std::string& i_String)
	{
	//	*this << (unsigned _int32 )(i_String.length());

		// Place the actual text onto the stream.
		WriteExactly((const unsigned char*) i_String.c_str(),(int)i_String.length());
		return *this;
	}

	//##ModelId=39FF1ECA0299
	Util::CMSGByteStream& operator >> (std::string& i_String)
	{
		unsigned __int32 l_Size;
		//
		// Pull the size of the string off the stream.
		// 
	//	*this >> l_Size ; 
		l_Size = (unsigned __int32)size();		
		i_String.resize(l_Size,0);
// CJC Incompatible		ReadExactly((unsigned char*)i_String.begin().base(),l_Size);
		ReadExactly((unsigned char*)i_String.c_str(),l_Size);
		return *this;
	}



	//##ModelId=39FF1ECA02FD
	Util::CMSGByteStream& operator << (const std::wstring& i_String)
	{
		*this << (unsigned _int32 )(i_String.length()*sizeof(wchar_t));

		// Place the actual text onto the stream.
		WriteExactly((const unsigned char*) i_String.c_str(),(int)(i_String.length()*sizeof(wchar_t)));
		return *this;
	}

	//##ModelId=39FF1ECA02A2
	Util::CMSGByteStream& operator >> (std::wstring& i_String)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		unsigned _int32 l_Size;
		//
		// Pull the size of the string off the stream.
		// 
		*this >> l_Size ; 
		// We are streaming bytes into a wide string
		
		i_String.resize(l_Size/sizeof(wchar_t),0);
// CJC Incompatible		ReadExactly((unsigned char *)i_String.begin().base(),l_Size);
		ReadExactly((unsigned char *)i_String.c_str(),l_Size);
		return *this;
	}

	//##ModelId=39FF1ECA02FF
	virtual Util::CMSGByteStream& operator << (const char i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02A4
	virtual Util::CMSGByteStream& operator >> (char& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

#if _MSC_VER > 1100	& _MSC_VER < 1300		
	//##ModelId=39FF1ECA0307
	virtual Util::CMSGByteStream& operator << (const __int8 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02AC
	virtual Util::CMSGByteStream& operator >> (__int8& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	//##ModelId=39FF1ECA030A
	virtual Util::CMSGByteStream& operator << (const unsigned char i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02AF
	virtual Util::CMSGByteStream& operator >> (unsigned char& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
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
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	//##ModelId=39FF1ECA0312
	virtual Util::CMSGByteStream& operator << (const __int16 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02B7
	virtual Util::CMSGByteStream& operator >> (__int16& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
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
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	//##ModelId=39FF1ECA0315
	virtual Util::CMSGByteStream& operator << (const unsigned short i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02BA
	virtual Util::CMSGByteStream& operator >> (unsigned short& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	//##ModelId=39FF1ECA031D
	virtual Util::CMSGByteStream& operator << (const __int32 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02C2
	virtual Util::CMSGByteStream& operator >> (__int32& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	virtual Util::CMSGByteStream& operator << (const long i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	virtual Util::CMSGByteStream& operator >> (long& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
	//##ModelId=39FF1ECA0325
	virtual Util::CMSGByteStream& operator << (const UINT32 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02CA
	virtual Util::CMSGByteStream& operator >> (UINT32& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
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
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	//##ModelId=39FF1ECA0328
	virtual Util::CMSGByteStream& operator << (const unsigned long i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}
	
	//##ModelId=39FF1ECA02CD
	virtual Util::CMSGByteStream& operator >> (unsigned long& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	//##ModelId=39FF1ECA0330
	virtual Util::CMSGByteStream& operator << (const __int64 i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	//##ModelId=39FF1ECA02D6
	virtual Util::CMSGByteStream& operator >> (__int64& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
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
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}
#endif

	//##ModelId=39FF1ECA0333
	virtual Util::CMSGByteStream& operator << (const float i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	//##ModelId=39FF1ECA02D9
	virtual Util::CMSGByteStream& operator >> (float& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	//##ModelId=39FF1ECA033B
	virtual Util::CMSGByteStream& operator << (const double i_Val)
	{
		WriteExactly((BYTE*)&i_Val,sizeof(i_Val));
		return *this;
	}

	//##ModelId=39FF1ECA02E1
	virtual Util::CMSGByteStream& operator >> (double& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		ReadExactly((BYTE*)&o_Val,sizeof(o_Val));
		return *this;
	}

	//##ModelId=39FF1ECA033E
	virtual Util::CMSGByteStream& operator << (const Util::CMSGTime& i_Val)
	{
		::FILETIME l_FileTime = i_Val.GetNTFileTime();
		WriteExactly((BYTE*)&l_FileTime,sizeof(l_FileTime));
		return *this;
	}

	//##ModelId=39FF1ECA02E4
	virtual Util::CMSGByteStream& operator >> (Util::CMSGTime& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		::FILETIME l_FileTime;
		ReadExactly((BYTE*)&l_FileTime,sizeof(l_FileTime));
		o_Val = l_FileTime;
		return *this;
	}

	//##ModelId=39FF1ECA0345
	virtual Util::CMSGByteStream& operator << (const Util::CMSGTimeSpan& i_Val)
	{
		return *this << i_Val.GetTimeSpan();
	}

	//##ModelId=39FF1ECA02EB
	virtual Util::CMSGByteStream& operator >> (Util::CMSGTimeSpan& o_Val)
	{
		throw Util::CNamedException(L"Can only stream out to std::string.");
		__int64 timeSpan;
		*this >> timeSpan;
		o_Val = Util::CMSGTimeSpan(timeSpan);
		return *this;
	}

};

} // End namespace Util

#endif // _CMSG_STR_BYTE_STREAM_HEADER_
