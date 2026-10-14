/*
 * KPCBootUpgraderAck.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "KPCBootUpgraderAck.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootUpgraderAck");
#endif

KPCBootUpgraderAck::KPCBootUpgraderAck(const KPCBootUpgraderAck& newBAck) : EvolMessage(newBAck){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	source = newBAck.getSource();
    destination = newBAck.getDestination();
    ackedEvent = newBAck.getAckedEvent();
}

KPCBootUpgraderAck::KPCBootUpgraderAck(const char *buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &source;
	for (unsigned int i = 0; i < sizeof(source); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &destination;
	for (unsigned int i = 0; i < sizeof(destination); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &ackedEvent;
	for (unsigned int i = 0; i < sizeof(ackedEvent); i++)
	{
		*pChar++ = buffer[j++];
	}

}

KPCBootUpgraderAck::KPCBootUpgraderAck(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( KPCBootUpdater, KpcBootUpgraderAck, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	source = 0;
    destination = 0;
    ackedEvent = KpcNoUpgraderEvent;
}


KPCBootUpgraderAck::~KPCBootUpgraderAck()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderAck::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &source;
	for (unsigned int i = 0; i < sizeof(source); i++) {
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &destination;
	for (unsigned int i = 0; i < sizeof(destination); i++) {
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &ackedEvent;
	for (unsigned int i = 0; i < sizeof(ackedEvent); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootUpgraderAck::show(){
	bool isTracing = Debug::isTracing("KPCBootUpgraderAck");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootUpgraderAck", "KPCBootUpgraderAck: ");

	EvolMessage::show();

	Debug::trace("KPCBootUpgraderAck",
			HobString("Source: ") + HobString(Integer::toString(getSource())));
    Debug::trace("KPCBootUpgraderAck",
			HobString("Destination: ") + HobString(Integer::toString(getDestination())));
    Debug::trace("KPCBootUpgraderAck",
			HobString("AckedEvent: ") + HobString(Integer::toString(getAckedEvent())));


	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootUpgraderAck");
	}
}

bool KPCBootUpgraderAck::equals(const KPCBootUpgraderAck &KpcBootUpgraderAck) const{
	return ((source == KpcBootUpgraderAck.getSource()) &&
                (destination == KpcBootUpgraderAck.getDestination())&&
                (ackedEvent == KpcBootUpgraderAck.getAckedEvent()) );
}

uint8_t KPCBootUpgraderAck::getSource() const{
	return source;
}

void KPCBootUpgraderAck::setSource(uint8_t newSource){
	source = newSource;
}

uint8_t KPCBootUpgraderAck::getDestination() const{
	return destination;
}

void KPCBootUpgraderAck::setDestination(uint8_t newDestination){
	destination = newDestination;
}

KPCBootUpgraderEvent KPCBootUpgraderAck::getAckedEvent() const{
	return ackedEvent;
}

void KPCBootUpgraderAck::setAckedEvent(KPCBootUpgraderEvent newAckedEvent){
	ackedEvent = newAckedEvent;
}