// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef _CRAW_BYTE_STREAM_HEADER_
#define _CRAW_BYTE_STREAM_HEADER_
/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Base class
					

PURPOSE:		Base class for the Util::CMSGByteStream to allow the 
				functionality needed by the more generic Readable/Writeable
				data object hierarchy.

FUNCTION:		To allow access to the byte stream thorugh the Readable/Writeble
				object classes

				This class uses STL to manage the data store.

INTERFACES:		Constructor
				
				The class is designed to be initialy constructed and then 
				either -

				repeatedly written to so that the class can be slowly loaded 
				with data.

				Written to initially an then repeatedly read from until all the 
				data has been consumed.

				If the class needs to grow from the initially specified amount then
				STL will handle the growing.

				Note:
					if the c_str() method is called please do not then overwrite this data
					the const BYTE* should only be read from. 
					
					If data is written to this pointer location then the class will not function
					as expected as STL cannot keep track of how much data is loaded in the class.


RESOURCES:		Negligable


PROCESSING:		Construct the class then use the 
				
				Read,Write and peek Methods to process data.


DATA:			None:


LOGIC:		


****  END MODULE HEADER   ***
**************************************************************************/

#ifdef XTRA_COM_INFO
	#pragma message( "Compiling " __FILE__ ) 
#endif

#include "CReadWriteObject.h"
namespace Util
{
	class CRawByteStream : public Util::CReadWriteObject
	{
	public: 
		enum 
		{
			E_STREAM_DEFAULT_INITIAL_SIZE = 1000
		};

		// Description
		//		Construct a new stream and resize it to the specified size....
		// Arguments
		//		i_Size	the size of the stream
		// Returns
		//
		CRawByteStream(unsigned int i_Size= E_STREAM_DEFAULT_INITIAL_SIZE);


		// Description
		//		Copy constructor
		// Arguments
		//		i_Stream	the stream to read from 
		//		i_AmountToRead - The amount of information to read
		// Returns
		//
		CRawByteStream(Util::CReadableDataObject& i_Stream, 
					   unsigned int i_AmountToRead);


		// Description
		//		Construct a new stream and load it with data
		// Arguments
		//		i_T		The buffer to load from
		//		i_totalSize		The amount of information
		// Returns
		// 
		CRawByteStream(const BYTE* i_T,unsigned int i_TotalSize);

		// Description
		//		Destructor
		// Arguments
		//
		// Returns
		// 
		virtual ~CRawByteStream(){}
		
		
		// Description
		//		Read some data
		//	Arguments
		//		o_Buffer		the place to read data into
		//		i_Length		the amount of information to read
		// Returns
		//		int				the amount of information read
		virtual int Read(BYTE* o_Buffer, int i_Length);

		// Description
		//		Write some data
		//	Arguments
		//		i_Buffer		the place to write from
		//		i_Length		the amount of information written
		// Returns
		//		int				the amount of information written
		virtual int Write(const BYTE* i_Buffer, int i_Length);

		// Description		
		//		Peek Some Data
		// Arguments
		// 		o_Buffer		the place to peek to 
		// 		i_Length		the amount of data to peek
		// Returns 
		//		virtual CReadableDataObject& 
		virtual int Peek(BYTE* o_Buffer, int i_Length);

		// Description
		//		This operator is needed for the byte streams that are attached 
		//		to heap space/ attempt to convert the m_DataSource to a 
		//		CMSGStream<Byte,std::basic_string<BYTE> and use that
		//		if that does not happen then return NULL
		// Arguments
		//	
		// Returns
		//		const BYTE*		the start of the data	
		virtual const BYTE* c_str();

		// Description		
		//		Return the length of the raw byte stream.
		// Arguments
		//
		// Returns 
		//		int 
		int Length();
        const std::basic_string<BYTE> & GetDataStore() { return m_DataStore;}
	private:
		// the storage area
		std::basic_string<BYTE> m_DataStore ; 
	};
} // end of the namespace.
#endif
