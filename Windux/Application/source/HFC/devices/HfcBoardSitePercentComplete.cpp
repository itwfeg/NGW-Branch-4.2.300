#include "HfcBoardSitePercentComplete.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

HfcBoardSitePercentComplete::HfcBoardSitePercentComplete( const char *data ) : BoardSitePercentComplete( data )
{

}

HfcBoardSitePercentComplete::~HfcBoardSitePercentComplete()
{

}
const char *HfcBoardSitePercentComplete::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString( (int)getDeviceType() );
			HfcObject::serializedData.append( BrowserFormatSeparator );
			HfcObject::serializedData.append( Integer::toString((int)getEventType() ));
			unsigned int i = 1;
            while( i < endHfcBoardSitePercentCompleteFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetPercent: {
                        HfcObject::serializedData.append( Integer::toString( getPercentComplete() ) );
                        break;
                    }

                    case fieldOffsetTestCompete: {
                        HfcObject::serializedData.append( Integer::toString( getTest() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcBoardSitePercentComplete", "serialize() Invalid format!");
			break;
	}
    
    HfcObject::serializedData.append( "," );
    switch( getTest() )
    {        
        case ArkNoTest: {
            HfcObject::serializedData += "No Test";
            break;
        }
        case ArkFlashTest: 
        {
            HfcObject::serializedData += "ARK Serial Flash Test";
            break;
        }
        case ArkSDSlot0Test: {
            HfcObject::serializedData += "ARK SD Card Slot 0 Test";
            break;
        }
        case ArkSDSlot1Test: {
            HfcObject::serializedData += "ARK SD Card Slot 1 Test";
            break;
        }
        case ArkUSBTest: {
            HfcObject::serializedData += "ARK USB Test";
            break;
        }
        case ArkWifiTest: {
            HfcObject::serializedData += "ARK WIFI Test";
            break;
        }
        case ArkEthernetTest: {
            HfcObject::serializedData += "ARK Ethernet Test";
            break;
        }
        case ArkCan0Test: {
            HfcObject::serializedData += "ARK CAN 0 Test";
            break;
        }
        case ArkMemTest: {
            HfcObject::serializedData += "ARK Memory Test";
            break;
        }
        case ArkRTCTest: {
            HfcObject::serializedData += "ARK Real Time Clock Test";
            break;
        }
        case ArkBToothTest: {
            HfcObject::serializedData += "ARK Blue Tooth Test";
            break;
        }
        case KpcCan0Test: {
            HfcObject::serializedData += "KPC CAN 0 Test";
            break;
        }
        case KpcModelTest: {
            HfcObject::serializedData += "KPC Model Test";
            break;
        }
        case KpcSerialFlashTest: {
            HfcObject::serializedData += "KPC Serial Flash Test";
            break;
        }
        case KpcValueMaxTest: {
            HfcObject::serializedData += "KPC Value Max Test";
            break;
        }
        case KpcCutterTest: {
            HfcObject::serializedData += "KPC Cutter Test";
            break;
        }
        case KpcDotWearTest: {
            HfcObject::serializedData += "KPC Dot Wear Test";
            break;
        }
    }

    Debug::trace("HfcBoardSitePercentComplete", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}
