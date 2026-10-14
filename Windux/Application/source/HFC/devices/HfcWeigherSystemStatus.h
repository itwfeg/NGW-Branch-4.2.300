/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERSYSTEMSTATUS_H
#define HFCWEIGHERSYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "WeigherSystemStatus.h"

typedef enum HfcWgSysStatusFieldOffsets
{
	fieldOffsetHfcWgSysStatusSystemChecksum = 1,
	fieldOffsetHfcWgSysStatusRomChecksum,
	fieldOffsetHfcWgSysStatusIsEepromError,
	fieldOffsetHfcWgSysStatusIsBitTestError,
	//fieldOffsetHfcWgSysStatusBitTestAddress,
	fieldOffsetHfcWgSysStatusIsAddressError,
	//fieldOffsetHfcWgSysStatusAddressTestAddress,

	endHfcWgSysStatusFieldOffsets	//	MUST be last!
} HfcWgSysStatusFieldOffsets_t;

class HfcWeigherSystemStatus : public WeigherSystemStatus, public HfcObject
{
	public:

		static const short PASSED	= 0;
		static const short FAILED	= 1;

		HfcWeigherSystemStatus();
		HfcWeigherSystemStatus(const char *data);
		HfcWeigherSystemStatus(const char *data, int len, int formatType);

		~HfcWeigherSystemStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setBitError(SystemStatusInfo *wgSystemStatusInfo, bool trueOrFalse);
		void setAddressError(SystemStatusInfo *wgSystemStatusInfo, bool trueOrFalse);
		void setEepromError(SystemStatusInfo *wgSystemStatusInfo, bool trueOrFalse);

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWEIGHERSYSTEMSTATUS_H
