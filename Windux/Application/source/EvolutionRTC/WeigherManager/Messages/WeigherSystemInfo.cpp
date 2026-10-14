/**
 * @file WeigherSystemInfo.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max current x,y and z gain factor
 * Created on: Oct 2, 2017
 *
 **/
#include "WeigherSystemInfo.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

WeigherSystemInfo::WeigherSystemInfo( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgSystemInfo, pHobThread )
{
	position = FirstWeigher;
	serialFlashValid = false;
	eepromValid = false;
	softwareVersion.setVersionType(HobVersion::KpcWeigherSW);
	hardwareVersion.setVersionType(HobVersion::KpcWeigherHW);
}

WeigherSystemInfo::WeigherSystemInfo( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &position;
	for (unsigned int i = 0; i < sizeof(position); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &serialFlashValid;
	for (unsigned int i = 0; i < sizeof(serialFlashValid); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &eepromValid;
	for (unsigned int i = 0; i < sizeof(eepromValid); i++)
	{
		*pChar++ = buffer[j++];
	}

	softwareVersion = HobVersion( &buffer[j] );
	//SOF-3860 build number and eng number is the same value for the Weigher!
	// SOF-3978: Need to roll back change for SOF-3860 i.e. the line below ...
	//softwareVersion.setEngRevision( softwareVersion.getBuildNumber() );
	softwareVersion.serialize();
	j += softwareVersion.getSerializedLength();

	hardwareVersion = HobVersion(&buffer[j]);
	hardwareVersion.serialize();
}

WeigherSystemInfo::~WeigherSystemInfo()
{
}

const char *WeigherSystemInfo::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &position;
	for (unsigned int i = 0; i < sizeof(position); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &serialFlashValid;
	for (unsigned int i = 0; i < sizeof(serialFlashValid); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &eepromValid;
	for (unsigned int i = 0; i < sizeof(eepromValid); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pChar_ = softwareVersion.serialize();
	int length = softwareVersion.getSerializedLength();

	serializedData.append( pChar_, length );

	pChar_ = hardwareVersion.serialize();
	length = hardwareVersion.getSerializedLength();

	serializedData.append( pChar_, length );

	return serializedData.data();

	Debug::trace("WeigherSystemInfo", "serializedData: " + serializedData );
}

void WeigherSystemInfo::show()
{
	Debug::trace("WeigherSystemInfo", "position:" + Integer::toString( position ) );
	Debug::trace("WeigherSystemInfo", "serialFlashValid:" + Integer::toString( serialFlashValid ) );
	Debug::trace("WeigherSystemInfo", "eepromValid:" + Integer::toString( eepromValid ) );
	Debug::trace("WeigherSystemInfo", "softwareVersion:" + HobString(softwareVersion.toString()->c_str()) );
	Debug::trace("WeigherSystemInfo", "hardwareVersion:" + HobString(hardwareVersion.toString()->c_str()) );
}

