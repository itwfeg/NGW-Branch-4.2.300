/**
 * @file WeigherValueMaxWriteXGainFactor.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factor
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherValueMaxWriteXGainFactor.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include <utilstring.h>

WeigherValueMaxWriteXGainFactor::WeigherValueMaxWriteXGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgValueMaxWriteXGain, pHobThread )
{
	gain = x;
	std::string str	= util_string::doubletostring( *reinterpret_cast<const float*>( &gain ), 1, 3 );
	Debug::trace( "CanTranslator", "WeigherValueMaxWriteXGainFactor(): gain: " + str );

}

WeigherValueMaxWriteXGainFactor::WeigherValueMaxWriteXGainFactor( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		Debug::trace( "CanTranslator", "WeigherValueMaxWriteXGainFactor(): " + Hex::toString(buffer[j]));
		*pChar++ = buffer[j++];

	}
}

WeigherValueMaxWriteXGainFactor::~WeigherValueMaxWriteXGainFactor()
{
}

const char *WeigherValueMaxWriteXGainFactor::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &gain;
	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	for (unsigned int i = 0; i < sizeof(gain); i++)
	{
		Debug::trace( "CanTranslator", "WeigherValueMaxWriteXGainFactor serialize(): " + Hex::toString(pChar[i]));
	}
	return serializedData.data();
}




