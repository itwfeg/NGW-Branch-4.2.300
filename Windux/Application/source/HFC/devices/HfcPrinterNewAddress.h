/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERNEWADDRESS_H
#define HFCPRINTERNEWADDRESS_H

#include "evolution.h"
#include "HfcObject.h"
#include "PrinterNewAddress.h"

typedef enum HfcPrinterNewAddressFieldOffsets
{
	fieldOffsetNAPrinterType = 1,
	fieldOffsetNAPhysicalAddress,
	endHfcPrinterNewAddressFieldOffsets		//	MUST be last!
} HfcPrinterNewAddressFieldOffsets_t;

class HfcPrinterNewAddress : public PrinterNewAddress, public HfcObject
{
	public:
		HfcPrinterNewAddress(const char *data);
		HfcPrinterNewAddress(const char *data, int len, int formatType);

		~HfcPrinterNewAddress();

		virtual const char *serialize(int format = InternalFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCPRINTERNEWADDRESS_H
