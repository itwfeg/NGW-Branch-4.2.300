/**
 * @file WeigherValueMaxYGainFactor.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max y gain factor
 * Created on: Sept 20, 2017
 *
 **/

#ifndef WEIGHERVALUEMAXYGAINFACTOR_H
#define WEIGHERVALUEMAXYGAINFACTOR_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxYGainFactor : public EvolMessage
{
	public:


	WeigherValueMaxYGainFactor( float y, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxYGainFactor( const char *buffer );
	~WeigherValueMaxYGainFactor();


	float getGain() { return gain; }

	const char *serialize()const;

	private:
	float gain;
};
#endif
