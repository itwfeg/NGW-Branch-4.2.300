/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobOperator.h"
#include "HobOperatorTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBOperatorsRM.h"
#include "DBOperator.h"
#include "DBQuery.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "EvolMessage.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobOperator::dataType = 0;
std::string HobOperator::dataTypeName("HobOperator"); 
#endif

std::list<HobOperatorFieldOffsets> HobOperator::evolFieldList;
std::list<HobOperatorFieldOffsets> HobOperator::dbindependentFieldList;


HobOperator::HobOperator()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

#if defined(LINUX_FIX)
HobOperator::HobOperator(const HobOperator &source)
    : HfcObject()
#else
HobOperator::HobOperator(const HobOperator &source)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	opNum = source.getOpNum();
	opRFID = source.getOpRFID();
	opName = source.getOpName();
	accessLevel = source.getAccessLevel();
	opPLang = source.getOpLang();
	sawNote = source.hasSeenNote();
	selectedLookupMethod = source.getSelectedLookupMethod();
	loggedIn = source.isLoggedIn();
	loginMethod = source.getLoginMethod();

	//	this shouldn't be being used when a renumber is occurring, OperatorTotal * lnkOperatorTotal;
	//	this shouldn't be being used when a renumber is occurring, OperatorChange * lnkOperatorChange;

	currentMode = source.getCurrentMode();
	errorCode = source.getErrorCode();

	// don't set DBAccess *dbAccess;
	// don't set edb::DBOperator *myDbRecord;
}

HobOperator::HobOperator(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	int offset = 0;

	switch (formatType)
	{
		// InternalFormat will use the same format as the Browser format for the HobOperator contained within the message.
		case InternalFormat:
			offset = EvolMessage::getEvolMessageLengthInBuffer();
			// let this fall through to the BrowserFormat case directly below!!!
		case BrowserFormat:
		{
			HobString temp(data + offset, len - offset);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short offset = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(offset, (token.length() == 0) ? false : true);
				updateField(offset, token, BrowserFormat);
				offset++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			HobOperatorTags hobOperatorTags; 	

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? hobOperatorTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData(), formatType);
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

HobOperator::HobOperator(DBOperator *dbOperator, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("HobOperator", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbOperator)
	{
		Debug::trace("HobOperator", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbOperator;

		// operator number in the db can't be null, it is the primary key
		opNum = dbOperator->getNumber();

		if (!dbOperator->isNameNull())
			opName = dbOperator->getName();
		if (!dbOperator->isLanguageNull())
			opPLang = dbOperator->getLanguage();
		if (!dbOperator->isRFIDNull())
			opRFID = dbOperator->getRFID();
		if (!dbOperator->isClassificationNull())
			accessLevel = dbOperator->getClassification();
		if (!dbOperator->isSawNoteNull())
			sawNote = dbOperator->getSawNote();

		/* pre-db
		if (dbOperator->getName().size() > 15)
		{
			opRFID = dbOperator->getName().substr(0, 16);
			if (dbOperator->getName().size() > 16)
				opName = dbOperator->getName().substr(16);
		}
		else
			opName = dbOperator->getName();
		*/
		errorCode = RECORD_OK;
	}
}


HobOperator::~HobOperator()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("HobOperator.Delete", HobString("    HobOperator: ") + HobString(Hex::toString((long long) this)) + HobString("  HobOperator number:") + opNum);
}


// private
void HobOperator::init()
{
	opNum = "";
	opRFID = "";  // pre-db "0000000000000000";
	opName = "";
	opPLang = "1";
	accessLevel = 1;
	sawNote = false;
	//selectedLookupMethod = 0; /*PluLookupMethod_Classic*/
	loggedIn = false;
	loginMethod = LOGINMETHOD_OPNUM;

	dbAccess = 0;
	myDbRecord = 0;

	currentMode = 0;
	errorCode = RECORD_NOT_FOUND;

	initFieldList();
}

// static
void HobOperator::initFieldList()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetOpNum);
		evolFieldList.push_back(fieldOffsetOpRFID);
		evolFieldList.push_back(fieldOffsetOpName);
		evolFieldList.push_back(fieldOffsetAccessLevel);
		evolFieldList.push_back(fieldOffsetOpLang);
		evolFieldList.push_back(fieldOffsetSawNote);

		HobOperatorTags hobOperatorTags((FieldOffsetsList *)&evolFieldList);
	}

	if (dbindependentFieldList.empty())
	{
		dbindependentFieldList.push_back(fieldOffsetOpNum);
		dbindependentFieldList.push_back(fieldOffsetOpRFID);
		dbindependentFieldList.push_back(fieldOffsetOpName);
		dbindependentFieldList.push_back(fieldOffsetAccessLevel);
		dbindependentFieldList.push_back(fieldOffsetOpLang);
		dbindependentFieldList.push_back(fieldOffsetSawNote);

		HobOperatorTags hobOperatorTags((FieldOffsetsList *)&dbindependentFieldList);
	}
}


const HobString &HobOperator::getOpNum() const
{
	return opNum;
}
void HobOperator::setOpNum(const HobString &num)
{
	opNum = num;
}

const HobString &HobOperator::getOpRFID() const
{
	return opRFID;
}
void HobOperator::setOpRFID(const HobString &rfid)
{
	opRFID = rfid;
}

const HobString &HobOperator::getOpName() const
{
	return opName;
}
void HobOperator::setOpName(const HobString &name)
{
	opName = name;
}
const HobString &HobOperator::getOpLang() const
{
	return opPLang;
}
void HobOperator::setOpLang(const HobString &language)
{
	opPLang = language;
}

short HobOperator::getAccessLevel() const
{
	return accessLevel;
}
void HobOperator::setAccessLevel(short level)
{
	accessLevel = (level > 0 && level < 10) ? level : 1;
}

bool HobOperator::hasSeenNote() const
{
	return sawNote;
}
void HobOperator::setSawNote(bool saw)
{
	sawNote = saw;
}


short HobOperator::getSelectedLookupMethod() const
{
	return selectedLookupMethod;
}
void HobOperator::setSelectedLookupMethod(short selectedLookupMethod)
{
	this->selectedLookupMethod = selectedLookupMethod;
}


bool HobOperator::isLoggedIn() const
{
	return loggedIn;
}
void HobOperator::setLoggedIn(bool loggedIn)
{
	this->loggedIn = loggedIn;
}


/* Not stored in the database.  The Browser indicates into which mode the operator is logging in. */
short HobOperator::getCurrentMode() const
{
	return currentMode;
}
void HobOperator::setCurrentMode(short currentMode)
{
	this->currentMode	= currentMode;
}
void HobOperator::setCurrentMode(OpModeEvent mode)
{
	switch (mode)
	{
		case OpModeOperate:
		case OpModeAutoWeigh:
		case OpModeSemiWeigh:
		case OpModeManualWeigh:
		case OpModeWrapOnly:
		case OpModeAutoManualApply:
		case OpModeProductionEntry:
		case OpModeAccumulateItems:
		case OpModeBuildAMeal:
			currentMode = CURRENTMODE_OPERATE;
			break;

		case OpModeSupervisor:
			currentMode = CURRENTMODE_SUPERVISOR;
			break;

		case OpModeService:
			currentMode = CURRENTMODE_SERVICE;
			break;

		default:
			currentMode = CURRENTMODE_UNKNOWN;
			break;
	}
}

short HobOperator::getLoginMethod() const
{
	return loginMethod;
}
void HobOperator::setLoginMethod(short method)
{
	loginMethod = method;
}



HobErrorCode HobOperator::getErrorCode() const
{
	return errorCode;
}
void HobOperator::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


void HobOperator::validate()
{
}

void HobOperator::setWhatOperatorCanModify()
{
}

bool HobOperator::isAllowed(int /* function */) const
{
	return true;
}


/*****************************************************************************************
 *
 * updateField: Update an attribute for the field indicated by the fieldCode passed in.
 *              The data for the field is passed in as a string.
 *
 ****************************************************************************************/
int HobOperator::updateField(int fieldCode, const HobString &token, int formatType)
{
	bool traceOn = Debug::isTracing("HobOperator.updateField");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "updateField():  ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("fieldCode: ").append(Integer::toString(fieldCode));
		msgDebug2.append("  data: ").append(token);
		msgDebug2.append("  formatType: ").append(Integer::toString(formatType));
		Debug::trace("HobOperator.updateField", msgDebug2);
	}
	int resultCode = 0;

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetOpNum:
		 	setOpNum(token);
			break;	
		case fieldOffsetOpRFID:
		{
			HobString temp = token;
			temp.unQuote();
		 	setOpRFID(temp);
			break;
		}
		case fieldOffsetMode:
			setCurrentMode((short)Integer::valueOf(token));
			break;
		case fieldOffsetOpName: 
		{
			HobString temp = token;
			temp.unQuoteForCSV();
			setOpName(temp);
			break;
		}	
		case fieldOffsetAccessLevel:
			setAccessLevel(Integer::valueOf(token));
			break;
		case fieldOffsetOpLang:
		 	setOpLang(token);
			break;
		case fieldOffsetSawNote:
			setSawNote(Bool::valueOf(token));
			break;
//		case fieldOffsetSelectedLookupMethod:
//			setSelectedLookupMethod(Integer::valueOf(token.c_str()));
//			break;
		default:
			resultCode = -1;
			break;
	}

	return resultCode;
}


void HobOperator::updateOperator(HobOperator *newOperator)
{
	if (!newOperator)
		return;

	short loopCnt = 1;
	while (loopCnt < endHobOperatorFieldOffsets)
	{
		if(newOperator->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetOpNum:
					setOpNum(newOperator->getOpNum());
					break;
				case fieldOffsetOpRFID:
					setOpRFID(newOperator->getOpRFID());
					break;
				case fieldOffsetOpName:
					setOpName(newOperator->getOpName());
					break;
				case fieldOffsetAccessLevel:
					setAccessLevel(newOperator->getAccessLevel());
					break;
				case fieldOffsetOpLang:
					setOpLang(newOperator->getOpLang());
					break;
				case fieldOffsetSawNote:
					setSawNote(newOperator->hasSeenNote());
					break;
				case fieldOffsetMode:
					setCurrentMode(newOperator->getCurrentMode());
					break;
//				case fieldOffsetSelectedLookupMethod:
//					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void HobOperator::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetOpNum:
			result = opNum;
			break;
		case fieldOffsetOpRFID:
			result = opRFID;
			break;
		case fieldOffsetOpName:
			result = opName;
			break;
		case fieldOffsetAccessLevel:
			result = Integer::toString(accessLevel);
			break;
		case fieldOffsetOpLang:
			result = opPLang;
			break;
		case fieldOffsetSawNote:
			result = Bool::toString(sawNote);
			break;
		case fieldOffsetMode:
			result = Integer::toString(currentMode);
			break;
//		case fieldOffsetSelectedLookupMethod:
//			break;
		default:
			break;
	}
}


const char *HobOperator::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		// InternalFormat will use the same format as the Browser format for the HobOperator contained within the message.
		case InternalFormat:
		case BrowserFormat:
		{
			HobString fieldData = "";
			for (int loopCnt = 1; loopCnt < endHobOperatorFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(loopCnt, fieldData, BrowserFormat);

				if (loopCnt == fieldOffsetOpName)
					fieldData.quoteForCSV();
				serializedData.append(fieldData);
			}
			break;
		}

		case SynchronizationTaggedFormat:
/*		{
			HobString tag, fieldData;
			HobOperatorTags hobOperatorTags;

			std::list<HobOperatorFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while ( fieldIter != evolFieldList.end() )
			{
				if ( isReceived(*fieldIter) )
				{
					tag = hobOperatorTags.getTag(*fieldIter);
					if ( tag.size() )
					{
						getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

						serializedData.append(tag); 
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);	
					}
				}

				fieldIter++;
			}

			break;
		}
*/
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		{
			HobOperatorTags hobOperatorTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<HobOperatorFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = hobOperatorTags.getTag(*fieldIter);
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

		case DBIndependentFormatFormat:
		{
			HobOperatorTags hobOperatorTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<HobOperatorFieldOffsets>::iterator fieldIter = dbindependentFieldList.begin();
			while (fieldIter != dbindependentFieldList.end())
			{
				tag = hobOperatorTags.getTag(*fieldIter);
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

		case OperatorMaintenanceFormat:
		{
			HobString fieldData = "";
			for (int loopCnt = 1; loopCnt < fieldOffsetSawNote; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(loopCnt, fieldData, BrowserFormat);

				if (loopCnt == fieldOffsetOpName)
					fieldData.quoteForCSV();
				serializedData.append(fieldData);
			}
			break;
		}

		default:
			serializedData = HobString("HobOperator::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void HobOperator::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBOperator *HobOperator::getMyDbRecord()
{
	return myDbRecord;
}


HobOperator *HobOperator::find(const HobString &opNum, DBAccess *dbAccess)
{
	HobOperator *readOper = (HobOperator *)NULL;

	if (opNum.size() == 0 || !dbAccess)
		return readOper;

	DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
	if (!dbOperatorsRM)
		return readOper;

	try
	{
		DBOperator *dbOpRec = dbOperatorsRM->findNumber(opNum);
		if (dbOpRec)
		{
			readOper = new HobOperator(dbOpRec, dbAccess);
			if (!readOper)
				delete dbOpRec;
			else
			{
				if (Debug::isTracing("HobOperator"))
					Debug::trace("HobOperator", HobString("find() - HobOperator: ") + HobString(Hex::toString((long long) readOper)) +
														HobString("  opnum:") + opNum +
														HobString("  record:") + HobString(readOper->toString()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("HobOperator::find() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readOper;
}


HobOperator *HobOperator::findRFID(const HobString &rfid, DBAccess *dbAccess)
{
	HobOperator *readOper = (HobOperator *)NULL;

	if (rfid.size() == 0 || !dbAccess)
		return readOper;

	DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
	if (!dbOperatorsRM)
		return readOper;

	try
	{
		DBOperator *dbOpRec = dbOperatorsRM->findRFID(rfid);
		if (dbOpRec)
		{
			readOper = new HobOperator(dbOpRec, dbAccess);
			if (!readOper)
				delete dbOpRec;
			else
			{
				if (Debug::isTracing("HobOperator"))
					Debug::trace("HobOperator", HobString("findRFID() - HobOperator: ") + HobString(Hex::toString((long long) readOper)) +
														HobString("  opRFID:") + rfid +
														HobString("  record:") + HobString(readOper->toString()));
			}
		}

		/* pre-db
		DBQuery<DBOperator> *operatorsQuery = dbOperatorsRM->retrieveAll();
		if (operatorsQuery)
		{
			DBQuery<DBOperator>::const_iterator operatorsIter = operatorsQuery->begin();
			while (operatorsIter != operatorsQuery->end())
			{
				DBOperator *op = new DBOperator(*operatorsIter);
				if (op && !op->isNameNull() && op->getName().size() > 15 && op->getName().substr(0,16) == opRFID)
				{
					Debug::trace("HobOperator", HobString("findRFID() - found existing rfid in operator table ") +
															 HobString(op->getNumber()));
					readOper = new HobOperator(op, dbAccess);
					if (!readOper)
						delete op;
					else
					{
						if (Debug::isTracing("HobOperator"))
							Debug::trace("HobOperator", HobString("findRFID() - HobOperator: ") + HobString(Hex::toString((long) readOper)) +
																HobString("  opRFID:") + opRFID +
																HobString("  record:") + HobString(readOper->toString()));
						break;
					}
				}
				else if (op)
				{
					Debug::trace("HobOperator", HobString("findRFID() - not the right operator ") + HobString(op->getNumber()));
					delete op;
				}
				++operatorsIter;
			}
			delete operatorsQuery;
		}
		*/
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("HobOperator::findRFID() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readOper;
}


bool HobOperator::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int HobOperator::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
		if (dbOperatorsRM)
		{
			Debug::trace("HobOperator", HobString("Deleting Operator# ") + opNum);
			result = dbOperatorsRM->deleteRecord(myDbRecord);	
		}
	}
	return result;							
}

void HobOperator::saveRecord()
{
	if (opNum.size() > 0 && dbAccess)
	{
		DBOperatorsRM *dbOperatorsRM = dbAccess->getDBOperatorsRM();
		if (!dbOperatorsRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("HobOperator", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbOperatorsRM->createRecord();
		}

		if (myDbRecord)
		{
			// todo - May also want to check if the record has changed before saving to the database.

			//	Plug the changes back into the dbOperatorRecord

			// No longer a valid check for HT scales - with hands-free devices
			/*
			if (opNum.size() > 16)
				myDbRecord->setNumber(opNum.substr(0,16));
			else
			*/
				myDbRecord->setNumber(opNum);

			// pre-db  myDbRecord->setName(opRFID.substr(0, 16) + opName);

			if (opName.size() == 0)
				myDbRecord->setNameNull();
			else
				myDbRecord->setName(opName);

			if (opPLang.size() == 0)
				myDbRecord->setLanguageNull();
			else
				myDbRecord->setLanguage(opPLang);
			
			if (opRFID.size() == 0)
				myDbRecord->setRFIDNull();
			// No longer a valid check for HT scales - with hands-free devices
			/*
			else if (opRFID.size() > 16)
				myDbRecord->setRFID(opRFID.substr(0,16));
			*/
			else
				myDbRecord->setRFID(opRFID);

			myDbRecord->setClassification(accessLevel);
			myDbRecord->setSawNote(sawNote);

			Debug::trace("HobOperator", HobString("Saving ... sawNote:") + HobString(Bool::toString(sawNote)));

			try
			{
				dbOperatorsRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "HobOperator::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}
