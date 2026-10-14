/**
 * @file WeigherValueMaxAssembly.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max assembly number
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxAssembly.h"
#include "WeigherEvent.h"

WeigherValueMaxAssembly::WeigherValueMaxAssembly( std::string str, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxAssembly, pHobThread )
{
	assemblyNumber = str;
}

WeigherValueMaxAssembly::WeigherValueMaxAssembly( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	for( unsigned int i = 0; i < _AssemblyNumbSize_; i++ )
	{
		assemblyNumber.append( 1, buffer[j++] );
	}
}

WeigherValueMaxAssembly::~WeigherValueMaxAssembly()
{
}

const char *WeigherValueMaxAssembly::serialize()const
{
	EvolMessage::serialize();

	serializedData += assemblyNumber;

	return serializedData.data();
}





