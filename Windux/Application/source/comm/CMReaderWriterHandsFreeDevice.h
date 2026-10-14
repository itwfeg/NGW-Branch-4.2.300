/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CMREADERWRITERHANDSFREEDEVICE_H
#define CMREADERWRITERHANDSFREEDEVICE_H

#include <map>

#include "Event.h"
#include "CMReaderWriter.h"
#include "HobString.h"

/**
 * This class implements the interface to format Device messages to/from a stream (socket).
 */
class CMReaderWriterHandsFreeDevice : public CMReaderWriter
{
	public:

		static const int HANDSFREEDEVICE_MESSAGE_LENGTH = 4;


		CMReaderWriterHandsFreeDevice();
		virtual ~CMReaderWriterHandsFreeDevice();

		int getRecordHeaderLength() const;
		int getRecordFooterLength() const;

		int finalizeRecordForSending(char *buff, const char *msg, int length);

		bool finishedReadingRecord(const char *msg, int length);
		bool finishedReadingRecord(const HobString &msg);

		int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer);
		int formatFromReceive(HobString &msg);


private:


#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CMREADERWRITERHANDSFREEDEVICE_H
