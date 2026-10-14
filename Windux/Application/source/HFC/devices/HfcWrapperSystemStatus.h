/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSYSTEMSTATUS_H
#define HFCWRAPPERSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "WrapperSystemStatus.h"

typedef enum HfcWrSysStatusFieldOffsets
{
	fieldOffsetHfcWrSysStatusSystemChecksum = 1,
	fieldOffsetHfcWrSysStatusRomChecksum,
	fieldOffsetHfcWrSysStatusIsEepromError,
	fieldOffsetHfcWrSysStatusIsBitTestError,
	//fieldOffsetHfcWrSysStatusBitTestAddress,
	fieldOffsetHfcWrSysStatusIsAddressError,
	//fieldOffsetHfcWrSysStatusAddressTestAddress,

	endHfcWrSysStatusFieldOffsets	//	MUST be last!
} HfcWrSysStatusFieldOffsets_t;

class HfcWrapperSystemStatus : public WrapperSystemStatus, public HfcObject
{
	public:

		static const short PASSED	= 0;
		static const short FAILED	= 1;

		HfcWrapperSystemStatus();
		HfcWrapperSystemStatus(const char *data);
		HfcWrapperSystemStatus(const char *data, int len, int formatType);

		~HfcWrapperSystemStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setBitError(SystemStatusInfo *wrSystemStatusInfo, bool trueOrFalse);
		void setAddressError(SystemStatusInfo *wrSystemStatusInfo, bool trueOrFalse);
		void setEepromError(SystemStatusInfo *wrSystemStatusInfo, bool trueOrFalse);

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERSYSTEMSTATUS_H
