/* Fastrack Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "NetworkStatus.h"
#include "NetworkConnectivityEvents.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NetworkStatus::dataType = 0;
std::string NetworkStatus::dataTypeName("NetworkStatus"); 
#endif


NetworkStatus::NetworkStatus(Hobart::CEvolMsgReceiver *pReceiver, bool online, int networkType, int wirelessStrength)
	: EvolMessage(NetworkConnectivityDev, CurrentStatus, pReceiver),
	  online_(online), networkType_(networkType), wirelessStrength_(wirelessStrength)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

NetworkStatus::NetworkStatus(const char *buffer)
	: EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &online_;
	for (unsigned int i = 0; i < sizeof(online_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &networkType_;
	for (unsigned int i = 0; i < sizeof(networkType_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wirelessStrength_;
	for (unsigned int i = 0; i < sizeof(wirelessStrength_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NetworkStatus::~NetworkStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *NetworkStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &online_;
	for (unsigned int i = 0; i < sizeof(online_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &networkType_;
	for (unsigned int i = 0; i < sizeof(networkType_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &wirelessStrength_;
	for (unsigned int i = 0; i < sizeof(wirelessStrength_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
