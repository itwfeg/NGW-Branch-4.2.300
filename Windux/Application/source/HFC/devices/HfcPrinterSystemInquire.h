/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSYSTEMINQUIRE_H
#define HFCPRINTERSYSTEMINQUIRE_H

#include "HfcPrinterSystemConfig.h"

class HfcPrinterSystemInquire : public HfcPrinterSystemConfig
{
	public:

		HfcPrinterSystemInquire();
		HfcPrinterSystemInquire(const char *data);
		HfcPrinterSystemInquire(const char *data, int len, int formatType);

		~HfcPrinterSystemInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERSYSTEMINQUIRE_H
