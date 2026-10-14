/**
 * @file NGWBootVersion.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message containing source image
 * Created on: Dec 10, 2015
 *
 **/
#include "NGWBootVersion.h"
#include "Integer.h"

NGWBootVersion::NGWBootVersion(Hobart::CEvolMsgReceiver *pHobThread)  : EvolMessage( Wrapper, WrNGWBootVersion, pHobThread )
{
	nodeId = 0;
	appMinorVersion = 0;
	appMajorVersion = 0;
	dataMinorVersion = 0;
	dataMajorVersion = 0;
	bootMinorVersion = 0;
	bootMajorVersion = 0;

	appString = "";
}

NGWBootVersion::NGWBootVersion(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &appMinorVersion;
	for (unsigned int i = 0; i < sizeof(appMinorVersion); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &appMajorVersion;
	for (unsigned int i = 0; i < sizeof(appMajorVersion); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &dataMinorVersion;
	for (unsigned int i = 0; i < sizeof(dataMinorVersion); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &dataMajorVersion;
	for (unsigned int i = 0; i < sizeof(dataMajorVersion); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &bootMinorVersion;
	for (unsigned int i = 0; i < sizeof(bootMinorVersion); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &bootMajorVersion;
	for (unsigned int i = 0; i < sizeof(bootMajorVersion); i++)
	{
		*pChar++ = buffer[j++];
	}
//	if( appMinorVersion < 10 )
//	{
		//pad a zero
//		appString = ( Integer::toString(appMajorVersion) + "." +  Integer::toString(0) +  Integer::toString(appMinorVersion) );
//	}
//	else
//	{
		appString = ( Integer::toString(appMajorVersion) + "." +  Integer::toString(appMinorVersion) );
//	}
}

NGWBootVersion::~NGWBootVersion()
{

}

const char *NGWBootVersion::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &nodeId;
	for (unsigned int i = 0; i < sizeof(nodeId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &appMinorVersion;
	for (unsigned int i = 0; i < sizeof(appMinorVersion); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &appMajorVersion;
	for (unsigned int i = 0; i < sizeof(appMajorVersion); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &dataMinorVersion;
	for (unsigned int i = 0; i < sizeof(dataMinorVersion); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &dataMajorVersion;
	for (unsigned int i = 0; i < sizeof(dataMajorVersion); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &bootMinorVersion;
	for (unsigned int i = 0; i < sizeof(bootMinorVersion); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &bootMajorVersion;
	for (unsigned int i = 0; i < sizeof(bootMajorVersion); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	serializedData = serializedData + appString;
	return serializedData.data();
}

