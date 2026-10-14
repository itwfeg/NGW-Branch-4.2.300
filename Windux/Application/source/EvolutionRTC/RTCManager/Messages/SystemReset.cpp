//#include "vxWorks.h"
#include "evolution.h"
#include "SystemReset.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemReset::_dataType_ = 0;
std::string SystemReset::_dataTypeName_("SystemReset"); 
#endif

SystemReset::SystemReset(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, USBD_NODE_ID usbNodeId) : SystemGeneric(evolDevType, SysReset, pHobThread)
{
	this->usbNodeId = usbNodeId;

#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}

SystemReset::SystemReset(Hobart::CEvolMsgReceiver *pHobThread, USBD_NODE_ID usbNodeId) : SystemGeneric(System, SysReset, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->usbNodeId = usbNodeId;
}

SystemReset::SystemReset(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	// index into buffer
	SystemGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &usbNodeId;
	for (unsigned int i = 0; i < sizeof(usbNodeId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

SystemReset::~SystemReset()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *SystemReset::serialize() const
{
	SystemGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &usbNodeId;
	for (unsigned int i = 0; i < sizeof(usbNodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void SystemReset::show()
{
	bool isTracing = Debug::isTracing("SystemReset");

	if (isTracing == false)
	{
		Debug::addTraceCategory( std::string("SystemReset") );
	}

	Debug::trace("SystemReset", "SystemReset: ");

	SystemGeneric::show();

	Debug::trace("SystemReset", HobString("usbNodeId: ") + HobString(Hex::toString((long long) usbNodeId)));

	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("SystemReset") );
	}
}

void SystemReset::setUsbNodeId(USBD_NODE_ID usbNodeId)
{
	this->usbNodeId = usbNodeId;
}

USBD_NODE_ID SystemReset::getUsbNodeId()
{
	return usbNodeId;
}

