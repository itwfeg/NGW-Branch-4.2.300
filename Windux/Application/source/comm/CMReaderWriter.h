/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CMREADERWRITER_H
#define CMREADERWRITER_H

#include "evolution.h"
#include "HobString.h"


class CMReaderWriter
{
	public:

		static const int MAX_BUFFER_LENGTH = 10241;

		static const int LONGEST_IDENTIFIER_LENGTH = 6;

		static const int UNKNOWNFORMAT_ID  = 0;
		static const int EVOL_UI_ID        = 1;
		static const int EVOL_TAGGED_ID    = 2;
		static const int ULTIMA_DISK_ID    = 3;
		static const int SYNC_TAGGED_ID    = 4;
		static const int AGENT_ID          = 5;
		static const int DB_INDEPENDENT_ID = 6;
		static const int GCD_ID            = 7;
		static const int HANDSFREEDEVICE_ID = 8;

		virtual ~CMReaderWriter() {};

		virtual int getRecordHeaderLength() const = 0;
		virtual int getRecordFooterLength() const = 0;

		virtual int finalizeRecordForSending(char *buff, const char *msg, int length) = 0;

		virtual bool finishedReadingRecord(const char *msg, int length) = 0;
		virtual bool finishedReadingRecord(const HobString &msg) = 0;

		virtual int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer) = 0;
		virtual int formatFromReceive(HobString &msg) = 0;

		//virtual static bool canIdentifyBuffer(const char *buff) = 0;


		int getReaderWriterId() const;
		void setReaderWriterId(int id);

		int getRecordIdentifierLength() const;
		void setRecordIdentifierLength(int len);

		static CMReaderWriter *createReaderWriter(const char *buff);


	protected:

		int readerWriterId;

		char *footer;
		char *alternateFooter;

		bool hasRecordHeader;
		bool hasRecordFooter;

		bool hasRecordSeparator;

		int recordIdentifierLength;
};
#endif //CMREADERWRITER_H

