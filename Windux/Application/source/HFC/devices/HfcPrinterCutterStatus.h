/* Cutter Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERCUTTERSTATUS_H
#define HFCPRINTERCUTTERSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterCutterStatus.h"					// HfcPrinterCutterStatus

typedef enum HfcPrinterCutterStatusFieldOffsets
{
	fieldOffsetHome = 1,
	fieldOffsetInstalled,
	fieldOffsetInterlock,
	fieldOffsetJammed,
	endHfcPrinterCutterStatusFieldOffsets		//	Dont know if I need these!
} HfcPrinterCutterStatusFieldOffsets_t;

class HfcPrinterCutterStatus : public PrinterCutterStatus, public HfcObject
{
	public:

		HfcPrinterCutterStatus(const char *data);
		HfcPrinterCutterStatus(const char *data, int len, int formatType);

		~HfcPrinterCutterStatus();

		virtual const char *serialize(int format = InternalFormat) const;


private:
#ifdef USE_ACCOUNTANT
	static int dataType;
	static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERCUTTERSTATUS_H
