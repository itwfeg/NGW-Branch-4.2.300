/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERUSECONTINUOUSSTOCK_H
#define HFCPRINTERUSECONTINUOUSSTOCK_H

#include "HfcObject.h"
#include "PrinterGeneric.h"

class HfcPrinterUseContinuousStock : public PrinterGeneric, public HfcObject
{
	public:

		HfcPrinterUseContinuousStock();
		HfcPrinterUseContinuousStock(const char *data);
		HfcPrinterUseContinuousStock(const char *data, int len, int formatType);

		~HfcPrinterUseContinuousStock();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERUSECONTINUOUSSTOCK_H
