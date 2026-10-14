#ifndef HFCPRINTERGAPCALSTATUS_H
#define HFCPRINTERGAPCALSTATUS_H
#include "HfcObject.h"
#include "PrinterGapCalStatus.h"
typedef enum HfcPrinterGapCalStatusFieldOffsets
{
	fieldOffsetGapState = 1,
	fieldOffsetDeflectionVoltage,
    fieldOffsetDriveCurrent,
    fieldOffsetBackingVoltage,
    fieldOffsetLabelBackingVoltage,
	endHfcPrinterGapCalStatusFieldOffsets
} HfcPrinterGapCalStatusFieldOffsets_t;

class HfcPrinterGapCalStatus : public PrinterGapCalStatus, public HfcObject
{
	public:

		HfcPrinterGapCalStatus( const char *data );
		~HfcPrinterGapCalStatus();

		virtual const char *serialize( int format = BrowserFormat ) const;

};
#endif
