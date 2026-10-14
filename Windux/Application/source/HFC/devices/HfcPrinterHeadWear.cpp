/*
 * HfcPrinterHeadWear.cpp
 *
 *  Created on: Feb 1, 2018
 *      Author: aaron
 */
#include "HfcPrinterHeadWear.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

HfcPrinterHeadWear::HfcPrinterHeadWear(const char *data)
	: PrinterHeadWear(data)
{

}

HfcPrinterHeadWear::~HfcPrinterHeadWear()
{


}

const char *HfcPrinterHeadWear::serialize(int format) const
{
	Debug::trace("HfcPrinterHeadWear", "serialize()");
	HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat:
		break;

		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString( (int)getDeviceType() );
			HfcObject::serializedData.append( BrowserFormatSeparator );
			HfcObject::serializedData.append( Integer::toString( (int)getEventType() ) );
			HfcObject::serializedData.append( BrowserFormatSeparator );

			HfcObject::serializedData.append( Integer::toString((int)headSize_) );
			HfcObject::serializedData.append( BrowserFormatSeparator );
			
			HobString dots_ = "";
			for( int i = 0; i < headSize_; i++) {
				dots_.append( Integer::toString( headDotStatus_[i] ) );
				dots_.append( BrowserFormatSeparator );
			}

			HfcObject::serializedData.append( BrowserFormatSeparator );

			for( int i = 0; i < headSize_; i++) {
				dots_.append( Integer::toString( headDotCounts_[i] ) );
				dots_.append( BrowserFormatSeparator );

			}

			HfcObject::serializedData.append( dots_ );

			

			Debug::trace( "HfcPrinterHeadWear", HobString( "serialized print head wear: " ) + dots_ );
			break;
		}

		default:
			break;
	}
	return HfcObject::serializedData.data();
}




