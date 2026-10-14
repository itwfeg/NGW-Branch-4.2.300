/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERINQUIRE_H
#define HFCPRINTERINQUIRE_H

#include "HfcPrinterConfig.h"

class HfcPrinterInquire : public HfcPrinterConfig
{
	public:

		HfcPrinterInquire();
		HfcPrinterInquire(const char *data);
		HfcPrinterInquire(const char *data, int len, int formatType);

		~HfcPrinterInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERINQUIRE_H
