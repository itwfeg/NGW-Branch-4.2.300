/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLCategoryCountry.h"
#include "COOLCountry.h"
#include "COOLTags.h"
#include "HfcError.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryCountry.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolCountry.h"

#include "Debug.h"

#include "Configuration.h"
#include "e_string/utilstring.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace util_string;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLCategoryCountry::dataType = 0;
std::string COOLCategoryCountry::dataTypeName("COOLCategoryCountry"); 
#endif

std::list<COOLCategoryCountryFieldOffsets> COOLCategoryCountry::evolFieldList, COOLCategoryCountry::ultimaFieldList;


COOLCategoryCountry::COOLCategoryCountry(DBCoolCategoryCountry *dbCoolCategoryCountryRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCategoryCountry");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "Constructor passing in DBCoolCategoryCountry* & DBAccess*";
		Debug::trace("COOLCategoryCountry", msgDebug);
		msgDebug.append(":  ");
	}
	init();

	if (!dbCoolCategoryCountryRec || !access)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("One of these is null - EXITING!!!");
			msgDebug2.append("  dbCoolCategoryCountryRec = ").append(Hex::toString((long long) dbCoolCategoryCountryRec));
			msgDebug2.append("  access = ").append(Hex::toString((long long) access));
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		return;
	}
	dbAccess = access;
	myDbRecord = dbCoolCategoryCountryRec;

	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCoolCountriesRM)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbCoolCountriesRM is null!");
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
	}
	else
	{
		DBCoolCountry *dbCoolCountryRec = dbCoolCountriesRM->find(dbCoolCategoryCountryRec);
		if (!dbCoolCountryRec)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("dbCoolCountryRec is null!");
				Debug::trace("COOLCategoryCountry", msgDebug2);
			}
		}
		else
		{
			if (!dbCoolCountryRec->isName1Null())
				country = dbCoolCountryRec->getName1();
			if (!dbCoolCountryRec->isCountryNumberNull())
				countryNum = Long::valueOf(dbCoolCountryRec->getCountryNumber());
			delete dbCoolCountryRec;
		}
	}

	if (!dbCoolCategoryCountryRec->isCategoryNumberNull())
		categoryNum = Long::valueOf(dbCoolCategoryCountryRec->getCategoryNumber());
	deptNum = Long::valueOf(dbCoolCategoryCountryRec->getDeptNumber());


	errorCode = RECORD_OK;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  categoryNum = ").append(Long::toString(categoryNum));
		msgDebug2.append("  countryNum = ").append(Long::toString(countryNum));
		msgDebug2.append("  country = [").append(country).append("]");
		Debug::trace("COOLCategoryCountry", msgDebug2);
	}
//	QUESTION: How do we handle a corrupt COOLCategoryCountry record?
}


COOLCategoryCountry::COOLCategoryCountry(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	Debug::trace("COOLCategoryCountry", "Constructor 2 passing const char *data, int len, int formatType ");

	switch (formatType)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLCategoryCountryTags coolTags;

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
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCOOLCatCountryDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCOOLCatCountryDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}


COOLCategoryCountry::COOLCategoryCountry(COOLCountry *coolCountryRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("COOLCategoryCountry");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "Constructor passing in COOLCountry* & DBAccess*";
		Debug::trace("COOLCategoryCountry", msgDebug);
		msgDebug.append(":  ");
	}
	init();

	if (!coolCountryRec || !access)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("One of these is null - EXITING!!!");
			msgDebug2.append("  coolCountryRec = ").append(Hex::toString((long long) coolCountryRec));
			msgDebug2.append("  access = ").append(Hex::toString((long long) access));
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		return;
	}
	dbAccess = access;

	country = coolCountryRec->getName1();
	countryNum = coolCountryRec->getCountryNum();
	if (coolCountryRec->isAllCategoriesFlag())
		categoryNum = 0;

	errorCode = RECORD_OK;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  categoryNum = ").append(Long::toString(categoryNum));
		msgDebug2.append("  countryNum = ").append(Long::toString(countryNum));
		msgDebug2.append("  country = [").append(country).append("]");
		Debug::trace("COOLCategoryCountry", msgDebug2);
	}
//	QUESTION: How do we handle a corrupt COOLCategoryCountry record?
}


COOLCategoryCountry::~COOLCategoryCountry()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void COOLCategoryCountry::init()
{
	country		= "";
	categoryNum	= RECORD_NOT_FOUND;
	errorCode	= RECORD_NOT_FOUND;
	countryNum	= RECORD_NOT_FOUND;

	deptNum             = 0;
	dbAccess			= 0;
	myDbRecord			= 0;

	initFieldList();
}

// static
void COOLCategoryCountry::initFieldList()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCOOLCatCountryDeptNum);
		evolFieldList.push_back(fieldOffsetCOOLCatCountryCategoryNum);
		evolFieldList.push_back(fieldOffsetCOOLCatCountryCountryName);

		COOLCategoryCountryTags coolTags((FieldOffsetsList *)&evolFieldList);
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetCOOLCatCountryCountryNum);
		ultimaFieldList.push_back(fieldOffsetCOOLCatCountryCountryName);
		ultimaFieldList.push_back(fieldOffsetCOOLCatCountryCategoryNum);

		COOLCategoryCountryTags coolTags((FieldOffsetsList *)&ultimaFieldList);
	}
}


const HobString &COOLCategoryCountry::getCountry() const
{
	return country;
}
void COOLCategoryCountry::setCountry(const HobString &name)
{
	country = name;
}


HobRecordNum COOLCategoryCountry::getCategoryNum() const
{
	return categoryNum;
}
void COOLCategoryCountry::setCategoryNum(HobRecordNum recordNum)
{
	categoryNum = recordNum;
}

HobRecordNum COOLCategoryCountry::getDeptNum() const
{
	return deptNum;
}
void COOLCategoryCountry::setDeptNum(HobRecordNum number)
{
	deptNum = number;
}

HobErrorCode COOLCategoryCountry::getErrorCode() const
{
	return errorCode;
}
void COOLCategoryCountry::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


int COOLCategoryCountry::updateField(int fieldCode, const HobString &token)
{
	bool traceOn = Debug::isTracing("COOLCategoryCountry");
	HobString myDebug;
	if (traceOn)
	{
		myDebug = "updateField() - fieldCode: ";
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("COOLCategoryCountry", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetCOOLCatCountryCategoryNum:
			setCategoryNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCOOLCatCountryCountryName:
			setCountry(token);
			break;
		case fieldOffsetCOOLCatCountryDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
		default:
			if (traceOn)
				Debug::trace("COOLCategoryCountry", myDebug + "fieldCode unknown!!!");
			break;
	}

	return 0;
}


void COOLCategoryCountry::update(const COOLCategoryCountry *newCOOLCategoryCountry)
{
	if (!newCOOLCategoryCountry)
		return;

	short loopCnt = 1;
	while (loopCnt < endCOOLCategoryCountryFieldOffsets)
	{
		if (newCOOLCategoryCountry->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCOOLCatCountryCategoryNum:
					setCategoryNum(newCOOLCategoryCountry->getCategoryNum());
					break;
				case fieldOffsetCOOLCatCountryCountryName:
					setCountry(newCOOLCategoryCountry->getCountry());
					break;
				case fieldOffsetCOOLCatCountryDeptNum:
					setDeptNum(newCOOLCategoryCountry->getDeptNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


void COOLCategoryCountry::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCOOLCatCountryCategoryNum:
			if (categoryNum > RECORD_NOT_FOUND)
				result = Long::toString(categoryNum);
			break;
		case fieldOffsetCOOLCatCountryCountryName:
			result = country;
			break;
		case fieldOffsetCOOLCatCountryCountryNum:
			if (countryNum > RECORD_NOT_FOUND)
				result = Long::toString(countryNum);
			break;
		case fieldOffsetCOOLCatCountryDeptNum:
			result = Long::toString(deptNum);
			break;
		default:
			break;
	}
}


const char *COOLCategoryCountry::serialize(int format) const
{
	bool traceOn = Debug::isTracing("COOLCategoryCountry.serialize");
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<COOLCategoryCountryFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			COOLCategoryCountryTags coolTags;
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
			COOLCategoryCountryTags coolTags;
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
						Debug::trace("COOLCategoryCountry.serialize", msgDebug2);
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
			serializedData = HobString("COOLCategoryCountry::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void COOLCategoryCountry::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


DBCoolCategoryCountry *COOLCategoryCountry::getMyDbRecord()
{
	return myDbRecord;
}


COOLCategoryCountry *COOLCategoryCountry::findCOOLCategoryCountry(HobRecordNum categoryNum, DBCoolCountry *dbCoolCountry, HobRecordNum deptNum, DBAccess *dbAccess)
{
	COOLCategoryCountry *coolCategoryCountry = 0;

	if (!dbCoolCountry || !dbAccess)
		return coolCategoryCountry;

	if (Debug::isTracing("COOLCategoryCountry.find"))
	{
		HobString myDebug("Looking for category number: ");
		myDebug.append(Long::toString(categoryNum)).append(" for country [").append(dbCoolCountry->getName1()).append("]");
		Debug::trace("COOLCategoryCountry.find", myDebug);
	}

	DBCoolCategoryCountriesRM	*dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
	if (!dbCoolCategoryCountriesRM)
		return coolCategoryCountry;

	try
	{
		// This calls dblayer.find() which calls sqlitelayer.find() which pulls deptNum from inrec->deptNum
		DBCoolCategoryCountry *dbCoolCategoryCountry = dbCoolCategoryCountriesRM->find(Long::toString(categoryNum), dbCoolCountry);
		if (dbCoolCategoryCountry)
		{
			Debug::trace("COOLCategoryCountry.find", "Found category in database.");

			coolCategoryCountry = new COOLCategoryCountry(dbCoolCategoryCountry, dbAccess);
			if (!coolCategoryCountry)
				delete dbCoolCategoryCountry;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "COOLCategoryCountry::findCOOLCategoryCountry() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
	return coolCategoryCountry;
}

bool COOLCategoryCountry::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int COOLCategoryCountry::deleteRecord()
{
	int result = 0;

	if (myDbRecord && dbAccess)
	{
		DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
		if (dbCoolCategoryCountriesRM)
		{
			if (Debug::isTracing("COOLCategoryCountry"))
			{
				HobString myDebug("Deleting COOLCategoryCountry:  country = ");
				myDebug.append(country);
				myDebug.append("  categoryNum = ").append(Long::toString(categoryNum));
				Debug::trace("COOLCategoryCountry", myDebug);
			}
			result = dbCoolCategoryCountriesRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void COOLCategoryCountry::saveRecord()
{
	bool traceOn = Debug::isTracing("COOLCategoryCountry");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "saveRecord()";
		Debug::trace("COOLCategoryCountry", msgDebug);
		msgDebug.append(":  ");
	}
	if (!dbAccess)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbAccess is null - EXITING!!!");
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		return;
	}

	COOLCountry *coolCountry = 0;

	DBCoolCategoryCountriesRM	*dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
	if (!dbCoolCategoryCountriesRM)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbCoolCategoryCountriesRM is null - EXITING!!!");
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		return;
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("country = [").append(country).append("]");
		Debug::trace("COOLCategoryCountry", msgDebug2);
	}
	if (!country.empty())
	{
		country.stripChar(_LF_);
		country.stripChar(_CR_);
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("After stripChar, country = [").append(country).append("]");
		Debug::trace("COOLCategoryCountry", msgDebug2);
	}

	coolCountry = COOLCountry::findCOOLCountry(country, deptNum, dbAccess);

	if (!coolCountry)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Can't find a dbCoolCountry record - creating it...");
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		coolCountry = new COOLCountry();
		if (coolCountry)
		{
			coolCountry->setName1(country);
			coolCountry->setDeptNum(deptNum);
			coolCountry->saveRecord();
		}
	}
	if (!coolCountry)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("coolCountry is null - EXITING!!!");
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		return;
	}

	if (!myDbRecord)
	{
		myDbRecord = dbCoolCategoryCountriesRM->find(Long::toString(categoryNum), coolCountry->getMyDbRecord());
		if (!myDbRecord)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Don't have a db record - creating it.");
				Debug::trace("COOLCategoryCountry", msgDebug2);
			}
			myDbRecord = dbCoolCategoryCountriesRM->createRecord(Long::toString(categoryNum), coolCountry->getMyDbRecord());
		}
	}
	if (myDbRecord)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("Saving the db record.");
			Debug::trace("COOLCategoryCountry", msgDebug2);
		}
		try
		{
			myDbRecord->setCategoryNumber(Long::toString(categoryNum));
			myDbRecord->setDeptNumber(Long::toString(deptNum));
			dbCoolCategoryCountriesRM->saveRecord(myDbRecord);		//	Save the record to the database.
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "COOLCategoryCountry::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
	if (coolCountry)
		delete coolCountry;
}


void COOLCategoryCountry::show()
{
	if (Debug::isTracing("COOLCategoryCountry"))
	{
		HobString msgDebug = "country = [";
		msgDebug.append(country).append("]");
		Debug::trace("COOLCategoryCountry", msgDebug);

		msgDebug = "categoryNum = ";
		msgDebug.append(Long::toString(categoryNum));
		Debug::trace("COOLCategoryCountry", msgDebug);
	}
}
