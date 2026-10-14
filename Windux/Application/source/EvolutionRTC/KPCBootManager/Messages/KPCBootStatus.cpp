/*
 * KPCBootStatus.cpp
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#include "KPCBootStatus.h"
#include "KpcBootEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootStatus");
#endif

KPCBootStatus::KPCBootStatus(const KPCBootStatus& newBStat) : EvolMessage(newBStat){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	pid = newBStat.getId();
	status = newBStat.getStatusCode();
	fault = newBStat.getBootloaderFaults();
	faultInfo = newBStat.getFaultInfo();

}

KPCBootStatus::KPCBootStatus(const char *buffer) : EvolMessage(buffer){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &pid;
	for (unsigned int i = 0; i < sizeof(pid); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &fault;
	for (unsigned int i = 0; i < sizeof(fault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &faultInfo;
	for (unsigned int i = 0; i < sizeof(faultInfo); i++)
	{
		*pChar++ = buffer[j++];
	}

}

KPCBootStatus::KPCBootStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( KPCBootloader, KpcBootStatus, pHobThread ){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	pid = 0;
	status = BOOT_NO_STATUS;
	fault = NOFAULT;
	faultInfo = 0;
}


KPCBootStatus::~KPCBootStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootStatus::serialize() const
{

	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &pid;
	for (unsigned int i = 0; i < sizeof(pid); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &fault;
	for (unsigned int i = 0; i < sizeof(fault); i++) {
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &faultInfo;
	for (unsigned int i = 0; i < sizeof(faultInfo); i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void KPCBootStatus::show() const{
	bool isTracing = Debug::isTracing("KPCBootStatus");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootStatus", "KPCBootStatus: ");

	EvolMessage::show();

	Debug::trace("KPCBootStatus",
			HobString("ID: ") + HobString(Integer::toString(getId())));
	Debug::trace("KPCBootStatus",
				HobString("Status: ") + HobString(Integer::toString(getStatusCode())));
	Debug::trace("KPCBootStatus",
				HobString("Fault: ") + HobString(Integer::toString(getBootloaderFaults())));
	Debug::trace("KPCBootStatus",
				HobString("Fault Info: ") + HobString(Integer::toString(getFaultInfo())));

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootStatus");
	}
}

bool KPCBootStatus::equals(const KPCBootStatus &kpcBootCmd) const{
	return ( (pid == kpcBootCmd.getId()) && (status == kpcBootCmd.getStatusCode()) &&
			(fault == kpcBootCmd.getBootloaderFaults()) && (faultInfo == kpcBootCmd.getFaultInfo()) );
}

unsigned long KPCBootStatus::getId() const{
	return pid;
}

void KPCBootStatus::setId(unsigned long newPid){
	pid = newPid;
}

KPCBootStatus::BootStatusCode KPCBootStatus::getStatusCode() const{
	return status;
}

void KPCBootStatus::setStatusCode(BootStatusCode newStat){
	status = newStat;
}

KPCBootStatus::BootloaderFaults KPCBootStatus::getBootloaderFaults() const{
	return fault;
}

void KPCBootStatus::setBootloaderFaults(BootloaderFaults newFault){
	fault = newFault;
}

unsigned long KPCBootStatus::getFaultInfo() const{
	return faultInfo;
}

void KPCBootStatus::setFaultInfo(unsigned long newInfo){
	faultInfo = newInfo;
}
