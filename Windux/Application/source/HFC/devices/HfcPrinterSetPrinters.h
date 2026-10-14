/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSETPRINTERS_H
#define HFCPRINTERSETPRINTERS_H

#include "evolution.h"
#include "HfcObject.h"
#include "Printer.h"

typedef enum HfcPrinterSetPrintersFieldOffsets
{
	fieldOffsetSPPrinterType0 = 1,
	fieldOffsetSPPrinterNodeId0,
	fieldOffsetSPPrinterType1,
	fieldOffsetSPPrinterNodeId1,
	fieldOffsetSPPrinterType2,
	fieldOffsetSPPrinterNodeId2,
	endHfcPrinterSetPrintersFieldOffsets		//	MUST be last!
} HfcPrinterSetPrintersFieldOffsets_t;

class HfcPrinterSetPrinters : public HfcObject
{
	public:
		HfcPrinterSetPrinters(const char *data, int len, int formatType = BrowserFormat);
		~HfcPrinterSetPrinters();

		virtual const char *serialize(int format = BrowserFormat) const;

		PrinterType getPrinterType0() const;
		void setPrinterType0(PrinterType prType);

		USBD_NODE_ID getPrinterNodeId0() const;
		void setPrinterNodeId0(USBD_NODE_ID nodeId);

		PrinterType getPrinterType1() const;
		void setPrinterType1(PrinterType prType);

		USBD_NODE_ID getPrinterNodeId1() const;
		void setPrinterNodeId1(USBD_NODE_ID nodeId);

		PrinterType getPrinterType2() const;
		void setPrinterType2(PrinterType prType);

		USBD_NODE_ID getPrinterNodeId2() const;
		void setPrinterNodeId2(USBD_NODE_ID nodeId);

		bool validSetup();

	private:
		HfcPrinterSetPrinters();

		PrinterType prType0;
		PrinterType prType1;
		PrinterType prType2;

		USBD_NODE_ID nodeId0;
		USBD_NODE_ID nodeId1;
		USBD_NODE_ID nodeId2;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCPRINTERSETPRINTERS_H
