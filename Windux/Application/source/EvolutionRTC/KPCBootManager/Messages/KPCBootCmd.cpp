/*
 * KPCBootCmd.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "KPCBootCmd.h"
#include "KpcBootEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootCmd");
#endif

KPCBootCmd::KPCBootCmd(const KPCBootCmd& newBCmd) : EvolMessage(newBCmd){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	command = newBCmd.getCommand();
	destination = newBCmd.getDestination();
	sourceId = newBCmd.getSourceId();
	address = newBCmd.getAddress();
}

KPCBootCmd::KPCBootCmd(const char *buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &command;
	for (unsigned int i = 0; i < sizeof(command); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &destination;
	for (unsigned int i = 0; i < sizeof(destination); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sourceId;
	for (unsigned int i = 0; i < sizeof(sourceId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &address;
	for (unsigned int i = 0; i < sizeof(address); i++)
	{
		*pChar++ = buffer[j++];
	}

}

KPCBootCmd::KPCBootCmd(Hobart::CEvolMsgReceiver *pHobThread, UINT8 destination, UINT8 source) : EvolMessage( KPCBootloader, KpcBootCmd, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	command = NO_CMD;
	this->destination = destination;
	this->sourceId = source;
	this->address = 0x0;
}


KPCBootCmd::~KPCBootCmd()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootCmd::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &command;
	for (unsigned int i = 0; i < sizeof(command); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &destination;
	for (unsigned int i = 0; i < sizeof(destination); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sourceId;
	for (unsigned int i = 0; i < sizeof(sourceId); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &address;
	for (unsigned int i = 0; i < sizeof(address); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootCmd::show(){
	bool isTracing = Debug::isTracing("KPCBootCmd");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootCmd", "KPCBootCmd: ");

	EvolMessage::show();

	Debug::trace("KPCBootCmd",
			HobString("Command: ") + HobString(Integer::toString(getCommand())));
	Debug::trace("KPCBootCmd",
			HobString("Destination: ") + HobString(Integer::toString(getDestination())));
	Debug::trace("KPCBootCmd",
			HobString("Source: ") + HobString(Integer::toString(getSourceId())));
	Debug::trace("KPCBootCmd",
			HobString("Address: ") + HobString(Integer::toString(getAddress())));

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootCmd");
	}
}

bool KPCBootCmd::equals(const KPCBootCmd &kpcBootCmd) const{
	return (command == kpcBootCmd.getCommand());
}

BootCmd KPCBootCmd::getCommand() const{
	return command;
}
void KPCBootCmd::setCommand(BootCmd newCommand){
	command = newCommand;
}

