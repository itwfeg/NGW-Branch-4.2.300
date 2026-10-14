/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobEmailAlert.h"
#include "SendErrorEmailEvent.h"
#include "EmailAlertTags.h"
#include "EvolMessage.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBEmailAlertsRM.h"
#include "DBEmailAlert.h"

#include "MultiLanguageText.h"

#include "util/e_string/utilstring.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobEmailAlert::dataType = 0;
std::string HobEmailAlert::dataTypeName("HobEmailAlert"); 
#endif

std::list<HobEmailAlertFieldOffsets> HobEmailAlert::evolFieldList, HobEmailAlert::browserFieldList;


HobEmailAlert::HobEmailAlert()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

HobEmailAlert::HobEmailAlert(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	int offset = 0;

	Debug::trace("HobEmailAlert", "HobEmailAlert constructor, deserialize browser message");

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<HobEmailAlertFieldOffsets>::iterator fieldIter = browserFieldList.begin();

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.size())
				{
					if (offset == fieldOffsetEASubject)
						token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			EmailAlertTags	emailAlertTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? emailAlertTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();

						updateField(offset, temp);
					}
				}
				delete token;
			}
			break;
		}

		default:
			break;
	}

}


HobEmailAlert::HobEmailAlert(DBEmailAlert *dbEmailAlert, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("HobEmailAlert", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbEmailAlert)
	{
		Debug::trace("HobEmailAlert", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbEmailAlert;

		recordNum = (HobRecordNum) Long::valueOf(dbEmailAlert->getNumber());
		alertSubject = dbEmailAlert->getSubject1();
		notifyOnFailure = dbEmailAlert->getNotifyOnFailure();
		notifyOnResolution = dbEmailAlert->getNotifyOnResolution();
		notifyHandsFreeDeviceOnFailure = dbEmailAlert->getNotifyHandsFreeDeviceOnFailure();
		notifyHandsFreeDeviceOnResolution = dbEmailAlert->getNotifyHandsFreeDeviceOnResolution();
		delayTime = dbEmailAlert->getDelay();
		sentFailure = dbEmailAlert->getFailureSent();

		errorCode = RECORD_OK;
	}
}


HobEmailAlert::~HobEmailAlert()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("HobEmailAlert.Delete", "    HobEmailAlert: " + util_string::ptrtostring(this) + "  HobEmailAlert number:" +
			util_string::longtostring(recordNum));
}


// private
void HobEmailAlert::init()
{
	recordNum = (HobRecordNum) RECORD_NOT_FOUND;
	alertSubject = "";
	notifyOnFailure = false;
	notifyOnResolution = false;
	notifyHandsFreeDeviceOnFailure = false;
	notifyHandsFreeDeviceOnResolution = false;
	delayTime = 0;
	sentFailure = false;
	saveNeeded = false;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBEmailAlert *)NULL;

	errorCode = RECORD_NOT_FOUND;

	initFieldLists();
}


// static
void HobEmailAlert::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetEANum);
		browserFieldList.push_back(fieldOffsetEASubject);
		browserFieldList.push_back(fieldOffsetEANotifyOnFailure);
		browserFieldList.push_back(fieldOffsetEANotifyOnResolution);
		browserFieldList.push_back(fieldOffsetEANotifyHandsFreeDeviceOnFailure);
		browserFieldList.push_back(fieldOffsetEANotifyHandsFreeDeviceOnResolution);
		browserFieldList.push_back(fieldOffsetEADelay);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetEANum);
		evolFieldList.push_back(fieldOffsetEASubject);
		evolFieldList.push_back(fieldOffsetEANotifyOnFailure);
		evolFieldList.push_back(fieldOffsetEANotifyOnResolution);
		evolFieldList.push_back(fieldOffsetEANotifyHandsFreeDeviceOnFailure);
		evolFieldList.push_back(fieldOffsetEANotifyHandsFreeDeviceOnResolution);
		evolFieldList.push_back(fieldOffsetEADelay);

		EmailAlertTags emailAlertTags((std::list<UINT16> *)&evolFieldList);
	}
}


int HobEmailAlert::updateField(int fieldCode, const HobString &token)
{
	if (fieldCode < 0)
		return -1;

	Debug::trace("HobEmailAlert", HobString("    updateField(") + HobString(Integer::toString(fieldCode)) + HobString(", ") + token + HobString(")"));
	switch (fieldCode)
	{
		case fieldOffsetEANum:
			setNum((short)Integer::valueOf(token));
			break;
		case fieldOffsetEASubject:
			setSubject(token);
			break;
		case fieldOffsetEANotifyOnFailure:
			setNotifyOnFailure(Bool::valueOf(token));
			break;
		case fieldOffsetEANotifyOnResolution:
			setNotifyOnResolution(Bool::valueOf(token));
			break;
		case fieldOffsetEANotifyHandsFreeDeviceOnFailure:
			setNotifyHandsFreeDeviceOnFailure(Bool::valueOf(token));
			break;
		case fieldOffsetEANotifyHandsFreeDeviceOnResolution:
			setNotifyHandsFreeDeviceOnResolution(Bool::valueOf(token));
			break;
		case fieldOffsetEADelay:
			setDelay(Integer::valueOf(token));
			break;

		default:
			break;
	}

	return 0;/*resultCode;*/
}


void HobEmailAlert::update(const HobEmailAlert *newHobEmailAlert)
{
	if (Debug::isTracing("HobEmailAlert"))
	{
		HobString myDebug("update():  newHobEmailAlert = ");
		myDebug.append(util_string::ptrtostring(newHobEmailAlert));
		Debug::trace("HobEmailAlert", myDebug);
	}

	if (!newHobEmailAlert)
		return;

	short loopCnt = 1;
	while (loopCnt < endHobEmailAlertFieldOffsets)
	{
		if (Debug::isTracing("HobEmailAlert"))
		{
			HobString myDebug("update():  fieldCode = ");
			myDebug.append(Integer::toString(loopCnt)).append("   isReceived = ").append(Bool::toString(newHobEmailAlert->isReceived(loopCnt)));
			Debug::trace("HobEmailAlert", myDebug);
		}
		if (newHobEmailAlert->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetEANum:
					setNum(newHobEmailAlert->getNum());
					break;
				case fieldOffsetEASubject:
					setSubject(newHobEmailAlert->getSubject());
					break;
				case fieldOffsetEANotifyOnFailure:
					setNotifyOnFailure(newHobEmailAlert->getNotifyOnFailure());
					break;
				case fieldOffsetEANotifyOnResolution:
					setNotifyOnResolution(newHobEmailAlert->getNotifyOnResolution());
					break;
				case fieldOffsetEANotifyHandsFreeDeviceOnFailure:
					setNotifyHandsFreeDeviceOnFailure(newHobEmailAlert->getNotifyHandsFreeDeviceOnFailure());
					break;
				case fieldOffsetEANotifyHandsFreeDeviceOnResolution:
					setNotifyHandsFreeDeviceOnResolution(newHobEmailAlert->getNotifyHandsFreeDeviceOnResolution());
					break;
				case fieldOffsetEADelay:
					setDelay(newHobEmailAlert->getDelay());
					break;

				default:
					break;
			}
		}
		loopCnt++;
	}
}


void HobEmailAlert::getFieldAsString(int fieldCode, HobString &result) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetEANum:
			result = Long::toString((long)recordNum);
			break;
		case fieldOffsetEASubject:
			result = alertSubject;
			break;
		case fieldOffsetEANotifyOnFailure:
			result = Bool::toString(notifyOnFailure);
			break;
		case fieldOffsetEANotifyOnResolution:
			result = Bool::toString(notifyOnResolution);
			break;
		case fieldOffsetEANotifyHandsFreeDeviceOnFailure:
			result = Bool::toString(notifyHandsFreeDeviceOnFailure);
			break;
		case fieldOffsetEANotifyHandsFreeDeviceOnResolution:
			result = Bool::toString(notifyHandsFreeDeviceOnResolution);
			break;
		case fieldOffsetEADelay:
			result = Integer::toString(delayTime);
			break;
		default:
			Debug::trace("HobEmailAlert", "getFieldAsString() Don't recognize this fieldCode!");
			break;
	}
	if (Debug::isTracing("HobEmailAlert"))
	{
		HobString myDebug("getFieldAsString(");
		myDebug.append(Integer::toString(fieldCode)).append(") = ").append(result).append(" *");
		Debug::trace("HobEmailAlert", myDebug);
	}
}


const char *HobEmailAlert::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<HobEmailAlertFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (fieldIter != browserFieldList.begin())
					serializedData.append(BrowserFormatSeparator);
				getFieldAsString(*fieldIter, fieldData);
				switch (*fieldIter)
				{
					case fieldOffsetEASubject:
						fieldData.quoteForCSV();
						break;
					default:
						break;
				}
				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case DBIndependentFormatFormat:
		case EvolutionTaggedFormat:
		{
			EmailAlertTags emailAlertTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = emailAlertTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}


		default:
			serializedData = HobString("HobEmailAlert::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void HobEmailAlert::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


HobEmailAlert *HobEmailAlert::findEmailAlert(HobRecordNum searchNumber, DBAccess *dbAccess)
{
	HobEmailAlert *readEmailAlert = (HobEmailAlert *)NULL;
	HobString myDebug("");
	if (Debug::isTracing("HobEmailAlert"))
		myDebug.append("findEmailAlert():  ");

	if (searchNumber < 0 || !dbAccess)
	{
		if (Debug::isTracing("HobEmailAlert"))
		{
			HobString myDebug2(myDebug);
			myDebug2.append(" No EmailAlert found!  searchNumber = ").append(Long::toString(searchNumber));
			myDebug2.append(" dbAccess = ").append(util_string::ptrtostring(dbAccess));
			Debug::trace("HobEmailAlert", myDebug2);
		}
		return readEmailAlert;
	}

	DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
	if (!dbEmailAlertsRM)
		return readEmailAlert;

	try
	{
		if (Debug::isTracing("HobEmailAlert"))
		{
			HobString myDebug2(myDebug);
			myDebug2.append("Looking for record # ").append(Long::toString(searchNumber));
			Debug::trace("HobEmailAlert", myDebug2);
		}
		DBEmailAlert *dbEmailAlert = dbEmailAlertsRM->findNumber(Long::toString(searchNumber));
		if (dbEmailAlert)
		{
			if (Debug::isTracing("HobEmailAlert"))
			{
				HobString myDebug2(myDebug);
				myDebug2.append("Found record # ").append(Long::toString(searchNumber));
				Debug::trace("HobEmailAlert", myDebug2);
			}
			readEmailAlert = new HobEmailAlert(dbEmailAlert, dbAccess);
			if (!readEmailAlert)
				delete dbEmailAlert;
			else
			{
				if (Debug::isTracing("HobEmailAlert"))
				{
					HobString myDebug2(myDebug);
					myDebug2.append("new HobEmailAlert object readEmailAlert = ").append(util_string::ptrtostring(readEmailAlert)).append("  recordNum: ").append(Long::toString(readEmailAlert->getNum()));
					Debug::trace("HobEmailAlert", myDebug2);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("HobEmailAlert::findEmailAlert() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			HobString myDebug2("DBException: type:");
			myDebug2.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug2);
		}
	}
	return readEmailAlert;
}


bool HobEmailAlert::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int HobEmailAlert::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
		if (dbEmailAlertsRM)
		{
			if (Debug::isTracing("HobEmailAlert"))
			{
				HobString myDebug("deleteRecord(): Deleting HobEmailAlert# ");
				myDebug.append(Long::toString(recordNum));
				Debug::trace("HobEmailAlert", myDebug);
			}
			result = dbEmailAlertsRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void HobEmailAlert::saveRecord()
{
	HobString myDebug;
	if (Debug::isTracing("HobEmailAlert"))
	{
		myDebug = "saveRecord():  ";
		Debug::trace("HobEmailAlert", myDebug);
	}

	if (dbAccess && saveNeeded && recordNum >= 0)
	{
		DBEmailAlertsRM *dbEmailAlertsRM = dbAccess->getDBEmailAlertsRM();
		if (!dbEmailAlertsRM)
			return;

		if (!myDbRecord) // find only current record id. we do not allow the user to create new records.
		{
			if (Debug::isTracing("HobEmailAlert"))
			{
				HobString myDebug2(myDebug);
				myDebug2.append("Don't have a db record, creating it...");
				Debug::trace("HobEmailAlert", myDebug2);
			}
			myDbRecord = dbEmailAlertsRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbOperatorRecord
			myDbRecord->setNumber(std::string(Long::toString(recordNum)));
			myDbRecord->setSubject1(alertSubject);		//right now the user cannot change text
			myDbRecord->setNotifyOnFailure(notifyOnFailure);
			myDbRecord->setNotifyOnResolution(notifyOnResolution);
			myDbRecord->setNotifyHandsFreeDeviceOnFailure(notifyHandsFreeDeviceOnFailure);
			myDbRecord->setNotifyHandsFreeDeviceOnResolution(notifyHandsFreeDeviceOnResolution);
			myDbRecord->setDelay(delayTime);
			myDbRecord->setFailureSent(sentFailure);

			//	Now save the Product record to the database.
			if (Debug::isTracing("HobEmailAlert"))
			{
				HobString myDebug2(myDebug);
				myDebug2.append("Before saving to db ... HobEmailAlert # ").append(Long::toString(recordNum));
				myDebug2.append(", subject ").append(alertSubject);
				myDebug2.append(", NOF ").append(Bool::toString(notifyOnFailure));
				myDebug2.append(", NOR ").append(Bool::toString(notifyOnResolution));
				myDebug2.append(", NHFDOF ").append(Bool::toString(notifyHandsFreeDeviceOnFailure));
				myDebug2.append(", NHFDOR ").append(Bool::toString(notifyHandsFreeDeviceOnResolution));
				myDebug2.append(", delay ").append(Integer::toString(delayTime));
				myDebug2.append(", FailSent ").append(Bool::toString(sentFailure));
				Debug::trace("HobEmailAlert", myDebug2);
			}
			try
			{
				dbEmailAlertsRM->saveRecord(myDbRecord);
				if (Debug::isTracing("HobEmailAlert"))
				{
					HobString myDebug2(myDebug);
					myDebug2.append("After saving to db ... HobEmailAlert # ").append(Long::toString(recordNum));
					Debug::trace("HobEmailAlert", myDebug2);
				}
			}
			catch (DBException &dbe)
			{
				if (Debug::isTracing("DBAccess.exception"))
				{
					Debug::trace("DBAccess.exception", "HobEmailAlert::saveRecord() ********** database exception **********");
					DBException::Type t = dbe.getType();

					HobString myDebug2(" DBException: type: ");
					myDebug2.append(Integer::toString(t)).append("  info: ").append(dbe.info());
					Debug::trace("DBAccess.exception", myDebug2);
				}
			}
		}
	}
	else
	{
		if (Debug::isTracing("HobEmailAlert"))
		{
			if (!dbAccess)
				myDebug.append("  dbAccess is NULL! ");
			if (!saveNeeded)
				myDebug.append("  saveNeeded is false! ");
			if (recordNum < 0)
				myDebug.append("  recordNum = ").append(Long::toString(recordNum));
		}
	}
}


void HobEmailAlert::show()
{
	if (Debug::isTracing("HobEmailAlert"))
	{
		Debug::trace("HobEmailAlert", "    show(START)   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		HobString fieldData = "";
		std::list<HobEmailAlertFieldOffsets>::iterator fieldIter = browserFieldList.begin();
		while (fieldIter != browserFieldList.end())
			getFieldAsString(*fieldIter, fieldData);
		Debug::trace("HobEmailAlert", "    show(END)   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
	}
}

// static
void HobEmailAlert::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("HobEmailAlert", "verifyReservedRecords - no dbAccess passed in!");
		return;
	}

	Debug::trace("HobEmailAlert", "verifying all reserved emailAlerts ...");
	HobEmailAlert *reservedAlert = (HobEmailAlert *)NULL;
	hfc::MultiLanguageText internationalize;

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeOutOfLabels, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #0: Out Of Labels");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailCodeOutOfLabels);
			reservedAlert->setSubject(internationalize("EmailAlerts.OutOfLabels"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeCassetteOpen, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #1: Cassette Open");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailCodeCassetteOpen);
			reservedAlert->setSubject(internationalize("EmailAlerts.CassetteOpen"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeGcdPlaylistError, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #2: GCD Playlist Error");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailCodeGcdPlaylistError);
			reservedAlert->setSubject(internationalize("EmailAlerts.GCDPlaylistError"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeGcdFtpError, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #3: GCD FTP Error");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailCodeGcdFtpError);
			reservedAlert->setSubject(internationalize("EmailAlerts.GCDFTPError"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeNetworkOffline, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #4: Network Offline Notice");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailCodeNetworkOffline);
			reservedAlert->setSubject(internationalize("EmailAlerts.NetworkOfflineNotice"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailCodeLabelPresent, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #5: Label Present");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailCodeLabelPresent);
			reservedAlert->setSubject(internationalize("EmailAlerts.LabelPresent"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

	reservedAlert = findEmailAlert(SendErrorEmailEvent::ErrorEmailFPPUpdateFailed, dbAccess);
	if (!reservedAlert)
	{
		Debug::trace("HobEmailAlert", "creating reserved emailAlert #6: FPP Update Failed");
		reservedAlert = new HobEmailAlert();
		if (reservedAlert)
		{
			reservedAlert->setNum(SendErrorEmailEvent::ErrorEmailFPPUpdateFailed);
			reservedAlert->setSubject(internationalize("EmailAlerts.FPPUpdateFailed"));
			reservedAlert->setDBAccess(dbAccess);
			reservedAlert->saveRecord();
		}
	}

	if (reservedAlert)
	{
		delete reservedAlert;
		reservedAlert = (HobEmailAlert *)NULL;
	}

}

HobRecordNum HobEmailAlert::getNum() const
{
	return recordNum;
}
void HobEmailAlert::setNum(HobRecordNum number)
{
	if (recordNum != number)
	{
		recordNum = number;
		saveNeeded = true;
	}
}

HobString HobEmailAlert::getSubject() const
{
	return alertSubject;
}
void HobEmailAlert::setSubject(HobString subject)
{
	if (alertSubject != subject)
	{
		alertSubject = subject;
		saveNeeded = true;
	}
}

bool HobEmailAlert::getNotifyOnFailure() const
{
	return notifyOnFailure;
}
void HobEmailAlert::setNotifyOnFailure(bool notify)
{
	if (notifyOnFailure != notify)
	{
		notifyOnFailure = notify;
		saveNeeded = true;
	}
}

bool HobEmailAlert::getNotifyOnResolution() const
{
	return notifyOnResolution;
}
void HobEmailAlert::setNotifyOnResolution(bool notify)
{
	if (notifyOnResolution != notify)
	{
		notifyOnResolution = notify;
		saveNeeded = true;
	}
}

bool HobEmailAlert::getNotifyHandsFreeDeviceOnFailure() const
{
	return notifyHandsFreeDeviceOnFailure;
}
void HobEmailAlert::setNotifyHandsFreeDeviceOnFailure(bool notify)
{
	if (notifyHandsFreeDeviceOnFailure != notify)
	{
		notifyHandsFreeDeviceOnFailure = notify;
		saveNeeded = true;
	}
}

bool HobEmailAlert::getNotifyHandsFreeDeviceOnResolution() const
{
	return notifyHandsFreeDeviceOnResolution;
}
void HobEmailAlert::setNotifyHandsFreeDeviceOnResolution(bool notify)
{
	if (notifyHandsFreeDeviceOnResolution != notify)
	{
		notifyHandsFreeDeviceOnResolution = notify;
		saveNeeded = true;
	}
}

int HobEmailAlert::getDelay() const
{
	return delayTime;
}
void HobEmailAlert::setDelay(int delay)
{
	if (delayTime != delay)
	{
		delayTime = delay;
		saveNeeded = true;
	}
}

bool HobEmailAlert::getFailureSent() const
{
	return sentFailure;
}
void HobEmailAlert::setFailureSent(bool sent)
{
	if (sentFailure != sent)
	{
		sentFailure = sent;
		saveNeeded = true;
	}
}
