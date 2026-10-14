/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcUsbWeigherSystemStatus.h"
#include "Bool.h"
#include "Integer.h"
#include "UInteger.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcUsbWeigherSystemStatus::dataType = 0;
std::string HfcUsbWeigherSystemStatus::dataTypeName("HfcUsbWeigherSystemStatus"); 
#endif

HfcUsbWeigherSystemStatus::HfcUsbWeigherSystemStatus() : UsbWeigherSystemStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcUsbWeigherSystemStatus::HfcUsbWeigherSystemStatus(const char *data) : UsbWeigherSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcUsbWeigherSystemStatus::~HfcUsbWeigherSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcUsbWeigherSystemStatus::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
			{
				HfcObject::serializedData = Integer::toString((int)getDeviceType());
				HfcObject::serializedData.append(BrowserFormatSeparator);
				HfcObject::serializedData.append(Integer::toString((int)getEventType()));

				for (short loopCnt = 1; loopCnt < endHfcUsbWgSysStatusFieldOffsets; loopCnt++)
				{
					HfcObject::serializedData.append(BrowserFormatSeparator);
					switch (loopCnt)
					{
						case fieldOffsetHfcUsbWgSysStatusSystemEepromError:
							HfcObject::serializedData.append(Bool::toString(isSystemEepromError()));
							break;
						case fieldOffsetHfcUsbWgSysStatusWeigherEepromError:
							HfcObject::serializedData.append(Bool::toString(isWeigherEepromError()));
							break;
						case fieldOffsetHfcUsbWgSysStatusSystemSerialNumber:
							HfcObject::serializedData.append(UInteger::toString(getSerialNumber()));
							break;
						case fieldOffsetHfcUsbWgSysStatusSystemSoftwareVersion:
							{
								std::string ver;
								/*
								if (getSoftwareVersion().getVersionType() == HobVersion::KpcWeigherSW)
									ver = getSoftwareWeigherVersion().getRevMajMinBuildStr();
								else
								*/
									ver = getSoftwareWeigherVersion().getRevMajMinEngStr();
								HfcObject::serializedData.append(ver);
								break;
							}
						case fieldOffsetHfcUsbWgSysStatusSystemHardwareVersion:
							HfcObject::serializedData.append(getHardwareVersion().getRevMajMinEngStr());
							break;
						case fieldOffsetHfcUsbWgSysStatusWeigherSystemSoftwareVersion:
							{
								std::string ver;
								//PJR-301
								ver = getSoftwareWeigherVersion().getRevNTEPVersionStr();
								HfcObject::serializedData.append(ver);
								break;
							}

						default:
							break;
					}
				}
			}
			break;
		default:
			// todo - throw exception("invalid format");
			break;
	}

	Debug::trace("Session.UsbWgSystem", HobString("getHardwareVersion: ") + HobString(getHardwareVersion().getRevisionString()));
	Debug::trace("Session.UsbWgSystem", HobString("getSoftwareVersion: ") + HobString(getSoftwareVersion().getRevisionString()));
	Debug::trace("Session.UsbWgSystem", HobString("getSoftwareWeigherVersion: ") + HobString(getSoftwareWeigherVersion().getRevisionString()));
	Debug::trace("Session.UsbWgSystem", HobString(HfcObject::serializedData.c_str()));

	return HfcObject::serializedData.data();
}
