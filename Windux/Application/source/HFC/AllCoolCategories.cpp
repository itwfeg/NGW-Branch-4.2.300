/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCoolCategories.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCoolCategoriesRM.h"
#include "DBCoolCategory.h"
#include "Configuration.h"

#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryCountry.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolCountry.h"

#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCategoryProductionStep.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolProductionStep.h"

#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryPreText.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolPreText.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"
#include "util/e_string/utilstring.h"

using namespace edb;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCoolCategories::dataType = 0;
std::string AllCoolCategories::dataTypeName("AllCoolCategories"); 
#endif

AllCoolCategories::AllCoolCategories()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


AllCoolCategories::AllCoolCategories(const char *data, int len, int /* formatType */)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	if (len > 0)
	{
		HobString temp(data, len);
		setLinkTo(temp);
	}
}


// private
void AllCoolCategories::init()
{
	recordNumList = new HobList();
	namesList = new HobList();

	linkTo = "";
}

AllCoolCategories::~AllCoolCategories()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (recordNumList)
		delete recordNumList;
	if (namesList)
		delete namesList;
}


HobList *AllCoolCategories::getRecordNumList()
{
	return recordNumList;
}
HobList *AllCoolCategories::getNamesList()
{
	return namesList;
}


const HobString &AllCoolCategories::getLinkTo() const
{
	return linkTo;
}
void AllCoolCategories::setLinkTo(HobString &name)
{
	linkTo = name;
}


void AllCoolCategories::findAllCategories(DBAccess *dbAccess)
{
	DBCoolCategoriesRM *dbCoolCategoriesRM = dbAccess->getDBCoolCategoriesRM();
	if (!dbCoolCategoriesRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBQuery<DBCoolCategory>* allCoolCategoriesQuery = dbCoolCategoriesRM->retrieveAllForDept(deptNumStr);

		if (!allCoolCategoriesQuery)
			return;

		DBQuery<DBCoolCategory>::const_iterator tempIter = allCoolCategoriesQuery->begin();
		while (tempIter != allCoolCategoriesQuery->end())
		{
			//	Push the country name on the list
			recordNumList->push_back(tempIter->getNumber());
			HobString tempName(tempIter->getName1());
			tempName.quoteForCSV();
			namesList->push_back(tempName);
			++tempIter;
		}
		delete allCoolCategoriesQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategories() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
//	setQuotingElements(true);
}


void AllCoolCategories::findAllCategoriesLinkedToCountry(DBAccess *dbAccess)
{
	if (linkTo.empty())
		return;

	DBCoolCountriesRM *dbCoolCountriesRM = dbAccess->getDBCoolCountriesRM();
	if (!dbCoolCountriesRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBCoolCountry *dbCoolCountry = dbCoolCountriesRM->findCountry(linkTo, deptNumStr);
		if (dbCoolCountry)
		{
			DBCoolCategoryCountriesRM *dbCoolCategoryCountriesRM = dbAccess->getDBCoolCategoryCountriesRM();
			if (dbCoolCategoryCountriesRM)
			{
				try
				{
					DBQuery<DBCoolCategoryCountry>* allCoolCategoryCountriesQuery = dbCoolCategoryCountriesRM->retrieveLinkedtoCoolCountry(dbCoolCountry);
					if (allCoolCategoryCountriesQuery)
					{
						DBCoolCategoriesRM *dbCoolCategoriesRM = dbAccess->getDBCoolCategoriesRM();
						if (dbCoolCategoriesRM)
						{
							DBCoolCategory *coolCategory = (DBCoolCategory *) NULL;
							DBQuery<DBCoolCategoryCountry>::const_iterator tempIter = allCoolCategoryCountriesQuery->begin();
							while (tempIter != allCoolCategoryCountriesQuery->end())
							{
								coolCategory = dbCoolCategoriesRM->findNumber(tempIter->getCategoryNumber(), tempIter->getDeptNumber() );

								if (coolCategory)
								{
										//	Push the country name on the list
									recordNumList->push_back(coolCategory->getNumber());
									HobString tempName(coolCategory->getName1());
									tempName.quoteForCSV();
									namesList->push_back(tempName);
									delete coolCategory;
								}
								++tempIter;
							}
						}
					}
					delete allCoolCategoryCountriesQuery;
				}
				catch (DBException &dbe)
				{
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategoriesLinkedToCountry() ********** database exception **********");
						DBException::Type t = dbe.getType();
						HobString myDebug("     DBException: type: ");
						myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
						Debug::trace("DBAccess.exception", myDebug);
					}
				}
			}
			delete dbCoolCountry;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategoriesLinkedToCountry() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}


void AllCoolCategories::findAllCategoriesLinkedToProductionStep(DBAccess *dbAccess)
{
	if (linkTo.empty())
		return;

	DBCoolProductionStepsRM *dbCoolProductionStepsRM = dbAccess->getDBCoolProductionStepsRM();
	if (!dbCoolProductionStepsRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBCoolProductionStep *dbCoolProductionStep = dbCoolProductionStepsRM->findProductionStep(linkTo, deptNumStr);
		if (dbCoolProductionStep)
		{
			DBCoolCategoryProductionStepsRM *dbCoolCategoryProductionStepsRM = dbAccess->getDBCoolCategoryProductionStepsRM();
			if (dbCoolCategoryProductionStepsRM)
			{
				try
				{
					DBQuery<DBCoolCategoryProductionStep>* allCoolCategoryCountriesQuery = dbCoolCategoryProductionStepsRM->retrieveLinkedtoCoolProductionStep(dbCoolProductionStep);
					if (allCoolCategoryCountriesQuery)
					{
						DBCoolCategoriesRM *dbCoolCategoriesRM = dbAccess->getDBCoolCategoriesRM();
						if (dbCoolCategoriesRM)
						{
							DBCoolCategory *coolCategory = (DBCoolCategory *) NULL;
							DBQuery<DBCoolCategoryProductionStep>::const_iterator tempIter = allCoolCategoryCountriesQuery->begin();
							while (tempIter != allCoolCategoryCountriesQuery->end())
							{
								//	Get the production step record
								// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
								HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
								coolCategory = dbCoolCategoriesRM->findNumber(tempIter->getCategoryNumber(), deptNumStr);
								if (coolCategory)
								{
										//	Push the country name on the list
									recordNumList->push_back(coolCategory->getNumber());
									HobString tempName(coolCategory->getName1());
									tempName.quoteForCSV();
									namesList->push_back(tempName);
									delete coolCategory;
								}
								++tempIter;
							}
						}
					}
					delete allCoolCategoryCountriesQuery;
				}
				catch (DBException &dbe)
				{
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategoriesLinkedToProductionStep() ********** database exception **********");
						DBException::Type t = dbe.getType();
						HobString myDebug("     DBException: type: ");
						myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
						Debug::trace("DBAccess.exception", myDebug);
					}
				}
			}
			delete dbCoolProductionStep;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategoriesLinkedToProductionStep() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}


void AllCoolCategories::findAllCategoriesLinkedToPretext(DBAccess *dbAccess)
{
	if (linkTo.empty())
		return;

	DBCoolPreTextsRM *dbCoolPreTextsRM = dbAccess->getDBCoolPreTextsRM();
	if (!dbCoolPreTextsRM)
		return;

	try
	{
		// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
		HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		DBCoolPreText *dbCoolPreText = dbCoolPreTextsRM->findNumber(linkTo, deptNumStr);
		if (dbCoolPreText)
		{
			DBCoolCategoryPreTextsRM *dbCoolCategoryPreTextsRM = dbAccess->getDBCoolCategoryPreTextsRM();
			if (dbCoolCategoryPreTextsRM)
			{
				try
				{
					DBQuery<DBCoolCategoryPreText>* allCoolCategoryPreTextsQuery = dbCoolCategoryPreTextsRM->retrieveLinkedtoCoolPreText(dbCoolPreText->getName1(), dbCoolPreText->getDeptNumber());
					if (allCoolCategoryPreTextsQuery)
					{
						DBCoolCategoriesRM *dbCoolCategoriesRM = dbAccess->getDBCoolCategoriesRM();
						if (dbCoolCategoriesRM)
						{
							DBCoolCategory *coolCategory = (DBCoolCategory *) NULL;
							DBQuery<DBCoolCategoryPreText>::const_iterator tempIter = allCoolCategoryPreTextsQuery->begin();
							while (tempIter != allCoolCategoryPreTextsQuery->end())
							{
								//	Get the category record
								// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
								HobString deptNumStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum);
								coolCategory = dbCoolCategoriesRM->findNumber(tempIter->getCategoryNumber(), deptNumStr);
								if (coolCategory)
								{
										//	Push the country name on the list
									recordNumList->push_back(coolCategory->getNumber());
									HobString tempName(coolCategory->getName1());
									tempName.quoteForCSV();
									namesList->push_back(tempName);
									delete coolCategory;
								}
								++tempIter;
							}
						}
					}
					delete allCoolCategoryPreTextsQuery;
				}
				catch (DBException &dbe)
				{
					if (Debug::isTracing("DBAccess.exception"))
					{
						Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategoriesLinkedToPretext() ********** database exception **********");
						DBException::Type t = dbe.getType();
						HobString myDebug("     DBException: type: ");
						myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
						Debug::trace("DBAccess.exception", myDebug);
					}
				}
			}
			delete dbCoolPreText;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllCoolCategories::findAllCategoriesLinkedToPretext() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("   DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info:").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}


const char *AllCoolCategories::serialize(int format) const
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
