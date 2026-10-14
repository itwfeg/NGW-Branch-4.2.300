/*
 * KPCBootUpgraderFlush.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTUPGRADERFLUSH_H_
#define KPCBOOTUPGRADERFLUSH_H_

#include "EvolMessage.h"


class KPCBootUpgraderFlush: public EvolMessage{
public:
	KPCBootUpgraderFlush(const KPCBootUpgraderFlush&);
	KPCBootUpgraderFlush(const char *buffer);
	KPCBootUpgraderFlush(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootUpgraderFlush();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootUpgraderFlush &kpcBootUpgraderFlush) const;

private:
};

#endif /* KPCBOOTUPGRADERFLUSH_H_ */
