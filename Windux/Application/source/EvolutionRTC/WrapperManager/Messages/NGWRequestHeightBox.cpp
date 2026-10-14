/*
 * NGWRequestHeightBox.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */
#include "NGWRequestHeightBox.h"
NGWRequestHeightBox::NGWRequestHeightBox(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrRequestHeightBox, pHobThread){}


NGWRequestHeightBox::NGWRequestHeightBox(const char *buffer)
		: WrapperGeneric(buffer){}


NGWRequestHeightBox::~NGWRequestHeightBox(){}



