
#include "../Include/ScannerProductRead.h"
#include "BarcodeScannerEvent.h"
#include "Debug.h"
#include "Hex.h"
#include "Integer.h"

#include <string>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ScannerProductRead::dataType = 0;
std::string ScannerProductRead::dataTypeName("ScannerProductRead"); 
#endif

ScannerProductRead::ScannerProductRead(Hobart::CEvolMsgReceiver *pHobThread,
                                                 const std::string& scancode) : EvolMessage(BarcodeScannerDev, BarcodeReadScannerCode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	scancode_.append(scancode);
	
	Debug::trace("ScannerProductRead", "Constructor 1 scancode_: " + scancode_ );
}

ScannerProductRead::ScannerProductRead(const ScannerProductRead& newSPR) : EvolMessage(newSPR)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	scancode_ = newSPR.getScanCode();
	Debug::trace("ScannerProductRead", "Constructor 2 scancode_: " + scancode_ );
}

ScannerProductRead::ScannerProductRead(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();	

	std::string strbuf(buffer, (MAX_SCANNED_HEADER_STRING + MAX_SCANNED_CODE_STRING) );
	std::string scancode_ = strbuf.substr(MAX_SCANNED_HEADER_STRING, MAX_SCANNED_CODE_STRING);	// get the code off the tail

	Debug::trace("ScannerProductRead", "Constructor 3: " + scancode_ );
}

ScannerProductRead::ScannerProductRead() : EvolMessage()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ScannerProductRead", "Constructor 4: " );
}

ScannerProductRead::~ScannerProductRead()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ScannerProductRead::serialize() const
{
	EvolMessage::serialize();

	serializedData.append(scancode_);

	return serializedData.data();
}

const char *ScannerProductRead::serialize(int justcode) const
{
	// Is this called? By who?
	justcode = justcode + 42;

	EvolMessage::serialize();

	serializedData.append(scancode_);

	return serializedData.data();
}

void ScannerProductRead::show()
{
	bool isTracing = Debug::isTracing("ScannerProductRead.show");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("ScannerProductRead") );
#else
		Debug::addTraceCategory("ScannerProductRead");
#endif
	}

	EvolMessage::show();

    std::string msg("scan = ");
    Hex::nibblize(msg, scancode_.data(), (int)scancode_.size(), true);

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("ScannerProductRead") );
#else
		Debug::removeTraceCategory("ScannerProductRead");
#endif
	}
}