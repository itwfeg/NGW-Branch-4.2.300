/*
 * KPCBootUpgraderGeneric.cpp
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#include "KPCBootUpgraderGeneric.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int KPCBootUpgraderGeneric::dataType = 0;
std::string KPCBootUpgraderGeneric::dataTypeName("KPCBootUpgraderGeneric");
#endif

KPCBootUpgraderGeneric::KPCBootUpgraderGeneric(const KPCBootUpgraderGeneric& newKG) : EvolMessage(newKG)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

KPCBootUpgraderGeneric::KPCBootUpgraderGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

KPCBootUpgraderGeneric::KPCBootUpgraderGeneric(Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( KPCBootUpdater, KpcBootUpgraderGeneric, pHobThread )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

KPCBootUpgraderGeneric::KPCBootUpgraderGeneric(EvolDeviceType deviceType, KPCBootUpgraderEvent eventType,
				Hobart::CEvolMsgReceiver * pSource=NULL ) : EvolMessage (deviceType, eventType, pSource){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


KPCBootUpgraderGeneric::~KPCBootUpgraderGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderGeneric::serialize() const
{
	EvolMessage::serialize();

	return serializedData.data();
}

void KPCBootUpgraderGeneric::show(){

	bool isTracing = Debug::isTracing("KPCBootUpgraderGeneric");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootUpgraderGeneric", "KPCBootUpgraderGeneric: ");

	EvolMessage::show();

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootUpgraderGeneric");
	}
}



