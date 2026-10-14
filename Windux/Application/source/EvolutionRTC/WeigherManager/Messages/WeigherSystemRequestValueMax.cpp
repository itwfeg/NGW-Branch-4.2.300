/*
 * WeigherSystemRequestValueMax.cpp
 *
 *  Created on: January 31, 2020
 *      Author: Doug Antonides
 */

#include "WeigherEvent.h"
#include "WeigherSystemRequestValueMax.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherSystemRequestValueMax::dataType = 0;
std::string WeigherSystemRequestValueMax::dataTypeName("WeigherSystemRequestInquire");
#endif

WeigherSystemRequestValueMax::WeigherSystemRequestValueMax(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Weigher, WgReqValueMax, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	_fitParamRequested = 0;
}

WeigherSystemRequestValueMax::WeigherSystemRequestValueMax(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *)&_fitParamRequested;
	for (unsigned int i = 0; i < sizeof(_fitParamRequested); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherSystemRequestValueMax::~WeigherSystemRequestValueMax()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WeigherSystemRequestValueMax::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *)&_fitParamRequested;
	for (unsigned int i = 0; i < sizeof(_fitParamRequested); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}
