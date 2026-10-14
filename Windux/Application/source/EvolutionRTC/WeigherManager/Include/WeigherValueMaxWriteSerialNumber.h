/**
 * @file WeigherValueMaxWriteSerialNumber.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board serial number
 * Created on: Sept 20, 2017
 *
 **/

#ifndef WEIGHERVALUEMAXWRITESERIALNUMBER_H
#define WEIGHERVALUEMAXWRITESERIALNUMBER_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxWriteSerialNumber : public EvolMessage
{
	public:
	WeigherValueMaxWriteSerialNumber( std::string str, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxWriteSerialNumber( const char *buffer );
	~WeigherValueMaxWriteSerialNumber();

	static const unsigned int _SerialSize_	= 16;

	std::string getSerialNumber() { return serialNumber; }

	const char *serialize()const;

	private:
	std::string  serialNumber;

};





#endif
