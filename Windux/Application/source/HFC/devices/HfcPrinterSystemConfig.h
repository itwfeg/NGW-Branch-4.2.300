/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSYSTEMCONFIG_H
#define HFCPRINTERSYSTEMCONFIG_H

#include "HfcObject.h"
#include "HfcSystemConfig.h"
#include "PrinterSystemConfiguration.h"


class HfcPrinterSystemConfig : public PrinterSystemConfiguration, public HfcObject
{
	public:

		HfcPrinterSystemConfig();
		HfcPrinterSystemConfig(const char *data);
		HfcPrinterSystemConfig(const char *data, int len, int formatType);

		~HfcPrinterSystemConfig();

		virtual const char *serialize(int format = InternalFormat) const;
		

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERSYSTEMCONFIG_H
