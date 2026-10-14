/**
 * @file WeigherValueMaxSerialNumber.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board serial number
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxSerialNumber.h"
#include "WeigherEvent.h"

WeigherValueMaxSerialNumber::WeigherValueMaxSerialNumber( std::string str, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxSerialNumber, pHobThread )
{
	serialNumber = str;
}

WeigherValueMaxSerialNumber::WeigherValueMaxSerialNumber( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	for( unsigned int i = 0; i < _SerialSize_; i++ )
	{
		serialNumber.append( 1, buffer[j++] );
	}
}

WeigherValueMaxSerialNumber::~WeigherValueMaxSerialNumber()
{
}

const char *WeigherValueMaxSerialNumber::serialize()const
{
	EvolMessage::serialize();

	serializedData += serialNumber;

	return serializedData.data();
}


