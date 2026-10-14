/*
 * KPCBootFlush.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "KPCBootFlush.h"
#include "KpcBootEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootFlush");
#endif

KPCBootFlush::KPCBootFlush(const KPCBootFlush& newBFlush) : EvolMessage(newBFlush){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

KPCBootFlush::KPCBootFlush(const char *buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
}

KPCBootFlush::KPCBootFlush(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( KPCBootloader, KpcBootFlush, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


KPCBootFlush::~KPCBootFlush()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootFlush::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	return serializedData.data();
}

void KPCBootFlush::show(){
	bool isTracing = Debug::isTracing("KPCBootFlush");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootFlush", "KPCBootFlush: ");

	EvolMessage::show();



	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootFlush");
	}
}

bool KPCBootFlush::equals(const KPCBootFlush &kpcBootFlush) const{
	return (true); //all flush commands are the same
}

