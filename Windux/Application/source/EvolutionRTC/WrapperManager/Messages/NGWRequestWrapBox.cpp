/*
 * NGWRequestWrapBox.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */
#include "NGWRequestWrapBox.h"

NGWRequestWrapBox::NGWRequestWrapBox(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrRequestWrapBox, pHobThread){}


NGWRequestWrapBox::NGWRequestWrapBox(const char *buffer)
		: WrapperGeneric(buffer){}


NGWRequestWrapBox::~NGWRequestWrapBox(){}





