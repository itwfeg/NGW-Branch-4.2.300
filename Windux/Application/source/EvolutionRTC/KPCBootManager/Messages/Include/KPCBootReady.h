/*
 * KPCBootReady.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTREADY_H_
#define KPCBOOTREADY_H_
#include "EvolMessage.h"

class KPCBootReady: public EvolMessage {

public:
	KPCBootReady(const KPCBootReady&);
	KPCBootReady(const char *buffer);
	KPCBootReady(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootReady();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootReady &kpcBootReady) const;

	uint16_t getSource() const;
	void setSource(uint16_t newSource);

	uint16_t getDestination() const;
	void setDestination(uint16_t newDestination);

private:
	uint16_t source;
	uint16_t destination;
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

#endif /* KPCBOOTREADY_H_ */
