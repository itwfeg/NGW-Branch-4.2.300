/**
 * @file WeigherValueMaxYGainFactor.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factor
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxYGainFactor.h"
#include "WeigherEvent.h"

WeigherValueMaxYGainFactor::WeigherValueMaxYGainFactor( float y, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxYGain, pHobThread )
{
	gain = y;
}

WeigherValueMaxYGainFactor::WeigherValueMaxYGainFactor( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherValueMaxYGainFactor::~WeigherValueMaxYGainFactor()
{
}

const char *WeigherValueMaxYGainFactor::serialize()const
{
	EvolMessage::serialize();
	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}




