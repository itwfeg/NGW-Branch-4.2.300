/**
 * @file WeigherValueMaxXGainFactor.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factor
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxXGainFactor.h"
#include "WeigherEvent.h"

WeigherValueMaxXGainFactor::WeigherValueMaxXGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxXGain, pHobThread )
{
	gain = x;
}

WeigherValueMaxXGainFactor::WeigherValueMaxXGainFactor( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		*pChar++ = buffer[j++];
	}


}

WeigherValueMaxXGainFactor::~WeigherValueMaxXGainFactor()
{
}

const char *WeigherValueMaxXGainFactor::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}



