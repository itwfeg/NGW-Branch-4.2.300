#include "HfcNGWWrapperPause.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcNGWWrapperPause::HfcNGWWrapperPause( const char *data ) : NGWWrapperPause( data )
{

}

HfcNGWWrapperPause::~HfcNGWWrapperPause()
{

}

const char *HfcNGWWrapperPause::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcNGWWrapperPauseFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetState: {
                        HfcObject::serializedData.append( Integer::toString( getState() ) );
                        break;
                    }                
                    case fieldOffsetPkgId: {
                        HfcObject::serializedData.append( Integer::toString( getPackageId() ) );
                        break;
                    }
                    case fieldOffsetPriPosition: {
                        HfcObject::serializedData.append( Integer::toString( getPrimaryPlatePosition() ) );
                        break;
                    }
                    case fieldOffsetReqPriPosition: {
                        HfcObject::serializedData.append( Integer::toString( getRequiredPrimaryPlatePosition() ) );
                        break;
                    }
                    case fieldOffsetSecPosition: {
                        HfcObject::serializedData.append( Integer::toString( getSecondaryPlatePosition() ) );
                        break;
                    }
                    case fieldOffsetReqSecPosition: {
                        HfcObject::serializedData.append( Integer::toString( getRequiredSecondaryPlatePosition() ) );
                        break;
                    }

                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcNGWWrapperPause", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcNGWWrapperPause", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}