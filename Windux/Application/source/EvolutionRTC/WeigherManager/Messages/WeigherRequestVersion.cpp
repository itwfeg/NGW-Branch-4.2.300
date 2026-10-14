/*
 * WeigherRequestVersion.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */
#include "WeigherRequestVersion.h"

WeigherRequestVersion::WeigherRequestVersion( Hobart::CEvolMsgReceiver *pHobThread ) : WeigherGeneric( WgReqVersion, pHobThread ){}

WeigherRequestVersion::WeigherRequestVersion( const char *buffer ) : WeigherGeneric( buffer ){}
WeigherRequestVersion::~WeigherRequestVersion() {}





