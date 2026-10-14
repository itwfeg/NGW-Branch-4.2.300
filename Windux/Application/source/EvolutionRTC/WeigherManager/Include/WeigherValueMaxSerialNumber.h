/**
 * @file WeigherValueMaxSerialNumber.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max board serial number
 * Created on: Sept 20, 2017
 *
 **/
#ifndef WEIGHERVALUEMAXSERIALNUMBER_H
#define WEIGHERVALUEMAXSERIALNUMBER_H
#include <string>
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"

class WeigherValueMaxSerialNumber : public EvolMessage
{
	public:
	WeigherValueMaxSerialNumber( std::string str, Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherValueMaxSerialNumber( const char *buffer );
	~WeigherValueMaxSerialNumber();

	static const unsigned int _SerialSize_	= 16;

	std::string getSerialNumber() { return serialNumber; }

	const char *serialize()const;

	private:
	std::string  serialNumber;

};
#endif
