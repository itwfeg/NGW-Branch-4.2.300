/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERREQUESTSTOCKMETRICS_H
#define HFCPRINTERREQUESTSTOCKMETRICS_H

#include "HfcObject.h"
#include "PrinterRequestStockMetrics.h"

class HfcPrinterRequestStockMetrics : public PrinterRequestStockMetrics, public HfcObject
{
	public:

		HfcPrinterRequestStockMetrics();
		HfcPrinterRequestStockMetrics(const char *data);
		HfcPrinterRequestStockMetrics(const char *data, int len, int formatType);

		~HfcPrinterRequestStockMetrics();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERREQUESTSTOCKMETRICS_H
