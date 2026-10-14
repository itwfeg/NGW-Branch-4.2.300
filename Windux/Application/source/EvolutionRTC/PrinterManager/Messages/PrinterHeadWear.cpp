/**
 * @file PrinterHeadWear.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Value Max print head dot wear
 * Created on: Jan 31, 2018
 *
 **/
#include "PrinterHeadWear.h"
#include "Debug.h"
#include "Integer.h"
#include <string>

PrinterHeadWear::PrinterHeadWear( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, PrinterDotStatus* dots, uint8_t * counts, int headSize) :
								  PrinterGeneric( PrHeadDotWear, pHobThread, printerType )
{
	this->headSize_ = headSize;
	this->headDotStatus_.assign(dots, dots + headSize);
	this->headDotCounts_.assign(counts, counts + headSize);
}

PrinterHeadWear::PrinterHeadWear( const char *buffer ) : PrinterGeneric( buffer )
{
	PrinterGeneric::serialize();

	int j = getSerializedLength();

	char * pChar = (char *) &headSize_;
	for (unsigned int i = 0; i < sizeof(headSize_); i++)
	{
		*pChar++ = buffer[j++];
	}

	for( int i = 0; i < headSize_; i++ ) {
		headDotStatus_.push_back( (PrinterDotStatus)buffer[j++] );
	}

	for( int i = 0; i < headSize_; i++ ) {
		headDotCounts_.push_back( buffer[j++] );
	}


}

PrinterHeadWear::~PrinterHeadWear()
{

}

const char *PrinterHeadWear::serialize() const
{
	PrinterGeneric::serialize();

	char *pChar = (char *) &headSize_;
	for (unsigned int i = 0; i < sizeof(headSize_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	for( int i = 0; i < headSize_; i++ ) {
		serializedData.append( 1, headDotStatus_[i] );
	}
	for( int i = 0; i < headSize_; i++ ) {
		serializedData.append( 1, headDotCounts_[i] );
	}

	return serializedData.data();
}

void PrinterHeadWear::show()
{
	Debug::trace("PrinterHeadWear", "Number of dots: " + Integer::toString(headSize_));
	for( int i = 0; i < headSize_; i++ ) {
		Debug::trace("PrinterHeadWear", "Dot " + Integer::toString(i) + " Status: " +Integer::toString(headDotStatus_[i]));
		Debug::trace("PrinterHeadWear", "Dot " + Integer::toString(i) + " Counts: " +Integer::toString(headDotCounts_[i]));
	}
}


