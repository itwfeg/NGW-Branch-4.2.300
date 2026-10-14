/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLCountry.h"
#include "COOLTags.h"
#include "COOLCategoryCountry.h"

#include "HfcError.h"
#include "Configuration.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolCountry.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryCountry.h"

#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLCountry::dataType = 0;
std::string COOLCountry::dataTypeName("COOLCountry"); 
#endif

std::list<COOLCountryFieldOffsets> COOLCountry::browserFieldList,
												COOLCountry::evolFieldList,
												COOLCountry::ultimaFieldList;


COOLCountry::COOLCountry()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLCountry", "Constructor");
	init();
}

COOLCountry::COOLCountry(const COOLCountry *countryRec)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("COOLCountry", "Constructor passing in COOLCountry*");
	init();

	if (countryRec)
	{
		setDeptNum(countryRec->getDeptNum() );
		setName1(countryRec->getName1());
		setName2(countryRec->getName2());
		setAllCategoriesFlag(countryRec->isAllCategoriesFlag());
		setCategoryNum(countryRec->getCategoryNum());
		setCountryNum(countryRec->getCountryNum());
	}
}

COOLCountry::COOLCountry(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCountry");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "Constructor passing in data, len, formatType = ";

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			if (traceOn)
				msgDebug.append("BrowserFormat");

			HobString temp(data, len);
			std::list<COOLCountryFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while (tok.next(&token) != -1 && fieldIter != browserFieldList.end())
			{
				offset = *fieldIter;
				if (token.length())
				{
					setReceived(offset, true);
					if (offset == fieldOffsetCOOLCountryName1 || offset == fieldOffsetCOOLCountryName2)
						token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the Class info, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLCountryDeptNum))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetCOOLCountryDeptNum, true);
			}

			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			if (traceOn)
				msgDebug.append(formatType == EvolutionTaggedFormat ? "EvolutionTaggedFormat" : (formatType == DBIndependentFormatFormat ? "DBIndependentFormatFormat" : "UltimaTaggedFormat"));

			COOLCountryTags coolTags;

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
					offset = (temp.size()) ? coolTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());

						if (offset == fieldOffsetCOOLCountryCategory && !isReceived(fieldOffsetCOOLCountryAllCategoriesFlag))
						{
							setReceived(fieldOffsetCOOLCountryAllCategoriesFlag, true);
							allCategoriesFlag = (categoryNum == 0) ? true : false;
						}
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLCountryDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLCountryDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			if (traceOn)
				msgDebug.append(Integer::toString(formatType)).append(" - Unknown!!!");

			// todo - throw exception("invalid format");
			break;
	}
	if (traceOn)
		Debug::trace("COOLCountry", msgDebug);
}

COOLCountry::COOLCountry(DBCoolCountry *dbCOOLCountry, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCountry");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "Constructor passing in DBCoolCountry *";
		Debug::trace("COOLCountry", msgDebug);
		msgDebug.append(" - ");
	}

	init();

	dbAccess = access;

	if (dbCOOLCountry)
	{
		if (traceOn)
			Debug::trace("COOLCountry", msgDebug + "dbrec is not null");
		myDbRecord = dbCOOLCountry;

		if (!dbCOOLCountry->isName1Null())
			name1 = dbCOOLCountry->getName1();

		if (!dbCOOLCountry->isName2Null())
			name2 = dbCOOLCountry->getName2();

		allCategoriesFlag = (dbCOOLCountry->isAllCategoriesNull()) ? false : dbCOOLCountry->getAllCategories();
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug.append("allCategoriesFlag = ").append(Bool::toString(allCategoriesFlag));
			Debug::trace("COOLCountry", msgDebug2);
		}

		if (!dbCOOLCountry->isCountryNumberNull())
			countryNum = Long::valueOf(dbCOOLCountry->getCountryNumber());

		deptNum = Long::valueOf(dbCOOLCountry->getDeptNumber());

		errorCode = RECORD_OK;
	}
//	QUESTION: How do we handle a corrupt COOLCountry record?
}

COOLCountry::~COOLCountry()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLCountry::init()
{
	name1		= "";
	name2		= "";
	deptNum     = 0;
	allCategoriesFlag = true;
	categoryNum	= 0; // was RECORD_NOT_FOUND... we want 0 for the "All" category
	countryNum	= RECORD_NOT_FOUND;
	errorCode	= RECORD_NOT_FOUND;

	dbAccess	= (DBAccess *)NULL;
	myDbRecord	= (DBCoolCountry *)NULL;

	initFieldList();
}

// static
void COOLCountry::initFieldList()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetCOOLCountryName1);
		browserFieldList.push_back(fieldOffsetCOOLCountryName2);
		browserFieldList.push_back(fieldOffsetCOOLCountryCategory);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLCountryDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLCountryName1);
		evolFieldList.push_back(fieldOffsetCOOLCountryName2);
		evolFieldList.push_back(fieldOffsetCOOLCountryAllCategoriesFlag);
		evolFieldList.push_back(fieldOffsetCOOLCountryNum);					//	To retain the country num from an Ultima/Quantum file.

		COOLCountryTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLCountryNum);
		ultimaFieldList.push_back(fieldOffsetCOOLCountryName1);
		ultimaFieldList.push_back(fieldOffsetCOOLCountryCategory);

		COOLCountryTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}

HobRecordNum COOLCountry::getDeptNum() const
{
	return deptNum;
}
void COOLCountry::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}


const HobString &COOLCountry::getName1() const
{
	return name1;
}
void COOLCountry::setName1(const HobString &name)
{
	if (name.length() > 100)
	{
		name1.assign(name, 0, 100);
		Debug::trace("ClipText", "COOLCountry: name1 clipped to [" + name1 + "]");
	}
	else
	{
		name1 = name;
		Debug::trace("ClipText", "COOLCountry: name1 is [" + name1 + "]");
	}
}


const HobString &COOLCountry::getName2() const
{
	return name2;
}
void COOLCountry::setName2(const HobString &name)
{
	if (name.length() > 100)
	{
		name2.assign(name, 0, 100);
		Debug::trace("ClipText", "COOLCountry: name2 clipped to [" + name2 + "]");
	}
	else
	{
		name2 = name;
		Debug::trace("ClipText", "COOLCountry: name2 is [" + name2 + "]");
	}
}


bool COOLCountry::isAllCategoriesFlag() const
{
	return allCategoriesFlag;
}
void COOLCountry::setAllCategoriesFlag(bool flag)
{
	allCategoriesFlag = flag;

	if (allCategoriesFlag)
	{
		categoryNum	= 0;
		Debug::trace("COOLCountry", "setAllCategoriesFlag(true):  Set categoryNum to zero");
	}
	else
	{
		categoryNum	= RECORD_NOT_FOUND;
		Debug::trace("COOLCountry", "setAllCategoriesFlag(false):  Set categoryNum to RECORD_NOT_FOUND");
	}
}


HobRecordNum COOLCountry::getCategoryNum() const
{
	return categoryNum;
}
void COOLCountry::setCategoryNum(HobRecordNum recordNum)
{
	categoryNum = recordNum;

	if (categoryNum == 0)
	{
		allCategoriesFlag = true;
		Debug::trace("COOLCountry", "setCategoryNum(0):  Set allCategoriesFlag to true");
	}
	else
	{
		allCategoriesFlag = false;
		Debug::trace("COOLCountry", "setCategoryNum(!0):  Set allCategoriesFlag to false");
	}
}


HobRecordNum COOLCountry::getCountryNum() const
{
	return countryNum;
}
void COOLCountry::setCountryNum(HobRecordNum num)
{
	countryNum = num;
}


HobErrorCode COOLCountry::getErrorCode() const
{
	return errorCode;
}
void COOLCountry::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


int COOLCountry::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("COOLCountry"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("COOLCountry", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetCOOLCountryName1:
			setName1(token);
			break;
		case fieldOffsetCOOLCountryName2:
			setName2(token);
			break;
		case fieldOffsetCOOLCountryAllCategoriesFlag:
			setAllCategoriesFlag(Bool::valueOf(token));
			break;
		case fieldOffsetCOOLCountryCategory:
			setCategoryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCountryNum:
			setCountryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCountryDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		default:
			break;
	}

	return 0;
}


void COOLCountry::update(const COOLCountry *newCOOLCountry)
{
	if (!newCOOLCountry)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLCountryFieldOffsets)
	{
		if (newCOOLCountry->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLCountryName1:
					setName1(newCOOLCountry->getName1());
					break;
				case fieldOffsetCOOLCountryName2:
					setName2(newCOOLCountry->getName2());
					break;
				case fieldOffsetCOOLCountryAllCategoriesFlag:
					setAllCategoriesFlag(newCOOLCountry->isAllCategoriesFlag());
					break;
				case fieldOffsetCOOLCountryCategory:
					setCategoryNum(newCOOLCountry->getCategoryNum());
					break;
				case fieldOffsetCOOLCountryNum:
					setCountryNum(newCOOLCountry->getCountryNum());
					break;
				case fieldOffsetCOOLCountryDeptNum:
					setDeptNum(newCOOLCountry->getDeptNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLCountry::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCOOLCountryName1:
			result =	name1;
			break;
		case fieldOffsetCOOLCountryName2:
			result =	name2;
			break;
		case fieldOffsetCOOLCountryAllCategoriesFlag:
			result =	Bool::toString(allCategoriesFlag);
			break;
		case fieldOffsetCOOLCountryCategory:
			if (allCategoriesFlag)
				result = "0";
			else if (categoryNum > RECORD_NOT_FOUND)
				result = longtostring(categoryNum);
			break;
		case fieldOffsetCOOLCountryNum:
			if (countryNum > RECORD_NOT_FOUND)
				result = longtostring(countryNum);
			break;
		case fieldOffsetCOOLCountryDeptNum:
			if (deptNum > RECORD_NOT_FOUND)
				result = longtostring(deptNum);
			break;
		default:
			break;
	}
}


const char *COOLCountry::serialize(int format) const
{
	bool traceOn = Debug::isTracing("COOLCountry");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "serialize():  format = ";
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<COOLCountryFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			if (traceOn)
				msgDebug.append("BrowserFormat");

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
				if (*fieldIter == fieldOffsetCOOLCountryName1 || *fieldIter == fieldOffsetCOOLCountryName2)
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			if (traceOn)
				msgDebug.append(format == EvolutionTaggedFormat ? "EvolutionTaggedFormat" : "DBIndependentFormatFormat");

			COOLCountryTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = coolTags.getTag(*fieldIter);
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

		case UltimaTaggedFormat:
		{
			if (traceOn)
				msgDebug.append("UltimaTaggedFormat");

			COOLCountryTags coolTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = ultimaFieldList.begin();
			while (fieldIter != ultimaFieldList.end())
			{
				tag = coolTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, UltimaTaggedFormat);
					if (traceOn)
					{
						HobString msgDebug2 = "tag = ";
						msgDebug2.append(tag);
						msgDebug2.append("  tagString = [").append(fieldData).append("]");
						Debug::trace("COOLCountry", msgDebug2);
					}

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			if (traceOn)
				msgDebug.append(Integer::toString(format)).append(" - Unknown!!!");
			serializedData = HobString("COOLCountry::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	if (traceOn)
		Debug::trace("COOLCountry", msgDebug);
	return serializedData.data();
}


void COOLCountry::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBCoolCountry *COOLCountry::getMyDbRecord()
{
	return myDbRecord;
}


COOLCountry *COOLCountry::findCOOLCountry(const HobString &countryName, HobRecordNum deptNum, DBAccess *dbAccess)
{
	COOLCountry *readCOOLCountry = (COOLCountry *)NULL;

	if (Debug::isTracing("COOLCountry.find"))
	{
		HobString myDebug("Looking for country name: ");
		myDebug.append(countryName);
		Debug::trace("COOLCountry.find", myDebug);
	}

	if (!dbAccess)
		return readCOOLCountry;

	DBCoolCountriesRM	*dbCOOLCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCOOLCountriesRM)
		return readCOOLCountry;

	try
	{
		DBCoolCountry *dbCOOLCountryRec = dbCOOLCountriesRM->findCountry(countryName, longtostring(deptNum) );
		if (dbCOOLCountryRec)
		{
			Debug::trace("COOLCountry.find", "Found country in database.");

			readCOOLCountry = new COOLCountry(dbCOOLCountryRec, dbAccess);
			if (!readCOOLCountry)
				delete dbCOOLCountryRec;
			else
			{
/*
				if (Debug::isTracing("COOLCountry.find"))
				{
					HobString myDebug("COOLCountry object: ");
					myDebug.append(Hex::toString((long) readCOOLCountry);
					myDebug.append("   COOLCountry number: ").append(Long::toString(readCOOLCountry->getRecordNum()));
					myDebug.append("   Dept number:" + longtostring(readCOOLCountry->getDeptNum()));
					Debug::trace("COOLCountry.find", myDebug);
				}
*/
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLCountry::findCOOLCountry() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readCOOLCountry;
}


bool COOLCountry::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLCountry::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCoolCountriesRM	*dbCOOLCountriesRM = dbAccess->getDBCoolCountriesRM();
		if (dbCOOLCountriesRM)
		{
			if (Debug::isTracing("COOLCountry"))
			{
				HobString myDebug("Deleting COOLCountry: ");
				myDebug.append(name1);
				Debug::trace("COOLCountry", myDebug);
			}
			result = dbCOOLCountriesRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLCountry::saveRecord()
{
	bool traceOn = Debug::isTracing("COOLCountry");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "saveRecord():  ";

	if (!dbAccess)
		return;

	DBCoolCountriesRM	*dbCOOLCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCOOLCountriesRM)
		return;

	if (!name1.empty())
	{
		name1.stripChar(_LF_);
		name1.stripChar(_CR_);
	}
	HobString tempName1 = name1;
	tempName1.stripWhiteSpace(HobString::LEADING_TRAILING);

	if (!name2.empty())
	{
		name2.stripChar(_LF_);
		name2.stripChar(_CR_);
	}
	HobString tempName2 = name2;
	tempName2.stripWhiteSpace(HobString::LEADING_TRAILING);

	bool deleteRec = (name1.empty() && name2.empty()) || (tempName1.empty() && tempName2.empty());
	if (deleteRec)
	{
		if (!myDbRecord)
		{
			if (traceOn)
				Debug::trace("COOLCountry", msgDebug + "Nothing to save.");
		}
		else
		{
			if (traceOn)
				Debug::trace("COOLCountry", msgDebug + "Don't have a Country, deleting it.");
			dbCOOLCountriesRM->deleteRecord(myDbRecord);
		}
		return;
	}

	if (!myDbRecord)
	{
		if (traceOn)
			Debug::trace("COOLCountry", msgDebug + "Don't have a db record, creating it...");
		myDbRecord = dbCOOLCountriesRM->createRecord();
	}

	if (myDbRecord)
	{
		if (traceOn)
			Debug::trace("COOLCountry", msgDebug + "Going to set data elements");

		//	Plug the changes back into the dbCOOLCountry Record
		if (name1.empty())
			myDbRecord->setName1Null();
		else
			myDbRecord->setName1(name1);
		if (name2.empty())
			myDbRecord->setName2Null();
		else
			myDbRecord->setName2(name2);

		myDbRecord->setAllCategories(allCategoriesFlag);
		myDbRecord->setDeptNumber(Long::toString(deptNum));

		if (countryNum > 0)
			myDbRecord->setCountryNumber(Long::toString(countryNum));

		try
		{
			dbCOOLCountriesRM->saveRecord(myDbRecord);		//save record to the database
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "COOLCountry::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}

		//	Now that the COOLCountry record is saved, check the Category number
		if (!allCategoriesFlag && categoryNum > 0)
		{
			DBCoolCategoryCountriesRM	*dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
			if (dbCoolCategoryCountriesRM)
			{
				DBCoolCategoryCountry *dbCoolCategoryCountry = dbCoolCategoryCountriesRM->find(Long::toString(categoryNum), myDbRecord);
				if (!dbCoolCategoryCountry)
				{
					if (traceOn)
						Debug::trace("COOLCountry", msgDebug + "Don't have a COOLCategoryCountry db record - creating it.");

					dbCoolCategoryCountry = dbCoolCategoryCountriesRM->createRecord(Long::toString(categoryNum), myDbRecord);
				}
				if (dbCoolCategoryCountry)
				{
					if (traceOn)
						Debug::trace("COOLCountry", msgDebug + "Saving the COOLCategoryCountry db record.");

					try
					{
						dbCoolCategoryCountry->setCategoryNumber(Long::toString(categoryNum));
						dbCoolCategoryCountriesRM->saveRecord(dbCoolCategoryCountry);		//	Save the record to the database.
					}
					catch (DBException &dbe)
					{
						Debug::trace("DBAccess.exception", "dbCoolCategoryCountriesRM->saveRecord() in COOLCountry::saveRecord()  ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
				}
			}
		}
		if (traceOn)
			Debug::trace("COOLCountry", msgDebug + "Saved to the database");
	}
}
