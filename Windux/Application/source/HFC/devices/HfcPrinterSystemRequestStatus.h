/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSYSTEMREQUESTSTATUS_H
#define HFCPRINTERSYSTEMREQUESTSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterSystemRequestStatus.h"

class HfcPrinterSystemRequestStatus : PrinterSystemRequestStatus, public HfcObject
{
	public:

		HfcPrinterSystemRequestStatus();
		HfcPrinterSystemRequestStatus(const char *data);
		HfcPrinterSystemRequestStatus(const char *data, int len, int formatType);

		~HfcPrinterSystemRequestStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCPRINTERSYSTEMREQUESTSTATUS_H
