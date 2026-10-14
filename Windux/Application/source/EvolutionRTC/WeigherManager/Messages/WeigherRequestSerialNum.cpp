/*
 * WeigherRequestSerialNum.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */
#include "WeigherRequestSerialNum.h"

WeigherRequestSerialNum::WeigherRequestSerialNum( Hobart::CEvolMsgReceiver *pHobThread ) : WeigherGeneric( WgReqSerialNum, pHobThread ){}
WeigherRequestSerialNum::WeigherRequestSerialNum( const char *buffer ) : WeigherGeneric( buffer ) {}
WeigherRequestSerialNum::~WeigherRequestSerialNum() {}




