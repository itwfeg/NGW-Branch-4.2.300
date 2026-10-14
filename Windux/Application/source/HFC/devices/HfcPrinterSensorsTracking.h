/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSENSORSTRACKING_H
#define HFCPRINTERSENSORSTRACKING_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterSensorsTracking.h"

typedef enum HfcPrinterSensorsTrackingFieldOffsets
{
	fieldOffsetPSTPrinterType = 1,
	fieldOffsetPSTSensorsTracking,

	endHfcPrinterSensorsTrackingFieldOffsets		//	MUST be last!
} HfcPrinterSensorsTrackingFieldOffsets_t;

class HfcPrinterSensorsTracking : public PrinterSensorsTracking, public HfcObject
{
	public:

		HfcPrinterSensorsTracking(const char *data);
		HfcPrinterSensorsTracking(const char *data, int len, int formatType);

		~HfcPrinterSensorsTracking();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setSensorsTrackingState(PrinterStateType sensorsTracking);
		PrinterStateType getSensorsTrackingState();


	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERSENSORSTRACKING_H
