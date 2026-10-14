#include "HfcPrinterGapCalStatus.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Long.h"

HfcPrinterGapCalStatus::HfcPrinterGapCalStatus(const char *data)
	: PrinterGapCalStatus(data)
{

}

HfcPrinterGapCalStatus::~HfcPrinterGapCalStatus()
{


}

const char *HfcPrinterGapCalStatus::serialize(int format) const
{
	
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

			HfcObject::serializedData.append( Integer::toString((int)getState()) );
			HfcObject::serializedData.append( BrowserFormatSeparator );

			HfcObject::serializedData.append( util_string::doubletostring( getDeflectionVoltage() ) );
			HfcObject::serializedData.append( BrowserFormatSeparator );

			HfcObject::serializedData.append( util_string::doubletostring( getDriveCurrent() ) );
			HfcObject::serializedData.append( BrowserFormatSeparator );

			HfcObject::serializedData.append( util_string::doubletostring( getBackingVoltage() ) );
			HfcObject::serializedData.append( BrowserFormatSeparator );

			HfcObject::serializedData.append( util_string::doubletostring( getlabelBackingVoltage() ) );

			Debug::trace( "HfcPrinterGapCalStatus", "HfcPrinterGapCalStatus" + HfcObject::serializedData );
			break;
		}

		default:
			break;
	}
	return HfcObject::serializedData.data();
}




