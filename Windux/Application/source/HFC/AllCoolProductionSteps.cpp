/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCoolProductionSteps.h"

#include "HobTokenizerCSV.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolProductionStep.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCategoryProductionStep.h"

#include "utilstring.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCoolProductionSteps::dataType = 0;
std::string AllCoolProductionSteps::dataTypeName("AllCoolProductionSteps"); 
#endif

AllCoolProductionSteps::AllCoolProductionSteps()
	: HobList(), forAllCategories(true), excludeAllCategories(false), categoryNum(RECORD_NOT_FOUND), category("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AllCoolProductionSteps", "Constructor 1.");
	show();
}

AllCoolProductionSteps::AllCoolProductionSteps(const char *data, int len, int /* formatType */)
	: HobList(), forAllCategories(true), excludeAllCategories(false), categoryNum(RECORD_NOT_FOUND), category("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AllCoolProductionSteps", "Constructor 2 with data. len = " + HobString(Integer::toString(len)));

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

	show();
}

// private
AllCoolProductionSteps::~AllCoolProductionSteps()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


HobRecordNum AllCoolProductionSteps::getCategoryNum()
{
	return categoryNum;
}
void AllCoolProductionSteps::setCategoryNum(HobRecordNum number)
{
	categoryNum = number;
}

HobString AllCoolProductionSteps::getCategory()
{
	return category;
}
void AllCoolProductionSteps::setCategory(HobString name)
{
	category = name;
}


void AllCoolProductionSteps::findAllProductionSteps(DBAccess *dbAccess)
{
	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolProductionStep>* allCoolProductionStepsQuery = dbCoolProductionStepsRM->retrieveAllForDept(deptNumStr);

		if (!allCoolProductionStepsQuery)
			return;

		DBQuery<DBCoolProductionStep>::const_iterator tempIter = allCoolProductionStepsQuery->begin();
		while (tempIter != allCoolProductionStepsQuery->end())
		{
			//	Push the country name on the list
			HobString tempName(tempIter->getName1());
			tempName.quoteForCSV();
			push_back(tempName);
			++tempIter;
		}
		delete allCoolProductionStepsQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolProductionSteps::findAllProductionSteps() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
//	setQuotingElements(true);
}


void AllCoolProductionSteps::findAllProductionStepsLinkedToCategory(DBAccess *dbAccess)
{
	if (forAllCategories)
		findAllProductionStepsLinkedToAllCategories(dbAccess);
	else
	{
		Debug::trace("AllCoolProductionSteps", "findAllProductionStepsLinkedToCategory()");

		DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
		if (!dbCoolCategoryProductionStepsRM)
			return;

		DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
		if (!dbCoolProductionStepsRM)
			return;

		try
		{
			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
			DBQuery<DBCoolCategoryProductionStep>* allCoolCategoryProductionStepsQuery = dbCoolCategoryProductionStepsRM->retrieveLinkedtoCoolCategory(Long::toString(categoryNum), deptNumStr);
			if (!allCoolCategoryProductionStepsQuery)
				return;

			DBCoolProductionStep *coolProductionStep = (DBCoolProductionStep *) NULL;
			DBCoolCategoryProductionStep *ccc = (DBCoolCategoryProductionStep *) NULL;
			DBQuery<DBCoolCategoryProductionStep>::const_iterator tempIter = allCoolCategoryProductionStepsQuery->begin();
			while (tempIter != allCoolCategoryProductionStepsQuery->end())
			{
				ccc = new DBCoolCategoryProductionStep(*tempIter);
				if (ccc)
				{
					//	Get the country record
					coolProductionStep = dbCoolProductionStepsRM->find(ccc);
					if (coolProductionStep)
					{
						//	Push the country name on the list
						HobString tempName(coolProductionStep->getName1());
						tempName.quoteForCSV();
						push_back(tempName);
						delete coolProductionStep;
					}
					delete ccc;
				}
				++tempIter;
			}
			delete allCoolCategoryProductionStepsQuery;
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", "AllCoolProductionSteps::findAllProductionStepsLinkedToCategory() ********** database exception **********");
				DBException::Type t = dbe.getType();
				HobString myDebug("     DBException: type: ");
				myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
				Debug::trace("DBAccess.exception", myDebug);
			}
		}
	}
}


void AllCoolProductionSteps::findAllProductionStepsLinkedToAllCategories(DBAccess *dbAccess)
{
	Debug::trace("AllCoolProductionSteps", "findAllProductionStepsLinkedToAllCategories()");

	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolProductionStep>* allCoolProductionStepsQuery = dbCoolProductionStepsRM->retrieveAllCategories(deptNumStr);
		if (!allCoolProductionStepsQuery)
			return;

		DBCoolProductionStep *coolProductionStep = (DBCoolProductionStep *) NULL;
		DBQuery<DBCoolProductionStep>::const_iterator tempIter = allCoolProductionStepsQuery->begin();
		while (tempIter != allCoolProductionStepsQuery->end())
		{
			//	Get the country record
			coolProductionStep = new DBCoolProductionStep(*tempIter);
			if (coolProductionStep)
			{
				//	Push the country name on the list
				HobString tempName(coolProductionStep->getName1());
				tempName.quoteForCSV();
				push_back(tempName);
				delete coolProductionStep;
			}
			++tempIter;
		}
		delete allCoolProductionStepsQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolProductionSteps::findAllProductionStepsLinkedToAllCategories() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}

bool AllCoolProductionSteps::doExcludeAllCategories()
{
	return excludeAllCategories;
}
void AllCoolProductionSteps::setExcludeAllCategories(bool value)
{
	excludeAllCategories = value;
}

void AllCoolProductionSteps::show()
{
	if (Debug::isTracing("AllCoolProductionSteps"))
	{
		HobString msgDebug = "forAllCategories = ";
		msgDebug.append(Bool::toString(forAllCategories));
		Debug::trace("AllCoolProductionSteps", msgDebug);

		msgDebug = "excludeAllCategories = ";
		msgDebug.append(Bool::toString(excludeAllCategories));
		Debug::trace("AllCoolProductionSteps", msgDebug);

		msgDebug = "categoryNum = ";
		msgDebug.append(Long::toString(categoryNum));
		Debug::trace("AllCoolProductionSteps", msgDebug);

		msgDebug = "categoryName = [";
		msgDebug.append(category).append("]");
		Debug::trace("AllCoolProductionSteps", msgDebug);
	}
}
