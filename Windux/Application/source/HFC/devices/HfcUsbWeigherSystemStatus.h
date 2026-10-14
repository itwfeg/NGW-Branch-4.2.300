/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCUSBWEIGHERSYSTEMSTATUS_H
#define HFCUSBWEIGHERSYSTEMSTATUS_H

#include "HfcObject.h"
#include "UsbWeigherSystemStatus.h"

typedef enum HfcUsbWgSysStatusFieldOffsets
{
	fieldOffsetHfcUsbWgSysStatusSystemEepromError = 1,
	fieldOffsetHfcUsbWgSysStatusWeigherEepromError,
	fieldOffsetHfcUsbWgSysStatusSystemSerialNumber,
	fieldOffsetHfcUsbWgSysStatusSystemSoftwareVersion,
	fieldOffsetHfcUsbWgSysStatusSystemHardwareVersion,
	fieldOffsetHfcUsbWgSysStatusWeigherSystemSoftwareVersion,

	endHfcUsbWgSysStatusFieldOffsets	//	MUST be last!
} HfcUsbWgSysStatusFieldOffsets_t;

class HfcUsbWeigherSystemStatus : public UsbWeigherSystemStatus, public HfcObject
{
	public:
		HfcUsbWeigherSystemStatus();
		HfcUsbWeigherSystemStatus(const char *data);

		virtual ~HfcUsbWeigherSystemStatus();

		const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCUSBWEIGHERSYSTEMSTATUS_H
