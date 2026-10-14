/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERREQUESTLABELSIZING_H
#define HFCPRINTERREQUESTLABELSIZING_H

#include "HfcObject.h"
#include "PrinterGeneric.h"

class HfcPrinterRequestLabelSizing : public PrinterGeneric, public HfcObject
{
	public:

		HfcPrinterRequestLabelSizing();
		HfcPrinterRequestLabelSizing(const char *data);
		HfcPrinterRequestLabelSizing(const char *data, int len, int formatType);

		~HfcPrinterRequestLabelSizing();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERREQUESTLABELSIZING_H
