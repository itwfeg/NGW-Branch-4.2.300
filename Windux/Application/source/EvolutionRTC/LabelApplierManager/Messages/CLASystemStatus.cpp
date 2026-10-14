#include "CLASystemStatus.h"
#include "LabelApplierEvent.h"
//Alex Zavala 8/21
#include "Debug.h"
#include "Hex.h"
#include "Bool.h"
#include "Integer.h"
#include "Long.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLASystemStatus::_dataType_ = 0;
std::string CLASystemStatus::_dataTypeName_("CLASystemStatus"); 
#endif


CLASystemStatus::CLASystemStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(LabelApplier, LaSysStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	configurationChecksum_ = 0;

	appFlashMajor_ = 0;
	appFlashMinor_ = 0;

	dataFlashMajor_ = 0;
	dataFlashMinor_ = 0;

	bootFlashMajor_ = 0;
	bootFlashMinor_ = 0;
}

CLASystemStatus::CLASystemStatus(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items

	/*	CLASysInfo	*/

	char *pChar = (char *) &configurationChecksum_;
	for (unsigned int i = 0; i < sizeof(configurationChecksum_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &appFlashMajor_;
	for (unsigned int i = 0; i < sizeof(appFlashMajor_); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &appFlashMinor_;
	for (unsigned int i = 0; i < sizeof(appFlashMinor_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dataFlashMajor_;
	for (unsigned int i = 0; i < sizeof(dataFlashMajor_); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &dataFlashMinor_;
	for (unsigned int i = 0; i < sizeof(dataFlashMinor_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &bootFlashMajor_;
	for (unsigned int i = 0; i < sizeof(bootFlashMajor_); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &bootFlashMinor_;
	for (unsigned int i = 0; i < sizeof(bootFlashMinor_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

CLASystemStatus::~CLASystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *CLASystemStatus::serialize()const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	/*	CLASysInfo	*/

	char *pChar = (char *) &configurationChecksum_;
	for (unsigned int i = 0; i < sizeof(configurationChecksum_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &appFlashMajor_;
	for (unsigned int i = 0; i < sizeof(appFlashMajor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &appFlashMinor_;
	for (unsigned int i = 0; i < sizeof(appFlashMinor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &dataFlashMajor_;
	for (unsigned int i = 0; i < sizeof(dataFlashMajor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &dataFlashMinor_;
	for (unsigned int i = 0; i < sizeof(dataFlashMinor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &bootFlashMajor_;
	for (unsigned int i = 0; i < sizeof(bootFlashMajor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &bootFlashMinor_;
	for (unsigned int i = 0; i < sizeof(bootFlashMinor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
void CLASystemStatus::show() const
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("CLASystemStatus") );
#else
	Debug::addTraceCategory("CLASystemStatus");
#endif
	
	Debug::trace("CLASystemStatus", "CLASystemStatus: ");

	/*	CLASysInfo	*/
	Debug::trace("CLASystemStatus", HobString("configurationChecksum_: ") +
		HobString(Long::toString(configurationChecksum_)));

	Debug::trace("CLASystemStatus", HobString("appFlashMajor_: ") +
		HobString(Integer::toString(appFlashMajor_)));
	Debug::trace("CLASystemStatus", HobString("appFlashMinor_: ") +
		HobString(Integer::toString(appFlashMinor_)));

	Debug::trace("CLASystemStatus", HobString("dataFlashMajor_: ") +
		HobString(Integer::toString(dataFlashMajor_)));
	Debug::trace("CLASystemStatus", HobString("dataFlashMinor_: ") +
		HobString(Integer::toString(dataFlashMinor_)));

	Debug::trace("CLASystemStatus", HobString("bootFlashMajor_: ") +
		HobString(Integer::toString(bootFlashMajor_)));
	Debug::trace("CLASystemStatus", HobString("bootFlashMinor_: ") +
		HobString(Integer::toString(bootFlashMinor_)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("CLASystemStatus") );
#else
	Debug::removeTraceCategory("CLASystemStatus");
#endif
}