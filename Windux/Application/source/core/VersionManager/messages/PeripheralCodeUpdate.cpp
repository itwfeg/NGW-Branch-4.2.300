//#include "vxWorks.h"
#include "PeripheralCodeUpdate.h"

#include "Debug.h"
#include "Integer.h"
#include "ULong.h"
#include "HobString.h"
#include "filesys/directory.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int PeripheralCodeUpdate::dataType = 0;
std::string PeripheralCodeUpdate::dataTypeName("PeripheralCodeUpdate"); 
#endif

PeripheralCodeUpdate::PeripheralCodeUpdate(USBD_NODE_ID nodeId,
										   SoftwareSystem::PeripheralVersionCheck pResponseType,
										   const HobComplexVersion& softwareVer,
										   Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev,
  																			    PeripheralUpdateCode,
  																			    pHobThread),
																	nodeId_(nodeId),
																	softwareVersion_(softwareVer),
  																   responseType_(pResponseType),
																   sourcePath_("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	HobComplexVersion::getExtractionDir(softwareVersion_.getVersionType(), sourcePath_, nodeId_);
}

PeripheralCodeUpdate::PeripheralCodeUpdate(const char *buffer) : EvolMessage(buffer),
																 sourcePath_("")
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

	pChar = (char *) &responseType_;
	for (UINT16 i = 0; i < sizeof(responseType_); i++)
	{
		*pChar++ = buffer[j++];
	}

	softwareVersion_ = HobComplexVersion(&buffer[j]);
	softwareVersion_.serialize();
	j += softwareVersion_.getSerializedLength();

	currentSoftwareVersion_ = HobComplexVersion(&buffer[j]);

	HobComplexVersion::getExtractionDir(softwareVersion_.getVersionType(), sourcePath_, nodeId_);
}

PeripheralCodeUpdate::~PeripheralCodeUpdate()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *PeripheralCodeUpdate::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	const char *pChar = (char *) & nodeId_;
	for (unsigned int i = 0; i < sizeof(nodeId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) & responseType_;
	for (unsigned int i = 0; i < sizeof(responseType_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = softwareVersion_.serialize();
	serializedData.append(pChar, softwareVersion_.getSerializedLength());

	pChar = currentSoftwareVersion_.serialize();
	serializedData.append(pChar, currentSoftwareVersion_.getSerializedLength());

	return serializedData.data();
}

void PeripheralCodeUpdate::show()
{
	bool isTracing = Debug::isTracing("PeripheralCodeUpdate");
	if (isTracing == false)
		return;

	Debug::trace("PeripheralCodeUpdate", "PeripheralCodeUpdate: ");

	EvolMessage::show();

    Debug::trace("PeripheralCodeUpdate", "nodeId_: " + ULong::toString( nodeId_ ));
	Debug::trace("PeripheralCodeUpdate", "softwareVersion_: " + softwareVersion_.getRevisionString());

	if (responseType_ == SoftwareSystem::NotAvailable)
	{
		Debug::trace("PeripheralCodeUpdate", "responseType_: NotAvailable");
	}
	else if (responseType_ == SoftwareSystem::AvailableUpgrade)
	{
		Debug::trace("PeripheralCodeUpdate", "responseType_: AvailableUpgrade");
	}
	else if (responseType_ == SoftwareSystem::AvailableInvalidHW)
	{
		Debug::trace("PeripheralCodeUpdate", "responseType_: AvailableInvalidHW");
	}
	else if (responseType_ == SoftwareSystem::AvailableSameVersion)
	{
		Debug::trace("PeripheralCodeUpdate", "responseType_: AvailableSameVersion");
	}
	else
	{
		Debug::trace("PeripheralCodeUpdate", "responseType_: Unknown " +
											 Integer::toString((UINT16) responseType_));
	}
}

HobVersion::VersionType PeripheralCodeUpdate::getVersionType()
{
	return softwareVersion_.getVersionType();
}

util_filesystem::Directory* PeripheralCodeUpdate::getPath()
{

	return &sourcePath_;
}

void PeripheralCodeUpdate::setComplexVersion(const HobComplexVersion &softwareVer)
{
	softwareVersion_ = softwareVer;
}

SoftwareSystem::PeripheralVersionCheck PeripheralCodeUpdate::getResponseType()
{
	return responseType_;
}

void PeripheralCodeUpdate::setResponseType(SoftwareSystem::PeripheralVersionCheck pResponseType)
{
	responseType_ = pResponseType;
}


void PeripheralCodeUpdate::setCurrentSoftwareVersion(const HobVersion &currVersion)
{
	currentSoftwareVersion_ = currVersion;	 
}

HobVersion& PeripheralCodeUpdate::getCurrentSoftwareVersion()
{
	return currentSoftwareVersion_;
}