/*
 * NGWTimingsSaveComplete.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWTIMINGSSAVECOMPLETE_H_
#define NGWTIMINGSSAVECOMPLETE_H_
#include "WrapperGeneric.h"

class NGWTimingsSaveComplete : public WrapperGeneric
{

	public:
	NGWTimingsSaveComplete(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWTimingsSaveComplete(const char *buffer);
	~NGWTimingsSaveComplete();

};




#endif /* NGWTIMINGSSAVECOMPLETE_H_ */
