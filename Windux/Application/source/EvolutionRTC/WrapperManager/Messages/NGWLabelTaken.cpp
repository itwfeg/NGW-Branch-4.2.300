/*
 * NGWLabelTaken.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: aaron
 */
#include "NGWLabelTaken.h"
#include "WrapperEvent.h"

NGWLabelTaken::NGWLabelTaken(Hobart::CEvolMsgReceiver *pHobThread, UINT16 pkgid, NGWApplierStation Idstation   ) : EvolMessage(Wrapper, WrLabelTaken, pHobThread)
{
	packageId = pkgid;
	stationId = Idstation;
}

NGWLabelTaken::NGWLabelTaken(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWLabelTaken::~NGWLabelTaken()
{

}

const char *NGWLabelTaken::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}





