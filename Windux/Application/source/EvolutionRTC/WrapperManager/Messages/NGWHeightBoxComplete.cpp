/*
 * NGWHeightBoxComplete.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */
#include "NGWHeightBoxComplete.h"
NGWHeightBoxComplete::NGWHeightBoxComplete(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrHeightBoxComplete, pHobThread)
{
	_wrapBoxIndex = 0;
	_heightBoxIndex = 0;
}


NGWHeightBoxComplete::NGWHeightBoxComplete(const char *buffer)
		: WrapperGeneric(buffer)
{
	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &_wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(_wrapBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_heightBoxIndex;
	for (unsigned int i = 0; i < sizeof(_heightBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

}


NGWHeightBoxComplete::~NGWHeightBoxComplete(){}

const char *NGWHeightBoxComplete::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &_wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(_wrapBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &_heightBoxIndex;
	for (unsigned int i = 0; i < sizeof(_heightBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}




