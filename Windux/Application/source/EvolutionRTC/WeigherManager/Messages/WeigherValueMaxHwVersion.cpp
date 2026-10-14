/**
 * @file WeigherValueMaxHwVersion.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max assembly number
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxHwVersion.h"
#include "WeigherEvent.h"

WeigherValueMaxHwVersion::WeigherValueMaxHwVersion( std::string str, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxHwVersion, pHobThread )
{
	hardwareVersion = str;
}

WeigherValueMaxHwVersion::WeigherValueMaxHwVersion( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	for( unsigned int i = 0; i < _HwVersionSize_; i++ )
	{
		hardwareVersion.append( 1, buffer[j++] );
	}
}

WeigherValueMaxHwVersion::~WeigherValueMaxHwVersion()
{
}

const char *WeigherValueMaxHwVersion::serialize()const
{
	EvolMessage::serialize();

	serializedData += hardwareVersion;

	return serializedData.data();
}





