/* HeadDot Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERHEADDOTWEAR_H
#define HFCPRINTERHEADDOTWEAR_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterHeadWear.h"					// HfcPrinterHeadDotWear

typedef enum HfcPrinterHeadDotWearFieldOffsets
{
    fieldOffsetHeadDotWearSize = 1,
    fieldOffsetHeadDotWearStatus,
    fieldOffsetHeadDotWearCounts,
    endHfcPrinterHeadDotWearFieldOffsets
} HfcPrinterHeadDotWearFieldOffsets_t;

class HfcPrinterHeadDotWear : public PrinterHeadWear, public HfcObject
{
	public:

		HfcPrinterHeadDotWear(const char *data);
		HfcPrinterHeadDotWear(const char *data, int len, int formatType);

		~HfcPrinterHeadDotWear();

		virtual const char *serialize(int format = InternalFormat) const;

private:
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif
};

#endif	//	HFCPRINTERHEADDOTWEAR_H
