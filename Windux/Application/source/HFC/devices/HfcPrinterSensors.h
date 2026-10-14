/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSENSORS_H
#define HFCPRINTERSENSORS_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterSensors.h"

typedef enum HfcPrinterSensorsFieldOffsets
{
	fieldOffsetPSPrinterType = 1,
	fieldOffsetPSHeadup,
	fieldOffsetPSHeadTemperature,
	fieldOffsetPSLabelWidth,
	fieldOffsetPSLabel,
	fieldOffsetPSMedia,
	fieldOffsetPSHeadVoltage,

	/* re-use for freestanding service scale to report low stock and engine state */
	fieldOffsetPSRegulatorTemperature,
/*
	fieldOffsetPSHeadCurrent,
	fieldOffsetPSLabelHighAverage,
	fieldOffsetPSLabelLowAverage,
	fieldOffsetPSLabelThreshold,
	fieldOffsetPSMediaHighAverage,
	fieldOffsetPSMediaLowAverage,
	fieldOffsetPSMediaThreshold,
*/
	endHfcPrinterSensorsFieldOffsets		//	MUST be last!
} HfcPrinterSensorsFieldOffsets_t;

class HfcPrinterSensors : public PrinterSensors, public HfcObject
{
	public:

		HfcPrinterSensors(const char *data);
		HfcPrinterSensors(const char *data, int len, int formatType);

		~HfcPrinterSensors();

		virtual const char *serialize(int format = InternalFormat) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERSENSORS_H
