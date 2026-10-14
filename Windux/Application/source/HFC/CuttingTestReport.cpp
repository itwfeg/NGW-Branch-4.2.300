/* Evolution Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CuttingTestReport.h"
#include "Configuration.h"
#include "HobList.h"
#include "DBQuery.h"
#if 0
#include "DBCutTestTotalsRM.h"
#include "DBCutTestTotal.h"
#include "Rounding.h"
#include "Power.h"
#endif
#include "Long.h"
#include "Integer.h"
#include "CutTestTotal.h"
#include "CuttingTest.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CuttingTestReport::_dataType_ = 0;
std::string CuttingTestReport::_dataTypeName_("CuttingTestReport"); 
#endif

std::list<CuttingTestReportFieldOffsets> CuttingTestReport::browserFieldList;

CuttingTestReport::CuttingTestReport(CuttingTest *cutTest)
									: cutTestNum(""), buyItem(""), vendorNum(""), operId(""), itemCost(0), itemWeight(0)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (cutTest)
	{
		cutTestNum = Long::toString(cutTest->getTestNumber());
		buyItem = cutTest->getProductNumber();
		vendorNum = cutTest->getVendorNumber();
		operId = cutTest->getOperatorId();
		itemCost = cutTest->getPrimalCost();
		itemWeight = cutTest->getPrimalWeight();
		startTime = cutTest->getStartDate();
		endTime = cutTest->getEndDate();
	}
	init();
}

CuttingTestReport::CuttingTestReport(HobString ctNum, HobString prodNum, HobString venNum, HobString opId, 
									HobPrice prodCost, HobWeight prodWeight, HobDateTime sTime, HobDateTime eTime)
									: cutTestNum(ctNum), buyItem(prodNum), vendorNum(venNum), operId(opId),
									  itemCost(prodCost), itemWeight(prodWeight), startTime(sTime), endTime(eTime)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

// private
CuttingTestReport::~CuttingTestReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void CuttingTestReport::init()
{
	totalSellWt = 0;
	totalShrinkWt = 0;
	totalSellYield = 0;
	totalShrinkYield = 0;
	percentProfit = 0;
	totalSellPrice = 0;
	avgUnitPrice = 0;

	initFieldLists();
}


void CuttingTestReport::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetCTReportNum);
		browserFieldList.push_back(fieldOffsetCTReportBuyItem);
		browserFieldList.push_back(fieldOffsetCTReportVendorNum);
		browserFieldList.push_back(fieldOffsetCTReportOperator);
		browserFieldList.push_back(fieldOffsetCTReportTotalSellWeight);
		browserFieldList.push_back(fieldOffsetCTReportTotalShrinkWeight);
		browserFieldList.push_back(fieldOffsetCTReportTotalItemWeight);
		browserFieldList.push_back(fieldOffsetCTReportTotalSellYield);
		browserFieldList.push_back(fieldOffsetCTReportTotalShrinkYield);
		browserFieldList.push_back(fieldOffsetCTReportTotalItemYield);
		browserFieldList.push_back(fieldOffsetCTReportAvgPricePerUnitWt);
		browserFieldList.push_back(fieldOffsetCTReportTotalSellPrice);
		browserFieldList.push_back(fieldOffsetCTReportTotalItemCost);
		browserFieldList.push_back(fieldOffsetCTReportPercentProfit);
	}
}


void CuttingTestReport::add(Util::CSmartPtr<CutTestTotal> cutTestTotal, DBAccess *dbAccess)
{
	Configuration *scaleConfig = Configuration::getInstance();
	if (!scaleConfig)
		return;

	bool traceOn = Debug::isTracing("CuttingTestReport");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "prepareReportData():  ";

	long productYield = 0;
	HobString result, description;
//	short pricePrecision = scaleConfig->getPricePrecision();
//	short weightPrecision = scaleConfig->getWeightPrecision();
	short yieldPower = 4, yieldPrecision = 1;
	unsigned int maxDesc = 35;
	long yieldFactor = Power::power(10, yieldPower);

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("itemCost = ").append(Long::toString(itemCost));
		msgDebug2.append("itemWeight = ").append(Long::toString(itemWeight));
		Debug::trace("CuttingTestReport", msgDebug2);
	}

	if (cutTestTotal)
	{
		totalSellWt += cutTestTotal->getWeight();
		scaleConfig->formatWeight(cutTestTotal->getWeight(), result);
		weightList.push_back(result);

		totalSellPrice += cutTestTotal->getPrice();
		scaleConfig->formatPrice(cutTestTotal->getPrice(), result);
		totalPriceList.push_back(result);

		if (itemWeight)
			productYield = (cutTestTotal->getWeight() * yieldFactor) / itemWeight;
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("1)   productYield = ").append(Integer::toString(productYield));
			Debug::trace("CuttingTestReport", msgDebug2);
		}
		productYield = Rounding::roundConventionalPriceWeight(productYield, 0, 2, yieldPrecision);
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("2)   productYield = ").append(Integer::toString(productYield));
			Debug::trace("CuttingTestReport", msgDebug2);
		}
		totalSellYield += productYield;
		scaleConfig->formatDecimalValue(productYield, result, yieldPrecision);
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("3)   productYield = ").append(result);
			Debug::trace("CuttingTestReport", msgDebug2);

			HobString msgDebug3 = msgDebug;
			msgDebug3.append("totalSellYield = ").append(Integer::toString(totalSellYield));
			Debug::trace("CuttingTestReport", msgDebug3);
		}
		yieldList.push_back(result);

		Product *theProduct = Product::findRecord(cutTestTotal->getProductNumber(), cutTestTotal->getDeptNum(), dbAccess);
		if (theProduct)
		{
			scaleConfig->formatPrice(theProduct->getUnitPrice(), result);
			unitPriceList.push_back(result);
			pluList.push_back(Integer::toString(theProduct->getProductNum()));

			result = theProduct->getDescription();		//	We should clip this
#if 1
			result.replaceChar(_LF_, ' ');
			result.replaceChar(_CR_, ' ');
#else
			result.stripChar(_LF_);			//	We really should replace with a space
			result.stripChar(_CR_);			//	We really should replace with a space
#endif
			result.stripFontSequences();
			if (result.length() > maxDesc)
				result.resize(maxDesc);
			result.quoteForCSV();
			descList.push_back(result);

			delete theProduct;
		}
		else
		{
			unitPriceList.push_back("");
			pluList.push_back("");
			descList.push_back("");
		}
	}
}


void CuttingTestReport::finalize()
{
	short yieldPower = Configuration::getInstance()->getWeightPrecision() + 1;
	long yieldFactor = Power::power(10, yieldPower);

	if (itemWeight)
		totalShrinkWt = itemWeight - totalSellWt;
	totalShrinkYield = 1000 - totalSellYield;		//	100.0 - xx.x

	if (itemWeight)
		avgUnitPrice = (itemCost * yieldFactor) / itemWeight;
	avgUnitPrice = Rounding::roundConventionalPriceWeight(avgUnitPrice, 3, 0, 2);

	yieldPower = 4;
	yieldFactor = Power::power(10, yieldPower);
	if (totalSellPrice)
		percentProfit = ((totalSellPrice - itemCost) * yieldFactor) / totalSellPrice;
	percentProfit = Rounding::roundConventionalPriceWeight(percentProfit, 2, 0, 1);
}

const char *CuttingTestReport::serialize(int format) const
{
	serializedData = "";

	std::list<CuttingTestReportFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData);

				serializedData.append(fieldData);
				fieldIter++;
			}
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(startTime.formatDateTime());
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(endTime.formatDateTime());


			serializedData.append(BrowserFormatSeparator);
			serializedData.append(pluList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(descList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(weightList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(yieldList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(unitPriceList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(totalPriceList.serialize(format));
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

void CuttingTestReport::getFieldAsString(int fieldCode, HobString &result) const
{
//	bool traceOn = Debug::isTracing("CuttingTestReport.getFieldAsString");
//	HobString msgDebug = "getFieldAsString(";
//	msgDebug.append(Integer::toString(fieldCode)).append("):  ");

	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCTReportNum:
			result = cutTestNum;
			break;
		case fieldOffsetCTReportBuyItem:
			result = buyItem;
			break;
		case fieldOffsetCTReportVendorNum:
			result = vendorNum;
			break;
		case fieldOffsetCTReportOperator:
			result = operId;
			break;
		case fieldOffsetCTReportTotalSellWeight:
			Configuration::getInstance()->formatWeight(totalSellWt, result);
			break;
		case fieldOffsetCTReportTotalShrinkWeight:
			Configuration::getInstance()->formatWeight(totalShrinkWt, result);
			break;
		case fieldOffsetCTReportTotalItemWeight:
			Configuration::getInstance()->formatWeight(itemWeight, result);
			break;

		case fieldOffsetCTReportTotalSellYield:
			Configuration::getInstance()->formatDecimalValue(totalSellYield, result, 1);
			break;
		case fieldOffsetCTReportTotalShrinkYield:
			Configuration::getInstance()->formatDecimalValue(totalShrinkYield, result, 1);
			break;
		case fieldOffsetCTReportTotalItemYield:
			Configuration::getInstance()->formatDecimalValue(1000, result, 1);			//	100.0
			break;

		case fieldOffsetCTReportAvgPricePerUnitWt:
			Configuration::getInstance()->formatValue(avgUnitPrice, result, Configuration::getInstance()->getPricePrecision());
			break;
		case fieldOffsetCTReportTotalSellPrice:
			Configuration::getInstance()->formatValue(totalSellPrice, result, Configuration::getInstance()->getPricePrecision());
			break;
		case fieldOffsetCTReportTotalItemCost:
			Configuration::getInstance()->formatValue(itemCost, result, Configuration::getInstance()->getPricePrecision());
			break;
		case fieldOffsetCTReportPercentProfit:
			Configuration::getInstance()->formatDecimalValue(percentProfit, result, 1);
			break;
		default:
			break;
	}
}
