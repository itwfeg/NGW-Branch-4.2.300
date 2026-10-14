/*
 * KPCUpgradeProgress.cpp
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#include "KPCUpgradeProgress.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int KPCUpgradeProgress::dataType = 0;
std::string KPCUpgradeProgress::dataTypeName("KPCUpgradeProgress");
#endif

KPCUpgradeProgress::KPCUpgradeProgress(const KPCUpgradeProgress& newKG) : EvolMessage(newKG)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
    progress = newKG.getProgress();

}

KPCUpgradeProgress::KPCUpgradeProgress(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	EvolMessage::serialize();

    int j = getSerializedLength();

    char *pChar = (char *) &progress;
	for (unsigned int i = 0; i < sizeof(progress); i++)
	{
		*pChar++ = buffer[j++];
	}

}



KPCUpgradeProgress::KPCUpgradeProgress(EvolDeviceType deviceType, Hobart::CEvolMsgReceiver * pSource=NULL ) :
                                                                         EvolMessage (deviceType, KpcBootUpgradeProgress, pSource){
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
    progress = 0;
}


KPCUpgradeProgress::~KPCUpgradeProgress()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *KPCUpgradeProgress::serialize() const
{
	EvolMessage::serialize();
    
    char *pChar = (char *) &progress;
	for (unsigned int i = 0; i < sizeof(progress); i++) {
		serializedData.append(1, pChar[i]);
	}


	return serializedData.data();
}

void KPCUpgradeProgress::show(){

	bool isTracing = Debug::isTracing("KPCUpgradeProgress");

	if (isTracing == false) {
		return;
	}
	Debug::trace("KPCUpgradeProgress", "KPCUpgradeProgress: ");

	EvolMessage::show();

    Debug::trace("KPCUpgradeProgress", "Progress: " + Integer::toString(progress));

	if (isTracing == false) {
		Debug::removeTraceCategory("KPCUpgradeProgress");
	}
}



