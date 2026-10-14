//#include "vxWorks.h"
#include "MarqueeSystemStatus.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int MarqueeSystemStatus::dataType = 0;
std::string MarqueeSystemStatus::dataTypeName("MarqueeSystemStatus"); 
#endif

MarqueeSystemStatus::MarqueeSystemStatus(Hobart::CEvolMsgReceiver *pHobThread,
										 SystemStatusInfo *systemStatus,
										 HobVersion::VersionType softwareType,
										 UINT16 softwareMajor,
										 UINT16 softwareMinor,
										 HobVersion::VersionType hardwareType,
										 UINT8 hardwareMajor,
										 UINT8 hardwareMinor) : SystemStatus(MarqueeSystem, pHobThread, systemStatus)
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

MarqueeSystemStatus::MarqueeSystemStatus(const char *buffer) : SystemStatus(buffer)
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

MarqueeSystemStatus::~MarqueeSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *MarqueeSystemStatus::serialize() const
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

void MarqueeSystemStatus::show() const
{
	bool isTracing = Debug::isTracing("MarqueeSystemStatus");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("MarqueeSystemStatus") );
#else
		Debug::addTraceCategory("MarqueeSystemStatus");
#endif
	}

	Debug::trace("MarqueeSystemStatus", "MarqueeSystemStatus: ");

	SystemStatus::show();

	Debug::trace("MarqueeSystemStatus", HobString("softwareVersion: ") + const_cast<HobVersion&>( softwareVersion ).toString()->c_str());
	Debug::trace("MarqueeSystemStatus", HobString("hardwareVersion: ") + const_cast<HobVersion&>( hardwareVersion ).toString()->c_str());

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("MarqueeSystemStatus") );
#else
		Debug::removeTraceCategory("MarqueeSystemStatus");
#endif
	}
}


UINT16 MarqueeSystemStatus::getSoftwareVersionMajor() const
{
	return softwareVersion.getMajorRevision();
}

UINT16 MarqueeSystemStatus::getSoftwareVersionMinor() const
{
	return softwareVersion.getMinorRevision();
}

const HobVersion &MarqueeSystemStatus::getSoftwareVersion() const
{
	return softwareVersion;
}

void MarqueeSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
}

void MarqueeSystemStatus::setSoftwareVersion(const HobVersion &softwareVersion)
{
	if ( ((softwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
         ((softwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		 ((softwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::EssPrinterSW)) )
	{
		this->softwareVersion.setMajorRevision(softwareVersion.getMajorRevision());
		this->softwareVersion.setMinorRevision(softwareVersion.getMinorRevision());
	}
}

UINT8 MarqueeSystemStatus::getHardwareVersionMajor() const
{
	return (UINT8)hardwareVersion.getMajorRevision();
}

UINT8 MarqueeSystemStatus::getHardwareVersionMinor() const
{
	return (UINT8)hardwareVersion.getMinorRevision();
}

const HobVersion &MarqueeSystemStatus::getHardwareVersion() const
{
	return hardwareVersion;
}

void MarqueeSystemStatus::setHardwareVersion(UINT8 major, UINT8 minor)
{
	hardwareVersion.setMajorRevision(major);
	hardwareVersion.setMinorRevision(minor);
}

void MarqueeSystemStatus::setHardwareVersion(const HobVersion &hardwareVersion)
{
		if ( ((hardwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
         ((hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)		
		 ((hardwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EssPrinterSW)) )
	{
		this->hardwareVersion.setMajorRevision(hardwareVersion.getMajorRevision());
		this->hardwareVersion.setMinorRevision(hardwareVersion.getMinorRevision());
	}
}
