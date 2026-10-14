/*
 * KPCUpgradeInfo.cpp
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#include "KPCUpgradeInfo.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"
#include "KpcBootEvent.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int KPCUpgradeInfo::dataType = 0;
std::string KPCUpgradeInfo::dataTypeName("KPCUpgradeInfo");
#endif

KPCUpgradeInfo::KPCUpgradeInfo(const KPCUpgradeInfo& newKG) : EvolMessage(newKG)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
    currentVersion = newKG.getCurrentVersion();
    availableVersion = newKG.getAvailableVersion();

	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo(newKG) Current Version: " + currentVersion);
	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo(newKG) Available Version: " + availableVersion);
}

KPCUpgradeInfo::KPCUpgradeInfo(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

    int j = getSerializedLength();
	int temp;

	char *pChar = (char *) &temp;
	for (unsigned int i = 0; i < sizeof(temp); i++)
	{
		*pChar++ = buffer[j++];
	}
	Debug::trace("KPCUpgradeInfo", "Constructor buffer: currentVersion str length: " + Integer::toString(temp));

    //pChar = (char *) &currentVersion[0];
	for (int i = 0; i < temp; i++)
	{
		//*pChar++ = buffer[j++];
		currentVersion.append(1, buffer[j++]);
	}
	Debug::trace("KPCUpgradeInfo", "Constructor buffer: currentVersion str: " + currentVersion);


	pChar = (char *) &temp;
	for (unsigned int i = 0; i < sizeof(temp); i++)
	{
		*pChar++ = buffer[j++];
	}
	Debug::trace("KPCUpgradeInfo", "Constructor buffer: availableVersion str length: " + Integer::toString(temp));

    //pChar = (char *) &availableVersion[0];
	for (int i = 0; i < temp; i++)
	{
		//*pChar++ = buffer[j++];
		availableVersion.append(1, buffer[j++]);
	}
	Debug::trace("KPCUpgradeInfo", "Constructor buffer: availableVersion str: " + availableVersion);
}



KPCUpgradeInfo::KPCUpgradeInfo(EvolDeviceType deviceType, Hobart::CEvolMsgReceiver * pSource=NULL ) :
                                                                         EvolMessage (deviceType, KpcBootUpgradeInfo, pSource){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
    currentVersion = "";
    availableVersion = "";

	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo(deviceType,src) Current Version: " + currentVersion);
	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo(deviceType,src) Available Version: " + availableVersion);
}


KPCUpgradeInfo::~KPCUpgradeInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCUpgradeInfo::serialize() const
{
	EvolMessage::serialize();
    
	int size = (int)currentVersion.length();
	char *pChar = (char *) &size;
	for (unsigned int i = 0; i < sizeof(size); i++) {
		serializedData.append(1, pChar[i]);
	}
	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo.serialize(): currentVersion str length" + Integer::toString(size));

    pChar = (char *) &currentVersion[0];
	for (int i = 0; i < size; i++) {
		serializedData.append(1, pChar[i]);
	}

	size = (int) availableVersion.length();
	pChar = (char *) &size;
	for (unsigned int i = 0; i < sizeof(size); i++) {
		serializedData.append(1, pChar[i]);
	}
	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo.serialize(): availableVersion str length" + Integer::toString(size));

    pChar = (char *) &availableVersion[0];
	for (int i = 0; i < size; i++) {
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

std::string KPCUpgradeInfo::getCurrentVersion() const
{
	Debug::trace("KPCUpgradeInfo", "START getCurrentVersion() return currentVersion: " + currentVersion);

	return currentVersion; 
}

std::string KPCUpgradeInfo::getAvailableVersion() const
{
	Debug::trace("KPCUpgradeInfo", "START getAvailableVersion() return availableVersion: " + availableVersion);

	return availableVersion;
}

void KPCUpgradeInfo::setCurrentVersion(std::string ver)
{
	Debug::trace("KPCUpgradeInfo", "START setCurrentVersion() ver: " + ver);

	currentVersion = ver;

	Debug::trace("KPCUpgradeInfo", "DONE setCurrentVersion() availableVersion: " + currentVersion);
}

void KPCUpgradeInfo::setAvailableVersion(std::string ver)
{
	Debug::trace("KPCUpgradeInfo", "START setAvailableVersion() ver: " + ver);

	availableVersion = ver;

	Debug::trace("KPCUpgradeInfo", "DONE setAvailableVersion() availableVersion: " + availableVersion);
}

void KPCUpgradeInfo::show(){

	bool isTracing = Debug::isTracing("KPCUpgradeInfo");

	if (isTracing == false) {
		return;
	}

	Debug::trace("KPCUpgradeInfo", "KPCUpgradeInfo show(): ");

	EvolMessage::show();

    Debug::trace("KPCUpgradeInfo", "show() Current Version: " + currentVersion);
    Debug::trace("KPCUpgradeInfo", "show() Available Version: " + availableVersion);

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCUpgradeInfo");
	}
}



