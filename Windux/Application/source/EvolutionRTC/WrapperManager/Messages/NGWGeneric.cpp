/*
 * NGWGeneric.cpp
 *
 *  Created on: Aug 2, 2016
 *      Author: aaron
 */

#include "NGWGeneric.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NGWGeneric::dataType = 0;
std::string NGWGeneric::dataTypeName("NGWGeneric");
#endif

NGWGeneric::NGWGeneric(NGWEvents ngwEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(NGWrapper, (UINT8)ngwEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

NGWGeneric::NGWGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

NGWGeneric::~NGWGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}



