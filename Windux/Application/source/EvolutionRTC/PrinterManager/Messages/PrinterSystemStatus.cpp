//#include "vxWorks.h"
#include "PrinterSystemStatus.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemStatus::dataType = 0;
std::string PrinterSystemStatus::dataTypeName("PrinterSystemStatus"); 
#endif

PrinterSystemStatus::PrinterSystemStatus(
	Hobart::CEvolMsgReceiver *pHobThread,
	PrinterType prType,
	PrinterKind prKind,
	SystemStatusInfo *systemStatus,
	UINT16 softwareMajor,
	UINT16 softwareMinor,
	UINT16 softwareEng,
	UINT16 softwareWeigherMajor,
	UINT16 softwareWeigherMinor,
	UINT16 softwareWeigherEng,
	UINT8 hardwareMajor,
	UINT8 hardwareMinor,
	UINT16 softwareBuild ) : SystemStatus( PrinterSystem, pHobThread, systemStatus )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->prType = prType;

	softwareVersion.setMajorRevision(softwareMajor);
	softwareVersion.setMinorRevision(softwareMinor);

	//pjr-301
	softwareVersion.setEngRevision(softwareEng);
	
	softwareWeigherVersion.setMajorRevision(softwareWeigherMajor);
	softwareWeigherVersion.setMinorRevision(softwareWeigherMinor);
	softwareWeigherVersion.setEngRevision(softwareWeigherEng);


	if( (prKind == KPCPrinter) || ( prKind == SSKPCPrinter ) ){
		softwareVersion.setBuildNumber(softwareBuild);
	}

	hardwareVersion.setMajorRevision(hardwareMajor);
	hardwareVersion.setMinorRevision(hardwareMinor);

	if (prKind == EppPrinter)
	{
		softwareVersion.setVersionType(HobVersion::EppPrinterSW);
		hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
	}
	else if (prKind == Epp2Printer)	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
	{
		softwareVersion.setVersionType(HobVersion::Epp2PrinterSW);
		hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);
	}
	else if (prKind == EssPrinter)
	{
		softwareVersion.setVersionType(HobVersion::EssPrinterSW);
		hardwareVersion.setVersionType(HobVersion::EssPrinterHW);
	}
	else if (prKind == EscPrinter)
	{
		softwareVersion.setVersionType(HobVersion::EscPrinterSW);
		hardwareVersion.setVersionType(HobVersion::EscPrinterHW);
	}
	else if (prKind == ApcPrinter)
	{
		softwareVersion.setVersionType(HobVersion::ApcPrinterSW);		
		softwareWeigherVersion.setVersionType(HobVersion::ApcWeigherSW);
		hardwareVersion.setVersionType(HobVersion::ApcPrinterHW);
	}
	else if (prKind == KPCPrinter)
	{
		softwareVersion.setVersionType(HobVersion::KpcPrinterSW);
		hardwareVersion.setVersionType(HobVersion::KpcPrinterHW);
	}
	else if (prKind == SSKPCPrinter)
	{
		softwareVersion.setVersionType(HobVersion::FSSPrinterSW);
		hardwareVersion.setVersionType(HobVersion::FSSPrinterHW);
	}
	else
	{
		softwareVersion.setVersionType(HobVersion::UnknownComponent);
		hardwareVersion.setVersionType(HobVersion::UnknownComponent);
	}
}

PrinterSystemStatus::PrinterSystemStatus(const char *buffer) : SystemStatus(buffer)
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
	char *pChar = (char *) &prType;
	for (unsigned int i = 0; i < sizeof(prType); i++)
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

PrinterSystemStatus::~PrinterSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSystemStatus::serialize() const
{
	SystemStatus::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &prType;
	for (unsigned int i = 0; i < sizeof(prType); i++)
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

void PrinterSystemStatus::show() const
{
	bool isTracing = Debug::isTracing("PrinterSystemStatus");

	if (isTracing == false)
		return;

	Debug::trace("PrinterSystemStatus", "PrinterSystemStatus: ");

	SystemStatus::show();

	if (prType == FirstPrinter)
	{
		Debug::trace("PrinterSystemStatus", HobString("prType: FirstPrinter"));
	}
	else if (prType == SecondPrinter)
	{
		Debug::trace("PrinterSystemStatus", HobString("prType: SecondPrinter"));
	}
	else if (prType == ThirdPrinter)
	{
		Debug::trace("PrinterSystemStatus", HobString("prType: ThirdPrinter"));
	}
	else
	{
		Debug::trace("PrinterSystemStatus", HobString("prType: Unknown ") +
											 HobString(Integer::toString((UINT16) prType)));
	}

	//pjr301
	Debug::trace("PrinterSystemStatus", "MajorRevision_ " + Integer::toString(softwareVersion.getMajorRevision()) );	
	Debug::trace("PrinterSystemStatus", "MinorRevision_ " + Integer::toString(softwareVersion.getMinorRevision()) );	
	Debug::trace("PrinterSystemStatus", "engRevision_ " + Integer::toString(softwareVersion.getEngRevision()) );	
	
	Debug::trace("PrinterSystemStatus", "MajorRevision_ " + Integer::toString(softwareWeigherVersion.getMajorRevision()) );	
	Debug::trace("PrinterSystemStatus", "MinorRevision_ " + Integer::toString(softwareWeigherVersion.getMinorRevision()) );	
	Debug::trace("PrinterSystemStatus", "engRevision_ " + Integer::toString(softwareWeigherVersion.getEngRevision()) );	

	Debug::trace("PrinterSystemStatus", HobString("softwareVersion: ") + const_cast<HobVersion&>(softwareVersion).toString()->c_str());
	Debug::trace("PrinterSystemStatus", HobString("softwareWeigherVersion: ") + const_cast<HobVersion&>(softwareWeigherVersion).toString()->c_str());
	Debug::trace("PrinterSystemStatus", HobString("hardwareVersion: ") + const_cast<HobVersion&>(hardwareVersion).toString()->c_str());
}

void PrinterSystemStatus::setPrinterType(PrinterType prType)
{
	this->prType = prType;
}

PrinterType PrinterSystemStatus::getPrinterType() const
{
	return prType;
}

UINT16 PrinterSystemStatus::getSoftwareVersionMajor() const
{
	return softwareVersion.getMajorRevision();
}

UINT16 PrinterSystemStatus::getSoftwareVersionMinor() const
{
	return softwareVersion.getMinorRevision();
}

//PJR-301
UINT16 PrinterSystemStatus::getSoftwareVersionEng() const
{
	return softwareVersion.getEngRevision();
}

const HobVersion &PrinterSystemStatus::getSoftwareVersion() const
{
	return softwareVersion;
}


//weigher
//PJR-301
UINT16 PrinterSystemStatus::getSoftwareWeigherVersionMajor() const
{
	return softwareWeigherVersion.getMajorRevision();
}

UINT16 PrinterSystemStatus::getSoftwareWeigherVersionMinor() const
{
	return softwareWeigherVersion.getMinorRevision();
}

UINT16 PrinterSystemStatus::getSoftwareWeigherVersionEng() const
{
	return softwareWeigherVersion.getEngRevision();
}

const HobVersion &PrinterSystemStatus::getSoftwareWeigherVersion() const
{
	return softwareWeigherVersion;
}

void PrinterSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
}

//pjr-301
void PrinterSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor, UINT16 eng)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
	softwareVersion.setEngRevision(eng);
}

void PrinterSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor, UINT32 build)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
	softwareVersion.setBuildNumber(build);
}

//pjr-301
void PrinterSystemStatus::setSoftwareVersion(UINT16 major, UINT16 minor, UINT16 eng, UINT32 build)
{
	softwareVersion.setMajorRevision(major);
	softwareVersion.setMinorRevision(minor);
	softwareVersion.setEngRevision(eng);
	softwareVersion.setBuildNumber(build);
}

void PrinterSystemStatus::setSoftwareVersion(const HobVersion &softwareVersion)
{
	if (((softwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
	    ((softwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		((softwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->softwareVersion.getVersionType() == HobVersion::EssPrinterSW)))
	{
		this->softwareVersion.setMajorRevision(softwareVersion.getMajorRevision());
		this->softwareVersion.setMinorRevision(softwareVersion.getMinorRevision());
		//pjr-301
		this->softwareVersion.setEngRevision(softwareVersion.getEngRevision());
	}
}

//PJR-301
void PrinterSystemStatus::setSoftwareWeigherVersion(UINT16 major, UINT16 minor)
{
	softwareWeigherVersion.setMajorRevision(major);
	softwareWeigherVersion.setMinorRevision(minor);
}

void PrinterSystemStatus::setSoftwareWeigherVersion(UINT16 major, UINT16 minor, UINT16 eng)
{
	softwareWeigherVersion.setMajorRevision(major);
	softwareWeigherVersion.setMinorRevision(minor);
	softwareWeigherVersion.setEngRevision(eng);
}

void PrinterSystemStatus::setSoftwareWeigherVersion(UINT16 major, UINT16 minor, UINT32 build)
{
	softwareWeigherVersion.setMajorRevision(major);
	softwareWeigherVersion.setMinorRevision(minor);
	softwareWeigherVersion.setBuildNumber(build);
}

void PrinterSystemStatus::setSoftwareWeigherVersion(UINT16 major, UINT16 minor, UINT16 eng, UINT32 build)
{
	softwareWeigherVersion.setMajorRevision(major);
	softwareWeigherVersion.setMinorRevision(minor);
	softwareWeigherVersion.setEngRevision(eng);
	softwareWeigherVersion.setBuildNumber(build);
}

void PrinterSystemStatus::setSoftwareWeigherVersion(const HobVersion &softwareVersion)
{
	if (((softwareWeigherVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->softwareWeigherVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
	    ((softwareWeigherVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->softwareWeigherVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		((softwareWeigherVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->softwareWeigherVersion.getVersionType() == HobVersion::EssPrinterSW)))
	{
		this->softwareWeigherVersion.setMajorRevision(softwareWeigherVersion.getMajorRevision());
		this->softwareWeigherVersion.setMinorRevision(softwareWeigherVersion.getMinorRevision());
		this->softwareWeigherVersion.setEngRevision(softwareWeigherVersion.getEngRevision());
	}
}


UINT8 PrinterSystemStatus::getHardwareVersionMajor() const
{
	return (UINT8)hardwareVersion.getMajorRevision();
}

UINT8 PrinterSystemStatus::getHardwareVersionMinor() const
{
	return (UINT8)hardwareVersion.getMinorRevision();
}

const HobVersion &PrinterSystemStatus::getHardwareVersion() const
{
	return hardwareVersion;
}

void PrinterSystemStatus::setHardwareVersion(UINT8 major, UINT8 minor)
{
	hardwareVersion.setMajorRevision(major);
	hardwareVersion.setMinorRevision(minor);
}

void PrinterSystemStatus::setHardwareVersion(const HobVersion &hardwareVersion)
{
	if (((hardwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
        ((hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		((hardwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EssPrinterSW)))
	{
		this->hardwareVersion.setMajorRevision(hardwareVersion.getMajorRevision());
		this->hardwareVersion.setMinorRevision(hardwareVersion.getMinorRevision());
	}
}
