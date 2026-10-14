/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTHEADPOWER_H
#define HFCPRINTHEADPOWER_H

#include "HfcObject.h"
#include "PrintheadPower.h"

typedef enum HfcPrintheadPowerFieldOffsets
{
	fieldOffsetPHPPrinterType = 1,
	fieldOffsetPHPPowerState,
	fieldOffsetPHPShortCircuitProtection,
	endHfcPrintheadPowerFieldOffsets		//	MUST be last!
} HfcPrintheadPowerFieldOffsets_t;

class HfcPrintheadPower : public PrintheadPower, public HfcObject
{
	public:
		HfcPrintheadPower();
		HfcPrintheadPower(const char *data);
		HfcPrintheadPower(const char *data, int len, int formatType);

		virtual ~HfcPrintheadPower();

		const char *serialize(int format) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCPRINTHEADPOWER_H
