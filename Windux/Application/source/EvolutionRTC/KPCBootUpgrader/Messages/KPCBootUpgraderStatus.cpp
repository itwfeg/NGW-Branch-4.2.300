/*
 * KPCBootUpgraderStatus.cpp
 *
 *  Created on: Mar 15, 2019
 *      Author: hobart
 */

#include "KPCBootUpgraderStatus.h"
#include "KpcBootUpgraderEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterGeneric::dataType = 0;
std::string PrinterGeneric::dataTypeName("KPCBootUpgraderStatus");
#endif

KPCBootUpgraderStatus::KPCBootUpgraderStatus(const KPCBootUpgraderStatus& newBStat) : EvolMessage(newBStat){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	pid = newBStat.getId();
	status = newBStat.getStatusCode();
	fault = newBStat.getBootUpgraderFaults();
	faultInfo = newBStat.getFaultInfo();

}

KPCBootUpgraderStatus::KPCBootUpgraderStatus(const char *buffer) : EvolMessage(buffer){
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

KPCBootUpgraderStatus::KPCBootUpgraderStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage( KPCBootUpdater, KpcBootUpgraderStatus, pHobThread ){
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


KPCBootUpgraderStatus::~KPCBootUpgraderStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCBootUpgraderStatus::serialize() const
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

void KPCBootUpgraderStatus::show(){
	bool isTracing = Debug::isTracing("KPCBootUpgraderStatus");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCBootUpgraderStatus", "KPCBootUpgraderStatus: ");

	EvolMessage::show();

	Debug::trace("KPCBootUpgraderStatus",
			HobString("ID: ") + HobString(Integer::toString(getId())));
	Debug::trace("KPCBootUpgraderStatus",
				HobString("Status: ") + HobString(Integer::toString(getStatusCode())));
	Debug::trace("KPCBootUpgraderStatus",
				HobString("Fault: ") + HobString(Integer::toString(getBootUpgraderFaults())));
	Debug::trace("KPCBootUpgraderStatus",
				HobString("Fault Info: ") + HobString(Integer::toString(getFaultInfo())));

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCBootUpgraderStatus");
	}
}

bool KPCBootUpgraderStatus::equals(const KPCBootUpgraderStatus &kpcBootCmd) const{
	return ( (pid == kpcBootCmd.getId()) && (status == kpcBootCmd.getStatusCode()) &&
			(fault == kpcBootCmd.getBootUpgraderFaults()) && (faultInfo == kpcBootCmd.getFaultInfo()) );
}

unsigned long KPCBootUpgraderStatus::getId() const{
	return pid;
}

void KPCBootUpgraderStatus::setId(unsigned long newPid){
	pid = newPid;
}

KPCBootUpgraderStatus::BootUpgraderStatusCode KPCBootUpgraderStatus::getStatusCode() const{
	return status;
}

void KPCBootUpgraderStatus::setStatusCode(BootUpgraderStatusCode newStat){
	status = newStat;
}

KPCBootUpgraderStatus::BootUpgraderFaults KPCBootUpgraderStatus::getBootUpgraderFaults() const{
	return fault;
}

void KPCBootUpgraderStatus::setBootUpgraderFaults(BootUpgraderFaults newFault){
	fault = newFault;
}

unsigned long KPCBootUpgraderStatus::getFaultInfo() const{
	return faultInfo;
}

void KPCBootUpgraderStatus::setFaultInfo(unsigned long newInfo){
	faultInfo = newInfo;
}
