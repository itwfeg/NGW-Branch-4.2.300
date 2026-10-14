/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERREQUESTINQUIRE_H
#define HFCPRINTERREQUESTINQUIRE_H

#include "HfcObject.h"
#include "PrinterRequestInquire.h"

class HfcPrinterRequestInquire : public PrinterRequestInquire, public HfcObject
{
	public:

		HfcPrinterRequestInquire();
		HfcPrinterRequestInquire(const char *data);
		HfcPrinterRequestInquire(const char *data, int len, int formatType);

		~HfcPrinterRequestInquire();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERREQUESTINQUIRE_H
