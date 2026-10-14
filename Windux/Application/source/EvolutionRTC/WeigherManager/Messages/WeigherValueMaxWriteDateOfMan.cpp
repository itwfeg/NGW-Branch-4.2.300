/**
 * @file WeigherValueMaxWriteDateOfMan.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board date of manufacture
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteDateOfMan.h"
#include "WeigherEvent.h"

WeigherValueMaxWriteDateOfMan::WeigherValueMaxWriteDateOfMan( unsigned long date, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteDateOfManufacture, pHobThread )
{
	epoch = date;
}

WeigherValueMaxWriteDateOfMan::WeigherValueMaxWriteDateOfMan( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &epoch;
	for (unsigned int i = 0; i < sizeof(epoch); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherValueMaxWriteDateOfMan::~WeigherValueMaxWriteDateOfMan()
{
}

const char *WeigherValueMaxWriteDateOfMan::serialize()const
{
	EvolMessage::serialize();
	char *pChar = (char *) &epoch;
	for (unsigned int i = 0; i < sizeof(epoch); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}




