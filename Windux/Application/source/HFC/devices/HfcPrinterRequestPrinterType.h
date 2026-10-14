/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERREQUESTPRINTERTYPE_H
#define HFCPRINTERREQUESTPRINTERTYPE_H

#include "HfcObject.h"
#include "PrinterRequestPrinterType.h"

class HfcPrinterRequestPrinterType : public PrinterRequestPrinterType, public HfcObject
{
	public:

		HfcPrinterRequestPrinterType();
		HfcPrinterRequestPrinterType(const char *data);
		HfcPrinterRequestPrinterType(const char *data, int len, int formatType);

		~HfcPrinterRequestPrinterType();

		virtual const char *serialize(int format = InternalFormat);

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCPRINTERREQUESTPRINTERTYPE_H
