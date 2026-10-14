/**
 * @file WeigherValueMaxZGainFactor.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factor
 * Created on: Sept 20, 2017
 *
 **/

#ifndef WEIGHERVALUEMAXZGAINFACTOR_H
#define WEIGHERVALUEMAXZGAINFACTOR_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxZGainFactor : public EvolMessage
{
	public:


	WeigherValueMaxZGainFactor( float z, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxZGainFactor( const char *buffer );
	~WeigherValueMaxZGainFactor();


	float getGain() { return gain; }

	const char *serialize()const;

	private:
	float gain;
};
#endif
