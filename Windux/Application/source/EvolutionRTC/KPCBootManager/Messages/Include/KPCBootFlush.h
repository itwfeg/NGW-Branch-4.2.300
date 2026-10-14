/*
 * KPCBootFlush.h
 *
 *  Created on: Mar 13, 2019
 *      Author: Eric Landes
 */

#ifndef KPCBOOTFLUSH_H_
#define KPCBOOTFLUSH_H_

#include "EvolMessage.h"


class KPCBootFlush: public EvolMessage{
public:
	KPCBootFlush(const KPCBootFlush&);
	KPCBootFlush(const char *buffer);
	KPCBootFlush(Hobart::CEvolMsgReceiver *pHobThread);
	~KPCBootFlush();

	const char *serialize() const;

	void show();

	bool equals(const KPCBootFlush &kpcBootFlush) const;

private:
};

#endif /* KPCBOOTFLUSH_H_ */
