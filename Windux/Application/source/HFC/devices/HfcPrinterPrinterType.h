/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERPRINTERTYPE_H
#define HFCPRINTERPRINTERTYPE_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterPrinterType.h"

typedef enum HfcPrinterPrinterTypeFieldOffsets
{
	fieldOffsetPTPrinterType = 1,
	fieldOffsetPTUsbNodeId,
	endHfcPrinterPrinterTypeFieldOffsets		//	MUST be last!
} HfcPrinterPrinterTypeFieldOffsets_t;

class HfcPrinterPrinterType : public PrinterPrinterType, public HfcObject
{
	public:

		HfcPrinterPrinterType(const char *data);
		HfcPrinterPrinterType(const char *data, int len, int formatType);

		~HfcPrinterPrinterType();

		virtual const char *serialize(int format = InternalFormat) const;


	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERPRINTERTYPE_H
