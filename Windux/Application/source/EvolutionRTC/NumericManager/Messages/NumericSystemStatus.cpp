//#include "vxWorks.h"
#include "NumericSystemStatus.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericSystemStatus::dataType = 0;
std::string NumericSystemStatus::dataTypeName("NumericSystemStatus"); 
#endif

NumericSystemStatus::NumericSystemStatus(Hobart::CEvolMsgReceiver *pHobThread,
										 SystemStatusInfo *systemStatus,
										 HobVersion::VersionType softwareType,
										 UINT16 softwareMajor,
										 UINT16 softwareMinor,
										 HobVersion::VersionType hardwareType,
										 UINT8 hardwareMajor,
										 UINT8 hardwareMinor) : SystemStatus(NumericSystem, pHobThread, systemStatus)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	softwareVersion.setMajorRevision(softwareMajor);
	softwareVersion.setMinorRevision(softwareMinor);

	hardwareVersion.setMajorRevision(hardwareMajor);
	hardwareVersion.setMinorRevision(hardwareMinor);

	softwareVersion.setVersionType(softwareType);
	hardwareVersion.setVersionType(hardwareType);

}

NumericSystemStatus::NumericSystemStatus(const char *buffer) : SystemStatus(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemStatus::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items

	softwareVersion = HobVersion(&buffer[j]);
	softwareVersion.serialize();
	j += softwareVersion.getSerializedLength();

	hardwareVersion = HobVersion(&buffer[j]);
	hardwareVersion.serialize();
}

NumericSystemStatus::~NumericSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *NumericSystemStatus::serialize() const
{
	SystemStatus::serialize();

	// Now, for the derived class items
	
	const char *pCC = softwareVersion.serialize();
	int mLength = softwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = hardwareVersion.serialize();
	mLength = hardwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

void NumericSystemStatus::show() const
{
	bool isTracing = Debug::isTracing("NumericSystemStatus");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("NumericSystemStatus") );
#else
		Debug::addTraceCategory("NumericSystemStatus");
#endif
	}

	Debug::trace("NumericSystemStatus", "NumericSystemStatus: ");

	SystemStatus::show();


	Debug::trace("NumericSystemStatus", HobString("softwareVersion: ") + const_cast<HobVersion&>(softwareVersion).toString()->c_str());
	Debug::trace("NumericSystemStatus", HobString("hardwareVersion: ") + const_cast<HobVersion&>(hardwareVersion).toString()->c_str());

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("NumericSystemStatus") );
#else
		Debug::removeTraceCategory("NumericSystemStatus");
#endif
	}
}


UINT16 NumericSystemStatus::getSoftwareVersionMajor() const
{
	return softwareVersion.getMajorRevision();
}

UINT16 NumericSystemStatus::getSoftwareVersionMinor() const
{
	return softwareVersion.getMinorRevision();
}

const HobVersion &NumericSystemStatus::getSoftwareVersion() const
{
	return softwareVersion;
}

void NumericSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
}

void NumericSystemStatus::setSoftwareVersion(const HobVersion &softwareVersion)
{
	if (((softwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
         ((softwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		((softwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::EssPrinterSW)))
	{
		this->softwareVersion.setMajorRevision(softwareVersion.getMajorRevision());
		this->softwareVersion.setMinorRevision(softwareVersion.getMinorRevision());
	}
}

UINT8 NumericSystemStatus::getHardwareVersionMajor() const
{
	return (UINT8)hardwareVersion.getMajorRevision();
}

UINT8 NumericSystemStatus::getHardwareVersionMinor() const
{
	return (UINT8)hardwareVersion.getMinorRevision();
}

const HobVersion &NumericSystemStatus::getHardwareVersion() const
{
	return hardwareVersion;
}

void NumericSystemStatus::setHardwareVersion(UINT8 major, UINT8 minor)
{
	hardwareVersion.setMajorRevision(major);
	hardwareVersion.setMinorRevision(minor);
}

void NumericSystemStatus::setHardwareVersion(const HobVersion &hardwareVersion)
{
	if (((hardwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
	     ((hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		((hardwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EssPrinterSW)))
	{
		this->hardwareVersion.setMajorRevision(hardwareVersion.getMajorRevision());
		this->hardwareVersion.setMinorRevision(hardwareVersion.getMinorRevision());
	}
}
