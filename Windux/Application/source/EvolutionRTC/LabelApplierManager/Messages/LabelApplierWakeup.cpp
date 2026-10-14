//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierWakeup.h"

#include "Debug.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierWakeup::dataType = 0;
std::string LabelApplierWakeup::dataTypeName("LabelApplierWakeup"); 
#endif

LabelApplierWakeup::LabelApplierWakeup(Hobart::CEvolMsgReceiver *pHobThread,
									   bool readyToRun) : EvolMessage(LabelApplier, LaWakeup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->readyToRun = readyToRun;
}

LabelApplierWakeup::LabelApplierWakeup(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &readyToRun;
	for (unsigned int i = 0; i < sizeof(readyToRun); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierWakeup::~LabelApplierWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierWakeup::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &readyToRun;
	for (unsigned int i = 0; i < sizeof(readyToRun); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierWakeup::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierWakeup") );
#else
	Debug::addTraceCategory("LabelApplierWakeup");
#endif
	Debug::trace("LabelApplierWakeup", "LabelApplierWakeup: ");

	EvolMessage::show();

	Debug::trace("LabelApplierWakeup", HobString("readyToRun: ") +
									   HobString(Bool::toString(readyToRun)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierWakeup") );
#else
	Debug::removeTraceCategory("LabelApplierWakeup");
#endif
}

bool LabelApplierWakeup::getReadyToRun()
{
	return readyToRun;
}

void LabelApplierWakeup::setReadyToRun(bool readyToRun)
{
	this->readyToRun = readyToRun;
}
