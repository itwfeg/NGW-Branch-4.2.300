/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERPRINTERTYPETRACKING_H
#define HFCPRINTERPRINTERTYPETRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterGeneric.h"

typedef enum HfcPrinterPrinterTypeTrackingFieldOffsets
{
	fieldOffsetPPTTPrinterType = 1,
	fieldOffsetPPTTPrinterTypeTracking,

	endHfcPrinterPrinterTypeTrackingFieldOffsets		//	MUST be last!
} HfcPrinterPrinterTypeTrackingFieldOffsets_t;

class HfcPrinterPrinterTypeTracking : public PrinterGeneric, public HfcObject
{
	public:

		HfcPrinterPrinterTypeTracking(const char *data);
		HfcPrinterPrinterTypeTracking(const char *data, int len, int formatType);

		~HfcPrinterPrinterTypeTracking();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setPrinterTypeTrackingState(PrinterStateType printerTypeTracking);
		PrinterStateType getPrinterTypeTrackingState();

	private:
		PrinterStateType printerTypeTracking;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCPRINTERPRINTERTYPETRACKING_H
