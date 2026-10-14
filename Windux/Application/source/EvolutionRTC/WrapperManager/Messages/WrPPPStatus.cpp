/**
 * @file WrPPPStatus.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate status message
 * Created on: Mar 26, 2021
 */
#include "WrPPPStatus.h"
#include "Integer.h"
#include "Debug.h"

PPPStatus::PPPStatus( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrPPPStatus, pHobThread )
{
    mode_ = RUN;
    sensorStatus_ = OPERATIONAL;
}

PPPStatus::PPPStatus( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &mode_;
	for (unsigned int i = 0; i < sizeof(mode_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensorStatus_;
	for (unsigned int i = 0; i < sizeof(sensorStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PPPStatus::~PPPStatus()
{

}

const char *PPPStatus::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &mode_;
	for (unsigned int i = 0; i < sizeof(mode_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &sensorStatus_;
	for (unsigned int i = 0; i < sizeof(sensorStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

void PPPStatus::show()
{
    Debug::trace("PPPStatus","mode_: " + Integer::toString(mode_) );    
    Debug::trace("PPPStatus","sensorStatus_: " + Integer::toString(sensorStatus_) );        
}
