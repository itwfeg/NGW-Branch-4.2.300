/*
 * NGWWrapBoxComplete.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */
#include "NGWWrapBoxComplete.h"
NGWWrapBoxComplete::NGWWrapBoxComplete(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrWrapBoxComplete, pHobThread){}


NGWWrapBoxComplete::NGWWrapBoxComplete(const char *buffer)
		: WrapperGeneric(buffer){}


NGWWrapBoxComplete::~NGWWrapBoxComplete(){}




