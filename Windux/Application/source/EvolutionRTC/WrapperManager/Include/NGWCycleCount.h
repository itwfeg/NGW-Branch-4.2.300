/*
 * NGWCycleCount.h
 *
 *  Created on: Aug 10, 2015
 *      Author: aaron
 */

#ifndef NGWCYCLECOUNT_H_
#define NGWCYCLECOUNT_H_
#include "WrapperGeneric.h"

class NGWCycleCount : public WrapperGeneric
{

	public:
	NGWCycleCount(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWCycleCount(const char *buffer);
	~NGWCycleCount();

	UINT32 getCycleCount() { return _cycleCount; }
	void setCycleCount(UINT32 count) { _cycleCount = count; }

	const char *serialize()const;

	private:
	UINT32 _cycleCount;
};



#endif /* NGWCYCLECOUNT_H_ */
