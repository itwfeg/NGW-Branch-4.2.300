/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Peripheral.h"
#include "PeripheralTags.h"
#include "HfcError.h"
#include "HobString.h"
#include "HobDateTime.h"
//#include "Configuration.h"
//#include "filename.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBDevicesRM.h"
#include "DBDevice.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Peripheral::dataType = 0;
std::string Peripheral::dataTypeName("Peripheral"); 
#endif

std::list<PeripheralFieldOffsets> Peripheral::evolFieldList, Peripheral::browserFieldList;


Peripheral::Peripheral()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Peripheral::Peripheral(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<PeripheralFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);

				if (token.length())
				{
					if (isFieldQuoted(offset))
						token.unQuoteForCSV();

					updateField(offset, token);
				}
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			PeripheralTags peripheralTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? peripheralTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
					}
				}
				delete token;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

Peripheral::Peripheral(edb::DBDevice *dbDevice, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
    HobString debugHeader = HobString("Peripheral(2): ");
	Debug::trace("Peripheral", debugHeader+"Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbDevice)
	{
		Debug::trace("Peripheral", debugHeader+HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbDevice;

		type = (EvolDeviceType) Integer::valueOf(dbDevice->getType());
		subType = Integer::valueOf(dbDevice->getSubtype());
		kind = dbDevice->getKind();
		serialNumber = dbDevice->getSerialNumber();
		serviceHistory = dbDevice->getServiceHistory();
		hardwareVersion = dbDevice->getHardwareVersion();
		softwareVersion = dbDevice->getSoftwareVersion();
		checkingPresence = dbDevice->getCheckPresence();
        
        //Show the values assigned to the peripheral
        Debug::trace("Peripheral", debugHeader+HobString("Peripheral Values: - type:")+Integer::toString(type)+
                HobString(",subType:")+Integer::toString(subType)+
                HobString(",kind:")+Integer::toString(kind)
        );

		if (!dbDevice->isInstallationDateNull())
		{
			HobDateTime dt;
			HobString dts(dbDevice->getInstallationDate());
			dt.convertFromDBFormat(&dts);
			installationDate = dt;
		}

		errorCode = RECORD_OK;
	}
}


Peripheral::~Peripheral()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("Peripheral.Delete", HobString("    Peripheral: ") + HobString(Hex::toString((long long) this)) +
												 HobString("  type:") + HobString(Integer::toString((int)type)) +
												 HobString("  subType:") + HobString(Integer::toString(subType)));
}


//	private
void Peripheral::init()
{
	type = UnknownDevice;
	subType = 0;
	kind = 0;
	serialNumber = "";
	// installationDate automatically initialized to 'now'
	serviceHistory = "";
	hardwareVersion = "";
	softwareVersion = "";
	checkingPresence = false;

	errorCode = RECORD_NOT_FOUND;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBDevice *)NULL;

	initFieldList();
}


// static
void Peripheral::initFieldList()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetPeripheralType);
		browserFieldList.push_back(fieldOffsetPeripheralSubType);
		browserFieldList.push_back(fieldOffsetPeripheralKind);
		browserFieldList.push_back(fieldOffsetPeripheralSerialNumber);
		browserFieldList.push_back(fieldOffsetPeripheralServiceHistory);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetPeripheralType);
		evolFieldList.push_back(fieldOffsetPeripheralSubType);
		evolFieldList.push_back(fieldOffsetPeripheralKind);
		evolFieldList.push_back(fieldOffsetPeripheralSerialNumber);
		evolFieldList.push_back(fieldOffsetPeripheralInstallationDate);
		evolFieldList.push_back(fieldOffsetPeripheralServiceHistory);
		evolFieldList.push_back(fieldOffsetPeripheralHardwareVersion);
		evolFieldList.push_back(fieldOffsetPeripheralSoftwareVersion);
		evolFieldList.push_back(fieldOffsetPeripheralCheckPresence);

		PeripheralTags peripheralTags((FieldOffsetsList *)&evolFieldList);
	}
}


EvolDeviceType Peripheral::getType() const
{
	return type;
}
void Peripheral::setType(EvolDeviceType dev)
{
	type = dev;
}

short Peripheral::getSubType() const
{
	return subType;
}
void Peripheral::setSubType(short sub)
{
	subType = sub;
}

short Peripheral::getKind() const
{
	return kind;
}
void Peripheral::setKind(short peripheralKind)
{
	kind = peripheralKind;
}

const HobString &Peripheral::getSerialNumber() const
{
	return serialNumber;
}
void Peripheral::setSerialNumber(const HobString &serNum)
{
	serialNumber = serNum;
}

const HobDateTime &Peripheral::getInstallationDate() const
{
	return installationDate;
}
void Peripheral::setInstallationDate(const HobDateTime &date)
{
	installationDate = date;
}

const HobString &Peripheral::getServiceHistory() const
{
	return serviceHistory;
}
void Peripheral::setServiceHistory(const HobString &history)
{
	serviceHistory = history;
}

const HobString &Peripheral::getHardwareVersion() const
{
	return hardwareVersion;
}
void Peripheral::setHardwareVersion(const HobString &ver)
{
	hardwareVersion = ver;
}

const HobString &Peripheral::getSoftwareVersion() const
{
	return softwareVersion;
}
void Peripheral::setSoftwareVersion(const HobString &ver)
{
	softwareVersion = ver;
}

bool Peripheral::isCheckingPresence() const
{
	return checkingPresence;
}
void Peripheral::setCheckingPresence(bool checking)
{
	checkingPresence = checking;
}

HobErrorCode Peripheral::getErrorCode() const
{
	return errorCode;
}
void Peripheral::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


bool Peripheral::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetPeripheralSerialNumber:
		case fieldOffsetPeripheralInstallationDate:
		case fieldOffsetPeripheralServiceHistory:
		case fieldOffsetPeripheralHardwareVersion:
		case fieldOffsetPeripheralSoftwareVersion:
			return true;
			break;

		default:
			break;
	}
	return false;
}


int Peripheral::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("Peripheral"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("Peripheral", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetPeripheralType:
			setType((EvolDeviceType)Integer::valueOf(token));
			break;
		case fieldOffsetPeripheralSubType:
			setSubType(Integer::valueOf(token));
			break;
		case fieldOffsetPeripheralKind:
			setKind(Integer::valueOf(token));
			break;
		case fieldOffsetPeripheralSerialNumber:
			setSerialNumber(token);
			break;
		case fieldOffsetPeripheralInstallationDate:
			//setInstallationDate(token);
			break;
		case fieldOffsetPeripheralServiceHistory:
			setServiceHistory(token);
			break;
		case fieldOffsetPeripheralHardwareVersion:
			setHardwareVersion(token);
			break;
		case fieldOffsetPeripheralSoftwareVersion:
			setSoftwareVersion(token);
			break;
		case fieldOffsetPeripheralCheckPresence:
			setCheckingPresence(Bool::valueOf(token));
			break;
		default:
			break;
	}

	return 0;
}

void Peripheral::updatePeripheral(const Peripheral *newPeripheral)
{
	if (!newPeripheral)
		return;

	short loopCnt = 1;
	while (loopCnt < endPeripheralFieldOffsets)
	{
		if (newPeripheral->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetPeripheralType:
					setType(newPeripheral->getType());
					break;
				case fieldOffsetPeripheralSubType:
					setSubType(newPeripheral->getSubType());
					break;
				case fieldOffsetPeripheralKind:
					setKind(newPeripheral->getKind());
					break;
				case fieldOffsetPeripheralSerialNumber:
					setSerialNumber(newPeripheral->getSerialNumber());
					break;
				case fieldOffsetPeripheralInstallationDate:
					setInstallationDate(newPeripheral->getInstallationDate());
					break;
				case fieldOffsetPeripheralServiceHistory:
					setServiceHistory(newPeripheral->getServiceHistory());
					break;
				case fieldOffsetPeripheralHardwareVersion:
					setHardwareVersion(newPeripheral->getHardwareVersion());
					break;
				case fieldOffsetPeripheralSoftwareVersion:
					setSoftwareVersion(newPeripheral->getSoftwareVersion());
					break;
				case fieldOffsetPeripheralCheckPresence:
					setCheckingPresence(newPeripheral->isCheckingPresence());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *Peripheral::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<PeripheralFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			PeripheralTags peripheralTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = peripheralTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("Peripheral::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void Peripheral::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetPeripheralType:
			result = Integer::toString((int)type);
			break;
		case fieldOffsetPeripheralSubType:
			result = Integer::toString(subType);
			break;
		case fieldOffsetPeripheralKind:
			result = Integer::toString(kind);
			break;
		case fieldOffsetPeripheralSerialNumber:
			result = serialNumber;
			break;
		case fieldOffsetPeripheralInstallationDate:
			result = installationDate.convertToDBFormat( (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat) ? FormatDateTime_Windux : FormatDateTime_HTSeries );
			break;
		case fieldOffsetPeripheralServiceHistory:
			result = serviceHistory;
			break;
		case fieldOffsetPeripheralHardwareVersion:
			result = hardwareVersion;
			break;
		case fieldOffsetPeripheralSoftwareVersion:
			result = softwareVersion;
			break;
		case fieldOffsetPeripheralCheckPresence:
			result = Bool::toString(checkingPresence);
			break;
		default:
			break;
	}
}


void Peripheral::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


edb::DBDevice *Peripheral::getMyDbRecord() const
{
	return myDbRecord;
}


Peripheral *Peripheral::findPeripheral(EvolDeviceType peripheralType, short peripheralSubType, DBAccess *dbAccess)
{
	Peripheral *readPeripheral = (Peripheral *)NULL;

	if (peripheralType == UnknownDevice || !dbAccess)
		return readPeripheral;

	DBDevicesRM	*dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return readPeripheral;

	try
	{
		DBDevice *dbDeviceRec = dbDevicesRM->find(HobString(Integer::toString((int)peripheralType)), HobString(Integer::toString((int)peripheralSubType)));
		if (dbDeviceRec)
		{
			Debug::trace("Peripheral", "findPeripheral() - found this record in the db");
			readPeripheral = new Peripheral(dbDeviceRec, dbAccess);
			if (!readPeripheral)
				delete dbDeviceRec;
			else
			{
				if (Debug::isTracing("Peripheral"))
					Debug::trace("Peripheral", HobString("findPeripheral() - created the hfc Peripheral object - Peripheral: ") + HobString(Hex::toString((long long) readPeripheral)) +
													HobString("  type:") + HobString(Integer::toString((int)readPeripheral->getType())) + 
													HobString("  subType:") + HobString(Integer::toString(readPeripheral->getSubType())));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Peripheral::findPeripheral() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readPeripheral;
}


bool Peripheral::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Peripheral::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
		if (dbDevicesRM)
		{
			Debug::trace("Peripheral", HobString("Deleting Peripheral type:") + HobString(Integer::toString((int)type)) +
												HobString("  subType:") + HobString(Integer::toString(subType)) +
												HobString("  kind:") + HobString(Integer::toString(kind)));
			result = dbDevicesRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void Peripheral::saveRecord()
{
	if (type != UnknownDevice && dbAccess)
	{
		DBDevicesRM	*dbDevicesRM = dbAccess->getDBDevicesRM();
		if (!dbDevicesRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Peripheral", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbDevicesRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbDeviceRecord
			myDbRecord->setType(Integer::toString((int)type));
			myDbRecord->setSubtype(Integer::toString(subType));
			myDbRecord->setKind(kind);

			if (serialNumber.empty())
				myDbRecord->setSerialNumberNull();
			else
				myDbRecord->setSerialNumber(serialNumber);

			if (serviceHistory.empty())
				myDbRecord->setServiceHistoryNull();
			else
				myDbRecord->setServiceHistory(serviceHistory);

			if (hardwareVersion.empty())
				myDbRecord->setHardwareVersionNull();
			else
				myDbRecord->setHardwareVersion(hardwareVersion);

			if (softwareVersion.empty())
				myDbRecord->setSoftwareVersionNull();
			else
				myDbRecord->setSoftwareVersion(softwareVersion);

			myDbRecord->setCheckPresence(checkingPresence);
			myDbRecord->setInstallationDate(installationDate.convertToDBFormat());

			try
			{
				dbDevicesRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Peripheral::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}


// virtual, override from basic functionality in HfcObject
HobString Peripheral::getMyKeyValue() const
{
	return HobString(Long::toString((int)type));
}

// virtual, override from basic functionality in HfcObject
EvolDeviceType Peripheral::getMyEvolDevice() const
{
	return PeripheralDev;
}

