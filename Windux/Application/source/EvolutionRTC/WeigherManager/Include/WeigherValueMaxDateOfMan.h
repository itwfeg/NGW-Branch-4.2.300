/**
 * @file WeigherValueMaxDateOfMan.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board date of manufacture
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXDATEOFMAN_H
#define WEIGHERVALUEMAXDATEOFMAN_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxDateOfMan : public EvolMessage
{
	public:
	WeigherValueMaxDateOfMan( unsigned long date, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxDateOfMan( const char *buffer );
	~WeigherValueMaxDateOfMan();

	unsigned long getDate() { return epoch; }

	const char *serialize()const;

	private:
	unsigned long epoch;

};
#endif
