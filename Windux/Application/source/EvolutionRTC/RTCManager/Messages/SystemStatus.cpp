//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemStatus.h"
#include "SystemStatusInfo.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemStatus::_dataType_ = 0;
std::string SystemStatus::_dataTypeName_("SystemStatus"); 
#endif

SystemStatus::SystemStatus(EvolDeviceType evolDevType,
						   Hobart::CEvolMsgReceiver *pHobThread,
						   SystemStatusInfo *systemStatusInfo) : EvolMessage(evolDevType, SysStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (systemStatusInfo == NULL)
	{
		this->systemStatusInfo = new SystemStatusInfo();
	}
	else
	{
		this->systemStatusInfo = new SystemStatusInfo(systemStatusInfo->getBitTestAddress(),
												  	  systemStatusInfo->getAddressTestAddress(),
													  systemStatusInfo->getROM_Checksum(),
													  systemStatusInfo->getFlags(),
													  systemStatusInfo->getPartNumber(),
													  systemStatusInfo->getSystemChecksum(),
													  systemStatusInfo->getInitFlags());
	}
}

SystemStatus::SystemStatus(const char *buffer) : EvolMessage(buffer)
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
	systemStatusInfo = new SystemStatusInfo(&buffer[j]);
}

SystemStatus::~SystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	delete systemStatusInfo;
}

const char *SystemStatus::serialize() const
{
	EvolMessage::serialize();

	const char *pCC = systemStatusInfo->serialize();
	int mLength = systemStatusInfo->getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

void SystemStatus::show() const
{
	Debug::addTraceCategory( std::string("SystemStatus") );
	Debug::trace("SystemStatus", "SystemStatus: ");

	systemStatusInfo->show();

	Debug::removeTraceCategory( std::string("SystemStatus") );
}

const SystemStatusInfo *SystemStatus::getSystemStatusInfo() const
{
	return (const SystemStatusInfo *) systemStatusInfo;
}

void SystemStatus::setSystemStatusInfo(SystemStatusInfo *systemStatusInfo)
{
	if (systemStatusInfo == NULL)
	{
		this->systemStatusInfo = new SystemStatusInfo();
	}
	else
	{
		this->systemStatusInfo = new SystemStatusInfo(systemStatusInfo->getBitTestAddress(),
												  	  systemStatusInfo->getAddressTestAddress(),
													  systemStatusInfo->getROM_Checksum(),
													  systemStatusInfo->getFlags(),
													  systemStatusInfo->getPartNumber(),
													  systemStatusInfo->getSystemChecksum(),
													  systemStatusInfo->getInitFlags());
	}
}
