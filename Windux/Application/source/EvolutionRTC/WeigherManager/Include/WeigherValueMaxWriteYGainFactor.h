/**
 * @file WeigherValueMaxWriteYGainFactor.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max x gain factors
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXWRITEYGAINFACTOR_H
#define WEIGHERVALUEMAXWRITEYGAINFACTOR_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
class WeigherValueMaxWriteYGainFactor : public EvolMessage
{
	public:

	WeigherValueMaxWriteYGainFactor( float x, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteYGainFactor( const char *buffer );
	~WeigherValueMaxWriteYGainFactor();

	const char *serialize()const;

	float getGain() { return gain; }
	private:
	float gain;

};
#endif
