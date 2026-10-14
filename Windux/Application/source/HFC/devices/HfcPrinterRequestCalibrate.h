/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERREQUESTCALIBRATE_H
#define HFCPRINTERREQUESTCALIBRATE_H

#include "HfcObject.h"
#include "PrinterRequestCalibrate.h"

typedef enum HfcPrinterRequestCalibrateFieldOffsets
{
	fieldOffsetPrReqCalibratePrinterType = 1,
	fieldOffsetPrReqCalibratePrinterCalibrateType,
	endHfcPrinterRequestCalibrateFieldOffsets		//	MUST be last!
} HfcPrinterRequestCalibrateFieldOffsets_t;

class HfcPrinterRequestCalibrate : public PrinterRequestCalibrate, public HfcObject
{
	public:

		HfcPrinterRequestCalibrate();
		HfcPrinterRequestCalibrate(const char *data);
		HfcPrinterRequestCalibrate(const char *data, int len, int formatType);

		~HfcPrinterRequestCalibrate();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERREQUESTCALIBRATE_H
