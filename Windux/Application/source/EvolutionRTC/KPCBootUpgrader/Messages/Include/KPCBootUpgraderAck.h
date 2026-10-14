/*
 * KPCBootAck.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADERACK_H_
#define KPCBOOTUPGRADERACK_H_

#include "EvolMessage.h"
#include "KpcBootUpgraderEvent.h"

class KPCBootUpgraderAck: public EvolMessage{
public:
	KPCBootUpgraderAck(const KPCBootUpgraderAck&);
	KPCBootUpgraderAck(const char *buffer);
	KPCBootUpgraderAck(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootUpgraderAck();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootUpgraderAck&) const;

	uint8_t getSource() const;
	void setSource(uint8_t newSource);

	uint8_t getDestination() const;
	void setDestination(uint8_t newDestination);

	KPCBootUpgraderEvent_t getAckedEvent() const;
	void setAckedEvent(KPCBootUpgraderEvent_t newAckedEvent);

private:

	uint8_t source;
	uint8_t	destination;
	KPCBootUpgraderEvent_t ackedEvent;
};

#endif /* KPCBOOTUPGRADERACK_H_ */
