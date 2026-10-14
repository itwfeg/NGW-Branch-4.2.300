/*
 * WeigherValueMaxWriteHwVersion.h
 *
 *  Created on: Sep 20, 2017
 *      Author: aaron
 */

#ifndef WEIGHERVALUEMAXWRITEHWVERSION_H
#define WEIGHERVALUEMAXWRITEHWVERSION_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxWriteHwVersion : public EvolMessage
{
	public:
	WeigherValueMaxWriteHwVersion( std::string str, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteHwVersion( const char *buffer );
	~WeigherValueMaxWriteHwVersion();

	static const unsigned int _HwVersionSize_	= 2;

	std::string getHardwareVersion() { return hardwareVersion; }

	const char *serialize()const;

	private:
	std::string  hardwareVersion;

};





#endif
