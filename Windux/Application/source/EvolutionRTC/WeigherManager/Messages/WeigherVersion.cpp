/*
 * WeigherVersion.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */
#include "WeigherVersion.h"

WeigherVersion::WeigherVersion( Hobart::CEvolMsgReceiver *pHobThread ) : WeigherGeneric( WgVersion, pHobThread )
{
	_softwareMajor = 0;
	_softwareMinor = 0;
	_hardwareMajor = 0;
	_hardwareMinor = 0;
}

WeigherVersion::WeigherVersion( const char *buffer ) : WeigherGeneric( buffer )
{

	WeigherGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &_softwareMajor;
	for (unsigned int i = 0; i < sizeof(_softwareMajor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_softwareMinor;
	for (unsigned int i = 0; i < sizeof(_softwareMinor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_hardwareMajor;
	for (unsigned int i = 0; i < sizeof(_hardwareMajor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_hardwareMinor;
	for (unsigned int i = 0; i < sizeof(_hardwareMinor); i++)
	{
		*pChar++ = buffer[j++];
	}

}

WeigherVersion::~WeigherVersion()
{

}

const char *WeigherVersion::serialize() const
{
	WeigherGeneric::serialize();


	char *pChar = (char *) &_softwareMajor;
	for (unsigned int i = 0; i < sizeof(_softwareMajor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_softwareMinor;
	for (unsigned int i = 0; i < sizeof(_softwareMinor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_hardwareMajor;
	for (unsigned int i = 0; i < sizeof(_hardwareMajor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_hardwareMinor;
	for (unsigned int i = 0; i < sizeof(_hardwareMinor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


