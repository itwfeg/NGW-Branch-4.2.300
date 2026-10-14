//#include "vxWorks.h"
#include "SystemStatusInfo.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemStatusInfo::_dataType_ = 0;
std::string SystemStatusInfo::_dataTypeName_("SystemStatusInfo"); 
#endif

SystemStatusInfo::SystemStatusInfo(UINT16 bitTestAddress, 
								   UINT16 addressTestAddress,
								   UINT16 ROM_Checksum,
								   UINT32 flags,
								   const UINT8 partNumber[],
								   UINT16 systemChecksum,
								   UINT32 initFlags)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->bitTestAddress = bitTestAddress;
	this->addressTestAddress = addressTestAddress;
	this->ROM_Checksum = ROM_Checksum;
	this->flags = flags;

	for (int i = 0; i < SystemPartNumberSize; i++)
	{
		if (partNumber == NULL)
		{
			this->partNumber[i] = '\0';
		}
		else
		{
			this->partNumber[i] = partNumber[i];
		}
	}

	this->systemChecksum = systemChecksum;
	this->initFlags = initFlags;
}

SystemStatusInfo::~SystemStatusInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

SystemStatusInfo::SystemStatusInfo(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	int j = 0;

	char *pChar = (char *) &bitTestAddress;
	for (unsigned int i = 0; i < sizeof(bitTestAddress); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &addressTestAddress;
	for (unsigned int i = 0; i < sizeof(addressTestAddress); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &ROM_Checksum;
	for (unsigned int i = 0; i < sizeof(ROM_Checksum); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &flags;
	for (unsigned int i = 0; i < sizeof(flags); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &partNumber;
	for (unsigned int i = 0; i < sizeof(partNumber); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &systemChecksum;
	for (unsigned int i = 0; i < sizeof(systemChecksum); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &initFlags;
	for (unsigned int i = 0; i < sizeof(initFlags); i++)
	{
		*pChar++ = buffer[j++];
	}
}

const char *SystemStatusInfo::serialize() const
{
	serializedData = "";

	char *pChar = (char *) &bitTestAddress;
	for (unsigned int i = 0; i < sizeof(bitTestAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &addressTestAddress;
	for (unsigned int i = 0; i < sizeof(addressTestAddress); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &ROM_Checksum;
	for (unsigned int i = 0; i < sizeof(ROM_Checksum); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &flags;
	for (unsigned int i = 0; i < sizeof(flags); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &partNumber;
	for (unsigned int i = 0; i < sizeof(partNumber); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &systemChecksum;
	for (unsigned int i = 0; i < sizeof(systemChecksum); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &initFlags;
	for (unsigned int i = 0; i < sizeof(initFlags); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 SystemStatusInfo::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void SystemStatusInfo::show()
{
	Debug::addTraceCategory( std::string("SystemStatusInfo") );
	Debug::trace("SystemStatusInfo", "SystemStatusInfo: ");

	Debug::trace("SystemStatusInfo", HobString("bitTestAddress: ") +
									 HobString(Hex::toString(bitTestAddress)));
	Debug::trace("SystemStatusInfo", HobString("addressTestAddress: ") +
									 HobString(Hex::toString(addressTestAddress)));
	Debug::trace("SystemStatusInfo", HobString("ROM_Checksum: ") +
									 HobString(Hex::toString(ROM_Checksum)));
	Debug::trace("SystemStatusInfo", HobString("flags: ") +
									 HobString(Hex::toString(flags)));

	Debug::trace("SystemStatusInfo", "partNumber: ");
	Debug::trace("SystemStatusInfo", Hex::toString((const char *) &partNumber[0], SystemPartNumberSize));

	Debug::trace("SystemStatusInfo", HobString("systemChecksum: ") +
									 HobString(Hex::toString(systemChecksum)));
	Debug::trace("SystemStatusInfo", HobString("initFlags: ") +
									 HobString(Hex::toString(initFlags)));

	Debug::removeTraceCategory( std::string("SystemStatusInfo") );
}

UINT16 SystemStatusInfo::getBitTestAddress() const
{
	return bitTestAddress;
}

void SystemStatusInfo::setBitTestAddress(UINT16 bitTestAddress)
{
	this->bitTestAddress = bitTestAddress;
}

UINT16 SystemStatusInfo::getAddressTestAddress() const
{
	return addressTestAddress;
}

void SystemStatusInfo::setAddressTestAddress(UINT16 addressTestAddress)
{
	this->addressTestAddress = addressTestAddress;
}

UINT16 SystemStatusInfo::getROM_Checksum() const
{
	return ROM_Checksum;
}

void SystemStatusInfo::setROM_Checksum(UINT16 ROM_Checksum)
{
	this->ROM_Checksum = ROM_Checksum;
}

UINT32 SystemStatusInfo::getFlags() const
{
	return flags;
}

void SystemStatusInfo::setFlags(UINT32 flags)
{
	this->flags = flags;
}

const UINT8 *SystemStatusInfo::getPartNumber() const
{
	return (const UINT8 *) partNumber;
}

void SystemStatusInfo::setPartNumber(const UINT8 *partNumber)
{
	for (int i = 0; i < SystemPartNumberSize; i++)
	{
		if (partNumber == NULL)
		{
			this->partNumber[i] = '\0';
		}
		else
		{
			this->partNumber[i] = partNumber[i];
		}
	}
}

UINT16 SystemStatusInfo::getSystemChecksum() const
{
	return systemChecksum;
}

void SystemStatusInfo::setSystemChecksum(UINT16 systemChecksum)
{
	this->systemChecksum = systemChecksum;
}

UINT32 SystemStatusInfo::getInitFlags() const
{
	return initFlags;
}

void SystemStatusInfo::setInitFlags(UINT32 initFlags)
{
	this->initFlags = initFlags;
}


void SystemStatusInfo::setBitError()
{
	flags |= BitError;
}

void SystemStatusInfo::clearBitError()
{
	flags &= ~BitError;
}

bool SystemStatusInfo::isBitError() const
{
	return ((flags & BitError) == BitError);
}

void SystemStatusInfo::setAddressError()
{
	flags |= AddressError;
}

void SystemStatusInfo::clearAddressError()
{
	flags &= ~AddressError;
}

bool SystemStatusInfo::isAddressError() const
{
	return ((flags & AddressError) == AddressError);
}

void SystemStatusInfo::setEpromError()
{
	flags |= EpromError;
}

void SystemStatusInfo::clearEpromError()
{
	flags &= ~EpromError;
}

bool SystemStatusInfo::isEpromError() const
{
	return ((flags & EpromError) == EpromError);
}

void SystemStatusInfo::setEepromError()
{
	flags |= EepromError;
}

void SystemStatusInfo::clearEepromError()
{
	flags &= ~EepromError;
}

bool SystemStatusInfo::isEepromError() const
{
	return ((flags & EepromError) == EepromError);
}

void SystemStatusInfo::setRegisterError()
{
	flags |= RegisterError;
}

void SystemStatusInfo::clearRegisterError()
{
	flags &= ~RegisterError;
}

bool SystemStatusInfo::isRegisterError() const
{
	return ((flags & RegisterError) == RegisterError);
}

void SystemStatusInfo::setCommunicationError()
{
	flags |= CommunicationError;
}

void SystemStatusInfo::clearCommunicationError()
{
	flags &= ~CommunicationError;
}

bool SystemStatusInfo::isCommunicationError() const
{
	return ((flags & CommunicationError) == CommunicationError);
}

void SystemStatusInfo::setSystemError()
{
	flags |= SystemError;
}

void SystemStatusInfo::clearSystemError()
{
	flags &= ~SystemError;
}

bool SystemStatusInfo::isSystemError() const
{
	return ((flags & SystemError) == SystemError);
}

void SystemStatusInfo::setKeyboardError()
{
	flags |= KeyboardError;
}

void SystemStatusInfo::clearKeyboardError()
{
	flags &= ~KeyboardError;
}

bool SystemStatusInfo::isKeyboardError() const
{
	return ((flags & KeyboardError) == KeyboardError);
}

void SystemStatusInfo::setGraphicError()
{
	flags |= GraphicError;
}

void SystemStatusInfo::clearGraphicError()
{
	flags &= ~GraphicError;
}

bool SystemStatusInfo::isGraphicError() const
{
	return ((flags & GraphicError) == GraphicError);
}

void SystemStatusInfo::setNumericError()
{
	flags |= NumericError;
}

void SystemStatusInfo::clearNumericError()
{
	flags &= ~NumericError;
}

bool SystemStatusInfo::isNumericError() const
{
	return ((flags & NumericError) == NumericError);
}

void SystemStatusInfo::setPrinterError()
{
	flags |= PrinterError;
}

void SystemStatusInfo::clearPrinterError()
{
	flags &= ~PrinterError;
}

bool SystemStatusInfo::isPrinterError() const
{
	return ((flags & PrinterError) == PrinterError);
}

void SystemStatusInfo::setWeigherError()
{
	flags |= WeigherError;
}

void SystemStatusInfo::clearWeigherError()
{
	flags &= ~WeigherError;
}

bool SystemStatusInfo::isWeigherError() const
{
	return ((flags & WeigherError) == WeigherError);
}

void SystemStatusInfo::setClaError()
{
	flags |= ClaError;
}

void SystemStatusInfo::clearClaError()
{
	flags &= ~ClaError;
}

bool SystemStatusInfo::isClaError() const
{
	return ((flags & ClaError) == ClaError);
}

void SystemStatusInfo::setDiskError()
{
	flags |= DiskError;
}

void SystemStatusInfo::clearDiskError()
{
	flags &= ~DiskError;
}

bool SystemStatusInfo::isDiskError() const
{
	return ((flags & DiskError) == DiskError);
}

void SystemStatusInfo::setMarqueError()
{
	flags |= MarqueError;
}

void SystemStatusInfo::clearMarqueError()
{
	flags &= ~MarqueError;
}

bool SystemStatusInfo::isMarqueError() const
{
	return ((flags & MarqueError) == MarqueError);
}

void SystemStatusInfo::setWrapperError()
{
	flags |= WrapperError;
}

void SystemStatusInfo::clearWrapperError()
{
	flags &= ~WrapperError;
}

bool SystemStatusInfo::isWrapperError() const
{
	return ((flags & WrapperError) == WrapperError);
}
