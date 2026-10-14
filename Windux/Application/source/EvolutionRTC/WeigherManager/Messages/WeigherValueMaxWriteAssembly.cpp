/**
 * @file WeigherValueMaxWriteAssembly.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max assembly number
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteAssembly.h"
#include "WeigherEvent.h"

WeigherValueMaxWriteAssembly::WeigherValueMaxWriteAssembly( std::string str, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteAssembly, pHobThread )
{
	assemblyNumber = str;
}

WeigherValueMaxWriteAssembly::WeigherValueMaxWriteAssembly( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	for( unsigned int i = 0; i < _AssemblyNumbSize_; i++ )
	{
		assemblyNumber.append( 1, buffer[j++] );
	}
}

WeigherValueMaxWriteAssembly::~WeigherValueMaxWriteAssembly()
{
}

const char *WeigherValueMaxWriteAssembly::serialize()const
{
	EvolMessage::serialize();

	serializedData += assemblyNumber;

	return serializedData.data();
}





