/*
 * NGWLabelSetup.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: aaron
 */
#include "NGWLabelSetup.h"
#include "WrapperEvent.h"

NGWLabelSetup::NGWLabelSetup(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Wrapper, WrLabelSetup, pHobThread)
{

}

NGWLabelSetup::NGWLabelSetup(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &labelSize;
	for (unsigned int i = 0; i < sizeof(labelSize); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &labelRotation;
	for (unsigned int i = 0; i < sizeof(labelRotation); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &labelLocation;
	for (unsigned int i = 0; i < sizeof(labelLocation); i++)
	{
		*pChar++ = buffer[j++];
	}


}
NGWLabelSetup::~NGWLabelSetup()
{

}

const char *NGWLabelSetup::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &labelSize;
	for (unsigned int i = 0; i < sizeof(labelSize); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &labelRotation;
	for (unsigned int i = 0; i < sizeof(labelRotation); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &labelLocation;
	for (unsigned int i = 0; i < sizeof(labelLocation); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}





