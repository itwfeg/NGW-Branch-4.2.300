//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemConfiguration.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemConfiguration::_dataType_ = 0;
std::string SystemConfiguration::_dataTypeName_("SystemConfiguration"); 
#endif

SystemConfiguration::SystemConfiguration(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, SysConfigType sysConfigType, UINT8 *deviceSignature,
										 DeviceAddress stationAddress, DeviceAddress controllerAddress) : EvolMessage(evolDevType, SysConfig, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->sysConfigType = sysConfigType;

	if (deviceSignature == NULL)
	{
		this->deviceSignature[0] = 'S';
		this->deviceSignature[1] = 'Y';
		this->deviceSignature[2] = 'S';
		this->deviceSignature[3] = '\0';
		this->deviceSignature[4] = '\0';
		this->deviceSignature[5] = '\0';
	}
	else
	{
		for (int i = 0; i < SystemDevSignatureSize; i++)
		{
			this->deviceSignature[i] = deviceSignature[i];
		}
	}

	this->stationAddress = stationAddress;
	this->controllerAddress = controllerAddress;
}

SystemConfiguration::SystemConfiguration(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sysConfigType;
	for (unsigned int i = 0; i < sizeof(sysConfigType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &stationAddress;
	for (unsigned int i = 0; i < sizeof(stationAddress); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &controllerAddress;
	for (unsigned int i = 0; i < sizeof(controllerAddress); i++)
	{
		*pChar++ = buffer[j++];
	}
}

SystemConfiguration::~SystemConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *SystemConfiguration::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sysConfigType;
	for (unsigned int i = 0; i < sizeof(sysConfigType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &stationAddress;
	for (unsigned int i = 0; i < sizeof(stationAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &controllerAddress;
	for (unsigned int i = 0; i < sizeof(controllerAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void SystemConfiguration::show()
{
	Debug::addTraceCategory( std::string("SystemConfiguration") );
	Debug::trace("SystemConfiguration", "SystemConfiguration: ");

	if (sysConfigType == SysPermanent)
	{
		Debug::trace("SystemConfiguration", "configType: SysPermanent");
	}
	else if (sysConfigType == SysTemporary)
	{
		Debug::trace("SystemConfiguration", "configType: SysTemporary");
	}
	else if (sysConfigType == SysDefault)
	{
		Debug::trace("SystemConfiguration", "configType: SysDefault");
	}
	else if ((sysConfigType & SysCalibration) == SysCalibration)
	{
		Debug::trace("SystemConfiguration", "configType: SysCalibration");
	}
	else
	{
		Debug::trace("SystemConfiguration", HobString("sysConfigType: Unknown ") +
											HobString(Integer::toString((UINT16) sysConfigType)));
	}

	Debug::trace("SystemConfiguration", "deviceSignature: ");
	Debug::trace("SystemConfiguration", Hex::toString((const char *) &deviceSignature[0], SystemDevSignatureSize));

	Debug::trace("SystemConfiguration", HobString("stationAddress: ") +
										HobString(Hex::toString(stationAddress)));
	Debug::trace("SystemConfiguration", HobString("controllerAddress: ") +
										HobString(Hex::toString(controllerAddress)));

	Debug::removeTraceCategory( std::string("SystemConfiguration") );
}

SysConfigType SystemConfiguration::getSysConfigType() const
{
	return sysConfigType;
}

void SystemConfiguration::setSysConfigType(SysConfigType sysConfigType)
{
	this->sysConfigType = sysConfigType;
}

void SystemConfiguration::getDeviceSignature(UINT8 *deviceSignature) const
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = this->deviceSignature[i];
	}
}

void SystemConfiguration::setDeviceSignature(UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		this->deviceSignature[i] = deviceSignature[i];
	}
}

DeviceAddress SystemConfiguration::getStationAddress() const
{
	return stationAddress;
}

void SystemConfiguration::setStationAddress(DeviceAddress stationAddress)
{
	this->stationAddress = stationAddress;
}

DeviceAddress SystemConfiguration::getControllerAddress() const
{
	return controllerAddress;
}

void SystemConfiguration::setControllerAddress(DeviceAddress controllerAddress)
{
	this->controllerAddress = controllerAddress;
}

