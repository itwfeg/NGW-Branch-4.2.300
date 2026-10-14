/**
 * @file WeigherValueMaxDateOfMan.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board date of manufacture
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxDateOfMan.h"
#include "WeigherEvent.h"

WeigherValueMaxDateOfMan::WeigherValueMaxDateOfMan( unsigned long date, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxDateOfManufacture, pHobThread )
{
	epoch = date;
}

WeigherValueMaxDateOfMan::WeigherValueMaxDateOfMan( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &epoch;
	for (unsigned int i = 0; i < sizeof(epoch); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherValueMaxDateOfMan::~WeigherValueMaxDateOfMan()
{
}

const char *WeigherValueMaxDateOfMan::serialize()const
{
	EvolMessage::serialize();
	char *pChar = (char *) &epoch;
	for (unsigned int i = 0; i < sizeof(epoch); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}


