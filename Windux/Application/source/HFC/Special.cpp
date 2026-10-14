/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Special.h"
#include "SpecialsTags.h"
#include "Configuration.h"
#include "filesys/filename.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBSpecialProgramsRM.h"
#include "DBSpecialProgram.h"

#include "MultiLanguageText.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Special::dataType = 0;
string Special::dataTypeName("Special"); 
#endif

list<SpecialFieldOffsets> Special::uiFieldList;
list<SpecialFieldOffsets> Special::evolFieldList;


Special::Special()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Special::Special(const char *data, int len, int formatType)
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
			std::list<SpecialFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSpecialDeptNum))
			{
				setReceived(fieldOffsetSpecialDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			SpecialsTags specialsTags;

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
					offset = (temp.size()) ? specialsTags.getOffset(temp) : 0;
					if (Debug::isTracing("Special.ctor"))
						Debug::trace("Special.ctor", string("Special(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSpecialDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetSpecialDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

Special::Special(DBSpecialProgram *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Special.ctor", string("Special(DBSpecialProgram, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum            = Long::valueOf(dbRec->getDeptNumber());
		specialNum         = Long::valueOf(dbRec->getNumber());
		specialDescription = dbRec->getDescription();
		imageName1         = dbRec->getImage1();
		imageName2         = dbRec->getImage2();
		defaultLength      = dbRec->getDefaultLength();
		errorCode          = RECORD_OK;
	}
}


Special::~Special()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("Special.dtor", "    Special: " + ptrtostring(this) + "  num " + longtostring(specialNum) + "  deptNum " + longtostring(deptNum));
}


//	private
void Special::init()
{
	deptNum = 0;
	specialNum = (HobRecordNum) RECORD_NOT_FOUND;
	specialDescription = "";
	imageName1 = "";
	imageName2 = "";
	defaultLength = 0;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBSpecialProgram *)NULL;

	errorCode = RECORD_NOT_FOUND;

	initFieldLists();
}

// static
void Special::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetSpecialNum);
		uiFieldList.push_back(fieldOffsetSpecialDescription);
		uiFieldList.push_back(fieldOffsetSpecialImageName1);
		uiFieldList.push_back(fieldOffsetSpecialImageName2);
		uiFieldList.push_back(fieldOffsetSpecialDefaultLength);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetSpecialDeptNum);
		evolFieldList.push_back(fieldOffsetSpecialNum);
		evolFieldList.push_back(fieldOffsetSpecialDescription);
		evolFieldList.push_back(fieldOffsetSpecialImageName1);
		evolFieldList.push_back(fieldOffsetSpecialImageName2);
		evolFieldList.push_back(fieldOffsetSpecialDefaultLength);

		SpecialsTags specialsTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum Special::getDeptNum() const
{
	return deptNum;
}
void Special::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum Special::getSpecialNum() const
{
	return specialNum;
}
void Special::setSpecialNum(HobRecordNum num)
{
	specialNum = num;
}

const HobString &Special::getSpecialDescription() const
{
	return specialDescription;
}
void Special::setSpecialDescription(const HobString &name)
{
	specialDescription = name;
}

const HobString &Special::getImageName1() const
{
	return imageName1;
}
void Special::setImageName1(const HobString &name)
{
	imageName1 = name;
}

const HobString &Special::getImageName2() const
{
	return imageName2;
}
void Special::setImageName2(const HobString &name)
{
	imageName2 = name;
}

int Special::getDefaultLength() const
{
	return defaultLength;
}
void Special::setDefaultLength(int length)
{
	defaultLength = length;
}

HobErrorCode Special::getErrorCode() const
{
	return errorCode;
}
void Special::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


// Returns true if the field was updated, false if not
bool Special::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("Special"))
		Debug::trace("Special", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetSpecialDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetSpecialNum:
			setSpecialNum(Long::valueOf(newValue));
			break;
		case fieldOffsetSpecialDescription:
			setSpecialDescription(newValue);
			break;
		case fieldOffsetSpecialImageName1:
			setImageName1(newValue);
			break;
		case fieldOffsetSpecialImageName2:
			setImageName2(newValue);
			break;
		case fieldOffsetSpecialDefaultLength:
			setDefaultLength(Integer::valueOf(newValue));
			break;
		default:
			break;
	}

	return true;
}

void Special::updateRecord(const Special *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("Special"))
	{
		Debug::trace("Special", string("updateRecord() - this num ") + longtostring(specialNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("Special", string("updateRecord() - passed in num ") + longtostring(newData->getSpecialNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endSpecialFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetSpecialDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetSpecialNum:
					setSpecialNum(newData->getSpecialNum());
					break;
				case fieldOffsetSpecialDescription:
					setSpecialDescription(newData->getSpecialDescription());
					break;
				case fieldOffsetSpecialImageName1:
					setImageName1(newData->getImageName1());
					break;
				case fieldOffsetSpecialImageName2:
					setImageName2(newData->getImageName2());
					break;
				case fieldOffsetSpecialDefaultLength:
					setDefaultLength(newData->getDefaultLength());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool Special::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetSpecialDescription:
		case fieldOffsetSpecialImageName1:
		case fieldOffsetSpecialImageName2:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void Special::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetSpecialDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetSpecialNum:
			result = longtostring(specialNum);
			break;
		case fieldOffsetSpecialDescription:
			result = specialDescription;
			break;
		case fieldOffsetSpecialImageName1:
		{
			if (format == BrowserFormat)
			{
				HobString tempName(imageName1);
				Filename file1(tempName);
				if (!tempName.empty() && file1.getFileNameExtension().empty())
					tempName.append(Configuration::getInstance()->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
			}
			else
				result = imageName1;
			break;
		}
		case fieldOffsetSpecialImageName2:
		{
			if (format == BrowserFormat)
			{
				HobString tempName(imageName2);
				Filename file1(tempName);
				if (!tempName.empty() && file1.getFileNameExtension().empty())
					tempName.append(Configuration::getInstance()->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
			}
			else
				result = imageName2;
			break;
		}
		case fieldOffsetSpecialDefaultLength:
			result = inttostring(defaultLength);
			break;
		default:
			break;
	}

	if (Debug::isTracing("Special.getFieldAsString"))
		Debug::trace("Special.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *Special::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<SpecialFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = uiFieldList.begin();
			while (fieldIter != uiFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, format);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			SpecialsTags specialsTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<SpecialFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = specialsTags.getTag(*fieldIter);
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
			serializedData = string("Special::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


void Special::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool Special::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Special::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBSpecialProgramsRM *dbRM = dbAccess->getDBSpecialsRM();
		if (dbRM)
		{
			Debug::trace("Special", string("deleteRecord() - num ") + longtostring(specialNum) + string("  deptNum ") + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void Special::saveRecord()
{
	if (specialNum > 0 && dbAccess)
	{
		DBSpecialProgramsRM *dbRM = dbAccess->getDBSpecialsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Special", string("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the DBSpecialProgram record
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(specialNum));
			myDbRecord->setDescription(specialDescription);
			myDbRecord->setImage1(imageName1);
			myDbRecord->setImage2(imageName2);
			myDbRecord->setDefaultLength(defaultLength);

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("Special", string("saveRecord() - saved num ") + longtostring(specialNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Special::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
Special *Special::findRecord(HobRecordNum specialNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	Special *rec = 0;

	// DeptNum can be 0, so leave next check as < 0
	if (specialNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBSpecialProgramsRM *dbRM = dbAccess->getDBSpecialsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBSpecialProgram *dbRec = dbRM->findNumber(longtostring(specialNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new Special(dbRec, dbAccess);
			if (!rec)
				delete rec;
			else
			{
				if (Debug::isTracing("Special"))
					Debug::trace("Special", "findRecord() - rec ptr " + ptrtostring(rec) + "  num " + longtostring(rec->getSpecialNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Special::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


// static
void Special::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("Special", "verifyReservedRecords - no dbAccess passed in!");
		return;
	}

	bool traceOn = Debug::isTracing("Special");
	HobString msgDebug = "verifyReservedRecords() - Created Special ";
	if (traceOn)
		Debug::trace("Special", "verifying all reserved Specials ...");

	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();

	HobString deptNumStr = "0";
	long deptNum = 0;

	Special *reservedSpecial = (Special *)NULL;
	hfc::MultiLanguageText internationalize;

	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		reservedSpecial = Special::findRecord((HobRecordNum)999990, deptNum, dbAccess);
		if (!reservedSpecial)
		{
			reservedSpecial = new Special();
			if (reservedSpecial)
			{
				reservedSpecial->setDeptNum(deptNum);
				reservedSpecial->setSpecialNum((HobRecordNum)999990);
				reservedSpecial->setSpecialDescription(internationalize("Specials.NewProducts"));
				reservedSpecial->setImageName1("new");
				reservedSpecial->setImageName2("new");
				reservedSpecial->setDefaultLength(30);
				reservedSpecial->setDBAccess(dbAccess);
				reservedSpecial->saveRecord();
				if (traceOn)
					Debug::trace("Special", msgDebug + "999990  deptNum " + deptNumStr);
			}
		}
		if (reservedSpecial)
		{
			delete reservedSpecial;
			reservedSpecial = (Special *)NULL;
		}

		reservedSpecial = Special::findRecord((HobRecordNum)999991, deptNum, dbAccess);
		if (!reservedSpecial)
		{
			reservedSpecial = new Special();
			if (reservedSpecial)
			{
				reservedSpecial->setDeptNum(deptNum);
				reservedSpecial->setSpecialNum((HobRecordNum)999991);
				reservedSpecial->setSpecialDescription(internationalize("Specials.PriceReductions"));
				reservedSpecial->setImageName1("sale");
				reservedSpecial->setImageName2("sale");
				reservedSpecial->setDefaultLength(30);
				reservedSpecial->setDBAccess(dbAccess);
				reservedSpecial->saveRecord();
				if (traceOn)
					Debug::trace("Special", msgDebug + "999991  deptNum " + deptNumStr);
			}
		}
		if (reservedSpecial)
		{
			delete reservedSpecial;
			reservedSpecial = (Special *)NULL;
		}


#if 0	//	Switched out on Nov. 1, 2005 per Dave Miller (PT # 2190)
		reservedSpecial = Special::findRecord((HobRecordNum)999992, deptNum, dbAccess);
		if (!reservedSpecial)
		{
			reservedSpecial = new Special();
			if (reservedSpecial)
			{
				reservedSpecial->setDeptNum(deptNum);
				reservedSpecial->setSpecialNum((HobRecordNum)999992);
				reservedSpecial->setSpecialDescription("More Quantity");
				reservedSpecial->setImageName1("");
				reservedSpecial->setImageName2("");
				reservedSpecial->setDefaultLength(30);
				reservedSpecial->setDBAccess(dbAccess);
				reservedSpecial->saveRecord();
				if (traceOn)
					Debug::trace("Special", msgDebug + "999992  deptNum " + deptNumStr);
			}
		}
		if (reservedSpecial)
		{
			delete reservedSpecial;
			reservedSpecial = (Special *)NULL;
		}

		reservedSpecial = Special::findRecord((HobRecordNum)999993, deptNum, dbAccess);
		if (!reservedSpecial)
		{
			reservedSpecial = new Special();
			if (reservedSpecial)
			{
				reservedSpecial->setDeptNum(deptNum);
				reservedSpecial->setSpecialNum((HobRecordNum)999993);
				reservedSpecial->setSpecialDescription("More Points");
				reservedSpecial->setImageName1("");
				reservedSpecial->setImageName2("");
				reservedSpecial->setDefaultLength(30);
				reservedSpecial->setDBAccess(dbAccess);
				reservedSpecial->saveRecord();
				if (traceOn)
					Debug::trace("Special", msgDebug + "999993  deptNum " + deptNumStr);
			}
		}
		if (reservedSpecial)
		{
			delete reservedSpecial;
			reservedSpecial = (Special *)NULL;
		}

		reservedSpecial = Special::findRecord((HobRecordNum)999994, deptNum, dbAccess);
		if (!reservedSpecial)
		{
			reservedSpecial = new Special();
			if (reservedSpecial)
			{
				reservedSpecial->setDeptNum(deptNum);
				reservedSpecial->setSpecialNum((HobRecordNum)999994);
				reservedSpecial->setSpecialDescription("FS New Products");
				reservedSpecial->setImageName1("");
				reservedSpecial->setImageName2("");
				reservedSpecial->setDefaultLength(30);
				reservedSpecial->setDBAccess(dbAccess);
				reservedSpecial->saveRecord();
				if (traceOn)
					Debug::trace("Special", msgDebug + "999994  deptNum " + deptNumStr);
			}
		}
		if (reservedSpecial)
		{
			delete reservedSpecial;
			reservedSpecial = (Special *)NULL;
		}

		reservedSpecial = Special::findRecord((HobRecordNum)999995, deptNum, dbAccess);
		if (!reservedSpecial)
		{
			reservedSpecial = new Special();
			if (reservedSpecial)
			{
				reservedSpecial->setDeptNum(deptNum);
				reservedSpecial->setSpecialNum((HobRecordNum)999995);
				reservedSpecial->setSpecialDescription("FS Price Reductions");
				reservedSpecial->setImageName1("");
				reservedSpecial->setImageName2("");
				reservedSpecial->setDefaultLength(30);
				reservedSpecial->setDBAccess(dbAccess);
				reservedSpecial->saveRecord();
				if (traceOn)
					Debug::trace("Special", msgDebug + "999995  deptNum " + deptNumStr);
			}
		}
		if (reservedSpecial)
		{
			delete reservedSpecial;
			reservedSpecial = (Special *)NULL;
		}
#endif

		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically created the reserved records for dept 0 in the first pass through the while loop (since deptNumStr was initialized to "0").
		// If the database doesn't contain the default dept record, we still need the reserved records for the default dept (Dept 0).
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}
	}
}
