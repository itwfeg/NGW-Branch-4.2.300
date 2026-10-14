/*
 * KPCBootAck.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "KPCBootAck.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootAck");
#endif

KPCBootAck::KPCBootAck(const KPCBootAck& newBAck) : EvolMessage(newBAck){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	source = newBAck.getSource();
    destination = newBAck.getDestination();
    ackedEvent = newBAck.getAckedEvent();
}

KPCBootAck::KPCBootAck(const char *buffer) : EvolMessage(buffer){
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

KPCBootAck::KPCBootAck(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( KPCBootloader, KpcBootAck, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	source = 0;
    destination = 0;
    ackedEvent = KpcNoEvent;
}


KPCBootAck::~KPCBootAck()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootAck::serialize() const
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

void KPCBootAck::show(){
	bool isTracing = Debug::isTracing("KPCBootAck");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootAck", "KPCBootAck: ");

	EvolMessage::show();

	Debug::trace("KPCBootAck",
			HobString("Source: ") + HobString(Integer::toString(getSource())));
    Debug::trace("KPCBootAck",
			HobString("Destination: ") + HobString(Integer::toString(getDestination())));
    Debug::trace("KPCBootAck",
			HobString("AckedEvent: ") + HobString(Integer::toString(getAckedEvent())));


	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootAck");
	}
}

bool KPCBootAck::equals(const KPCBootAck &KpcBootAck) const{
	return ((source == KpcBootAck.getSource()) &&
                (destination == KpcBootAck.getDestination())&&
                (ackedEvent == KpcBootAck.getAckedEvent()) );
}

uint8_t KPCBootAck::getSource() const{
	return source;
}

void KPCBootAck::setSource(uint8_t newSource){
	source = newSource;
}

uint8_t KPCBootAck::getDestination() const{
	return destination;
}

void KPCBootAck::setDestination(uint8_t newDestination){
	destination = newDestination;
}

KPCBootEvent KPCBootAck::getAckedEvent() const{
	return ackedEvent;
}

void KPCBootAck::setAckedEvent(KPCBootEvent newAckedEvent){
	ackedEvent = newAckedEvent;
}