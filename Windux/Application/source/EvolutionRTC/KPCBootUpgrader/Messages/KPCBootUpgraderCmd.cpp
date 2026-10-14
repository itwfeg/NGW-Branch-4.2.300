/*
 * KPCBootUpgraderCmd.cpp
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#include "KPCBootUpgraderCmd.h"
#include "KpcBootUpgraderEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootUpgraderCmd");
#endif

KPCBootUpgraderCmd::KPCBootUpgraderCmd(const KPCBootUpgraderCmd& newBCmd) : EvolMessage(newBCmd){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	command = newBCmd.getCommand();
	sourceId = newBCmd.getSourceId();
	destination = newBCmd.getDestination();
}

KPCBootUpgraderCmd::KPCBootUpgraderCmd(const char *buffer) : EvolMessage(buffer){
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

}

KPCBootUpgraderCmd::KPCBootUpgraderCmd(Hobart::CEvolMsgReceiver *pHobThread, UINT8 destination, UINT8 source) : EvolMessage( KPCBootUpdater, KpcBootUpgraderCmd, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	command = NO_UPGRADER_CMD;
	this->destination = destination;
	this->sourceId = source;
}


KPCBootUpgraderCmd::~KPCBootUpgraderCmd()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderCmd::serialize() const
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


	return serializedData.data();
}

void KPCBootUpgraderCmd::show(){
	bool isTracing = Debug::isTracing("KPCBootUpgraderCmd");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootUpgraderCmd", "KPCBootUpgraderCmd: ");

	EvolMessage::show();

	Debug::trace("KPCBootUpgraderCmd",
			HobString("Command: ") + HobString(Integer::toString(getCommand())));
	Debug::trace("KPCBootUpgraderCmd",
			HobString("Destination: ") + HobString(Integer::toString(getDestination())));
	Debug::trace("KPCBootUpgraderCmd",
			HobString("Source: ") + HobString(Integer::toString(getSourceId())));

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootUpgraderCmd");
	}
}

bool KPCBootUpgraderCmd::equals(const KPCBootUpgraderCmd &kpcBootCmd) const{
	return (command == kpcBootCmd.getCommand() &&
			sourceId == kpcBootCmd.getSourceId() &&
			destination == kpcBootCmd.getDestination());
}

BootUpgraderCmd KPCBootUpgraderCmd::getCommand() const{
	return command;
}
void KPCBootUpgraderCmd::setCommand(BootUpgraderCmd newCommand){
	command = newCommand;
}

