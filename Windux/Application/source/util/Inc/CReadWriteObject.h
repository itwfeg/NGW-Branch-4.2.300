// Header file included by ANTLR UniLexer
#if defined(PAL_WIN32_RUN)
#include <tchar.h>
#endif
#ifndef _CREAD_WRITE_OBJECT_HEADER_
#define _CREAD_WRITE_OBJECT_HEADER_

/*******************************************************************************

TYPE:			Base classes
					

PURPOSE:		These are the base classes to allow the reading and writing of data
				without needing to identify its source.


FUNCTION:		To allow access to a MMF, File, Socket etc without identifying what the
				data is.


INTERFACES:		Three classes :
				CReadableDataObject   A Data object to be read from 
				CWriteableDataObject  A Data Object to be Written to
				CReadWriteObject	  A Data object to both read and write with.

				
				Three Exceptions for each of the types of operations
				CReadException
				CWriteException
				CPeekException

RESOURCES:		Negligable


PROCESSING:		The objects are pure virtal base classes.

DATA:			None:


LOGIC:		


*******************************************************************************/

#include "ErrorHandling.h"


namespace Util
{


	class CReadException : public Util::CBaseException
	{

	public:

		// Description:	This function returns the Exception text.
		// Returns:		The exception text.
		virtual std::wstring ExceptionMsg() const throw()
		{
			return std::wstring(_T("Error in the Reading of Data"));
		}
	};


	class CWriteException : public Util::CBaseException
	{

	public:

		// Description:	This function returns the Exception text.
		// Returns:		The exception text.
		virtual std::wstring ExceptionMsg() const throw()
		{
			return std::wstring(_T("Error in the Writing of Data"));
		}

	};


	class CPeekException : public Util::CBaseException
	{

	public:

		// Description:	This function returns the Exception text.
		// Returns:		The exception text.
		virtual std::wstring ExceptionMsg() const throw()
		{
			return std::wstring(_T("Error in the Peeking of Data"));
		}

	};


	// Read object
	class CReadableDataObject
	{

	public:

		// Description
		//		Constructor
		CReadableDataObject(){}

		// Description
		//		Destructor
		virtual ~CReadableDataObject(){};

		// Description
		//		Read from a data source
		// Returns
		//		The length of the data read into the buffer or -1 if there was an error.
		//		An exception may also be thrown in the case of an error.
		//		The length of data read may be less than the amount requested.
		//		The value zero will only be returned if i_Length was 0 or there is no more
		//		data to be read from the data source.
		virtual int Read
		(
			BYTE*	o_Buffer,	// The data place to read into.
			int		i_Length	// The length of data to read.
		) = 0;

		// Description
		//		Read an exact number of bytes from a data source
		// Returns
		//		The length of the data read into the buffer or -1 if there was an error.
		//		An exception may also be thrown in the case of an error.
		//		The length of data read may NOT be less than the amount requested, unless it is -1.
		void ReadExactly
		(
			BYTE*	o_Buffer,	// The data place to read into.
			int		i_Length	// The length of data to read.
		)
		{
			try {
				int read_so_far = 0;
				int read_this_time = 0;

				while (read_so_far < i_Length)
				{
					read_this_time = Read( &o_Buffer[read_so_far], i_Length - read_so_far );
					Assert (read_this_time > 0, CReadException());

					read_so_far += read_this_time;
				};
			}
			catch(...) 
			{
				LogCatchAndThrow;
			}
		}

		// Description		
		//		Peek from a data source
		// Returns 
		//		The number of bytes copied from the start of the data object or -1 if in error.
		//		An exception may also be thrown in the case of an error.
		//		The length of data copied may be less than the amount requested.
		//		Subsequent calls to Peek without an intermediate call to Read will start with the
		//		same data.
		virtual int Peek
		(
			BYTE*	o_Buffer,
			int		i_Length
		) = 0;

		// Description		
		//		attempt to produce a contiguous amount of memory which can be read
		//		Note that this is dangerous as pointer arithmetic will need to be done 
		//		to ensure that an access violation does not occur.
		// Returns 
		//		virtual BYTE* 
		virtual const BYTE* c_str()
		{
			return NULL;
		}

	};


	// Write object
	class CWriteableDataObject
	{
	public:

		// Description
		//		Constructor
		CWriteableDataObject(){}

		
		// Description
		//		Destructor
		virtual ~CWriteableDataObject(){}

		// Description
		//		Write to a data sink
		// Returns
		//		The number of bytes written.
		virtual int Write
		(
			const BYTE*	i_Buffer,	// The data place to write from.
			int			i_Length	// The length of data to write.
		) = 0;

		// Description
		//		Write to a data sink, throws an exception if it fails.
		void WriteExactly
		(
			const BYTE*	i_Buffer,	// The data place to write from.
			int			i_Length	// The length of data to write.
		)
		{
			try
			{
				int written_so_far = 0;

				while (written_so_far < i_Length)
				{
					int written_this_time = Write( &i_Buffer[written_so_far], i_Length - written_so_far );
					Assert (written_this_time > 0, CWriteException());

					written_so_far += written_this_time;
				}
			}
			catch(...) 
			{
				LogCatchAndThrow;
			}
		}

	};


	// Read/Write object
	class CReadWriteObject : public Util::CWriteableDataObject, 
							 public Util::CReadableDataObject
	{

	public:
		
		// Description
		//		Constructor
		CReadWriteObject(){}

		// Description
		//		Destructor
		virtual ~CReadWriteObject(){}

	};


} // end namespace


#endif
