/*
 * PrinterRequestHeadDotWear.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "PrinterRequestHeadDotWear.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("PrinterRequestHeadDotWear");
#endif

PrinterRequestHeadDotWear::PrinterRequestHeadDotWear( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, uint16_t source, uint16_t destination) :
								  PrinterGeneric( PrReqHeadDotWear, pHobThread, printerType )
{
	this->source = source;
	this->destination = destination;
}


PrinterRequestHeadDotWear::PrinterRequestHeadDotWear(const char *buffer) : PrinterGeneric(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();
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

}


PrinterRequestHeadDotWear::~PrinterRequestHeadDotWear()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *PrinterRequestHeadDotWear::serialize() const
{

	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &source;
	for (unsigned int i = 0; i < sizeof(source); i++) {
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &destination;
	for (unsigned int i = 0; i < sizeof(destination); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterRequestHeadDotWear::show(){
	bool isTracing = Debug::isTracing("PrinterRequestHeadDotWear");

	if (isTracing == false) {
		return;
	}
	Debug::trace("PrinterRequestHeadDotWear", "PrinterRequestHeadDotWear: ");

	PrinterGeneric::show();

	Debug::trace("PrinterRequestHeadDotWear",
			HobString("Source: ") + HobString(Integer::toString(getSource())));
    Debug::trace("PrinterRequestHeadDotWear",
			HobString("Destination: ") + HobString(Integer::toString(getDestination())));


	if (isTracing == false) {
		Debug::removeTraceCategory("PrinterRequestHeadDotWear");
	}
}