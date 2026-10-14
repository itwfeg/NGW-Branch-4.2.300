/*
 * NGWTimingsSave.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#include "NGWTimingsSave.h"

NGWTimingsSave::NGWTimingsSave(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrTimingsSave, pHobThread){}


NGWTimingsSave::NGWTimingsSave(const char *buffer)
		: WrapperGeneric(buffer){}


NGWTimingsSave::~NGWTimingsSave(){}



