//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemMode.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemMode::_dataType_ = 0;
std::string SystemMode::_dataTypeName_("SystemMode"); 
#endif

SystemMode::SystemMode(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, SystemModeType systemMode) : EvolMessage(evolDevType, SysMode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->systemMode = systemMode;
}

SystemMode::SystemMode(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &systemMode;
	for (unsigned int i = 0; i < sizeof(systemMode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

SystemMode::~SystemMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *SystemMode::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &systemMode;
	for (unsigned int i = 0; i < sizeof(systemMode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

bool SystemMode::isSystemOperateMode()
{
	return (systemMode == SystemOperateMode);
}

void SystemMode::setSystemOperateMode()
{
	systemMode = SystemOperateMode;
}

bool SystemMode::isSystemTestMode()
{
	return (systemMode == SystemTestMode);
}

void SystemMode::setSystemTestMode()
{
	systemMode = SystemTestMode;
}
