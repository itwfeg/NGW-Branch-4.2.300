#include "HfcWrPPPConfig.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"

HfcWrPPPConfig::HfcWrPPPConfig( const char *data ) : PPPConfig( data )
{

}

HfcWrPPPConfig::~HfcWrPPPConfig()
{

}
const char *HfcWrPPPConfig::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat: {

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			unsigned int i = 1;
            while( i < endHfcPPPConfigFieldOffsets ) {
                HfcObject::serializedData.append(BrowserFormatSeparator);
                switch( i )
                {
                    case fieldOffsetPriCal: {
                        HfcObject::serializedData.append( Integer::toString( getPrimaryCalDistance() ) );
                        break;
                    }
                    case fieldOffsetSecCal: {
                        HfcObject::serializedData.append( Long::toString( getSecondaryCalDistance() ) );
                        break;
                    }
                    case fieldOffsetRate: {
                        HfcObject::serializedData.append( Long::toString( getSampleRate() ) );
                        break;
                    }
                    case fieldOffsetPriSlope: {
                        HfcObject::serializedData.append( Long::toString( getPrimarySlope() ) );
                        break;
                    }
                    case fieldOffsetSecSlope: {
                        HfcObject::serializedData.append( Long::toString( getSecondarySlope() ) );
                        break;
                    }
                    case fieldOffsetPriHomeDist: {
                        HfcObject::serializedData.append( Integer::toString( getPrimaryPlateHomeDistance()  ) );
                        break;
                    }
                    case fieldOffsetSecHomeDist: {
                        HfcObject::serializedData.append( Integer::toString( getSecondaryPlateHomeDistance() ) );
                        break;
                    }
                    case fieldOffsetMechanicalDist: {
                        HfcObject::serializedData.append( Integer::toString( getMechanicalHomeSpec() ) );
                        break;
                    }
                    #if 1 /* VL53L1X */
                        case fieldOffsetX: {
                            HfcObject::serializedData.append( Integer::toString( getXFieldOfView() ) );
                            break;
                        }
                        case fieldOffsetY: {
                            HfcObject::serializedData.append( Integer::toString( getYFieldOfView() ) );
                            break;
                        }
                        case fieldOffsetCenter: {
                            HfcObject::serializedData.append( Integer::toString( getCenterFieldOfView() ) );
                            break;
                        }
                        case fieldOffsetCalOffsetA: {
                            HfcObject::serializedData.append( Integer::toString( getSensorACalOffset() ) );
                            break;
                        }
                        case fieldOffsetCalOffsetB: {
                            HfcObject::serializedData.append( Integer::toString( getSensorBCalOffset() ) );
                            break;                        
                        }

                    #endif    

                }
                i++;                
            }
            break;
		}
		default:
			Debug::trace("HfcWrPPPConfig", "serialize() Invalid format!");
			break;
	}
    Debug::trace("HfcWrPPPConfig", "serialize(): " + HfcObject::serializedData );
	return HfcObject::serializedData.data();
}