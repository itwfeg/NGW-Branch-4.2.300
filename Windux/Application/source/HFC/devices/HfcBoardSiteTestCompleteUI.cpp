#include "HfcBoardSiteTestCompleteUI.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

HfcBoardSiteTestCompleteUI::HfcBoardSiteTestCompleteUI( const char *data ) : BoardSiteTestCompleteUI( data )
{

}

HfcBoardSiteTestCompleteUI::~HfcBoardSiteTestCompleteUI()
{

}
const char *HfcBoardSiteTestCompleteUI::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcBoardSiteTestCompleteUIFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetArkSerialFlash: {
                        HfcObject::serializedData.append( Bool::toString( getArkSerialFlashResult() ) );
                        break;
                    }

                    case fieldOffsetArkSDSlot0: {
                        HfcObject::serializedData.append( Bool::toString( getArkSDSlot0Result() ) );
                        break;
                    }
                    case fieldOffsetArkSDSlot1: {
                        HfcObject::serializedData.append( Bool::toString( getArkSDSlot1Result() ) );
                        break;
                    }
                    case fieldOffsetArkRTC: {
                        HfcObject::serializedData.append( Bool::toString( getArkRTCResult() ) );
                        break;
                    }
                    case fieldOffsetArkUSB: {
                        HfcObject::serializedData.append( Bool::toString( getArkUSBResult() ) );
                        break;
                    }
                    case fieldOffsetArkWifi: {
                        HfcObject::serializedData.append( Bool::toString( getArkWifiResult() ) );
                        break;
                    }
                    case fieldOffsetArkBlueTooth: {
                        HfcObject::serializedData.append( Bool::toString( getArkBlueToothResult() ) );
                        break;
                    }
                    case fieldOffsetArkEthernet: {
                        HfcObject::serializedData.append( Bool::toString( getArkEthernetResult() ) );
                        break;
                    }
                    case fieldOffsetArkMemory: {
                        HfcObject::serializedData.append( Bool::toString( getArkMemoryResult() ) );
                        break;
                    }
                    case fieldOffsetArkCan0: {
                        HfcObject::serializedData.append( Bool::toString( getArkCan0Result() ) );
                        break;
                    }
                    case fieldOffsetKpcCan: {
                        HfcObject::serializedData.append( Bool::toString( getKpcCanResult() ) );
                        break;
                    }
                    case fieldOffsetKpcModel: {
                        HfcObject::serializedData.append( Bool::toString( getKpcModelResult() ) );
                        break;
                    }
                    case fieldOffsetKpcSerialFlash: {
                        HfcObject::serializedData.append( Bool::toString( getKpcSerialFlashResult() ) );
                        break;
                    }
                    case fieldOffsetKpcValueMax: {
                        HfcObject::serializedData.append( Bool::toString( getKpcValueMaxResult() ) );
                        break;
                    }
                    case fieldOffsetKpcCutter: {
                        HfcObject::serializedData.append( Bool::toString( getKpcCutterResult() ) );
                        break;
                    }
                    case fieldOffsetKpcDotWear: {
                        HfcObject::serializedData.append( Bool::toString( getKpcDotWearResult() ) );
                        break;
                    }
                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcBoardSiteTestCompleteUI", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcBoardSiteTestCompleteUI", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}