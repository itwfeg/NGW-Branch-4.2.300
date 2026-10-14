/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CMReaderWriter.h"
#include "CMReaderWriterEvolTag.h"
#include "CMReaderWriterEvolUI.h"
#include "CMReaderWriterUltimaDisk.h"
#include "Integer.h"
#include "Hex.h"


/**
 * Determine what kind of reader is capable of reading messages from the stream.
 */
CMReaderWriter *CMReaderWriter::createReaderWriter(const char *buff)
{
	/* If the first two bytes are 'RT', assume we will receive data in the
	 * Hobart Tagged (Evolution Backup/Restore, DataGateweigh, or Intranet) format.
	 *
	 * If the first two bytes are '30', and the fifth character is an ETX,
	 * assume we will receive data in the Ultima Disk format.
	 *
	 * If the first six bytes are digits (part of the record length indicator),
	 * assume we will receive data in the Evolution Application Messaging format.
	 *
	 * Otherwise, write an error log message indicating we couldn't determine
	 * what format the data conforms to.
	 */

	if (Debug::isTracing("CMReaderWriter"))
	{
		Debug::trace("CMReaderWriter", HobString("createReaderWriter - buff: *") +
										HobString(buff, LONGEST_IDENTIFIER_LENGTH) + HobString("*"));
		Debug::trace("CMReaderWriter", HobString("createReaderWriter - hex buff: ") +
										HobString(Hex::toString(buff, LONGEST_IDENTIFIER_LENGTH)));
	}

	if (CMReaderWriterEvolTag::canIdentifyBuffer(buff))
	{
		Debug::trace("CMReaderWriter", "createReaderWriter - Creating a Evol (Hobart Tagged) ReaderWriter");
		CMReaderWriter *rw = new CMReaderWriterEvolTag();
		return rw;
	}
	else if (CMReaderWriterEvolUI::canIdentifyBuffer(buff))
	{
		Debug::trace("CMReaderWriter", "createReaderWriter - Creating an Evol UI ReaderWriter");
		CMReaderWriter *rw = new CMReaderWriterEvolUI();
		return rw;
	}
	else if (CMReaderWriterUltimaDisk::canIdentifyBuffer(buff))
	{
		Debug::trace("CMReaderWriter", "createReaderWriter - Creating an Ultima Disk ReaderWriter");
		CMReaderWriter *rw = new CMReaderWriterUltimaDisk();
		return rw;
	}

	/* Couldn't determine data format, couldn't read the number of bytes corresponding to the
	 * longest header, for known formats, from the socket. */
	return (CMReaderWriter *)NULL;
}


int CMReaderWriter::getReaderWriterId() const
{
	return readerWriterId;
}

void CMReaderWriter::setReaderWriterId(int id)
{
	readerWriterId = id;
}


int CMReaderWriter::getRecordIdentifierLength() const
{
	return recordIdentifierLength;
}

void CMReaderWriter::setRecordIdentifierLength(int len)
{
	recordIdentifierLength = len;
}

