/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERADVANCELABELSTOCK_H
#define HFCPRINTERADVANCELABELSTOCK_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterAdvanceLabelStock.h"

typedef enum HfcPrinterAdvanceLabelStockFieldOffsets
{
	fieldOffsetPALSPrinterType = 1,
	fieldOffsetPALSNumSteps,
	endHfcPrinterAdvanceLabelStockFieldOffsets		//	MUST be last!
} HfcPrinterAdvanceLabelStockFieldOffsets_t;

class HfcPrinterAdvanceLabelStock : public PrinterAdvanceLabelStock, public HfcObject
{
	public:

		HfcPrinterAdvanceLabelStock(const char *data);
		HfcPrinterAdvanceLabelStock(const char *data, int len, int formatType);

		~HfcPrinterAdvanceLabelStock();

		virtual const char *serialize(int format = InternalFormat) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERADVANCELABELSTOCK_H
