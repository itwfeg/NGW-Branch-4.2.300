/*
 * NGWTimingsSave.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWTIMINGSSAVE_H_
#define NGWTIMINGSSAVE_H_

#include "WrapperGeneric.h"

class NGWTimingsSave : public WrapperGeneric
{

	public:
	NGWTimingsSave(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWTimingsSave(const char *buffer);
	~NGWTimingsSave();

};



#endif /* NGWTIMINGSSAVE_H_ */
