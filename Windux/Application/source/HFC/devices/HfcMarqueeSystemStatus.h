/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCMARQUEESYSTEMSTATUS_H
#define HFCMARQUEESYSTEMSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "SystemStatusInfo.h"
#include "MarqueeSystemStatus.h"

typedef enum HfcMqSysStatusFieldOffsets
{
	fieldOffsetHfcMqSysStatusSystemChecksum = 1,
	fieldOffsetHfcMqSysStatusRomChecksum,
	fieldOffsetHfcMqSysStatusIsEepromError,
	fieldOffsetHfcMqSysStatusIsBitTestError,
	//fieldOffsetHfcMqSysStatusBitTestAddress,
	fieldOffsetHfcMqSysStatusIsAddressError,
	//fieldOffsetHfcMqSysStatusAddressTestAddress,
	fieldOffsetHfcMqSysStatusSoftwareVersion,
	fieldOffsetHfcMqSysStatusHardwareVersion,

	endHfcMqSysStatusFieldOffsets	//	MUST be last!
} HfcMqSysStatusFieldOffsets_t;

class HfcMarqueeSystemStatus : public MarqueeSystemStatus, public HfcObject
{
	public:

		static const short PASSED	= 0;
		static const short FAILED	= 1;

		HfcMarqueeSystemStatus();
		HfcMarqueeSystemStatus(const char *data);
		HfcMarqueeSystemStatus(const char *data, int len, int formatType);

		~HfcMarqueeSystemStatus();

		virtual const char *serialize(int format = InternalFormat) const;
		
		void setBitError(SystemStatusInfo *mqSystemStatusInfo, bool trueOrFalse);
		void setAddressError(SystemStatusInfo *mqSystemStatusInfo, bool trueOrFalse);
		void setEepromError(SystemStatusInfo *mqSystemStatusInfo, bool trueOrFalse);

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//HFCMARQUEESYSTEMSTATUS_H
