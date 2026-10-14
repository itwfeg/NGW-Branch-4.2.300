/* HeadDot Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERHEADDOTSTATUS_H
#define HFCPRINTERHEADDOTSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterHeadDotStatus.h"					// HfcPrinterHeadDotStatus

typedef enum HfcPrinterHeadDotStatusFieldOffsets
{
	fieldOffsetHeadDotSource = 1,
	fieldOffsetHeadDotDestination,
	fieldOffsetHeadDotStatus,
	endHfcPrinterHeadDotStatusFieldOffsets		//	Dont know if I need these!
} HfcPrinterHeadDotStatusFieldOffsets_t;

class HfcPrinterHeadDotStatus : public PrinterHeadDotStatus, public HfcObject
{
	public:

		HfcPrinterHeadDotStatus(const char *data);
		HfcPrinterHeadDotStatus(const char *data, int len, int formatType);

		~HfcPrinterHeadDotStatus();

		virtual const char *serialize(int format = InternalFormat) const;


private:
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERHEADDOTSTATUS_H
