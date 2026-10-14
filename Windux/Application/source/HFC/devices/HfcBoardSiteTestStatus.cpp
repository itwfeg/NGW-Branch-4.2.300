#include "HfcBoardSiteTestStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

HfcBoardSiteTestStatus::HfcBoardSiteTestStatus( const char *data ) : BoardSiteTestStatus( data )
{

}

HfcBoardSiteTestStatus::~HfcBoardSiteTestStatus()
{

}
const char *HfcBoardSiteTestStatus::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString( (int)getDeviceType() );
			HfcObject::serializedData.append( BrowserFormatSeparator );
			HfcObject::serializedData.append( Integer::toString((int)getEventType() ));
			unsigned int i = 1;
            while( i < endHfcBoardSiteTestStatusFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetReady: {
                        HfcObject::serializedData.append( Bool::toString( isReady() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcBoardSiteTestStatus", "serialize() Invalid format!");
			break;
	}
    

    Debug::trace("HfcBoardSiteTestStatus", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}
