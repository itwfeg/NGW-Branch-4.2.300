/*
 * WeigherSerialNumber.h
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */

#ifndef WEIGHERSERIALNUMBER_H_
#define WEIGHERSERIALNUMBER_H_
#include "WeigherGeneric.h"

class WeigherSerialNumber : public WeigherGeneric
{
	public:
	WeigherSerialNumber( Hobart::CEvolMsgReceiver *pHobThread = NULL );
	WeigherSerialNumber( const char *buffer );
	~WeigherSerialNumber();

	std::string getSerialNumber();
	void setSerialNumber( UINT8 *pNumber );

	const char *serialize() const;


	private:
	UINT8 _serialNumber[4];
};


#endif /* WEIGHERSERIALNUMBER_H_ */
