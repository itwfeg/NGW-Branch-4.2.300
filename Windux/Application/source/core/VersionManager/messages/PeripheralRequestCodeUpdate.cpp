//#include "vxWorks.h"
#include "PeripheralRequestCodeUpdate.h"
#include "VersionManagerEvent.h"
#include "HobVersion.h"
#include "Integer.h"
#if defined(LINUX_FIX)
#include "ULong.h"
#endif
#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int PeripheralRequestCodeUpdate::dataType = 0;
std::string PeripheralRequestCodeUpdate::dataTypeName("PeripheralRequestCodeUpdate"); 
#endif

PeripheralRequestCodeUpdate::PeripheralRequestCodeUpdate(USBD_NODE_ID nodeId, const std::string &hardwareVersion,
														 const std::string &softwareVersion,
														 const int failureCnt,
														 Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev,
														 									  PeripheralReqUpdateCode,
														 									  pHobThread), nodeId_(nodeId),
																							  failureCount_(failureCnt)

{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->hardwareVer_ = new HobVersion(hardwareVersion);
	this->softwareVer_ = new HobVersion(softwareVersion);

}

PeripheralRequestCodeUpdate::PeripheralRequestCodeUpdate(USBD_NODE_ID nodeId, const HobVersion &hardwareVersion,
														 const HobVersion &softwareVersion,
														 const int failureCnt,
														 Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev,
														 									  PeripheralReqUpdateCode,
														 									  pHobThread), nodeId_(nodeId),
																							  failureCount_(failureCnt)

{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->hardwareVer_ = new HobVersion(hardwareVersion);
	this->softwareVer_ = new HobVersion(softwareVersion);

}

PeripheralRequestCodeUpdate::PeripheralRequestCodeUpdate(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId_;
	for (UINT16 i = 0; i < sizeof(nodeId_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &failureCount_;
	for (UINT16 i = 0; i < sizeof(failureCount_); i++)
	{
		*pChar++ = buffer[j++];
	}

	hardwareVer_ = new HobVersion(&buffer[j]);

	hardwareVer_->serialize();
	j += hardwareVer_->getSerializedLength();

	softwareVer_ = new HobVersion(&buffer[j]);
}

PeripheralRequestCodeUpdate::~PeripheralRequestCodeUpdate()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	if (hardwareVer_ != NULL)
		delete (hardwareVer_);

	if (softwareVer_ != NULL)
		delete (softwareVer_);
}

const char *PeripheralRequestCodeUpdate::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	const char *pChar = (char *) & nodeId_;
	for (unsigned int i = 0; i < sizeof(nodeId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) & failureCount_;
	for (unsigned int i = 0; i < sizeof(failureCount_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pHardwareVersion = hardwareVer_->serialize();
	serializedData.append(pHardwareVersion, hardwareVer_->getSerializedLength());

	const char *pSoftwareVersion = softwareVer_->serialize();
	serializedData.append(pSoftwareVersion, softwareVer_->getSerializedLength());

	return serializedData.data();
}

const HobVersion *PeripheralRequestCodeUpdate::getHardwareVersion() const
{
	return hardwareVer_;
}

void PeripheralRequestCodeUpdate::setHardwareVersion(HobVersion &hardwareVer_)
{
	if (this->hardwareVer_ != NULL)
		delete (this->hardwareVer_);
	this->hardwareVer_ = new HobVersion(hardwareVer_);
}

const HobVersion *PeripheralRequestCodeUpdate::getSoftwareVersion() const
{
	return softwareVer_;
}

void PeripheralRequestCodeUpdate::setSoftwareVersion(HobVersion &softwareVer_)
{
	if (this->softwareVer_ != NULL)
		delete (this->softwareVer_);
	this->softwareVer_ = new HobVersion(softwareVer_);
}

void PeripheralRequestCodeUpdate::show()
{

	Debug::trace("PeripheralRequestCodeUpdate", "PeripheralRequestCodeUpdate: ");

	EvolMessage::show();

#if defined(LINUX_FIX)
    Debug::trace("PeripheralRequestCodeUpdate", "nodeId_: " + ULong::toString( nodeId_ ));
#else
    Debug::trace("PeripheralRequestCodeUpdate", "nodeId_: " + Integer::toString((int)nodeId_));
#endif
	Debug::trace("PeripheralRequestCodeUpdate", "failureCount_: " + Integer::toString(failureCount_));

	Debug::trace("PeripheralRequestCodeUpdate", "hardwareVer_: " + hardwareVer_->getRevisionString());
	Debug::trace("PeripheralRequestCodeUpdate", "softwareVer_: " + softwareVer_->getRevisionString());

}
