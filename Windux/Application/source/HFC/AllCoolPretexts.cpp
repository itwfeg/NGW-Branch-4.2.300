/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCoolPretexts.h"

#include "HobTokenizerCSV.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolPreText.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryPreText.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCoolPretexts::dataType = 0;
std::string AllCoolPretexts::dataTypeName("AllCoolPretexts"); 
#endif

AllCoolPretexts::AllCoolPretexts()
	: forAllCategories(true), excludeAllCategories(false), categoryNum(RECORD_NOT_FOUND), category("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AllCoolPretexts", "Constructor 1.");

	init();

	show();
}

AllCoolPretexts::AllCoolPretexts(const char *data, int len, int /* formatType */)
	: forAllCategories(true), excludeAllCategories(false), categoryNum(RECORD_NOT_FOUND), category("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AllCoolPretexts", "Constructor 2 with data. len = " + HobString(Integer::toString(len)));

	init();

	HobString temp(data, len);
	HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	HobString token;
	int offset = 1;

	while (tok.next(&token) != -1)
	{
		if (token.length())
		{
			setReceived(offset, true);
			if (offset == 1)
			{
				setCategoryNum(Long::valueOf(data));
				forAllCategories = false;
			}
			else if (offset == 2)
			{
				setExcludeAllCategories(Bool::valueOf(data));
			}

		}
		++offset;
	}

	Debug::trace("AllCoolPretexts", "Constructor 2 with data.");
	show();
}

// private
void AllCoolPretexts::init()
{
	recordNumList = new HobList();
	namesList = new HobList();

//	categoryNum	= RECORD_NOT_FOUND;
//	category = "";
}

AllCoolPretexts::~AllCoolPretexts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (recordNumList)
		delete recordNumList;
	if (namesList)
		delete namesList;
}


HobRecordNum AllCoolPretexts::getCategoryNum()
{
	return categoryNum;
}
void AllCoolPretexts::setCategoryNum(HobRecordNum number)
{
	categoryNum = number;
}

HobList *AllCoolPretexts::getRecordNumList()
{
	return recordNumList;
}
HobList *AllCoolPretexts::getNamesList()
{
	return namesList;
}


HobString AllCoolPretexts::getCategory()
{
	return category;
}
void AllCoolPretexts::setCategory(HobString name)
{
	category = name;
}


void AllCoolPretexts::findAllPretexts(DBAccess *dbAccess)
{
	HobString myDebug;
	bool traceOn = Debug::isTracing("AllCoolPretexts");
	if (traceOn)
		myDebug = "findAllPretexts():  ";

	DBCoolPreTextsRM *dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPretextsRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolPreText>* allCoolPretextsQuery = dbCoolPretextsRM->retrieveAllForDept(deptNumStr);

		if (!allCoolPretextsQuery)
			return;

		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("  Count = ").append(Integer::toString((int)allCoolPretextsQuery->size()));
			Debug::trace("AllCoolPretexts", myDebug2);
		}
		DBQuery<DBCoolPreText>::const_iterator tempIter = allCoolPretextsQuery->begin();
		while (tempIter != allCoolPretextsQuery->end())
		{
			HobString theNumber(tempIter->getNumber());
			HobString theName(tempIter->getName1());
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("  Number = ").append(theNumber);
				myDebug2.append("  Pretext = ").append(theName);
				Debug::trace("AllCoolPretexts", myDebug2);
			}
			//	Push the country name on the list
			recordNumList->push_back(theNumber);
			theName.quoteForCSV();
			namesList->push_back(theName);
			++tempIter;
		}
		delete allCoolPretextsQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolPretexts::findAllPretexts() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
//	setQuotingElements(true);
}


void AllCoolPretexts::findAllPretextsLinkedToCategory(DBAccess *dbAccess)
{
	if (forAllCategories)
		findAllPretextsLinkedToAllCategories(dbAccess);
	else
	{
		Debug::trace("AllCoolPretexts", "findAllPretextsLinkedToCategory()");

		DBCoolCategoryPreTextsRM *dbCoolCategoryPretextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
		if (!dbCoolCategoryPretextsRM)
			return;

		DBCoolPreTextsRM *dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
		if (!dbCoolPretextsRM)
			return;

		try
		{
			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
			DBQuery<DBCoolCategoryPreText>* allCoolCategoryPretextsQuery = dbCoolCategoryPretextsRM->retrieveLinkedtoCoolCategory(longtostring(categoryNum), deptNumStr);
			if (!allCoolCategoryPretextsQuery)
				return;

			HobString recordNumStr;
			DBCoolPreText *coolPretext = (DBCoolPreText *) NULL;
			DBCoolCategoryPreText *pt = (DBCoolCategoryPreText *) NULL;
			DBQuery<DBCoolCategoryPreText>::const_iterator tempIter = allCoolCategoryPretextsQuery->begin();
			while (tempIter != allCoolCategoryPretextsQuery->end())
			{
				pt = new DBCoolCategoryPreText(*tempIter);
				if (pt)
				{
					//	Get the pretext record
					recordNumStr = pt->getPreTextNumber();

					// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
					HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
					coolPretext = dbCoolPretextsRM->findNumber(recordNumStr, deptNumStr);
					
					if (coolPretext)
					{
						//	Push the country name on the list
						recordNumList->push_back(coolPretext->getNumber());
						HobString tempName(coolPretext->getName1());
						tempName.quoteForCSV();
						namesList->push_back(tempName);
						delete coolPretext;
					}
					delete pt;
				}
				++tempIter;
			}
			delete allCoolCategoryPretextsQuery;
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", "AllCoolPretexts::findPretextsLinkedToCategory() ********** database exception **********");
				DBException::Type t = dbe.getType();
				HobString myDebug("     DBException: type: ");
				myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
				Debug::trace("DBAccess.exception", myDebug);
			}
		}
	}
}


void AllCoolPretexts::findAllPretextsLinkedToAllCategories(DBAccess *dbAccess)
{
	Debug::trace("AllCoolPretexts", "findAllPretextsLinkedToAllCategories()");

	DBCoolPreTextsRM *dbCoolPretextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPretextsRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolPreText>* allCoolPretextsQuery = dbCoolPretextsRM->retrieveAllCategories(deptNumStr);
		if (!allCoolPretextsQuery)
			return;

		HobString recordNumStr;
		DBCoolPreText *coolPretext = 0;
		DBQuery<DBCoolPreText>::const_iterator tempIter = allCoolPretextsQuery->begin();
		while (tempIter != allCoolPretextsQuery->end())
		{
			//	Get the pretext record
			coolPretext = new DBCoolPreText(*tempIter);
			if (coolPretext)
			{
				//	Push the country name on the list
				recordNumList->push_back(coolPretext->getNumber());
				HobString tempName(coolPretext->getName1());
				tempName.quoteForCSV();
				namesList->push_back(tempName);
				delete coolPretext;
			}
			++tempIter;
		}
		delete allCoolPretextsQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolPretexts::findAllPretextsLinkedToAllCategories() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}


const char *AllCoolPretexts::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if (recordNumList && namesList)
			{
				serializedData.append(recordNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(namesList->serialize(format));
			}

			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

bool AllCoolPretexts::doExcludeAllCategories()
{
	return excludeAllCategories;
}
void AllCoolPretexts::setExcludeAllCategories(bool value)
{
	excludeAllCategories = value;
}

void AllCoolPretexts::show()
{
	if (Debug::isTracing("AllCoolPretexts"))
	{
		HobString msgDebug = "forAllCategories = ";
		msgDebug.append(Bool::toString(forAllCategories));
		Debug::trace("AllCoolPretexts", msgDebug);

		msgDebug = "excludeAllCategories = ";
		msgDebug.append(Bool::toString(excludeAllCategories));
		Debug::trace("AllCoolPretexts", msgDebug);

		msgDebug = "categoryNum = ";
		msgDebug.append(Long::toString(categoryNum));
		Debug::trace("AllCoolPretexts", msgDebug);

		msgDebug = "categoryName = [";
		msgDebug.append(category).append("]");
		Debug::trace("AllCoolPretexts", msgDebug);
	}
}
