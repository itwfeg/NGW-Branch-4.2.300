/**
 * @file WrPPPDist.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position message
 * Created on: Mar 9, 2021
 */
#include "WrPPPDist.h"
#include "Integer.h"
#include "Debug.h"

PPPDist::PPPDist( Hobart::CEvolMsgReceiver *pHobThread, PrPlate plate, unsigned long dmm, unsigned long din ) : WrapperGeneric( WrPPPDist, pHobThread )
{
    plate_ = plate;
    distanceMM_ = dmm;
    distanceIN_ = din;
}

PPPDist::PPPDist( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &plate_;
	for (unsigned int i = 0; i < sizeof(plate_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &distanceMM_;
	for (unsigned int i = 0; i < sizeof(distanceMM_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &distanceIN_;
	for (unsigned int i = 0; i < sizeof(distanceIN_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

PPPDist::~PPPDist()
{

}

const char *PPPDist::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &plate_;
	for (unsigned int i = 0; i < sizeof(plate_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &distanceMM_;
	for (unsigned int i = 0; i < sizeof(distanceMM_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &distanceIN_;
	for (unsigned int i = 0; i < sizeof(distanceIN_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PPPDist::show()
{
    Debug::trace("PPPDist","plate_: " + Integer::toString(plate_) );    
    Debug::trace("PPPDist","distanceMM_: " + Integer::toString(distanceMM_) );    
    Debug::trace("PPPDist","distanceIN_: " + Integer::toString(distanceIN_) );    
}
