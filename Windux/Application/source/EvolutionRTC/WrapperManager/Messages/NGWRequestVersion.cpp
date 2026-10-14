/*
 * NGWRequestVersion.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: aaron
 */
#include "NGWRequestVersion.h"


NGWRequestVersion::NGWRequestVersion( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrNgwReqVersion, pHobThread ){}
NGWRequestVersion::NGWRequestVersion(const char *buffer) : WrapperGeneric( buffer ){}
NGWRequestVersion::~NGWRequestVersion() {}

