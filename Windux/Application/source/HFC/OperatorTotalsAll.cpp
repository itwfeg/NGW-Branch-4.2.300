/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "OperatorTotalsAll.h"
#include "Configuration.h"
#include "HobString.h"
#include "Product.h"
#include "ProductTotal.h"
#include "OperatorManager.h"
#include "HobOperator.h"

#include "DBAccess.h"
#include "OperatorTotal.h"

#include "DBOperatorsRM.h"
#include "dboperatorchangesrm.h"

#include "Long.h"
#include "Integer.h"
#include "Double.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperatorTotalsAll::dataType = 0;
std::string OperatorTotalsAll::dataTypeName("OperatorTotalsAll"); 
#endif

OperatorTotalsAll::OperatorTotalsAll(OperatorManager *operatorManager, DBAccess *dbAccess)
	: listCount(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (operatorManager && dbAccess)
	{
		HobString tempString;
		OperatorTotal *opTotal;
		HobOperator *myOperator = operatorManager->firstOperator();
		while (myOperator)
		{
			++listCount;
			operatorId.push_back(myOperator->getOpNum());

			tempString = myOperator->getOpName();
			tempString.quote();
			operName.push_back(tempString);

			opTotal = OperatorTotal::findOperatorTotal(myOperator->getMyDbRecord(), dbAccess);
			if (opTotal)
			{
				totalWeight.push_back(Double::toString((opTotal->getWeight() / Configuration::getInstance()->getWeightDivisor()), 1, Configuration::getInstance()->getWeightPrecision()));
				totalPrice.push_back(Double::toString((opTotal->getPrice() / Configuration::getInstance()->getPriceDivisor()), 1, Configuration::getInstance()->getPricePrecision()));
				totalPackages.push_back(Integer::toString(opTotal->getPackages()));
				totalRuns.push_back(Integer::toString(opTotal->getRuns()));
				totalItems.push_back(Integer::toString(opTotal->getItems()));
				delete opTotal;
			}
			else
			{
				totalWeight.push_back(Double::toString((0 / Configuration::getInstance()->getWeightDivisor()), 1, Configuration::getInstance()->getWeightPrecision()));
				totalPrice.push_back(Double::toString((0 / Configuration::getInstance()->getPriceDivisor()), 1, Configuration::getInstance()->getPricePrecision()));
				totalPackages.push_back(Integer::toString(0));
				totalRuns.push_back(Integer::toString(0));
				totalItems.push_back(Integer::toString(0));
			}
			myOperator = operatorManager->nextOperator();
		}
	}
}

OperatorTotalsAll::~OperatorTotalsAll()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


int OperatorTotalsAll::getListCount()
{
	return listCount;
}

const char *OperatorTotalsAll::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(operatorId.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(operName.serialize(format));
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
