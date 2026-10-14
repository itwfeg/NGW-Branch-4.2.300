/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ClassTotalAllTotalTypes.h"
#include "Configuration.h"
#include "HobString.h"
#include "Class.h"
#include "ClassTotal.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBClassesRM.h"
#include "DBClass.h"
#include "DBTotalTypesRM.h"
#include "DBTotalType.h"
#include "DBClassTotalsRM.h"
#include "DBQuery.h"

#include "Long.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ClassTotalAllTotalTypes::dataType = 0;
std::string ClassTotalAllTotalTypes::dataTypeName("ClassTotalAllTotalTypes"); 
#endif

ClassTotalAllTotalTypes::ClassTotalAllTotalTypes(HobRecordNum classNum, DBAccess *dbAccess)
	: listCount(0), classNumber(RECORD_NOT_FOUND)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (dbAccess)
	{
		DBTotalTypesRM *dbTotalTypesRM = dbAccess->getDBTotalTypesRM();
		DBClassesRM *dbClassesRM = dbAccess->getDBClassesRM();
		DBClassTotalsRM *dbClassTotalsRM = dbAccess->getDBClassTotalsRM();

		if (dbTotalTypesRM && dbClassesRM && dbClassTotalsRM)
		{
			DBClass *dbClass = dbClassesRM->findNumber(Long::toString(classNum));
			if (dbClass)
			{
				DBQuery<DBTotalType> *allTotalTypesQuery = dbTotalTypesRM->retrieveAll();
				if (allTotalTypesQuery)
				{
					ClassTotal *classTotal = NULL;

					classNumber = classNum;
					description = dbClass->getDescription();

					DBQuery<DBTotalType>::iterator tempIter = allTotalTypesQuery->begin();
					while (tempIter != allTotalTypesQuery->end())
					{
						++listCount;
						totalTypeNumber.push_back(tempIter->getNumber());

						classTotal = ClassTotal::findClassTotal(dbClass, &*tempIter, dbAccess);
						if (classTotal)
						{
							totalWeight.push_back(Double::toString((classTotal->getWeight() / Configuration::getInstance()->getWeightDivisor()), 1, Configuration::getInstance()->getWeightPrecision()));
							totalPrice.push_back(Double::toString((classTotal->getPrice() / Configuration::getInstance()->getPriceDivisor()), 1, Configuration::getInstance()->getPricePrecision()));
							totalPackages.push_back(Integer::toString(classTotal->getPackages()));
							totalRuns.push_back(Integer::toString(classTotal->getRuns()));
							totalItems.push_back(Integer::toString(classTotal->getItems()));
							delete classTotal;
							classTotal = NULL;
						}
						else
						{
							totalWeight.push_back(Double::toString((0 / Configuration::getInstance()->getWeightDivisor()), 1, Configuration::getInstance()->getWeightPrecision()));
							totalPrice.push_back(Double::toString((0 / Configuration::getInstance()->getPriceDivisor()), 1, Configuration::getInstance()->getPricePrecision()));
							totalPackages.push_back(Integer::toString(0));
							totalRuns.push_back(Integer::toString(0));
							totalItems.push_back(Integer::toString(0));
						}
						++tempIter;
					}
					delete allTotalTypesQuery;
				}
				delete dbClass;
			}
		}
	}
}

ClassTotalAllTotalTypes::~ClassTotalAllTotalTypes()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


int ClassTotalAllTotalTypes::getListCount()
{
	return listCount;
}


const char *ClassTotalAllTotalTypes::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(Long::toString(classNumber));
			serializedData.append(BrowserFormatSeparator);

			HobString tempString = description;
			tempString.quote();
			serializedData.append(tempString);

			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalTypeNumber.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalWeight.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalPrice.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalPackages.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalRuns.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalItems.serialize(format));
		}
			break;

		default:
			serializedData = HobString("ClassTotalAllTotalTypes::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
