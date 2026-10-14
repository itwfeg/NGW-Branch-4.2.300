#include "HfcWrPPPStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcWrPPPStatus::HfcWrPPPStatus( const char *data ) : PPPStatus( data )
{

}

HfcWrPPPStatus::~HfcWrPPPStatus()
{

}

const char *HfcWrPPPStatus::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPStatusFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetSensorMode: {
                        HfcObject::serializedData.append( Integer::toString( getMode() ) );
                        break;
                    }
                    case fieldOffsetSensor: {
                        HfcObject::serializedData.append( Long::toString( getSensorStatus() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcWrPPPStatus", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPStatus", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}