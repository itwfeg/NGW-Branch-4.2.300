/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CMREADERWRITEREVOLUI_H
#define CMREADERWRITEREVOLUI_H

#include "CMReaderWriter.h"


/**
 * This class implements the interface to format Hobart Evolution UI (Browser) messages to/from a stream.
 */
class CMReaderWriterEvolUI : public CMReaderWriter
{
	public:

		// Allows the message length indicator to contain 6 digits.
		static const int BUFFER_LENGTH_INDICATOR_LENGTH = 6;

		CMReaderWriterEvolUI();
		virtual ~CMReaderWriterEvolUI();

		int getRecordHeaderLength() const;
		int getRecordFooterLength() const;

		int finalizeRecordForSending(char *buff, const char *msg, int length);

		bool finishedReadingRecord(const char *msg, int length);
		bool finishedReadingRecord(const HobString &msg);

		int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer);
		int formatFromReceive(HobString &msg);

		static bool canIdentifyBuffer(const char *buff);


	private:

		int totalRecordLength;
		bool finishedReadingHeader;
};

#endif //CMREADERWRITEREVOLUI_H
