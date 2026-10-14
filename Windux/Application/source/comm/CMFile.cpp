/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CMFile.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int CMFile::_dataType_ = 0;
	std::string CMFile::_dataTypeName_("CMFile"); 
#endif


using namespace std;


CMFile::CMFile(const char *fName, short initialMode)
	: CommMechanism(),
	  fileName(fName),
	  myFile((std::fstream *)NULL),
	  receivingBeginningOfRecord(true),
	  handlingLineSeparator(false)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	ios::openmode omode = ios::in|ios::binary;

	if (initialMode & CommMechanism::MODE_OUTPUT)
	{
		omode |= ios::trunc;
		omode |= ios::out;
	}

	if (!fileName.empty())
		myFile = new fstream(fileName.c_str(), omode);

	// In CommMechanism
	setMode(initialMode);
}

CMFile::~CMFile()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

	if (myReaderWriter)
		delete myReaderWriter;

	if (myFile)
	{
		myFile->close();

		delete myFile;
		myFile = NULL;
	}
}

int CMFile::cmSend(const char *msg, int length)
{
	if (msg && length > 0 && myFile)
	{
		/* buff is the buffer we will send.  If this file has a reader/writer, that reader/writer will
		 * have the chance to modify (tack on a header and/or footer) before we send the message.
		 */
		char *buff = const_cast<char *>(msg);
		int totalLength = length;

		if (myReaderWriter)
		{
			buff = new char[length + myReaderWriter->getRecordHeaderLength() + myReaderWriter->getRecordFooterLength() + 1];  // 2 for possible 'RT', 1 for possible RS, 1 for null
			totalLength = myReaderWriter->finalizeRecordForSending(buff, msg, length);
		}
		else
		{
			// todo - maybe write error log message indicating I don't have a reader for determining the format
			//			for the data to be written to this file.
		}

		myFile->write(buff, totalLength);
		myFile->flush();

		// Allow for writing out line separator character(s), if so desired,
		// independent of the reader/writer set up for this File.
		if (isHandlingLineSeparator() && myFile->good())
		{
			*myFile << endl;
			myFile->flush();
		}

		int bytesSent = totalLength;
		if (myFile->fail() || myFile->bad())
			bytesSent = -1;

		if (Debug::isTracing("CMFile"))
		{
			HobString f((myReaderWriter) ? Integer::toString(myReaderWriter->getReaderWriterId()) : "??");
			HobString bs(Integer::toString(bytesSent));
			if (bytesSent == ERROR_CODE)
			{
				Debug::trace("CMFile", "*******************************************************************************");
				Debug::trace("CMFile", HobString("cmSend - myReaderWriter=") + f + HobString("  error=") + bs + HobString("  message=") + HobString(Hex::toString(buff, totalLength)));
				Debug::trace("CMFile", "*******************************************************************************");
			}
			else if (bytesSent != totalLength)
			{
				Debug::trace("CMFile", "*******************************************************************************");
				Debug::trace("CMFile", HobString("cmSend - myReaderWriter=") + f + HobString("  bytesSent=") + bs + HobString("  message=") + HobString(Hex::toString(buff, totalLength)));
				Debug::trace("CMFile", "*******************************************************************************");
			}
		}

		if (bytesSent != totalLength)
		{
			// write log message (Warning/Error)
		}

		if (myReaderWriter)
			delete[] buff;

		return bytesSent;
	}
	return -1;
}

/*
int CMFile::cmSend(const HobString &msg)
{
	return cmSend(msg.data(), msg.size());
}
*/


int CMFile::cmReceive(char *msgbuf)
{
	// Assumption:  The char *msgbuf passed in will point to a large enough buffer to receive the
	//					 largest message that could be received.
	int bytesRead = -1;

	if (msgbuf && myFile)
	{
		if (!myReaderWriter)
			determineReaderWriter();

		if (!myReaderWriter)
		{
			Debug::trace("File", HobString("cmReceive - HEY! I still couldn't determine the reader/writer for file: ") + fileName);
			// todo - write error log message indicating I couldn't determine a reader for the incoming
			//				data to this file.
		}
		else
		{
			int count = 0;
			char *pMsgbuf = msgbuf;
			bool readEntireRecord = false;
			bytesRead = 0;

			/* Subtract off the recordIdentifierLength from the max possible bytes to read in case the buffer
			 * coming in is the initial buffer of a record and the record is greater than the max possible buffer size,
			 * because we will tack the previous record identifier onto this record.
			 */
			int maxBytesToRead = CMReaderWriter::MAX_BUFFER_LENGTH - myReaderWriter->getRecordIdentifierLength();

			while ((readEntireRecord == false) && (bytesRead < maxBytesToRead))
			{
				if ((bytesRead % 2048) == 0)
					PAL_absclass::PAL_sleep(0);

				myFile->read(pMsgbuf++, 1);

				count = (int) myFile->gcount();
				if (count == 0)
					break;
				else if (count != 1)
				{
					perror("read from file");
					*msgbuf = (char)NULL;
					return 0;
				}

				bytesRead++;

				if (myReaderWriter->finishedReadingRecord(msgbuf, bytesRead))
				{
					readEntireRecord = true;
				}
			}

			*(msgbuf + bytesRead) = '\0';

			Debug::trace("CMFile", HobString("bytesRead: ") + HobString(Integer::toString(bytesRead)));

			// If there is a header on the record, allow the reader/writer to remove it.
			bytesRead = myReaderWriter->formatFromReceive(msgbuf, bytesRead, receivingBeginningOfRecord);
			if (readEntireRecord)
				receivingBeginningOfRecord = true;
			else
			{
				bytesRead = -1;
				receivingBeginningOfRecord = false;
			}

			// Allow for reading in line separator character(s), if so desired,
			// independent of the reader/writer set up for this File.
			//if (isHandlingLineSeparator())
			//	*myFile << endl;

		}
	}
	return bytesRead;
}


int CMFile::cmReceive(HobString &msgbuf)
{
	int bytesRead = -1;
	msgbuf = "";

	if (myFile)
	{
		if (!myReaderWriter)
			determineReaderWriter();

		if (!myReaderWriter)
		{
			Debug::trace("File", HobString("cmReceive - HEY! I still couldn't determine the reader/writer for file: ") + fileName);
			// todo - write error log message indicating I couldn't determine a reader for the incoming
			//				data to this file.
		}
		else
		{
			msgbuf.reserve(CMReaderWriter::MAX_BUFFER_LENGTH);

			int count = 0;
			bool readEntireRecord = false;
			bytesRead = 0;
			char byte = 0;

			while (!readEntireRecord)
			{
				if ((bytesRead % 2048) == 0)
					PAL_absclass::PAL_sleep(0); 

				myFile->read(&byte, 1);

				count = (int) myFile->gcount();
				if (count == 0)
					break;
				else if (count != 1)
				{
					perror("read from file");
					msgbuf = "";
					msgbuf.reserve(0);
					return 0;
				}

				// if no more room to store data in the current msgbuf, then add some more.
				// Reallocation using reserve() allows for fewer reallocations as the size increases.
				if (!msgbuf.capacity())
				{
			Debug::trace("CMFile.cmReceive", HobString("count = ") + HobString(Integer::toString(count)));
			Debug::trace("CMFile.cmReceive", HobString("msgbuf.capacity() = ") + HobString(Integer::toString((int)msgbuf.capacity())) + HobString("   msgbuf.size() = ") + HobString(Integer::toString((int)msgbuf.size())));
					msgbuf.reserve(msgbuf.size() + CMReaderWriter::MAX_BUFFER_LENGTH);
			Debug::trace("CMFile.cmReceive", HobString("Increasing msgbuf.capacity()"));
			Debug::trace("CMFile.cmReceive", HobString("msgbuf.capacity() = ") + HobString(Integer::toString((int)msgbuf.capacity())) + HobString("   msgbuf.size() = ") + HobString(Integer::toString((int)msgbuf.size())));
				}

				bytesRead++;
				msgbuf += byte;

				if (myReaderWriter->finishedReadingRecord(msgbuf))
					readEntireRecord = true;
			}

			Debug::trace("CMFile.cmReceive", HobString("bytesRead: ") + HobString(Integer::toString(bytesRead)));

			// If there is a header and/or footer on the record, allow the reader/writer to remove it.
			bytesRead = myReaderWriter->formatFromReceive(msgbuf);

			Debug::trace("CMFile.cmReceive", HobString("bytesRead: ") + HobString(Integer::toString(bytesRead)));
		}
	}

	// Free any excess allocated memory in the string.
	// This will cut the capacity of the string to the string's current size.
	msgbuf.reserve(0);
	return (int) msgbuf.size();
}


int CMFile::cmNumMsgs()
{
	Debug::trace("CMFile.nummsgs", HobString("cmNumMsgs - fileName=") + fileName);
	if (myFile)
	{
		int ret = myFile->peek();
		if (ret != EOF)
		{
			Debug::trace("CMFile.nummsgs", HobString("cmNumMsgs - peek - ret= ") + HobString(Integer::toString(ret)));
			return 1;
		}
	}	
	Debug::trace("CMFile.nummsgs", HobString("cmNumMsgs - No messages left to read from file: ") + fileName);
	return 0;
}

void CMFile::cmClose()
{
	if (myFile)
	{
		Debug::trace("CMFile", HobString("cmClose - Closing file: ") + fileName);

		myFile->close();

		delete myFile;
		myFile = (fstream *)NULL;
	}
}

// Close and delete/remove the file.
void CMFile::cmDelete()
{
	cmClose();
	if (!fileName.empty())
	{
		Debug::trace("CMFile", HobString("cmDelete - Deleting file: ") + fileName);
		myFile = new fstream(fileName.c_str(), ios::trunc);
		cmClose();
	}
}

const char *CMFile::toString()
{
	toStringData = "CMFile: fileName=";
	toStringData += fileName;

	return toStringData.c_str();
}


bool CMFile::isHandlingLineSeparator()
{
	return handlingLineSeparator;
}
void CMFile::setHandlingLineSeparator(bool handlingLineSeparator)
{
	this->handlingLineSeparator = handlingLineSeparator;
}


void CMFile::determineReaderWriter()
{
	if (myFile)
	{
		int bytesRead = 0;
		char buff[CMReaderWriter::LONGEST_IDENTIFIER_LENGTH + 1];
		memset(buff, 0, CMReaderWriter::LONGEST_IDENTIFIER_LENGTH + 1);

		// Seek to the beginning of the file, read the number of bytes to determine type of data,
		// then reseek back to the beginning of the file.
		myFile->seekg(0, ios::beg);
		//long pos0 = myFile->tellg();
		//Debug::trace("CMFile", HobString("determineReaderWriter - seek before read to: ") + HobString(Long::toString((long)pos0)));

		myFile->read(buff, CMReaderWriter::LONGEST_IDENTIFIER_LENGTH);
		bytesRead = (int) myFile->gcount();
		Debug::trace("CMFile", HobString("determineReaderWriter - initial bytes read: ") + HobString(Integer::toString(bytesRead)));

		// If the eof flag has been set, the following will clear that status bit.
		// If we don't clear it, since we seeked to the beginning of the file, then we will still
		// think we are at the end of the file when we read from it, above.
		// ALSO, you have to do this before seeking back to the beginning with seekg right after this call!!!
		myFile->clear();

		myFile->seekg(0, ios::beg);
		//long pos1 = myFile->tellg();
		//Debug::trace("CMFile", HobString("determineReaderWriter - seek to 0 from the beginning after read to: ") + HobString(Long::toString((long)pos1)));

		*(buff + CMReaderWriter::LONGEST_IDENTIFIER_LENGTH) = (char)NULL;
		if (Debug::isTracing("CMFile"))
		Debug::trace("CMFile", HobString("determineReaderWriter - data to determine readerwriter: ") + HobString(Hex::toString(buff, CMReaderWriter::LONGEST_IDENTIFIER_LENGTH)));

		myReaderWriter = CMReaderWriter::createReaderWriter(buff);
		if (myReaderWriter)
			Debug::trace("CMFile", HobString("determineReaderWriter - determined ReaderWriter: "));
	}
}

