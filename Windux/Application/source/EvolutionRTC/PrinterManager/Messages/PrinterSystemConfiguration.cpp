//#include "vxWorks.h"
#include "PrinterSystemConfiguration.h"
#include "PrinterSystemInquire.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterSystemConfiguration::dataType = 0;
std::string PrinterSystemConfiguration::dataTypeName("PrinterSystemConfiguration"); 
#endif

PrinterSystemConfiguration::PrinterSystemConfiguration(Hobart::CEvolMsgReceiver *pHobThread, 
													   PrinterType prType,
													   PrinterKind prKind,
													   SysConfigType sysConfigType,
													   UINT8 *deviceSignature,
													   DeviceAddress stationAddress,
													   DeviceAddress controllerAddress,
													   UINT8 hardwareMajor,
													   UINT8 hardwareMinor) : SystemConfiguration(PrinterSystem, 
																								  pHobThread,
																								  sysConfigType,
																								  deviceSignature,
																								  stationAddress,
																								  controllerAddress)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->prType = prType;

	hardwareVersion.setMajorRevision(hardwareMajor);
	hardwareVersion.setMinorRevision(hardwareMinor);

	if (prKind == EppPrinter)
	{
		hardwareVersion.setVersionType(HobVersion::EppPrinterHW);
	}
	else if (prKind == Epp2Printer)
	{
		hardwareVersion.setVersionType(HobVersion::Epp2PrinterHW);	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
	}
	else if (prKind == EssPrinter)
	{
		hardwareVersion.setVersionType(HobVersion::EssPrinterHW);
	}
	else if (prKind == EscPrinter)
	{
		hardwareVersion.setVersionType(HobVersion::EscPrinterHW);
	}
	else if (prKind == ApcPrinter)
	{
		hardwareVersion.setVersionType(HobVersion::ApcPrinterHW);
	}
	else if (prKind == KPCPrinter)
	{
		hardwareVersion.setVersionType(HobVersion::KpcPrinterHW);
	}
	else if (prKind == SSKPCPrinter)
	{
		hardwareVersion.setVersionType(HobVersion::SSPrinterHW);
	}
	else
	{
		hardwareVersion.setVersionType(HobVersion::UnknownComponent);
	}
}

PrinterSystemConfiguration::PrinterSystemConfiguration(const char *buffer) : SystemConfiguration(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	SystemConfiguration::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &prType;
	for (unsigned int i = 0; i < sizeof(prType); i++)
	{
		*pChar++ = buffer[j++];
	}

	hardwareVersion = HobVersion(&buffer[j]);
	hardwareVersion.serialize();
}

PrinterSystemConfiguration::~PrinterSystemConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *PrinterSystemConfiguration::serialize() const
{
	SystemConfiguration::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &prType;
	for (unsigned int i = 0; i < sizeof(prType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	const char *pCC = hardwareVersion.serialize();
	int mLength = hardwareVersion.getSerializedLength();

	serializedData.append(pCC, mLength);

	return serializedData.data();
}

void PrinterSystemConfiguration::show()
{
	bool isTracing = Debug::isTracing("PrinterSystemStatus");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterSystemConfiguration") );
#else
		Debug::addTraceCategory("PrinterSystemConfiguration");
#endif
	}

	Debug::trace("PrinterSystemConfiguration", "PrinterSystemConfiguration: ");

	SystemConfiguration::show();

	if (prType == FirstPrinter)
	{
		Debug::trace("PrinterSystemConfiguration", HobString("prType: FirstPrinter"));
	}
	else if (prType == SecondPrinter)
	{
		Debug::trace("PrinterSystemConfiguration", HobString("prType: SecondPrinter"));
	}
	else if (prType == ThirdPrinter)
	{
		Debug::trace("PrinterSystemConfiguration", HobString("prType: ThirdPrinter"));
	}
	else
	{
		Debug::trace("PrinterSystemConfiguration", HobString("prType: Unknown ") +
												   HobString(Integer::toString((UINT16) prType)));
	}

	Debug::trace("PrinterSystemConfiguration", HobString("hardwareVersion: ") + HobString(hardwareVersion.toString()->c_str()));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterSystemConfiguration") );
#else
		Debug::removeTraceCategory("PrinterSystemConfiguration");
#endif
	}
}

bool PrinterSystemConfiguration::update(PrinterSystemInquire &prSysInquire)
{
	bool rv = false;

	if (prType != prSysInquire.getPrinterType())
	{
		prType = prSysInquire.getPrinterType();
		rv = true;
	}

	if (getSysConfigType() != prSysInquire.getSysConfigType())
	{
		setSysConfigType(prSysInquire.getSysConfigType());
		rv = true;
	}

	if (getStationAddress() != prSysInquire.getStationAddress())
	{
		setStationAddress(prSysInquire.getStationAddress());
		rv = true;
	}

	if (getControllerAddress() != prSysInquire.getControllerAddress())
	{
		setControllerAddress(prSysInquire.getControllerAddress());
		rv = true;
	}

	UINT8 deviceSignature1[SystemDevSignatureSize];
	UINT8 deviceSignature2[SystemDevSignatureSize];

	getDeviceSignature(deviceSignature1);
	prSysInquire.getDeviceSignature(deviceSignature2);

	if (memcmp(deviceSignature1, deviceSignature2, SystemDevSignatureSize) != 0)
	{
		setDeviceSignature(deviceSignature2);
		rv = true;
	}

	if ((prSysInquire.getHardwareVersionMajor() != getHardwareVersionMajor()) || 
		(prSysInquire.getHardwareVersionMinor() != getHardwareVersionMinor()))
	{
		setHardwareVersion(prSysInquire.getHardwareVersionMajor(), prSysInquire.getHardwareVersionMinor());
		rv = true;
	}

	return rv;
}

bool PrinterSystemConfiguration::equals(PrinterSystemConfiguration &prSysConfig)
{
	bool rv = false;

	if ((prType == prSysConfig.getPrinterType()) &&
		(getSysConfigType() == prSysConfig.getSysConfigType()) &&
		(getStationAddress() == prSysConfig.getStationAddress()) &&
		(getControllerAddress() == prSysConfig.getControllerAddress()))
	{
		UINT8 deviceSignature1[SystemDevSignatureSize];
		UINT8 deviceSignature2[SystemDevSignatureSize];

		getDeviceSignature(deviceSignature1);
		prSysConfig.getDeviceSignature(deviceSignature2);

		if (memcmp(deviceSignature1, deviceSignature2, SystemDevSignatureSize) == 0)
		{
			rv = true;
		}
	}

	return rv;
}

void PrinterSystemConfiguration::setPrinterType(PrinterType prType)
{
	this->prType = prType;
}

PrinterType PrinterSystemConfiguration::getPrinterType()
{
	return prType;
}

UINT8 PrinterSystemConfiguration::getHardwareVersionMajor() const
{
	return (UINT8)hardwareVersion.getMajorRevision();
}

UINT8 PrinterSystemConfiguration::getHardwareVersionMinor() const
{
	return (UINT8)hardwareVersion.getMinorRevision();
}

const HobVersion &PrinterSystemConfiguration::getHardwareVersion() const
{
	return hardwareVersion;
}

void PrinterSystemConfiguration::setHardwareVersion(UINT8 major, UINT8 minor)
{
	hardwareVersion.setMajorRevision(major);
	hardwareVersion.setMinorRevision(minor);
}

void PrinterSystemConfiguration::setHardwareVersion(const HobVersion &hardwareVersion)
{
	if (((hardwareVersion.getVersionType() == HobVersion::EppPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EppPrinterSW)) ||
	     ((hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::Epp2PrinterSW)) ||	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		((hardwareVersion.getVersionType() == HobVersion::EssPrinterSW) && (this->hardwareVersion.getVersionType() == HobVersion::EssPrinterSW)))
	{
		this->hardwareVersion.setMajorRevision(hardwareVersion.getMajorRevision());
		this->hardwareVersion.setMinorRevision(hardwareVersion.getMinorRevision());
	}
}
