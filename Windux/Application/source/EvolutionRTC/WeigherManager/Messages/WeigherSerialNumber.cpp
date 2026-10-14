/*
 * WeigherSerialNumber.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */
#include "WeigherSerialNumber.h"

WeigherSerialNumber::WeigherSerialNumber( Hobart::CEvolMsgReceiver *pHobThread ) : WeigherGeneric( WgSerialNumber, pHobThread )
{
	for (unsigned int i = 0; i < sizeof(_serialNumber); i++)
	{
		_serialNumber[i] = 0;
	}
}

WeigherSerialNumber::WeigherSerialNumber( const char *buffer ) : WeigherGeneric( buffer )
{
	WeigherGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &_serialNumber;
	for (unsigned int i = 0; i < sizeof(_serialNumber); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherSerialNumber::~WeigherSerialNumber(){}

std::string WeigherSerialNumber::getSerialNumber()
{
	std::string serialNumber;
	serialNumber.append( 1, static_cast<char>( _serialNumber[0] ) );
	serialNumber.append( 1, static_cast<char>( _serialNumber[1] ) );
	serialNumber.append( 1, static_cast<char>( _serialNumber[2] ) );
	serialNumber.append( 1, static_cast<char>( _serialNumber[3] ) );


	return  serialNumber;
}
void WeigherSerialNumber::setSerialNumber( UINT8 *pNumber )
{
	for (unsigned int i = 0; i < sizeof(_serialNumber); i++)
	{
		_serialNumber[i] = *pNumber++;
	}
}


const char *WeigherSerialNumber::serialize() const
{
	WeigherGeneric::serialize();


	char *pChar = (char *) &_serialNumber;
	for (unsigned int i = 0; i < sizeof(_serialNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}




