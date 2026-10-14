/*
 * KPCBootReady.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: hobart
 */
#include "KPCBootReady.h"
#include "KpcBootEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootReady"); 
#endif

KPCBootReady::KPCBootReady(const KPCBootReady& newKR) : EvolMessage(newKR)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	source = newKR.getSource();
	destination = newKR.getDestination();
}

KPCBootReady::KPCBootReady(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
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
}

KPCBootReady::KPCBootReady(Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( KPCBootloader, KpcBootReady, pHobThread )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	source = 0;
	destination = 0;
}

KPCBootReady::~KPCBootReady()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootReady::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &source;
	for (unsigned int i = 0; i < sizeof(source); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &destination;
	for (unsigned int i = 0; i < sizeof(destination); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootReady::show(){

	bool isTracing = Debug::isTracing("KPCBootReady");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootReady", "KPCBootReady: ");

	EvolMessage::show();

	Debug::trace("KPCBootReady",
			HobString("Source: ") + HobString(Integer::toString(getSource())));
	Debug::trace("KPCBootReady",
			HobString("Destination: ")
					+ HobString(Integer::toString(getDestination())));

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootReady");
	}
}

bool KPCBootReady::equals(const KPCBootReady &kpcBootReady) const{
return false;
//TODO:STUBB
}

uint16_t KPCBootReady::getSource() const{
	return source;
}

void KPCBootReady::setSource(uint16_t newSource){
	source = newSource;
}

uint16_t KPCBootReady::getDestination() const{
	return destination;
}

void KPCBootReady::setDestination(uint16_t newDestination){
	destination = newDestination;
}
