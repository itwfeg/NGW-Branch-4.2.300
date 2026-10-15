/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Device.h"
#include "DeviceTags.h"
#include "Configuration.h"
#include "HobDateTime.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBDevicesRM.h"
#include "DBDevice.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "utilstring.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Device::dataType = 0;
std::string Device::dataTypeName("Device"); 
#endif

std::list<DeviceFieldOffsets> Device::fieldList, Device::externalPrinterFieldList;	


Device::Device()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Device::Device(const char *data, int len, int formatType)
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
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetDeviceType:
						deviceType = Integer::valueOf(token);
						break;
					case fieldOffsetDeviceSubType:
						deviceSubType = Integer::valueOf(token);
						break;
					case fieldOffsetKind:
						kind = Integer::valueOf(token);
						break;
					case fieldOffsetSerialNumber:
						serialNumber = token;
						break;
					case fieldOffsetHardwareVersion:
						hardwareVersion = token;
						break;
					case fieldOffsetSoftwareVersion:
						softwareVersion = token;
						break;
					case fieldOffsetCheckPresence:
						checkPresence = Bool::valueOf(token);
						break;
					case fieldOffsetServiceHistory:
						serviceHistory = token;
						break;
					case fieldOffsetUserName:
						userName = token;
						break;
					case fieldOffsetSystemName:
						systemName = token;
						break;
					case fieldOffsetIPAddress:
						ipAddress = token;
						break;
				
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case ExternalPrinterSaveFormat:
		{
			deviceType = (int)Printer;
			checkPresence = false;

			std::list<DeviceFieldOffsets>::iterator fieldIter = externalPrinterFieldList.begin();
			int offset;

			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != externalPrinterFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
					
				switch (offset)
				{
					case fieldOffsetDeviceSubType:
						deviceSubType = Integer::valueOf(token);
						break;
					case fieldOffsetUserName:
						userName = token;
						break;
					case fieldOffsetKind:
						kind = Integer::valueOf(token);
						break;
					case fieldOffsetIPAddress:
						ipAddress = token;
						break;
					case fieldOffsetSystemName:
						systemName = token;
						break;
					default:
						break;
				}
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			DeviceTags deviceTags;

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
					offset = (temp.size()) ? deviceTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetDeviceType:
								setDeviceType(Integer::valueOf(temp));
								break;
							case fieldOffsetDeviceSubType:
								setDeviceSubType(Integer::valueOf(temp));
								break;
							case fieldOffsetKind:
								setKind(Integer::valueOf(temp));
								break;
							case fieldOffsetSerialNumber:
								setSerialNumber(temp);
								break;
							case fieldOffsetHardwareVersion:
								setHardwareVersion(temp);
								break;
							case fieldOffsetSoftwareVersion:
								setSoftwareVersion(temp);
								break;
							case fieldOffsetCheckPresence:
								setCheckPresence(Bool::valueOf(temp));
								break;
							case fieldOffsetInstalledDate:
							{
								HobDateTime newDateTime;
								newDateTime.convertFromDBFormat(temp);
								setInstalledDate(newDateTime);
								break;
							}	
							case fieldOffsetServiceHistory:
								setServiceHistory(temp);
								break;
							case fieldOffsetUserName:
								setUserName(temp);
								break;
							case fieldOffsetSystemName:
								setSystemName(temp);
								break;
							case fieldOffsetIPAddress:
								setIPAddress(temp);
								break;
				
							default:
								break;
						}
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

Device::Device(DBDevice *dbDevice, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Device", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbDevice)
	{
		Debug::trace("Device", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbDevice;

		deviceType = Integer::valueOf(dbDevice->getType());
		deviceSubType = Integer::valueOf(dbDevice->getSubtype());
		kind = dbDevice->getKind();
		serialNumber = dbDevice->getSerialNumber();
		hardwareVersion = dbDevice->getHardwareVersion();
		softwareVersion = dbDevice->getSoftwareVersion();
		checkPresence = dbDevice->getCheckPresence();
		installedDate.convertFromDBFormat(dbDevice->getInstallationDate().c_str());
		serviceHistory = dbDevice->getServiceHistory();
		userName = dbDevice->getUserName();
		systemName = dbDevice->getSystemName();
		ipAddress = dbDevice->getIPAddress();

		errorCode = RECORD_OK;
	}
}

Device::~Device()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("Device.Delete", "    Device: " + util_string::ptrtostring(this));
}


//	private
void Device::init()
{
	deviceType = 0;
	deviceSubType = 0;
	kind = 0;
	serialNumber = "";
	hardwareVersion = "";
	softwareVersion = "";
	checkPresence = false;
	//installedDate  already defaults to 'now'
	serviceHistory = "";
	userName = "";
	systemName = "";
	ipAddress = "";

	dbAccess = 0;
	myDbRecord = 0;

	errorCode = RECORD_NOT_FOUND;

	initFieldList();
}

// static
void Device::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetDeviceType);
		fieldList.push_back(fieldOffsetDeviceSubType);
		fieldList.push_back(fieldOffsetKind);
		fieldList.push_back(fieldOffsetSerialNumber);
		fieldList.push_back(fieldOffsetHardwareVersion);
		fieldList.push_back(fieldOffsetSoftwareVersion);
		fieldList.push_back(fieldOffsetCheckPresence);
		fieldList.push_back(fieldOffsetInstalledDate);
		fieldList.push_back(fieldOffsetServiceHistory);
		fieldList.push_back(fieldOffsetUserName);
		fieldList.push_back(fieldOffsetSystemName);
		fieldList.push_back(fieldOffsetIPAddress);

		DeviceTags deviceTags((FieldOffsetsList *)&fieldList);
	}

	if (externalPrinterFieldList.empty())
	{
		externalPrinterFieldList.push_back(fieldOffsetDeviceSubType);
		externalPrinterFieldList.push_back(fieldOffsetUserName);
		externalPrinterFieldList.push_back(fieldOffsetKind);
		externalPrinterFieldList.push_back(fieldOffsetIPAddress);
		externalPrinterFieldList.push_back(fieldOffsetSystemName);
	}
}



void Device::updateDevice(Device *newDevice)
{
	if (!newDevice)
		return;

	short loopCnt = 1;
	while (loopCnt < endDeviceFieldOffsets)
	{
		if ( newDevice->isReceived(loopCnt) )
		{
			switch (loopCnt)
			{
				case fieldOffsetKind:
					// The UI only really deals with 'kind' set to EppPrinter (1), so if we get the kind field back from the UI,
					// and it comes back as either type of EPP printer, and this Device is also either type of EPP printer,
					// then don't update it, leave it alone.
					if ( (kind == EppPrinter || kind == Epp2Printer) && (newDevice->getKind() == EppPrinter || newDevice->getKind() == Epp2Printer) )
						;
					else
						setKind(newDevice->getKind());
					break;
				case fieldOffsetSerialNumber:
					setSerialNumber(newDevice->getSerialNumber());
					break;
				case fieldOffsetHardwareVersion:
					setHardwareVersion(newDevice->getHardwareVersion());
					break;
				case fieldOffsetSoftwareVersion:
					setSoftwareVersion(newDevice->getSoftwareVersion());
					break;
				case fieldOffsetCheckPresence:
					setCheckPresence(newDevice->isCheckPresence());
					break;
				case fieldOffsetInstalledDate:
					setInstalledDate(newDevice->getInstalledDate());
					break;
				case fieldOffsetServiceHistory:
					setServiceHistory(newDevice->getServiceHistory());
					break;
				case fieldOffsetUserName:
					setUserName(newDevice->getUserName());
					break;
				case fieldOffsetSystemName:
					setSystemName(newDevice->getSystemName());
					break;
				case fieldOffsetIPAddress:
					setIPAddress(newDevice->getIPAddress());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *Device::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endDeviceFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetDeviceType:
						serializedData.append(Integer::toString(deviceType));
						break;
					case fieldOffsetDeviceSubType:
						serializedData.append(Integer::toString(deviceSubType));
						break;
					case fieldOffsetKind:
						serializedData.append(Integer::toString(kind));
						break;
					case fieldOffsetSerialNumber:
						serializedData.append(serialNumber);
						break;
					case fieldOffsetHardwareVersion:
						serializedData.append(hardwareVersion);
						break;
					case fieldOffsetSoftwareVersion:
						serializedData.append(softwareVersion);
						break;
					case fieldOffsetCheckPresence:
						serializedData.append(Bool::toString(checkPresence));
						break;
					case fieldOffsetInstalledDate:
						serializedData.append(installedDate.convertToDBFormat());
						break;
					case fieldOffsetServiceHistory:
						serializedData.append(serviceHistory);
						break;
					case fieldOffsetUserName:
						serializedData.append(userName);
						break;
					case fieldOffsetSystemName:
						serializedData.append(systemName);
						break;
					case fieldOffsetIPAddress:
						serializedData.append(ipAddress);
						break;
					default:
						break;
				}
			}
			break;

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			DeviceTags deviceTags;
			HobString tag = "";
			HobString fieldData = "";
			
			std::list<DeviceFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = deviceTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetDeviceType:
						fieldData = Integer::toString(deviceType);
						break;
					case fieldOffsetDeviceSubType:
						fieldData = Integer::toString(deviceSubType);
						break;
					case fieldOffsetKind:
						fieldData = Integer::toString(kind);
						break;
					case fieldOffsetSerialNumber:
						fieldData = serialNumber;
						break;
					case fieldOffsetHardwareVersion:
						fieldData = hardwareVersion;
						break;
					case fieldOffsetSoftwareVersion:
						fieldData = softwareVersion;
						break;
					case fieldOffsetCheckPresence:
						fieldData = Bool::toString(checkPresence);
						break;
					case fieldOffsetInstalledDate:
						fieldData = installedDate.convertToDBFormat();
						break;
					case fieldOffsetServiceHistory:
						fieldData = serviceHistory;
						break;
					case fieldOffsetUserName:
						fieldData = userName;
						break;
					case fieldOffsetSystemName:
						fieldData = systemName;
						break;
					case fieldOffsetIPAddress:
						fieldData = ipAddress;
						break;
					default:
						tag = "";
						fieldData = "";
						break;
				}
						
				if (tag.size())
				{
					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			if ( Debug::isTracing("Device") )
				serializedData = HobString("::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void Device::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBDevice *Device::getMyDbRecord()
{
	return myDbRecord;
}


Device *Device::findDevice(int type, int subtype, DBAccess *dbAccess)
{
	Device *readDevice = 0;

	if ( Debug::isTracing("Device") )
		Debug::trace("Device", "Looking for device type:" + HobString(Integer::toString(type)) + HobString("  subtype:") + HobString(Integer::toString(subtype)));

	if ( type <= 0 || subtype < 0 || !dbAccess )
		return readDevice;

	DBDevicesRM	*dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return readDevice;

	try
	{
		string sType = Integer::toString(type);
		string sSubType = Integer::toString(subtype);
		DBDevice *dbDeviceRec = dbDevicesRM->findTypeSubtype( sType, sSubType );
		if (dbDeviceRec)
		{
			readDevice = new Device(dbDeviceRec, dbAccess);
			if (!readDevice)
				delete dbDeviceRec;
			else
			{
				if ( Debug::isTracing("Device") )
					Debug::trace("Device", "findDevice() - found Device: " + ptrtostring(readDevice) +
								"  type:" + inttostring(readDevice->getDeviceType()) + "  subtype:" + inttostring(readDevice->getDeviceSubType()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Device::findDevice() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readDevice;
}

bool Device::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Device::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBDevicesRM	*dbDevicesRM = dbAccess->getDBDevicesRM();
		if (dbDevicesRM)
		{
			Debug::trace("Device", HobString("Deleting Device: type:") + HobString(Integer::toString(deviceType)) + HobString("  subtype:") + HobString(Integer::toString(deviceSubType)));
			result = dbDevicesRM->deleteRecord(myDbRecord);	
		}
	}
	return result;							
}

void Device::saveRecord()
{
	if (deviceType > 0 && deviceSubType >= 0 && dbAccess)
	{
		DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
		if (!dbDevicesRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Device", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbDevicesRM->createRecord();
		}

		if (myDbRecord)
		{
//			Debug::trace("Device", "Going to set data elements");

			//	Plug the changes back into the dbDeviceRecord
		   myDbRecord->setType(Integer::toString(deviceType));
		   myDbRecord->setSubtype(Integer::toString(deviceSubType));
		   myDbRecord->setKind(kind);
		   myDbRecord->setSerialNumber(serialNumber);
		   myDbRecord->setHardwareVersion(hardwareVersion);
		   myDbRecord->setSoftwareVersion(softwareVersion);
		   myDbRecord->setCheckPresence(checkPresence);
		   myDbRecord->setInstallationDate(installedDate.convertToDBFormat());
		   myDbRecord->setServiceHistory(serviceHistory);
		   myDbRecord->setUserName(userName);
		   myDbRecord->setSystemName(systemName);
		   myDbRecord->setIPAddress(ipAddress);

			try
			{
				dbDevicesRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Device::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

HobErrorCode Device::getErrorCode() const
{
	return errorCode;
}
void Device::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


bool Device::isANonHobartLabelPrinter() const
{
	HobString printerSystemName = systemName;
	util_string::tolower(printerSystemName);

	Debug::trace("Device", HobString("isANonHobartLabelPrinter() - Device: type:") + HobString(Integer::toString(deviceType)) + "  kind:" + HobString(Integer::toString(kind)) + "  driverName:" + printerSystemName);
	if ( deviceType == (int)Printer && 
		( kind == (int)USBPrinter || ( kind == (int)NetworkPrinter && printerSystemName.find("zebra") != std::string::npos ) ) )
		return true;
	else
		return false;
}

bool Device::isAHobartLabelPrinter() const
{
	if ( deviceType == (int)Printer && 
		 ( kind == (int)EppPrinter || kind == (int)EssPrinter || kind == (int)EscPrinter ||
		   kind == (int)ApcPrinter || kind == (int)Epp2Printer || kind == (int)KPCPrinter || 
		   kind == (int)ARKPrinterOnly || kind == (int)KPPPrinter || kind == (int)SSKPCPrinter ) )
		return true;
	else
		return false;
}

bool Device::isANetworkPrinter() const
{
	if ( deviceType == (int)Printer && kind == (int)NetworkPrinter )
		return true;
	else
		return false;
}

bool Device::isANetworkReportPrinter() const
{
	HobString printerSystemName = systemName;
	util_string::tolower(printerSystemName);

	if ( deviceType == (int)Printer && kind == (int)NetworkPrinter && printerSystemName.find("zebra") == std::string::npos )
		return true;
	else
		return false;
}

bool Device::isARealPrinter() const
{
	if ( deviceType == (int)Printer && kind != (int)NoPrinter && kind != (int)FakePrinter )
		return true;
	else
		return false;
}
