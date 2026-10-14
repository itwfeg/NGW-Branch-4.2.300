/*
 * NGWVersion.cpp
 *
 *  Created on: Aug 6, 2015
 *      Author: aaron
 */
#include "NGWVersion.h"
const int NGWVersion::MAX_VER_LENGTH = 5;

NGWVersion::NGWVersion( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrNgwVersion, pHobThread )
{
	_nodeId = 0;
	_wrapBoxChecksum = 0;

}

NGWVersion::NGWVersion( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &_nodeId;
	for (unsigned int i = 0; i < sizeof(_nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_wrapBoxChecksum;
	for (unsigned int i = 0; i < sizeof(_wrapBoxChecksum); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (unsigned int i = 0; i < MAX_VER_LENGTH; i++)
	{
		_configVersion.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < MAX_VER_LENGTH; i++)
	{
		_bootVersion.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < MAX_VER_LENGTH; i++)
	{
		_dataVersion.append(1, buffer[j++]);
	}

	for (unsigned int i = 0; i < MAX_VER_LENGTH; i++)
	{
		_appVersion.append(1, buffer[j++]);
	}

}

NGWVersion::~NGWVersion()
{

}

void NGWVersion::setConfigVersion( UINT16 major, UINT16 minor )
{
	char str[10];
	sprintf(str, "%d", major);

	if( major < 10 )
	{
		_configVersion.append(1, '0');
		_configVersion.append(1, str[0]);
	}
	else
	{
		_configVersion.append(2, str[0]);
	}

	_configVersion.append(1, '.');

	sprintf(str, "%d", minor);

	if( minor < 10 )
	{
		_configVersion.append(1, '0');
		_configVersion.append(1, str[0]);
	}
	else
	{
		_configVersion.append(2, str[0]);
	}
}

void NGWVersion::setBootVersion( UINT16 major, UINT16 minor )
{
	char str[10];
	sprintf(str, "%d", major);

	if( major < 10 )
	{
		_bootVersion.append(1, '0');
		_bootVersion.append(1, str[0]);
	}
	else
	{
		_bootVersion.append(2, str[0]);
	}

	_bootVersion.append(1, '.');

	sprintf(str, "%d", minor);

	if( minor < 10 )
	{
		_bootVersion.append(1, '0');
		_bootVersion.append(1, str[0]);
	}
	else
	{
		_bootVersion.append(2, str[0]);
	}

}

void NGWVersion::setDataVersion( UINT16 major, UINT16 minor )
{
	char str[10];
	sprintf(str, "%d", major);

	if( major < 10 )
	{
		_dataVersion.append(1, '0');
		_dataVersion.append(1, str[0]);
	}
	else
	{
		_dataVersion.append(2, str[0]);
	}

	_dataVersion.append(1, '.');
	sprintf(str, "%d", minor);

	if( minor < 10 )
	{
		_dataVersion.append(1, '0');
		_dataVersion.append(1, str[0]);
	}
	else
	{
		_dataVersion.append(2, str[0]);
	}
}

void NGWVersion::setAppVersion( UINT16 major, UINT16 minor )
{
	char str[10];
	sprintf(str, "%d", major);

	if( major < 10 )
	{
		_appVersion.append(1, '0');
		_appVersion.append(1, str[0]);
	}
	else
	{
		_appVersion.append(2, str[0]);
	}

	_appVersion.append(1, '.');
	sprintf(str, "%d", minor);

	if( minor < 10 )
	{
		_appVersion.append(1, '0');
		_appVersion.append(1, str[0]);
	}
	else
	{
		_appVersion.append(2, str[0]);
	}
}

const char *NGWVersion::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &_nodeId;
	for (unsigned int i = 0; i < sizeof(_nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &_wrapBoxChecksum;
	for (unsigned int i = 0; i < sizeof(_wrapBoxChecksum); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) _configVersion.data();
	for (unsigned int i = 0; i < _configVersion.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) _bootVersion.data();
	for (unsigned int i = 0; i < _bootVersion.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) _dataVersion.data();
	for (unsigned int i = 0; i < _dataVersion.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) _appVersion.data();
	for (unsigned int i = 0; i < _appVersion.size(); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}



