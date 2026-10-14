/*
 * KPCBootGeneric.h
 *
 *  Created on: Mar 14, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTGENERIC_H_
#define KPCBOOTGENERIC_H_
#include "EvolMessage.h"
#include "KpcBootEvent.h"

class KPCBootGeneric: public EvolMessage {

public:
	KPCBootGeneric(const KPCBootGeneric&);
	KPCBootGeneric(const char *buffer);
	KPCBootGeneric(Hobart::CEvolMsgReceiver *pHobThread);
	KPCBootGeneric(EvolDeviceType deviceType, KPCBootEvent eventType, Hobart::CEvolMsgReceiver * pSource);
	~KPCBootGeneric();

	const char *serialize() const;

	void show();

private:
	KPCBootGeneric();
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
};

#endif /* KPCBOOTGENERIC_H_ */
