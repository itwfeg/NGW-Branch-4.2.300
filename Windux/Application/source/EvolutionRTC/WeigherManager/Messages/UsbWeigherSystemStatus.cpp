//#include "vxWorks.h"
#include "UsbWeigherSystemStatus.h"

#include "WeigherEvent.h"

#include "Debug.h"
#include "UInteger.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int UsbWeigherSystemStatus::dataType = 0;
std::string UsbWeigherSystemStatus::dataTypeName("UsbWeigherSystemStatus"); 
#endif

#if 1
// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
UsbWeigherSystemStatus::UsbWeigherSystemStatus(Hobart::CEvolMsgReceiver *pHobThread,
                                               WeigherType wgType,
                                               WeigherKind wgKind) : SystemGeneric(WeigherSystem, SysUsbWgStatus, pHobThread)
#else
UsbWeigherSystemStatus::UsbWeigherSystemStatus(Hobart::CEvolMsgReceiver *pHobThread) : SystemGeneric(WeigherSystem, SysUsbWgStatus, pHobThread),
																		softwareVersion(HobVersion::UsbWeigher15KgSW, 0, 0),
																		hardwareVersion(HobVersion::UsbWeigher15KgHW, 0, 0)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	flags = 0;
	initFlags = 0;
	serialNumber = 0;

	this->wgType = wgType;
	if( wgKind == WgCanKpc ) {
		softwareVersion.setVersionType(HobVersion::KpcWeigherSW);
		hardwareVersion.setVersionType(HobVersion::KpcWeigherHW);
	} else {
		if (wgKind == WgUsbEvol)
		{
			softwareVersion.setVersionType(HobVersion::UsbWeigher15KgSW);
			hardwareVersion.setVersionType(HobVersion::UsbWeigher15KgHW);
		}
		else  // WgUsbAWS, AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		{
			softwareVersion.setVersionType(HobVersion::Aws2WeigherSW);
			hardwareVersion.setVersionType(HobVersion::Aws2WeigherHW);
		}
	}
}

UsbWeigherSystemStatus::UsbWeigherSystemStatus(const char *buffer) : SystemGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &wgType;
	for (unsigned int i = 0; i < sizeof(wgType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &flags;
	for (unsigned int i = 0; i < sizeof(flags); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &initFlags;
	for (unsigned int i = 0; i < sizeof(initFlags); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &serialNumber;
	for (unsigned int i = 0; i < sizeof(serialNumber); i++)
	{
		*pChar++ = buffer[j++];
	}

	softwareVersion = HobVersion(&buffer[j]);
	softwareVersion.serialize();
	j += softwareVersion.getSerializedLength();

	//pjr-301
	softwareWeigherVersion = HobVersion(&buffer[j]);
	softwareWeigherVersion.serialize();
	j += softwareWeigherVersion.getSerializedLength();

	hardwareVersion = HobVersion(&buffer[j]);
	hardwareVersion.serialize();
}

UsbWeigherSystemStatus::~UsbWeigherSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *UsbWeigherSystemStatus::serialize() const
{
	SystemGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wgType;
	for (unsigned int i = 0; i < sizeof(wgType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &flags;
	for (unsigned int i = 0; i < sizeof(flags); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &initFlags;
	for (unsigned int i = 0; i < sizeof(initFlags); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &serialNumber;
	for (unsigned int i = 0; i < sizeof(serialNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	const char *pCC = softwareVersion.serialize();
	int mLength = softwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	//pjr-301
	pCC = softwareWeigherVersion.serialize();
	mLength = softwareWeigherVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = hardwareVersion.serialize();
	mLength = hardwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

void UsbWeigherSystemStatus::show() const
{
	const bool isTracing = Debug::isTracing("UsbWeigherSystemStatus");

	if (isTracing == false)
	{
		Debug::addTraceCategory("UsbWeigherSystemStatus");
	}

	Debug::trace("UsbWeigherSystemStatus", "UsbWeigherSystemStatus: ");

	SystemGeneric::show();

	Debug::trace("UsbWeigherSystemStatus", HobString("type: ") + UInteger::toString(wgType));
	Debug::trace("UsbWeigherSystemStatus", HobString("flags: ") + Hex::toString(flags));
	Debug::trace("UsbWeigherSystemStatus", HobString("initFlags: ") + Hex::toString(initFlags));
	Debug::trace("UsbWeigherSystemStatus", HobString("serialNumber: ") + UInteger::toString(serialNumber));

	Debug::trace("UsbWeigherSystemStatus", HobString("softwareVersion: ") + const_cast<HobVersion&>(softwareVersion).toString()->c_str());
	//pjr-301
	Debug::trace("UsbWeigherSystemStatus", HobString("softwareWeigherVersion: ") + const_cast<HobVersion&>(softwareWeigherVersion).toString()->c_str());
	Debug::trace("UsbWeigherSystemStatus", HobString("hardwareVersion: ") + const_cast<HobVersion&>(hardwareVersion).toString()->c_str());

	if (isTracing == false)
	{
		Debug::removeTraceCategory("UsbWeigherSystemStatus");
	}

}

UINT32 UsbWeigherSystemStatus::getFlags() const
{
	return flags;
}

void UsbWeigherSystemStatus::setFlags(UINT32 flags)
{
	this->flags = flags;
}

UINT32 UsbWeigherSystemStatus::getInitFlags() const
{
	return initFlags;
}

void UsbWeigherSystemStatus::setInitFlags(UINT32 initFlags)
{
	this->initFlags = initFlags;
}

UINT32 UsbWeigherSystemStatus::getSerialNumber() const
{
	return serialNumber;
}

void UsbWeigherSystemStatus::setSerialNumber(UINT32 serialNumber)
{
	this->serialNumber = serialNumber;
}

UINT16 UsbWeigherSystemStatus::getSoftwareVersionMajor() const
{
	return softwareVersion.getMajorRevision();
}

UINT16 UsbWeigherSystemStatus::getSoftwareVersionMinor() const
{
	return softwareVersion.getMinorRevision();
}

//PJR-301
UINT16 UsbWeigherSystemStatus::getSoftwareVersionEng() const
{
	return softwareVersion.getEngRevision();
}

const HobVersion &UsbWeigherSystemStatus::getSoftwareVersion() const
{
	return softwareVersion;
}


//pjr-301
UINT16 UsbWeigherSystemStatus::getSoftwareWeigherVersionMajor() const
{
	return softwareWeigherVersion.getMajorRevision();
}

UINT16 UsbWeigherSystemStatus::getSoftwareWeigherVersionMinor() const
{
	return softwareWeigherVersion.getMinorRevision();
}

UINT16 UsbWeigherSystemStatus::getSoftwareWeigherVersionEng() const
{
	return softwareWeigherVersion.getEngRevision();
}

const HobVersion &UsbWeigherSystemStatus::getSoftwareWeigherVersion() const
{
	return softwareWeigherVersion;
}


void UsbWeigherSystemStatus::setSoftwareType(HobVersion::VersionType newVt)
{
	softwareVersion.setVersionType(newVt);
}

void UsbWeigherSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
}

//pjr-301
void UsbWeigherSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor, UINT16 eng)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
	softwareVersion.setEngRevision(eng);
}

void UsbWeigherSystemStatus::setSoftwareWeigherType(HobVersion::VersionType newVt)
{
	softwareWeigherVersion.setVersionType(newVt);
}

void UsbWeigherSystemStatus::setSoftwareWeigherVersion(UINT16 major, UINT16 minor)
{
	softwareWeigherVersion.setMajorRevision(major);
	softwareWeigherVersion.setMinorRevision(minor);
}

void UsbWeigherSystemStatus::setSoftwareWeigherVersion(UINT16 major, UINT16 minor, UINT16 eng)
{
	softwareWeigherVersion.setMajorRevision(major);
	softwareWeigherVersion.setMinorRevision(minor);
	softwareWeigherVersion.setEngRevision(eng);
}

void UsbWeigherSystemStatus::setSoftwareWeigherVersion(const HobVersion &softwareVersion)
{
	if ( (softwareVersion.getVersionType() == HobVersion::UsbWeigher15KgSW) ||
	     (softwareVersion.getVersionType() == HobVersion::Aws2WeigherSW) ||
	     (softwareVersion.getVersionType() == HobVersion::KpcWeigherSW) )
	{
		this->softwareWeigherVersion.setMajorRevision(softwareVersion.getMajorRevision());
		this->softwareWeigherVersion.setMinorRevision(softwareVersion.getMinorRevision());
		this->softwareWeigherVersion.setEngRevision(softwareVersion.getEngRevision());
		this->softwareWeigherVersion.setBuildNumber(softwareVersion.getBuildNumber());
	}
}


void UsbWeigherSystemStatus::setSoftwareVersion(const HobVersion &softwareVersion)
{
	if ( (softwareVersion.getVersionType() == HobVersion::UsbWeigher15KgSW) ||
	     (softwareVersion.getVersionType() == HobVersion::Aws2WeigherSW) ||
	     (softwareVersion.getVersionType() == HobVersion::KpcWeigherSW) )
	{
		this->softwareVersion.setMajorRevision(softwareVersion.getMajorRevision());
		this->softwareVersion.setMinorRevision(softwareVersion.getMinorRevision());
		//Pjr-301
		this->softwareVersion.setEngRevision(softwareVersion.getEngRevision());
		this->softwareVersion.setBuildNumber(softwareVersion.getBuildNumber());
	}
}

UINT8 UsbWeigherSystemStatus::getHardwareVersionMajor() const
{
	return (UINT8)hardwareVersion.getMajorRevision();
}

UINT8 UsbWeigherSystemStatus::getHardwareVersionMinor() const
{
	return (UINT8)hardwareVersion.getMinorRevision();
}

const HobVersion &UsbWeigherSystemStatus::getHardwareVersion() const
{
	return hardwareVersion;
}

void UsbWeigherSystemStatus::setHardwareType(HobVersion::VersionType newVt)
{
	hardwareVersion.setVersionType(newVt);
}

void UsbWeigherSystemStatus::setHardwareVersion(UINT8 major, UINT8 minor)
{
	hardwareVersion.setMajorRevision(major);
	hardwareVersion.setMinorRevision(minor);
}

void UsbWeigherSystemStatus::setHardwareVersion(const HobVersion &hardwareVersion)
{
	if ( (hardwareVersion.getVersionType() == HobVersion::UsbWeigher15KgHW) ||
         (hardwareVersion.getVersionType() == HobVersion::Aws2WeigherHW) ||
         (hardwareVersion.getVersionType() == HobVersion::KpcWeigherHW) )
	{
		this->hardwareVersion.setMajorRevision(hardwareVersion.getMajorRevision());
		this->hardwareVersion.setMinorRevision(hardwareVersion.getMinorRevision());
	}
}

bool UsbWeigherSystemStatus::isSystemEepromError() const
{
	return (isEepromError() && isSystemError());
}

bool UsbWeigherSystemStatus::isWeigherEepromError() const
{
	return (isEepromError() && isWeigherError());
}

void UsbWeigherSystemStatus::setEepromError()
{
	flags |= EepromError;
}

void UsbWeigherSystemStatus::clearEepromError()
{
	flags &= ~EepromError;
}

bool UsbWeigherSystemStatus::isEepromError() const
{
	return ((flags & EepromError) == EepromError);
}

void UsbWeigherSystemStatus::setSystemError()
{
	flags |= SystemError;
}

void UsbWeigherSystemStatus::clearSystemError()
{
	flags &= ~SystemError;
}

bool UsbWeigherSystemStatus::isSystemError() const
{
	return ((flags & SystemError) == SystemError);
}

void UsbWeigherSystemStatus::setWeigherError()
{
	flags |= WeigherError;
}

void UsbWeigherSystemStatus::clearWeigherError()
{
	flags &= ~WeigherError;
}

bool UsbWeigherSystemStatus::isWeigherError() const
{
	return ((flags & WeigherError) == WeigherError);
}

bool UsbWeigherSystemStatus::equals(UsbWeigherSystemStatus &wgSysStatus) const
{
	bool rv = false;

	if ((wgType == wgSysStatus.getWeigherType()) &&
		(flags == wgSysStatus.getFlags()) &&
		(initFlags == wgSysStatus.getInitFlags()) &&
		(serialNumber == wgSysStatus.getSerialNumber()) &&
		(softwareVersion == wgSysStatus.getSoftwareVersion()) &&
		(softwareWeigherVersion == wgSysStatus.getSoftwareWeigherVersion()) &&
		(hardwareVersion == wgSysStatus.getHardwareVersion()))
	{
		rv = true;
	}

	return rv;
}
