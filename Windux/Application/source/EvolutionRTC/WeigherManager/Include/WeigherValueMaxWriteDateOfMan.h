/**
 * @file WeigherValueMaxWriteDateOfMan.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board date of manufacture
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXWRITEDATEOFMAN_H
#define WEIGHERVALUEMAXWRITEDATEOFMAN_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxWriteDateOfMan : public EvolMessage
{
	public:
	WeigherValueMaxWriteDateOfMan( unsigned long date, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteDateOfMan( const char *buffer );
	~WeigherValueMaxWriteDateOfMan();

	const char *serialize()const;

	unsigned long getDate() { return epoch; }

	private:
	unsigned long epoch;

};
#endif
