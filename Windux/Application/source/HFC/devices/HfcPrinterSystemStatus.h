/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSYSTEMSTATUS_H
#define HFCPRINTERSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "PrinterSystemStatus.h"

typedef enum HfcPrSysStatusFieldOffsets
{
	fieldOffsetHfcPrSysStatusPrinterType = 1,
	fieldOffsetHfcPrSysStatusSystemChecksum,
	fieldOffsetHfcPrSysStatusRomChecksum,
	fieldOffsetHfcPrSysStatusIsEepromError,
	fieldOffsetHfcPrSysStatusIsBitTestError,
	//fieldOffsetHfcPrSysStatusBitTestAddress,
	fieldOffsetHfcPrSysStatusIsAddressError,
	//fieldOffsetHfcPrSysStatusAddressTestAddress,
	fieldOffsetHfcPrSysStatusSoftwareVersion,
	fieldOffsetHfcPrSysStatusHardwareVersion,

	endHfcPrSysStatusFieldOffsets	//	MUST be last!
} HfcPrSysStatusFieldOffsets_t;

class HfcPrinterSystemStatus : public PrinterSystemStatus, public HfcObject
{
	public:

		static const short PASSED	= 0;
		static const short FAILED	= 1;

		HfcPrinterSystemStatus();
		HfcPrinterSystemStatus(const char *data);
		HfcPrinterSystemStatus(const char *data, int len, int formatType);

		~HfcPrinterSystemStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setBitError(SystemStatusInfo *prSystemStatusInfo, bool trueOrFalse);
		void setAddressError(SystemStatusInfo *prSystemStatusInfo, bool trueOrFalse);
		void setEepromError(SystemStatusInfo *prSystemStatusInfo, bool trueOrFalse);

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCPRINTERSYSTEMSTATUS_H
