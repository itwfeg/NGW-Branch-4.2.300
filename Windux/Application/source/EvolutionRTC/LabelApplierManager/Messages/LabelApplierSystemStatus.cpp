//#include "vxWorks.h"
#include "SystemStatusInfo.h"
#include "LabelApplierSystemStatus.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSystemStatus::dataType = 0;
std::string LabelApplierSystemStatus::dataTypeName("LabelApplierSystemStatus"); 
#endif

LabelApplierSystemStatus::LabelApplierSystemStatus(Hobart::CEvolMsgReceiver *pHobThread,
												   SystemStatusInfo *masterStatus, 
												   SystemStatusInfo *slave1Status, 
												   SystemStatusInfo *slave2Status) : SystemStatus(LabelApplierSystem, pHobThread, masterStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// MasterStatus is taken care in the base class!!

	if (slave1Status == NULL)
	{
		this->slave1Status = new SystemStatusInfo();
	}
	else
	{
		this->slave1Status = new SystemStatusInfo(slave1Status->getBitTestAddress(),
												  slave1Status->getAddressTestAddress(),
												  slave1Status->getROM_Checksum(),
												  slave1Status->getFlags(),
												  slave1Status->getPartNumber(),
												  slave1Status->getSystemChecksum(),
												  slave1Status->getInitFlags());
	}

	if (slave2Status == NULL)
	{
		this->slave2Status = new SystemStatusInfo();
	}
	else
	{
		this->slave2Status = new SystemStatusInfo(slave2Status->getBitTestAddress(),
												  slave2Status->getAddressTestAddress(),
												  slave2Status->getROM_Checksum(),
												  slave2Status->getFlags(),
												  slave2Status->getPartNumber(),
												  slave2Status->getSystemChecksum(),
												  slave2Status->getInitFlags());
	}
}

LabelApplierSystemStatus::LabelApplierSystemStatus(const char *buffer) : SystemStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemStatus::serialize();

	int j = getSerializedLength();

	// MasterStatus is taken care in the base class!!

	// Now, for the derived class items
	slave1Status = new SystemStatusInfo(&buffer[j]);

	slave1Status->serialize();
	j += slave1Status->getSerializedLength();

	slave2Status = new SystemStatusInfo(&buffer[j]);

	slave2Status->serialize();
	j += slave2Status->getSerializedLength();
}

LabelApplierSystemStatus::~LabelApplierSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	delete slave2Status;
	delete slave1Status;

	// base class will delete systemStatusInfo
//	delete systemStatusInfo;
}

const char *LabelApplierSystemStatus::serialize() const
{
	SystemStatus::serialize();

	// MasterStatus is taken care in the base class!!

	const char *pCC = slave1Status->serialize();
	int mLength = slave1Status->getSerializedLength();

	serializedData.append(pCC, mLength);
	
	pCC = slave2Status->serialize();
	mLength = slave2Status->getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

void LabelApplierSystemStatus::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierSystemStatus") );
#else
	Debug::addTraceCategory("LabelApplierSystemStatus");
#endif
	Debug::trace("LabelApplierSystemStatus", "LabelApplierSystemStatus: ");

	Debug::trace("LabelApplierSystemStatus", "Master Status: ");
	SystemStatus::show();

	Debug::trace("LabelApplierSystemStatus", "Slave 1 Status: ");
	slave1Status->show();

	Debug::trace("LabelApplierSystemStatus", "Slave 2 Status: ");
	slave2Status->show();

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierSystemStatus") );
#else
	Debug::removeTraceCategory("LabelApplierSystemStatus");
#endif
}

const SystemStatusInfo *LabelApplierSystemStatus::getMasterStatus() const
{
	// Use base classes functionality!
	return SystemStatus::getSystemStatusInfo();
}

void LabelApplierSystemStatus::setMasterStatus(SystemStatusInfo *masterStatus)
{
	// Use base classes functionality!
	SystemStatus::setSystemStatusInfo(masterStatus);
}

const SystemStatusInfo *LabelApplierSystemStatus::getSlave1Status() const
{
	return (const SystemStatusInfo *) slave1Status;
}

void LabelApplierSystemStatus::setSlave1Status(SystemStatusInfo *slave1Status)
{
	if (slave1Status == NULL)
	{
		this->slave1Status = new SystemStatusInfo();
	}
	else
	{
		this->slave1Status = new SystemStatusInfo(slave1Status->getBitTestAddress(),
												  slave1Status->getAddressTestAddress(),
												  slave1Status->getROM_Checksum(),
												  slave1Status->getFlags(),
												  slave1Status->getPartNumber(),
												  slave1Status->getSystemChecksum(),
												  slave1Status->getInitFlags());
	}
}

const SystemStatusInfo *LabelApplierSystemStatus::getSlave2Status() const
{
	return (const SystemStatusInfo *) slave2Status;
}

void LabelApplierSystemStatus::setSlave2Status(SystemStatusInfo *slave2Status)
{
	if (slave2Status == NULL)
	{
		this->slave2Status = new SystemStatusInfo();
	}
	else
	{
		this->slave2Status = new SystemStatusInfo(slave2Status->getBitTestAddress(),
												  slave2Status->getAddressTestAddress(),
												  slave2Status->getROM_Checksum(),
												  slave2Status->getFlags(),
												  slave2Status->getPartNumber(),
												  slave2Status->getSystemChecksum(),
												  slave2Status->getInitFlags());
	}
}

