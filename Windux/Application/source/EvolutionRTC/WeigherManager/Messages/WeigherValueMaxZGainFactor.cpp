/**
 * @file WeigherValueMaxZGainFactor.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max z gain factor
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxZGainFactor.h"
#include "WeigherEvent.h"

WeigherValueMaxZGainFactor::WeigherValueMaxZGainFactor( float z, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxZGain, pHobThread )
{
	gain = z;
}

WeigherValueMaxZGainFactor::WeigherValueMaxZGainFactor( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherValueMaxZGainFactor::~WeigherValueMaxZGainFactor()
{
}

const char *WeigherValueMaxZGainFactor::serialize()const
{
	EvolMessage::serialize();
	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}




