//#include "vxWorks.h"
#include "evolution.h"
#include "UsbRemoveComponent.h"
#include "VersionManagerEvent.h"
#include "HobVersion.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UsbRemoveComponent::dataType = 0;
std::string UsbRemoveComponent::dataTypeName("UsbRemoveComponent"); 
#endif

UsbRemoveComponent::UsbRemoveComponent(USBD_NODE_ID nodeId, Hobart::CEvolMsgReceiver *pHobThread) : 
		EvolMessage(VersionManagerDev, RemoveUsbComponent, pHobThread), nodeId_(nodeId)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

UsbRemoveComponent::UsbRemoveComponent(const char *buffer) : EvolMessage(buffer)
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


}

UsbRemoveComponent::~UsbRemoveComponent()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *UsbRemoveComponent::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items

	const char *pChar = (char *) & nodeId_;
	for (unsigned int i = 0; i < sizeof(nodeId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void UsbRemoveComponent::show()
{
	EvolMessage::show();

}
