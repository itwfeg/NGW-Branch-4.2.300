/**
 * @file WeigherValueMaxHwVersion.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board hardware version
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXHWVERSION_H
#define WEIGHERVALUEMAXHWVERSION_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxHwVersion : public EvolMessage
{
	public:
	WeigherValueMaxHwVersion( std::string str, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxHwVersion( const char *buffer );
	~WeigherValueMaxHwVersion();

	static const unsigned int _HwVersionSize_	= 2;

	std::string getHardwareVersion() { return hardwareVersion; }
	const char *serialize()const;

	private:
	std::string  hardwareVersion;

};
#endif
