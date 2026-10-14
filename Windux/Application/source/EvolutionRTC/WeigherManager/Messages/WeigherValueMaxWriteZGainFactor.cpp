/**
 * @file WeigherValueMaxWriteZGainFactor.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factor
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteZGainFactor.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Hex.h"

WeigherValueMaxWriteZGainFactor::WeigherValueMaxWriteZGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteZGain, pHobThread )
{
	gain = x;
}

WeigherValueMaxWriteZGainFactor::WeigherValueMaxWriteZGainFactor( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		Debug::trace( "CanTranslator", "WeigherValueMaxWriteZGainFactor: " + Hex::toString(*pChar++) );
	}

}

WeigherValueMaxWriteZGainFactor::~WeigherValueMaxWriteZGainFactor()
{
}

const char *WeigherValueMaxWriteZGainFactor::serialize()const
{
	EvolMessage::serialize();
	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	Debug::trace( "CanTranslator", "WeigherValueMaxWriteZGainFactor serialize(): " + serializedData );

	return serializedData.data();
}



