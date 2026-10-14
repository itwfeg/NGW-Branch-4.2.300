#include "HfcWrPPPCalibration.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcWrPPPCal::HfcWrPPPCal( const char *data ) : PPPCalib( data )
{

}

HfcWrPPPCal::~HfcWrPPPCal()
{

}
const char *HfcWrPPPCal::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPCalFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetPlate0Status: {
                        HfcObject::serializedData.append( Integer::toString( getCalPlate0Status() ) );
                        break;
                    }
                    case fieldOffsetCalDist0MM: {
                        HfcObject::serializedData.append( Long::toString( getCalDistance0Millimeter() ) );
                        break;
                    }
                    case fieldOffsetCalDist0IN: {
                        HfcObject::serializedData.append( Long::toString( getCalDistance0Inches() ) );
                        break;
                    }
                    case fieldOffsetPlate1Status: {
                        HfcObject::serializedData.append( Integer::toString( getCalPlate1Status() ) );
                        break;
                    }
                    case fieldOffsetCalDist1MM: {
                        HfcObject::serializedData.append( Long::toString( getCalDistance1Millimeter() ) );
                        break;
                    }
                    case fieldOffsetCalDist1IN: {
                        HfcObject::serializedData.append( Long::toString( getCalDistance1Inches() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcWrPPPCal", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPCal", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}