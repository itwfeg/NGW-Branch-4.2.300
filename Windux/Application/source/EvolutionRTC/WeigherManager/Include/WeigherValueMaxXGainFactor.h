/**
 * @file WeigherValueMaxXGainFactor.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factor
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXXGAINFACTOR_H
#define WEIGHERVALUEMAXXGAINFACTOR_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxXGainFactor : public EvolMessage
{
	public:


	WeigherValueMaxXGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxXGainFactor( const char *buffer );
	~WeigherValueMaxXGainFactor();


	float getGain() { return gain; }

	const char *serialize()const;

	private:
	float gain;


};
#endif
