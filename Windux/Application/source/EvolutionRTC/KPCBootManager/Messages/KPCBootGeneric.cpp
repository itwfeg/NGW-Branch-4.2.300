/*
 * KPCBootGeneric.cpp
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#include "KPCBootGeneric.h"
#include "KpcBootEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootGeneric");
#endif

KPCBootGeneric::KPCBootGeneric(const KPCBootGeneric& newKG) : EvolMessage(newKG)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

KPCBootGeneric::KPCBootGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

KPCBootGeneric::KPCBootGeneric(Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( KPCBootloader, KpcBootGeneric, pHobThread )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

KPCBootGeneric::KPCBootGeneric(EvolDeviceType deviceType, KPCBootEvent eventType,
				Hobart::CEvolMsgReceiver * pSource=NULL ) : EvolMessage (deviceType, eventType, pSource){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


KPCBootGeneric::~KPCBootGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootGeneric::serialize() const
{
	EvolMessage::serialize();

	return serializedData.data();
}

void KPCBootGeneric::show(){

	bool isTracing = Debug::isTracing("KPCBootGeneric");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootGeneric", "KPCBootGeneric: ");

	EvolMessage::show();

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootGeneric");
	}
}



