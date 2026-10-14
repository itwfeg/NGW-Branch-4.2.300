/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTotalAllTotalTypes.h"
#include "Configuration.h"
#include "HobString.h"
#include "Product.h"
#include "ProductTotal.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductsRM.h"
#include "DBProduct.h"
#include "DBTotalTypesRM.h"
#include "DBTotalType.h"
#include "DBProductTotalsRM.h"
#include "DBQuery.h"

#include "Long.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTotalAllTotalTypes::dataType = 0;
std::string ProductTotalAllTotalTypes::dataTypeName("ProductTotalAllTotalTypes"); 
#endif

ProductTotalAllTotalTypes::ProductTotalAllTotalTypes(HobRecordNum productNum, DBAccess *dbAccess)
	: listCount(0), productNumber(RECORD_NOT_FOUND)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (dbAccess)
	{
		DBTotalTypesRM *dbTotalTypesRM = dbAccess->getDBTotalTypesRM();
		DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
		DBProductTotalsRM *dbProductTotalsRM = dbAccess->getDBProductTotalsRM();

		if (dbTotalTypesRM && dbProductsRM && dbProductTotalsRM)
		{
			DBProduct *dbProduct = dbProductsRM->findPLU(Long::toString(productNum));
			if (dbProduct)
			{
				Debug::trace("ProductTotalAllTotalTypes", HobString("Found product record number ") + HobString(Long::toString(productNum)));
				DBQuery<DBTotalType> *allTotalTypesQuery = dbTotalTypesRM->retrieveAll();
				if (allTotalTypesQuery)
				{
					Debug::trace("ProductTotalAllTotalTypes", HobString("dbTotalTypesRM->retrieveAll() was successful"));
					ProductTotal *productTotal = NULL;

					productNumber = productNum;
					description = dbProduct->getDescription();

					DBQuery<DBTotalType>::iterator tempIter = allTotalTypesQuery->begin();
					while (tempIter != allTotalTypesQuery->end())
					{
						++listCount;
						totalTypeNumber.push_back(tempIter->getNumber());

						productTotal = ProductTotal::findProductTotal(dbProduct, &*tempIter, dbAccess);
						if (productTotal)
						{
							productTotal->show();
							totalWeight.push_back(Double::toString((productTotal->getWeight() / Configuration::getInstance()->getWeightDivisor()), 1, Configuration::getInstance()->getWeightPrecision()));
							totalPrice.push_back(Double::toString((productTotal->getPrice() / Configuration::getInstance()->getPriceDivisor()), 1, Configuration::getInstance()->getPricePrecision()));
							totalPackages.push_back(Integer::toString(productTotal->getPackages()));
							totalRuns.push_back(Integer::toString(productTotal->getRuns()));
							totalItems.push_back(Integer::toString(productTotal->getItems()));
							delete productTotal;
							productTotal = NULL;
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
				delete dbProduct;
			}
		}
	}
}

ProductTotalAllTotalTypes::~ProductTotalAllTotalTypes()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


int ProductTotalAllTotalTypes::getListCount()
{
	return listCount;
}

const char *ProductTotalAllTotalTypes::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(Long::toString(productNumber));
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
			serializedData = "Serialized Class : unknown format";
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
