// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef _RAW_MEMORY_STREAM_HEADER_
#define _RAW_MEMORY_STREAM_HEADER_

/*******************************************************************************

TYPE:			Base class
					
PURPOSE:		This class is not a fully fledged streaming class and under some
				circumstances will become very ineficient.

				The class is designed to be either
				1) Created and then read into, followed by lots of write outs until
				   there is no more data - then destruction

				2) Created and suppplied with data. Data will then be pulled out of the
				   class until it is exhausted.

FUNCTION:		To allow access to the byte stream thorugh the Readable/Writeble
				object classes

INTERFACES:		The class will be initially constructed specifying the amount of data 
				that should be made available, or specifying the source of the data and
				how much data there is available.

				The read(), write(), peek(), c_str() and Length() methods can then be 
				called to gain access to 	the data

				The methods PeekFrom() and ResetReaderAndWriter have been provided
				as they are used by some of the communications elements.
		
RESOURCES:		Negligable

PROCESSING:		Construct the class then use the 
				
				Read and Write Methods to process data.

DATA:			None:

LOGIC:		

*******************************************************************************/

#include "CReadWriteObject.h"
#include "CKnownLengthByteStream.h"


namespace Util
{


class CRawMemoryStream : public Util::CReadWriteObject
{

public: 

	enum 
	{
		e_MAX_BUFFER_LENGTH = LONG_MAX,	// This is a huge size and the absolute max
										// we should attempt to allocate
		E_STREAM_DEFAULT_INITIAL_SIZE = 1000
	};

	// Description
	//		Construct a new stream and resize it to the specified size....
	// Returns
	//	
	CRawMemoryStream
	(
		DWORD i_Size= E_STREAM_DEFAULT_INITIAL_SIZE	// The size of the stream.
	)
	{
		try
		{
			// Make sure that the size is ok.
			Assert(i_Size < e_MAX_BUFFER_LENGTH, Util::CParamException());

			m_DataStore.resize(i_Size);
			m_TotalSize = i_Size;

			ResetReaderAndWriter();
		}
		catch(...)
		{
			LogCatchAndThrow;
		}
	}

	// Description		
	//		Read in the specified amount of data from the KnownLengthByteStream
	// Returns 
	//		
	CRawMemoryStream
	(
		Util::CKnownLengthByteStream&	i_Stream,	// The stream to read from.
		DWORD							i_Amount	// The amount to read.
	)
	{
		try
		{
			Assert((DWORD)i_Stream.UnreadLength() >= i_Amount &&
				   i_Amount < (DWORD)e_MAX_BUFFER_LENGTH,Util::CParamException());

			// Allocate some space for the data to go into
			m_DataStore.resize(i_Amount); 

			// pull in the required amount of data
			i_Stream.ReadExactly(&m_DataStore.front(),i_Amount);

			// Set the sizes which can be read
			m_TotalSize = i_Amount;
			m_AvailableReadSize = i_Amount;
			m_AvailableWriteSize = 0;
		
			// Set the pointers to read from - note no more data can be written without a 
			// reallocation of reaources.
			m_ReaderStart = &m_DataStore.front();
			m_WriterStart = m_ReaderStart + m_TotalSize;
			
			return;	
		}
		catch(...) 
		{
			LogCatchAndThrow;
		}
	}

	// Description
	//		Construct a new stream and load it with data
	//		Assumes that any new data will be added onto the end
	//		and that any subsequent reads will pop data from the front.
	// Returns
	//	
	CRawMemoryStream
	(
		std::vector<BYTE>&	i_T,		// The buffer to load from.
		DWORD				i_TotalSize	// The amount of information.
	)
	{
		try
		{
			Assert(i_TotalSize < e_MAX_BUFFER_LENGTH,Util::CParamException());

			// Set the dataStore
			m_DataStore = i_T;
			
			// Set the sizes which can be read
			m_TotalSize = i_TotalSize;
			m_AvailableReadSize = i_TotalSize;
			m_AvailableWriteSize = 0;
		
			// Set the pointers to read from - note no more data can be written without a 
			// reallocation of reaources.
			m_ReaderStart = &m_DataStore.front();
			m_WriterStart = m_ReaderStart + m_TotalSize;
			
			
		}
		catch(...) 
		{
			LogCatchAndThrow;
		}
	}

	// Description
	//		Destructor
	// Returns
	//	
	virtual ~CRawMemoryStream()
	{
	}

	//	Description
	//		Read some data
	//	Returns
	//		int				the amount of information read
	virtual int Read
	(
		BYTE*	o_Buffer,	// The place to read data into.
		int		i_Length	// The amount of information to read.
	)
	{
		try
		{
			if (i_Length > (int)m_AvailableReadSize)
				i_Length = (int)m_AvailableReadSize;

			// Copy out the needed data
			::memcpy(o_Buffer,m_ReaderStart,i_Length);
			m_ReaderStart += i_Length;
			m_AvailableReadSize -= i_Length;

			// if the pointers point to the same location then 
			if (m_ReaderStart == m_WriterStart)
			{
				ResetReaderAndWriter();
			}
		
			return i_Length;
		}
		catch(...)
		{
			LogCatchAndThrow ;
		}
	}

	// Description
	//		Write some data
	//	Returns
	//		int				the amount of information written
	virtual int Write
	(
		const BYTE*	i_Buffer,	// The place to write data from.
		int			i_Length	// The amount of information to write.
	)
	{
		try
		{
			// Make sure that this is valid
			Assert(i_Length < e_MAX_BUFFER_LENGTH, Util::CParamException());
			Assert(i_Buffer != NULL, Util::CParamException());

			if ((int)m_AvailableWriteSize < i_Length)
			{
				// Copy over that which can be read and also that which 
				// we want to add

				std::vector<BYTE> m_Temp(m_AvailableReadSize + i_Length);
				// Copy over the existing data
				::memcpy(&m_Temp.front(),m_ReaderStart,m_AvailableReadSize);
				m_TotalSize = m_AvailableReadSize + i_Length;
				
				m_DataStore = m_Temp;
				m_ReaderStart = &m_DataStore.front();

				// Set the writer to the correct place
				m_WriterStart = m_ReaderStart + m_AvailableReadSize;
				m_AvailableWriteSize = i_Length;
			}

			// Copy in the new data
			::memcpy(m_WriterStart,i_Buffer,i_Length);

			// Add the Length on to the size operator
			m_AvailableReadSize += i_Length;
			m_WriterStart += i_Length;
			m_AvailableWriteSize -= i_Length;

			return i_Length;
		}
		catch(...)
		{
			LogCatchAndThrow;
		}
	}

	// Description		
	//		Peek Some Data
	// Returns 
	//		int				the amount of information peeked.
	virtual int Peek
	(
		BYTE*	o_Buffer,	// The place to peek data into.
		int		i_Length	// The length of the data.
	)
	{
		try
		{
			// Make sure that this is valid
			Assert(o_Buffer != NULL, Util::CParamException());
			Assert(i_Length < e_MAX_BUFFER_LENGTH, Util::CParamException());

			if (i_Length > (int)m_AvailableReadSize)
				i_Length = (int)m_AvailableReadSize;

			// Copy out the needed data
			::memcpy(o_Buffer,m_ReaderStart,i_Length);

			return i_Length;
		}
		catch(...)
		{
			LogCatchAndThrow ;
		}
	}

	// Description
	//		This operator is needed for the byte streams that are attached 
	//		to heap space/ attempt to convert the m_DataSource to a 
	//		CMSGStream<Byte,std::basic_string<BYTE> and use that
	//		if that does not happen then return NULL
	// Returns
	//		const BYTE*		the header of the data stream
	virtual const BYTE* c_str()
	{
		return m_ReaderStart;
	}

	// Description		
	//		Return the length of the raw byte stream.
	// Returns 
	//		DWORD			the length of the data stream 
	DWORD Length()
	{
		return m_AvailableReadSize;
	}

	// Description		
	//		Peek the given number of bytes from the readable object into this object
	//		Throws an exception if the amount to be peeked is not available
	// Returns 
	//		void	
	void PeekFrom
	(
	Util::CReadableDataObject&	i_Source,	// The place to peek the data from.
		int						i_Length	// The amount of information to peek.
	)
	{
		try
		{
			m_ReaderStart = &m_DataStore.front();
			m_WriterStart = m_ReaderStart;
			m_AvailableReadSize = 0;
			while ((int)m_AvailableReadSize < i_Length)
			{
				int peeked = i_Source.Peek( m_WriterStart, i_Length );

				if (peeked < i_Length)
				{
					if (peeked <= 0)
						throw Util::CPeekException();
					else
						Sleep( 0 );	// Give the data a chance to appear
				}
				else
					m_AvailableReadSize = i_Length;
			}
			m_WriterStart += i_Length;
			m_AvailableReadSize += i_Length;
			m_AvailableWriteSize = m_TotalSize - m_AvailableReadSize;
		}
		catch(...) 
		{
			LogCatchAndThrow;
		}

	}

	// Description		
	//		Reset the reader and the writer to the original states.
	// Returns 
	//		void 
	void ResetReaderAndWriter()
	{
		m_ReaderStart = m_WriterStart = &m_DataStore.front();
			
		m_AvailableWriteSize = m_TotalSize;
		m_AvailableReadSize = 0;
	}

	// Description		
	//		Do we still have data to read
	// Returns 
	//		bool		Do we still have readable data
	bool StillHasReadableData()
	{
		try
		{
			return (m_AvailableReadSize > 0);
		}
		catch(...)
		{
			LogCatchAndThrow;
		}
	}

private:

	// the storage area
	std::vector<BYTE> m_DataStore; 
	DWORD m_TotalSize,			// Current size of the store
		  m_AvailableReadSize,  // Current amount in the store
		  m_AvailableWriteSize; // Current amount which can be written

	BYTE *m_ReaderStart,		// Pointer to the start of the data which can be read 
		 *m_WriterStart;		// Pointer to the start of where data can be written

};


} // end namespace


#endif
