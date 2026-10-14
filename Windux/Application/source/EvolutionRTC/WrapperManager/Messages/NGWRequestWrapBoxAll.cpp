/*
 * NGWRequestWrapBoxAll.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#include "NGWRequestWrapBoxAll.h"
NGWRequestWrapBoxAll::NGWRequestWrapBoxAll(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrRequestWrapBoxAll, pHobThread){}


NGWRequestWrapBoxAll::NGWRequestWrapBoxAll(const char *buffer)
		: WrapperGeneric(buffer){}


NGWRequestWrapBoxAll::~NGWRequestWrapBoxAll(){}



