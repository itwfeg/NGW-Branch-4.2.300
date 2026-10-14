/*
 * KPCBootUpgraderGeneric.h
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADERGENERIC_H_
#define KPCBOOTUPGRADERGENERIC_H_
#include "EvolMessage.h"
#include "KpcBootUpgraderEvent.h"

class KPCBootUpgraderGeneric: public EvolMessage {

public:
	KPCBootUpgraderGeneric(const KPCBootUpgraderGeneric&);
	KPCBootUpgraderGeneric(const char *buffer);
	KPCBootUpgraderGeneric(Hobart::CEvolMsgReceiver *pHobThread);
	KPCBootUpgraderGeneric(EvolDeviceType deviceType, KPCBootUpgraderEvent eventType, Hobart::CEvolMsgReceiver * pSource);
	~KPCBootUpgraderGeneric();

	const char *serialize() const;

	void show();

private:
	KPCBootUpgraderGeneric();
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
};

#endif /* KPCBOOTUPGRADERGENERIC_H_ */
