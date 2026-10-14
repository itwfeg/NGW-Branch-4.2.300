/*
 * KPCBootAck.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTACK_H_
#define KPCBOOTACK_H_

#include "EvolMessage.h"
#include "KpcBootEvent.h"

class KPCBootAck: public EvolMessage{
public:
	KPCBootAck(const KPCBootAck&);
	KPCBootAck(const char *buffer);
	KPCBootAck(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootAck();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootAck &KPCBootAck) const;

	uint8_t getSource() const;
	void setSource(uint8_t newSource);

	uint8_t getDestination() const;
	void setDestination(uint8_t newDestination);

	KPCBootEvent_t getAckedEvent() const;
	void setAckedEvent(KPCBootEvent_t newAckedEvent);

private:

	uint8_t source;
	uint8_t	destination;
	KPCBootEvent_t ackedEvent;
};

#endif /* KPCBOOTACK_H_ */
