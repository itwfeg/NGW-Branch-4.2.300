/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCNUMERICSYSTEMSTATUS_H
#define HFCNUMERICSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "NumericSystemStatus.h"

typedef enum HfcNmSysStatusFieldOffsets
{
	fieldOffsetHfcNmSysStatusSystemChecksum = 1,
	fieldOffsetHfcNmSysStatusRomChecksum,
	fieldOffsetHfcNmSysStatusIsEepromError,
	fieldOffsetHfcNmSysStatusIsBitTestError,
	//fieldOffsetHfcNmSysStatusBitTestAddress,
	fieldOffsetHfcNmSysStatusIsAddressError,
	//fieldOffsetHfcNmSysStatusAddressTestAddress,
	fieldOffsetHfcNmSysStatusSoftwareVersion,
	fieldOffsetHfcNmSysStatusHardwareVersion,

	endHfcNmSysStatusFieldOffsets	//	MUST be last!
} HfcNmSysStatusFieldOffsets_t;

class HfcNumericSystemStatus : public NumericSystemStatus, public HfcObject
{
	public:

		static const short PASSED	= 0;
		static const short FAILED	= 1;

		HfcNumericSystemStatus();
		HfcNumericSystemStatus(const char *data);
		HfcNumericSystemStatus(const char *data, int len, int formatType);

		~HfcNumericSystemStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setBitError(SystemStatusInfo *nmSystemStatusInfo, bool trueOrFalse);
		void setAddressError(SystemStatusInfo *nmSystemStatusInfo, bool trueOrFalse);
		void setEepromError(SystemStatusInfo *nmSystemStatusInfo, bool trueOrFalse);

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCNUMERICSYSTEMSTATUS_H
