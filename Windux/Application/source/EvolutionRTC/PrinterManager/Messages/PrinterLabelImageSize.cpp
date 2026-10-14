
/**
 * @file PrinterLabelImageSize.cpp
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Aaron Swift
 * @brief label image size message
 * Created on: May 10, 2019
 *
 **/
#include "PrinterLabelImageSize.h"
#include "PrinterEvent.h"
#include "Debug.h"
#include "Integer.h"
#include <string>

PrinterLabelImageSize::PrinterLabelImageSize( Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType, unsigned long size ) :
											  PrinterGeneric( PrLabelImageSize, pHobThread, printerType )
{
	size_ = size;
}

PrinterLabelImageSize::PrinterLabelImageSize( const char *buffer ) : PrinterGeneric( buffer )
{
	PrinterGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &size_;
	for( unsigned int i = 0; i < sizeof(size_); i++ ) {
		*pChar++ = buffer[j++];
	}
}

PrinterLabelImageSize::~PrinterLabelImageSize(){}

const char *PrinterLabelImageSize::serialize() const
{
	PrinterGeneric::serialize();

	char *pChar = (char *) &size_;
	for( unsigned int i = 0; i < sizeof(size_); i++ ) {
		serializedData.append( 1, pChar[i] );
	}
	return serializedData.data();
}







