/* HT Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */
#include "SessionEvent.h"
#include "ConfigurationEvent.h"

#include "CMReaderWriterHandsFreeDevice.h"
#include "Integer.h"
#include "Hex.h"

#include "scalemaster.h"
#include "scalemasterEvol.h"

#include "messagefactory.h"
#include "message.h"

#include "util/e_string/utilstring.h"

using namespace std;


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int CMReaderWriterHandsFreeDevice::_dataType_ = 0;
	std::string CMReaderWriterHandsFreeDevice::_dataTypeName_("CMReaderWriterHandsFreeDevice");
#endif


// This is the reader/writer when receiving/sending records to the Hands-Free Device device.

CMReaderWriterHandsFreeDevice::CMReaderWriterHandsFreeDevice()
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	readerWriterId = CMReaderWriter::HANDSFREEDEVICE_ID;
	recordIdentifierLength = 4;
}

CMReaderWriterHandsFreeDevice::~CMReaderWriterHandsFreeDevice()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}

int CMReaderWriterHandsFreeDevice::getRecordHeaderLength() const
{
	return 0;
}

int CMReaderWriterHandsFreeDevice::getRecordFooterLength() const
{
	return 0;
}


int CMReaderWriterHandsFreeDevice::finalizeRecordForSending(char *buff, const char *msg, int length)
{
	memcpy(buff, msg, length);
	return length;
}


bool CMReaderWriterHandsFreeDevice::finishedReadingRecord(const char *msg, int length)
{
	UINT32 messageLengthIndicator = 0;
	if (length >= recordIdentifierLength)
	{
		string a(msg);
		string lengthStr = a.substr(0, recordIdentifierLength);
		messageLengthIndicator = Hex::longValueOf(lengthStr);
		if (messageLengthIndicator == length - recordIdentifierLength)
			return true;
	}
	return false;
}

bool CMReaderWriterHandsFreeDevice::finishedReadingRecord(const HobString &msg)
{
	UINT32 messageLengthIndicator = 0;
	if (msg.length() >= (size_t) recordIdentifierLength)
	{
		string lengthStr = msg.substr(0, recordIdentifierLength);
		size_t messageLen(ntohl(reinterpret_cast<const uint32_t*>(lengthStr.c_str())[0]));
		if (messageLen == msg.length() - recordIdentifierLength)
			return true;
	}
	return false;
}


/*
 */
int CMReaderWriterHandsFreeDevice::formatFromReceive(char *msg, int length, bool /*isRecordBeginningBuffer*/)
{
	if (!msg || length <= 0)
		return 0;

	int finalLength = length;

	return finalLength;
}

int CMReaderWriterHandsFreeDevice::formatFromReceive(HobString &msg)
{
	if (!msg.size())
		return 0;

	int finalLength = (int)msg.size();

	string a = msg.substr(recordIdentifierLength);
	size_t l = 0;
	message *m = messageFactory::create(a.c_str(), a.length(), &l);

	return finalLength;
}
