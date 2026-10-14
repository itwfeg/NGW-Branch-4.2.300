// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef _CKNOWN_LENGTH_BYTE_STREAM_HEADER_
#define _CKNOWN_LENGTH_BYTE_STREAM_HEADER_
/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Derived class
					

PURPOSE:		To allow access to a MMF, File, Socket etc without identifying what the
				data is, but knowing how much we are allowed to read.
				

FUNCTION:		Part of a set of streaming base classes.
				These are the base classes to allow the reading and writing of data
				without needing to identify its source. 
				
				This class is an extention 
				of the CReadWriteObject that allows us to know the length which can be read.

INTERFACES:		Constructor
				
				Once the class is constructed then the peek(), read(), c_str(),TotalLength(),
				and UnreadLength() members may be used so manipiulate the class.

RESOURCES:		Negligable


PROCESSING:		

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
	class CKnownLengthByteStream : public Util::CReadableDataObject
	{
	public:
		// Description
		//		Constructor
		// Arguments
		//		i_MyParent	The data object that is going to supply the functionality.
		//					i.e. socket MMF etc,
		//		i_Length	The Length of the data that can be read
		// Returns
		CKnownLengthByteStream(Util::CReadableDataObject* i_MyParent,int i_Length);

		// Description
		//		Destructor
		// Arguments
		//
		// Returns
		virtual ~CKnownLengthByteStream(){}

		// Description
		//		Return the amount of information that we have drawn
		// Arguments
		//
		// Returns
		//		The total length of the stream
		int TotalLength();

		// Description
		//		Return the amount of information still left to draw
		// Arguments
		//
		// Returns
		//		The total length of the stream
		int UnreadLength();

		// Description
		//		Read some data
		// Arguments
		//		i_Buffer the data place to read into
		//		i_Length the length of data to read
		// Returns
		//		int The amount of information read
		virtual int Read(BYTE* o_Buffer,int i_Length);

		// Description
		//		Read some data
		// Arguments
		//		i_Buffer the data place to read into
		//		i_Length the length of data to read
		// Returns
		//		int The amount of information Peeked
		virtual int Peek(BYTE* o_Buffer,int i_Length);

		// Description		
		//		return the parent in a const string form if that is possible.
		//		this method may return NULL if the parent cannot be made contiguous.
		// Arguments
		// Returns 
		//		const BYTE* - The start of the data array
		virtual const BYTE* c_str();

	private:

		// DATA:
		// the Total Length of the data stream
		int m_TotalLength;

		// The unread length of the data stream
		int m_UnreadLength;

		// The object that will be being read / written to
		Util::CReadableDataObject* m_Parent;
	};

} // end of the namespace

#endif