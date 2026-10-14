/**
 * @file WrPPPCalibration.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position calibration message
 * Created on: Mar 15, 2021
 */
#include "WrPPPCalibration.h"
#include "Integer.h"
#include "Debug.h"

PPPCalib::PPPCalib( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrPPPCalibration, pHobThread )
{
    plate0Status_ = _READY;
    calDistance0MM_ = 0;
    calDistance0IN_ = 0;
	plate1Status_ = _READY;
    calDistance1MM_ = 0;
    calDistance1IN_ = 0;
}

PPPCalib::PPPCalib( const char *buffer ) : WrapperGeneric( buffer )
{
    setCalDistance0Inches();
    setCalDistance1Inches();
	
    WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &plate0Status_;
	for (unsigned int i = 0; i < sizeof(plate0Status_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &calDistance0MM_;
	for (unsigned int i = 0; i < sizeof(calDistance0MM_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &calDistance0IN_;
	for (unsigned int i = 0; i < sizeof(calDistance0IN_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &plate1Status_;
	for (unsigned int i = 0; i < sizeof(plate1Status_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &calDistance1MM_;
	for (unsigned int i = 0; i < sizeof(calDistance1MM_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &calDistance1IN_;
	for (unsigned int i = 0; i < sizeof(calDistance1IN_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PPPCalib::~PPPCalib()
{

}

const char *PPPCalib::serialize() const
{
	WrapperGeneric::serialize();
    
	char *pChar = (char *) &plate0Status_;
	for (unsigned int i = 0; i < sizeof(plate0Status_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &calDistance0MM_;
	for (unsigned int i = 0; i < sizeof(calDistance0MM_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &calDistance0IN_;
	for (unsigned int i = 0; i < sizeof(calDistance0IN_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &plate1Status_;
	for (unsigned int i = 0; i < sizeof(plate1Status_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &calDistance1MM_;
	for (unsigned int i = 0; i < sizeof(calDistance1MM_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &calDistance1IN_;
	for (unsigned int i = 0; i < sizeof(calDistance1IN_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PPPCalib::show()
{
    //setCalDistance0Inches();
    //setCalDistance1Inches();
    Debug::trace("PPPCal","plate0Status_: " + Integer::toString(plate0Status_) );    
    Debug::trace("PPPCal","calDistance0MM_: " + Integer::toString(calDistance0MM_) );    
    Debug::trace("PPPCal","calDistance0IN_: " + Integer::toString(calDistance0IN_) ); 
	Debug::trace("PPPCal","plate1Status_: " + Integer::toString(plate1Status_) );     
    Debug::trace("PPPCal","calDistance1MM_: " + Integer::toString(calDistance1MM_) );    
    Debug::trace("PPPCal","calDistance1IN_: " + Integer::toString(calDistance1IN_) );    
}
