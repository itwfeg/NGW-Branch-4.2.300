/*
 * PrinterHeadDotStatus.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "PrinterHeadDotStatus.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("PrinterHeadDotStatus");
#endif

PrinterHeadDotStatus::PrinterHeadDotStatus( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, 
	                                        uint16_t source, uint16_t destination, uint8_t status) : PrinterGeneric( PrHeadDotStatus, pHobThread, printerType )
{
	this->source = source;
	this->destination = destination;
    this->status = status;
}


PrinterHeadDotStatus::PrinterHeadDotStatus(const char *buffer) : PrinterGeneric(buffer){
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

    pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PrinterHeadDotStatus::~PrinterHeadDotStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *PrinterHeadDotStatus::serialize() const
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

    pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterHeadDotStatus::show(){
	bool isTracing = Debug::isTracing("PrinterHeadDotStatus");

	if (isTracing == false) {
		return;
	}
	Debug::trace("PrinterHeadDotStatus", "PrinterHeadDotStatus: ");

	PrinterGeneric::show();

	Debug::trace("PrinterHeadDotStatus",
			HobString("Source: ") + HobString(Integer::toString(getSource())));
    Debug::trace("PrinterHeadDotStatus",
			HobString("Destination: ") + HobString(Integer::toString(getDestination())));


	if (isTracing == false) {
		Debug::removeTraceCategory("PrinterHeadDotStatus");
	}
}
