/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSTOCKMETRICS_H
#define HFCPRINTERSTOCKMETRICS_H

#include "HfcObject.h"
#include "PrinterStockMetrics.h"

typedef enum HfcPrinterStockMetricsFieldOffsets
{
	fieldOffsetPSMPrinterType = 1,
	fieldOffsetPSMWidthInDots,
	fieldOffsetPSMLengthInLines,
	fieldOffsetPSMWidth,				//	Width in inches/mm
	fieldOffsetPSMLength,			//	Length in inches/mm

	endHfcPrinterStockMetricsFieldOffsets		//	MUST be last!
} HfcPrinterStockMetricsFieldOffsets_t;

class HfcPrinterStockMetrics : public PrinterStockMetrics, public HfcObject
{
	public:

		HfcPrinterStockMetrics();
		HfcPrinterStockMetrics(const char *data);

		~HfcPrinterStockMetrics();

		const char *serialize(int format) const;

		bool isContinuousLabelStock() const;

		std::string getHumanReadableLabelDimensions() const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCPRINTERSTOCKMETRICS_H
