/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERREQUESTPRINTHEADPOWER_H
#define HFCPRINTERREQUESTPRINTHEADPOWER_H

#include "HfcPrintheadPower.h"

class HfcPrinterRequestPrintheadPower : public HfcPrintheadPower
{
	public:
		HfcPrinterRequestPrintheadPower();
		HfcPrinterRequestPrintheadPower(const char *data);
		HfcPrinterRequestPrintheadPower(const char *data, int len, int formatType);

		~HfcPrinterRequestPrintheadPower();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERREQUESTPRINTHEADPOWER_H
