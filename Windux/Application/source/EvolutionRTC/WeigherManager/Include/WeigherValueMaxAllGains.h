/**
 * @file WeigherValueMaxAllGains.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max current x,y and z gain factor
 * Created on: Sept 25, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXALLGAINS_H
#define WEIGHERVALUEMAXALLGAINS_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxAllGains : public EvolMessage
{
	public:


	WeigherValueMaxAllGains( float x, float y, float z, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxAllGains( const char *buffer );
	~WeigherValueMaxAllGains();


	float getXGain() { return xGain; }
	float getYGain() { return yGain; }
	float getZGain() { return zGain; }

	const char *serialize()const;

	private:
	float xGain;
	float yGain;
	float zGain;
};
#endif
