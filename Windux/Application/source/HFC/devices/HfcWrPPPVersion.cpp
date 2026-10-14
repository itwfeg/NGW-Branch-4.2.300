#include "HfcWrPPPVersion.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcWrPPPVersion::HfcWrPPPVersion( const char *data ) : PPPVersion( data )
{

}

HfcWrPPPVersion::~HfcWrPPPVersion()
{

}
const char *HfcWrPPPVersion::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPVerFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetId: {
                        HfcObject::serializedData.append( Integer::toString( getProductId() ) );
                        break;
                    }
                    case fieldOffsetSwVersion: {
                        HfcObject::serializedData.append( getSoftwareVersion() );
                        break;
                    }
                    case fieldOffsetHwVersion: {
                        HfcObject::serializedData.append( getHardwareVersion() );
                        break;
                    }
                    case fieldOffsetBootVersion: {
                        HfcObject::serializedData.append( getBootVersion() );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcWrPPPVersion", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPVersion", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}