/*
 * KPCBootUpgraderFlush.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "KPCBootUpgraderFlush.h"
#include "KpcBootUpgraderEvent.h"
#include "Event.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootUpgraderFlush");
#endif

KPCBootUpgraderFlush::KPCBootUpgraderFlush(const KPCBootUpgraderFlush& newBFlush) : EvolMessage(newBFlush){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

KPCBootUpgraderFlush::KPCBootUpgraderFlush(const char *buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
}

KPCBootUpgraderFlush::KPCBootUpgraderFlush(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( KPCBootUpdater, KpcBootUpgraderFlush, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


KPCBootUpgraderFlush::~KPCBootUpgraderFlush()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderFlush::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	return serializedData.data();
}

void KPCBootUpgraderFlush::show(){
	bool isTracing = Debug::isTracing("KPCBootUpgraderFlush");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootUpgraderFlush", "KPCBootUpgraderFlush: ");

	EvolMessage::show();



	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootUpgraderFlush");
	}
}

bool KPCBootUpgraderFlush::equals(const KPCBootUpgraderFlush &kpcBootUpgraderFlush) const{
	return (true); //all flush commands are the same
}

