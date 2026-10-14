//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemWakeup.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemWakeup::_dataType_ = 0;
std::string SystemWakeup::_dataTypeName_("SystemWakeup"); 
#endif

SystemWakeup::SystemWakeup(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, SysCfgStatus sysCfgStatus) : EvolMessage(evolDevType, SysWakeup, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->sysCfgStatus = sysCfgStatus;
}

SystemWakeup::SystemWakeup(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &sysCfgStatus;
	for (unsigned int i = 0; i < sizeof(sysCfgStatus); i++)
	{
		*pChar++ = buffer[j++];
	}
}

SystemWakeup::~SystemWakeup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *SystemWakeup::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sysCfgStatus;
	for (unsigned int i = 0; i < sizeof(sysCfgStatus); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

SysCfgStatus SystemWakeup::getSysCfgStatus()
{
	return sysCfgStatus;
}

void SystemWakeup::setSysCfgStatus(SysCfgStatus sysCfgStatus)
{
	this->sysCfgStatus = sysCfgStatus;
}

