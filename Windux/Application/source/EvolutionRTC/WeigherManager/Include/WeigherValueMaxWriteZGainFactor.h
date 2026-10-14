/**
 * @file WeigherValueMaxWriteZGainFactor.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factors
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXWRITEZGAINFACTOR_H
#define WEIGHERVALUEMAXWRITEZGAINFACTOR_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
class WeigherValueMaxWriteZGainFactor : public EvolMessage
{
	public:

	WeigherValueMaxWriteZGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteZGainFactor( const char *buffer );
	~WeigherValueMaxWriteZGainFactor();

	const char *serialize()const;

	float getGain() { return gain; }
	private:
	float gain;

};
#endif
