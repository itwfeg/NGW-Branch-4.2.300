/**
 * @file WeigherValueMaxWriteYGainFactor.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max y gain factor
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteYGainFactor.h"
#include "WeigherEvent.h"
WeigherValueMaxWriteYGainFactor::WeigherValueMaxWriteYGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteYGain, pHobThread )
{
	gain = x;
}

WeigherValueMaxWriteYGainFactor::WeigherValueMaxWriteYGainFactor( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherValueMaxWriteYGainFactor::~WeigherValueMaxWriteYGainFactor()
{
}

const char *WeigherValueMaxWriteYGainFactor::serialize()const
{
	EvolMessage::serialize();
	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}



