
/**
 * @file WeigherValueMaxWriteSerialNumber.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board serial number
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteSerialNumber.h"
#include "WeigherEvent.h"

WeigherValueMaxWriteSerialNumber::WeigherValueMaxWriteSerialNumber( std::string str, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteSerialNumber, pHobThread )
{
	serialNumber = str;
}

WeigherValueMaxWriteSerialNumber::WeigherValueMaxWriteSerialNumber( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	for( unsigned int i = 0; i < _SerialSize_; i++ )
	{
		serialNumber.append( 1, buffer[j++] );
	}
}

WeigherValueMaxWriteSerialNumber::~WeigherValueMaxWriteSerialNumber()
{
}

const char *WeigherValueMaxWriteSerialNumber::serialize()const
{
	EvolMessage::serialize();

	serializedData += serialNumber;

	return serializedData.data();
}




