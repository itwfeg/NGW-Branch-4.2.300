/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCoolCountries.h"

#include "HobTokenizerCSV.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolCountry.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryCountry.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCoolCountries::dataType = 0;
std::string AllCoolCountries::dataTypeName("AllCoolCountries"); 
#endif

AllCoolCountries::AllCoolCountries()
	: HobList(), forAllCategories(true), excludeAllCategories(false), categoryNum(RECORD_NOT_FOUND), category("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AllCoolCountries", "Constructor 1.");
	show();
}

AllCoolCountries::AllCoolCountries(const char *data, int len, int /* formatType */)
	: HobList(), forAllCategories(true), excludeAllCategories(false), categoryNum(RECORD_NOT_FOUND), category("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AllCoolCountries", "Constructor 2 with data. len = " + HobString(Integer::toString(len)));

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

	Debug::trace("AllCoolCountries", "Constructor 2 with data.  show()");
	show();
}

// private
AllCoolCountries::~AllCoolCountries()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

HobRecordNum AllCoolCountries::getCategoryNum()
{
	return categoryNum;
}
void AllCoolCountries::setCategoryNum(HobRecordNum number)
{
	categoryNum = number;
}

const HobString &AllCoolCountries::getCategory() const
{
	return category;
}
void AllCoolCountries::setCategory(const HobString &name)
{
	category = name;
}

void AllCoolCountries::findAllCountries(DBAccess *dbAccess)
{
	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCoolCountriesRM)
		return;

    bool traceOn = Debug::isTracing("AllCoolCountries");
	HobString myDebug("findAllCountries():  ");
	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolCountry>* allCoolCountriesQuery = dbCoolCountriesRM->retrieveAllForDept(deptNumStr);

		if (!allCoolCountriesQuery)
			return;

		DBQuery<DBCoolCountry>::const_iterator tempIter = allCoolCountriesQuery->begin();
		while (tempIter != allCoolCountriesQuery->end())
		{
			//	Push the country name on the list
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("  found COOLCountry: ").append(tempIter->getName1());
				Debug::trace("AllCoolCountries", myDebug2);
			}
			HobString tempName(tempIter->getName1());
			tempName.quoteForCSV();
			push_back(tempName);
			++tempIter;
		}
		delete allCoolCountriesQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolCountries::findAllCountries() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
//	setQuotingElements(true);
}


void AllCoolCountries::findAllCountriesLinkedToCategory(DBAccess *dbAccess)
{
	if (forAllCategories)
		findAllCountriesLinkedToAllCategories(dbAccess);
	else
	{
	    bool traceOn = Debug::isTracing("AllCoolCountries");
		HobString myDebug;
		if (traceOn)
		{
			myDebug = "findAllCountriesLinkedToCategory():  ";
			Debug::trace("AllCoolCountries", myDebug);
		}

		DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
		if (!dbCoolCategoryCountriesRM)
			return;

		DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
		if (!dbCoolCountriesRM)
			return;

		try
		{
			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
			DBQuery<DBCoolCategoryCountry>* allCoolCategoryCountriesQuery = dbCoolCategoryCountriesRM->retrieveLinkedtoCoolCategory(Long::toString(categoryNum), deptNumStr);
			if (!allCoolCategoryCountriesQuery)
				return;

			DBCoolCountry *coolCountry = (DBCoolCountry *) NULL;
			DBCoolCategoryCountry *ccc = (DBCoolCategoryCountry *) NULL;
			DBQuery<DBCoolCategoryCountry>::const_iterator tempIter = allCoolCategoryCountriesQuery->begin();
			while (tempIter != allCoolCategoryCountriesQuery->end())
			{
				ccc = new DBCoolCategoryCountry(*tempIter);
				if (ccc)
				{
					//	Get the country record
					coolCountry = dbCoolCountriesRM->find(ccc);
					if (coolCountry)
					{
						//	Push the country name on the list
						if (traceOn)
						{
							HobString myDebug2 = myDebug;
							myDebug2.append("  found COOLCountry: ").append(coolCountry->getName1());
							Debug::trace("AllCoolCountries", myDebug2);
						}
						HobString tempName(coolCountry->getName1());
						tempName.quoteForCSV();
						push_back(tempName);
						delete coolCountry;
					}
					delete ccc;
				}
				++tempIter;
			}
			delete allCoolCategoryCountriesQuery;
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", "AllCoolCountries::findAllCountriesLinkedToCategory() ********** database exception **********");
				DBException::Type t = dbe.getType();
				HobString myDebug("     DBException: type: ");
				myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
				Debug::trace("DBAccess.exception", myDebug);
			}
		}
	}
}


void AllCoolCountries::findAllCountriesLinkedToAllCategories(DBAccess *dbAccess)
{
    bool traceOn = Debug::isTracing("AllCoolCountries");
	HobString myDebug;
	if (traceOn)
	{
		myDebug = "findAllCountriesLinkedToAllCategories():  ";
		Debug::trace("AllCoolCountries", myDebug);
	}

	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCoolCountriesRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolCountry>* allCoolCountriesQuery = dbCoolCountriesRM->retrieveAllCategories(deptNumStr);
		if (!allCoolCountriesQuery)
			return;

		DBCoolCountry *coolCountry = (DBCoolCountry *) NULL;
		DBQuery<DBCoolCountry>::const_iterator tempIter = allCoolCountriesQuery->begin();
		while (tempIter != allCoolCountriesQuery->end())
		{
			//	Get the country record
			coolCountry = new DBCoolCountry(*tempIter);
			if (coolCountry)
			{
				//	Push the country name on the list
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("  found COOLCountry: ").append(coolCountry->getName1());
					Debug::trace("AllCoolCountries", myDebug2);
				}
				HobString tempName(coolCountry->getName1());
				tempName.quoteForCSV();
				push_back(tempName);
				delete coolCountry;
			}
			++tempIter;
		}
		delete allCoolCountriesQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolCountries::findAllCountriesLinkedToAllCategories() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}

bool AllCoolCountries::doExcludeAllCategories()
{
	return excludeAllCategories;
}
void AllCoolCountries::setExcludeAllCategories(bool value)
{
	excludeAllCategories = value;
}

void AllCoolCountries::show()
{
	if (Debug::isTracing("AllCoolCountries"))
	{
		HobString msgDebug = "forAllCategories = ";
		msgDebug.append(Bool::toString(forAllCategories));
		Debug::trace("AllCoolCountries", msgDebug);

		msgDebug = "excludeAllCategories = ";
		msgDebug.append(Bool::toString(excludeAllCategories));
		Debug::trace("AllCoolCountries", msgDebug);

		msgDebug = "categoryNum = ";
		msgDebug.append(Long::toString(categoryNum));
		Debug::trace("AllCoolCountries", msgDebug);

		msgDebug = "categoryName = [";
		msgDebug.append(category).append("]");
		Debug::trace("AllCoolCountries", msgDebug);
	}
}
