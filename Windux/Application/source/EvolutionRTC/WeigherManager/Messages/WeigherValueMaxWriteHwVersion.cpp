/*
 * WeigherValueMaxWriteHwVersion.cpp
 *
 *  Created on: Sep 20, 2017
 *      Author: aaron
 */
/**
 * @file WeigherValueMaxWriteHwVersion.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max assembly number
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteHwVersion.h"
#include "WeigherEvent.h"

WeigherValueMaxWriteHwVersion::WeigherValueMaxWriteHwVersion( std::string str, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteHwVersion, pHobThread )
{
	hardwareVersion = str;
}

WeigherValueMaxWriteHwVersion::WeigherValueMaxWriteHwVersion( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	for( unsigned int i = 0; i < _HwVersionSize_; i++ )
	{
		hardwareVersion.append( 1, buffer[j++] );
	}
}

WeigherValueMaxWriteHwVersion::~WeigherValueMaxWriteHwVersion()
{
}

const char *WeigherValueMaxWriteHwVersion::serialize()const
{
	EvolMessage::serialize();

	serializedData += hardwareVersion;

	return serializedData.data();
}




