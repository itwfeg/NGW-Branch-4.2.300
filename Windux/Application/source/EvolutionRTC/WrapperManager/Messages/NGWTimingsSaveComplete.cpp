/*
 * NGWTimingsSaveComplete.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#include "NGWTimingsSaveComplete.h"
NGWTimingsSaveComplete::NGWTimingsSaveComplete(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrTimingsSaveComplete, pHobThread){}


NGWTimingsSaveComplete::NGWTimingsSaveComplete(const char *buffer)
		: WrapperGeneric(buffer){}


NGWTimingsSaveComplete::~NGWTimingsSaveComplete(){}




