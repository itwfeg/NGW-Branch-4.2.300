/**
 * @file WeigherValueMaxWriteXGainFactor.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factors
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXWRITEXGAINFACTOR_H
#define WEIGHERVALUEMAXWRITEXGAINFACTOR_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxWriteXGainFactor : public EvolMessage
{
	public:

	WeigherValueMaxWriteXGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteXGainFactor( const char *buffer );
	~WeigherValueMaxWriteXGainFactor();

	float getGain() { return gain; }

	const char *serialize()const;

	private:
	float gain;
};
#endif
