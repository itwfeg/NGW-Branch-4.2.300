/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCategories.h"
#include "Category.h"
#include "Configuration.h"
#include "Integer.h"
#include "Long.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCategoriesRM.h"
#include "DBCategory.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCategories::dataType = 0;
std::string AllCategories::dataTypeName("AllCategories"); 
#endif


AllCategories::AllCategories()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	nameList.setQuotingElements(true);
}

AllCategories::~AllCategories()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void AllCategories::findAll(DBAccess *dbAccess, LanguageSelection lang)
{
	if (!dbAccess)
		return;

	DBCategoriesRM *dbRM = dbAccess->getDBCategoriesRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllCategories");
	try
	{
		DBQuery<DBCategory>* allQuery = dbRM->retrieveAllByName( Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
		if (!allQuery)
			return;

		DBQuery<DBCategory>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllCategories", string("findAll - num ") + iter->getNumber() + "  name1 " + iter->getName1() + "  name2 " + iter->getName2());
			recordNumList.push_back(iter->getNumber());
			nameList.push_back((lang == PrimaryLanguage) ? iter->getName1() : iter->getName2());
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllCategories", string("findAll - recordNumList size ") + inttostring(recordNumList.size()) + "  nameList size " + inttostring(nameList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllCategories::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

void AllCategories::findAllMainCategories(DBAccess *dbAccess, LanguageSelection lang)
{
	if (!dbAccess)
		return;

	DBCategoriesRM *dbRM = dbAccess->getDBCategoriesRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllCategories");
	try
	{
		Category *masterCategory = Category::findRecord(Category::RootCategoryNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
		if (!masterCategory)
			return;

		//	The Master category should always be the first in the list.
		if (traceOn)
			Debug::trace("AllCategories", string("findAllMainCategories - num ") + longtostring(masterCategory->getCategoryNum()) + "  name1 " + masterCategory->getCategoryName1() + "  name2 " + masterCategory->getCategoryName2());
		recordNumList.push_back(longtostring(masterCategory->getCategoryNum()));
		nameList.push_back((lang == PrimaryLanguage) ? masterCategory->getCategoryName1() : masterCategory->getCategoryName2());

		//	Now query the categories that report to the Master category.
		DBQuery<DBCategory> *allQuery = dbRM->retrieveChildCategories(masterCategory->getMyDbRecord());
		if (allQuery)
		{
			DBQuery<DBCategory>::const_iterator iter = allQuery->begin();
			while (iter != allQuery->end())
			{
				HobRecordNum catNum = Long::valueOf(iter->getNumber());
				if (catNum != Category::RootCategoryNum)	//	Don't include the Master Category again.
				{
					if (traceOn)
						Debug::trace("AllCategories", string("findAllMainCategories - num ") + iter->getNumber() + "  name1 " + iter->getName1() + "  name2 " + iter->getName2());
					recordNumList.push_back(iter->getNumber());
					nameList.push_back((lang == PrimaryLanguage) ? iter->getName1() : iter->getName2());
				}
				++iter;
			}
			delete allQuery;
		}
		delete masterCategory;

		if (traceOn)
			Debug::trace("AllCategories", string("findAllMainCategories - recordNumList size ") + inttostring(recordNumList.size()) + "  nameList size " + inttostring(nameList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllCategories::findAllMainCategories() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}

const char *AllCategories::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			serializedData.append(Integer::toString(Configuration::getInstance()->getNumOfMostCommonCategories()));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(recordNumList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(nameList.serialize(format));
			break;

		default:
			break;
	}
	return serializedData.c_str();
}
