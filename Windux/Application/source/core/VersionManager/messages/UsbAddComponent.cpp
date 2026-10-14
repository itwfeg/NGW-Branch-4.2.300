//#include "vxWorks.h"

#include "UsbAddComponent.h"
#include "VersionManagerEvent.h"
#include "HobVersion.h"

#include "Debug.h"
#include "HobString.h"
#if defined(LINUX_FIX)
#include "ULong.h"                      // Convert pointer to string
#else
#include "Hex.h"
#endif

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UsbAddComponent::dataType = 0;
std::string UsbAddComponent::dataTypeName("UsbAddComponent"); 
#endif

UsbAddComponent::UsbAddComponent(USBD_NODE_ID nodeId,
								 const HobVersion &hardwareVersion,
								 Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(VersionManagerDev, AddUsbComponent, pHobThread), nodeId_(nodeId)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->hardwareVer_ = new HobVersion(hardwareVersion);
}

UsbAddComponent::UsbAddComponent(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items

	char *pChar = (char *) &nodeId_;
	for (UINT16 i = 0; i < sizeof(nodeId_); i++)
	{
		*pChar++ = buffer[j++];
	}

	hardwareVer_ = new HobVersion(&buffer[j]);

//	hardwareVer_->serialize();
//	j += hardwareVer_->getSerializedLength();
}

UsbAddComponent::~UsbAddComponent()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (hardwareVer_ != NULL)
		delete (hardwareVer_);
}

const char *UsbAddComponent::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	const char *pChar = (char *) & nodeId_;
	for (unsigned int i = 0; i < sizeof(nodeId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pHardwareVersion = hardwareVer_->serialize();
	serializedData.append(pHardwareVersion, hardwareVer_->getSerializedLength());

	return serializedData.data();
}

void UsbAddComponent::show()
{
	bool isTracing = Debug::isTracing("UsbAddComponent");

	if (isTracing == false)
	{
    #if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("UsbAddComponent") );
    #else
		Debug::addTraceCategory("UsbAddComponent");
    #endif
	}
	
	Debug::trace("UsbAddComponent", "UsbAddComponent");

	EvolMessage::show();

#if defined(LINUX_FIX)
	Debug::trace("UsbAddComponent", "nodeId: " + ULong::toString( nodeId_) );
#else
	Debug::trace("UsbAddComponent", "nodeId: " + Hex::toString((UINT32) nodeId_));
#endif
	Debug::trace("UsbAddComponent", "hardwareVer_: " + hardwareVer_->getRevisionString());

	if (isTracing == false)
	{
    #if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("UsbAddComponent") );
    #else
		Debug::removeTraceCategory("UsbAddComponent");
    #endif
	}
}

const HobVersion *UsbAddComponent::getHardwareVersion() const
{
	return hardwareVer_;
}

void UsbAddComponent::setHardwareVersion(HobVersion &hardwareVer_)
{
	if (this->hardwareVer_ != NULL)
		delete (this->hardwareVer_);
	this->hardwareVer_ = new HobVersion(hardwareVer_);
}

