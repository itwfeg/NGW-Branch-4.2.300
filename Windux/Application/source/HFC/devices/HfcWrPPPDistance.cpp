#include "HfcWrPPPDistance.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcWrPPPDist::HfcWrPPPDist( const char *data ) : PPPDist( data )
{

}

HfcWrPPPDist::~HfcWrPPPDist()
{

}

const char *HfcWrPPPDist::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPDistFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetPlate: {
                        HfcObject::serializedData.append( Integer::toString( getPlate() ) );
                        break;
                    }
                    case fieldOffsetDistMM: {
                        HfcObject::serializedData.append( Long::toString( getDistanceMillimeter() ) );
                        break;
                    }
                    case fieldOffsetDistIN: {
                        HfcObject::serializedData.append( Long::toString( getDistanceInches() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcWrPPPDist", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPDist", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}

