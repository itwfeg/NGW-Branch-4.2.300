/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERPRINTTESTLABEL_H
#define HFCPRINTERPRINTTESTLABEL_H

#include "HfcObject.h"
#include "PrinterPrintTestLabel.h"


typedef enum HfcPrinterTestLabelOffsets
{
	fieldOffsetPrTestLabelPrinterType = 1,
	fieldOffsetPrTestLabelLabelType,
	fieldOffsetPrTestLabelContinuous,
	fieldOffsetPrTestCommandOptions,
	endHfcPrinterTestLabelOffsets		//	MUST be last!
} HfcPrinterTestLabelOffsets_t;


class HfcPrinterPrintTestLabel : public PrinterPrintTestLabel, public HfcObject
{
	public:

		HfcPrinterPrintTestLabel();
		HfcPrinterPrintTestLabel(const char *data);
		HfcPrinterPrintTestLabel(const char *data, int len, int formatType);

		~HfcPrinterPrintTestLabel();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERPRINTTESTLABEL_H
