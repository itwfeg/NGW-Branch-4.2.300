/* HT Scale Project
 * COPYRIGHT (c) 2020.  All Rights Reserved.
 * Hobart, ITWFEG. Troy, Ohio  USA
 */

#include "MetSig.h"
#include "Rounding.h"
#include "Configuration.h"
#include "SessionManagerHelper.h"
#include "WeigherStatus.h"
#include "HfcWeigherStatus.h"
#include <math.h>
#include "Power.h"
#include "HobString.h"
#include "Debug.h"
#include "Double.h"
#include "Long.h"

using namespace std;

WeighMode MetSig::currentWeighMode = WeighMode_Avoir;
WgDualRangeDirection MetSig::currentDualRangeDirection = DualRangeUp;

Util::CSmartCriticalSection  MetSig::metSigCS_calculateUnitPriceRounded;
Util::CSmartCriticalSection  MetSig::metSigCS_calculateUnitPriceNoFractionalCents;
Util::CSmartCriticalSection  MetSig::metSigCS_calcFixedWeightPricePerUnit;
Util::CSmartCriticalSection  MetSig::metSigCS_divideAndRoundPortions;
Util::CSmartCriticalSection  MetSig::metSigCS_byCountCalcTotalPrice;
Util::CSmartCriticalSection  MetSig::metSigCS_determineImplicitDecimalPrice;

Util::CSmartCriticalSection  MetSig::metSigCS_setScaleWeighMode;
Util::CSmartCriticalSection  MetSig::metSigCS_setDualRangeDirection;
Util::CSmartCriticalSection  MetSig::metSigCS_checkToSwitchDualRangeDirection;
Util::CSmartCriticalSection  MetSig::metSigCS_checkToSwitchDualRangeDirectionOnNetWeight;

Util::CSmartCriticalSection  MetSig::metSigCS_isWeigherDualRangeCapable;
Util::CSmartCriticalSection  MetSig::metSigCS_applyScaleFactorToWeight;
Util::CSmartCriticalSection  MetSig::metSigCS_weightCountsToFixedWeight;
Util::CSmartCriticalSection  MetSig::metSigCS_ouncesToGrams;
Util::CSmartCriticalSection  MetSig::metSigCS_gramsToOunces;
Util::CSmartCriticalSection  MetSig::metSigCS_ouncesToLiters;
Util::CSmartCriticalSection  MetSig::metSigCS_BRConvertFixedWeightToTotalWeight;
Util::CSmartCriticalSection  MetSig::metSigCS_determineImplicitDecimalWeight;
Util::CSmartCriticalSection  MetSig::metSigCS_determineImplicitDecimalKeyboardNetWeight;
Util::CSmartCriticalSection  MetSig::metSigCS_roundWeightTo2DecimalsThenBackTo3;
Util::CSmartCriticalSection  MetSig::metSigCS_applyTare;
Util::CSmartCriticalSection  MetSig::metSigCS_applyTareSetback;
Util::CSmartCriticalSection  MetSig::metSigCS_applyProportionalTare;
Util::CSmartCriticalSection  MetSig::metSigCS_calculateRandomWeightTotalPrice;
Util::CSmartCriticalSection  MetSig::metSigCS_calculateRandomWeightOldTotalPrice;

bool MetSig::testingMetSig = false;


/***************************************************************************************************************************
****************************************************************************************************************************
*****
*****   THE FOLLOWING CODE IS CONSIDERED TO BE SEALABLE BY WEIGHTS & MEASURES; IT DETERMINES FIRST AND FINAL WEIGHT/PRICE.
*****   IF YOU CHANGE IT, YOU MUST CHANGE THE VERSION NUMBER DEFINED IN METSIG.H!!
*****
****************************************************************************************************************************
***************************************************************************************************************************/

string MetSig::getApplicationVersion()
{
	return inttostring(METROLOGICALLY_SIGNIFICANT_APPLICATION_VERSION);
}


/***************************************************************************************************************************
*****
*****   Price-related routines
*****
***************************************************************************************************************************/

//		calc


// Originally from Configuration.cpp
// Used by Configuration::formatUnitPriceForPrinting()
HobPrice MetSig::calculateUnitPriceRounded(const HobPrice unitPriceToUse, const int byCountToUse)
{
	Util::CLockCriticalSection lock(metSigCS_calculateUnitPriceRounded);

	double unitPriceD = ((double)unitPriceToUse) / byCountToUse;
	HobPrice roundedUnitPrice = Rounding::roundPerWM(unitPriceD);

	if ( Debug::isTracing("MetSig.calculateUnitPriceRounded") )
		Debug::trace("MetSig.calculateUnitPriceRounded", "calculateUnitPriceRounded(): unitPrice=" + longtostring(unitPriceToUse) + "  byCount=" + inttostring(byCountToUse) + "  roundedUnitPrice=" + longtostring(roundedUnitPrice));
	return roundedUnitPrice;
}


// Originally from Configuration.cpp
// Used by Configuration::formatUnitPriceForPrinting()
bool MetSig::calculateUnitPriceNoFractionalCents(HobPrice *calculatedUnitPrice, const HobPrice unitPriceToUse, const int byCountToUse)
{
	Util::CLockCriticalSection lock(metSigCS_calculateUnitPriceNoFractionalCents);

	bool result = false;
	*calculatedUnitPrice = 0;
	double unitPriceD = ((double)unitPriceToUse) / byCountToUse;
	long unitPriceL = (long)(unitPriceD * 10000);
	if ((unitPriceL % 10000) == 0)
	{
		*calculatedUnitPrice = (HobPrice)unitPriceD;
		result = true;
	}

	if ( Debug::isTracing("MetSig.calculateUnitPriceNoFractionalCents") )
		Debug::trace("MetSig.calculateUnitPriceNoFractionalCents", "calculateUnitPriceNoFractionalCents(): unitPrice=" + longtostring(unitPriceToUse) + "  byCount=" + inttostring(byCountToUse) + "  unitPriceL=" + longtostring(unitPriceL) +  "  calculatedUnitPrice=" + longtostring(*calculatedUnitPrice) + "  result=" + booltostring(result));
	return result;
}


// Originally from ProductTransaction.cpp
// Used by ProductTransaction::getFixedWeightUnitPrice(), ProductTransaction::getFixedWeightDiscountUnitPrice1(), ProductTransaction::getFixedWeightDiscountUnitPrice2(),
//         ProductTransaction::getFixedWeightDiscountUnitPrice3(), FrequentShopper::calculateDiscountUnitPrice()
HobPrice MetSig::calcFixedWeightPricePerUnit(const HobPrice thePrice, const HobWeight theFixedWeight)
{
	Util::CLockCriticalSection lock(metSigCS_calcFixedWeightPricePerUnit);

	/*  The commented-out code below runs through combinations of price and weight (one loop each for metric and pounds)
	*  to show what the unrounded unit prices would be when the fractional parts of a penney are at least .500 and .5000.
	*  This shows that you can't use longs when calculating the unit price because the code Angel L. developed to fix the Quantum
	*  only went to 3 places past the decimal point, resulting in fractional pennies of .5001 appearing to the code to be exactly .500,
	*  in which case the rule D rounding was used, when in reality, rule C should have been used.
	*
	if (thePrice == 100)
	{
	Debug::trace("ProductTransaction.fixedWeight", "start searching (kg)");
	double p = 1;
	while (p < 1000.0)
	{
	double w = 0.001;
	while (w < 1.001)
	{
	double up = p / w;
	HobString asdf = Double::toString(up,20,20);
	size_t dotpos = asdf.find(".");
	size_t a500pos = asdf.find("500");
	size_t a5000pos = asdf.find("5000");
	if (dotpos != string::npos && a5000pos!= string::npos && a5000pos == dotpos+1)
	Debug::trace("ProductTransaction.fixedWeight", "*******************) total price = " + HobString(Double::toString(p,2,2)) + "   weight = " + HobString(Double::toString(w,3,3)) + "   unit price = " + HobString(Double::toString(up,20,20)));
	else if (dotpos != string::npos && a500pos!= string::npos && a500pos == dotpos+1)
	Debug::trace("ProductTransaction.fixedWeight", "*****) total price = " + HobString(Double::toString(p,2,2)) + "   weight = " + HobString(Double::toString(w,3,3)) + "   unit price = " + HobString(Double::toString(up,20,20)));

	w += 0.001;
	}

	p += 1;
	}
	Debug::trace("ProductTransaction.fixedWeight", "done searching (kg)");

	Debug::trace("ProductTransaction.fixedWeight", "start searching (ounces)");
	p = 1;
	while (p < 1000.0)
	{
	double w = (double)1;
	while (w < (double)10000)
	{
	double up = p * 16 / w;
	HobString asdf = Double::toString(up,20,20);
	size_t dotpos = asdf.find(".");
	size_t a500pos = asdf.find("500");
	size_t a5000pos = asdf.find("5000");
	if (dotpos != string::npos && a5000pos!= string::npos && a5000pos == dotpos+1)
	Debug::trace("ProductTransaction.fixedWeight", "*******************) total price = " + HobString(Double::toString(p,2,2)) + "   weight = " + HobString(Double::toString(w,3,3)) + "   unit price = " + HobString(Double::toString(up,20,20)));
	else if (dotpos != string::npos && a500pos!= string::npos && a500pos == dotpos+1)
	Debug::trace("ProductTransaction.fixedWeight", "*****) total price = " + HobString(Double::toString(p,2,2)) + "   weight = " + HobString(Double::toString(w,3,3)) + "   unit price = " + HobString(Double::toString(up,20,20)));

	w += 1;
	}

	p += 1;
	}
	Debug::trace("ProductTransaction.fixedWeight", "done searching (ounces)");
	}
	*/

	Configuration *config = Configuration::getInstance();
	if (!config)
		return 0;

	bool traceOn = Debug::isTracing("MetSig.calcFixedWeightPricePerUnit");
	HobPrice returnVal = 0;

	int pricePrec = config->getPricePrecision();
	int weightPrec = config->getWeightPrecision();
	double wtFactor = 16.0;

	if (MetSig::isScaleMetric())
		wtFactor = Power::power(10, (weightPrec));
	else if (config->isFixedWeightInTenths())
		wtFactor *= 10;

	double pricePerUnit = (thePrice * wtFactor) / theFixedWeight;

	if (traceOn)
		Debug::trace("MetSig.calcFixedWeightPricePerUnit", string("price=") + longtostring(thePrice) + "  fixedWeight=" + longtostring(theFixedWeight) + 
						"  wtFactor=" +longtostring((long)wtFactor) + "  pricePerUnit to be rounded=" + doubletostring(pricePerUnit, 20, 20));

	HobValue lPricePerUnit = Rounding::roundPerWM(pricePerUnit);

	// When testing for SOF-5333, I determined that the following if-else clause will have no affect on the result being returned,
	// since we just rounded per W&M directly above, and based on the 3 parameters below after the lPricePerUnit, "pricePrec, 0, pricePrec",
	// all of the rounding methods that could be called would determine that there are no decimal places to round,
	// and just return the lPricePerUnit as is.  I'm just leaving it, in case there was some other code change that affected this.
	if (config->isInternationalRoundingLicensed())
	{
		// International Rounding
		returnVal = Rounding::roundPriceWeight(lPricePerUnit, pricePrec, 0, pricePrec, config->getCurrencyRoundingMethod());
	}
	else
	{
		// SOF-5333 - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
		returnVal = Rounding::roundPerWMPriceWeight(lPricePerUnit, pricePrec, 0, pricePrec);
	}

	if (traceOn)
	{
		HobString formattedTestVal;
		config->formatPrice(returnVal, formattedTestVal);
		Debug::trace("MetSig.calcFixedWeightPricePerUnit", "rounded pricePerUnit=" + longtostring(lPricePerUnit) + "  returnVal=$" + formattedTestVal + " per unit returning=" + longtostring(returnVal));
	}
	return returnVal;
}


// Originally from ProductTransaction.cpp
// Used by ProductTransaction::calculateAvgPortionData()
HobValue MetSig::divideAndRoundPortions(const long dividend, const int divisor, const short precision)
{
	Util::CLockCriticalSection lock(metSigCS_divideAndRoundPortions);

	bool traceOn = Debug::isTracing("MetSig.divideAndRoundPortions");
	HobValue result = (dividend * 10) / divisor;
	if (traceOn)
		Debug::trace("MetSig.divideAndRoundPortions", string("divideAndRoundPortions(): Divide  result=(") + longtostring(dividend) + " * 10) / " + inttostring(divisor) + " = " + longtostring(result));

	if (Configuration::getInstance()->isInternationalRoundingLicensed())
		// International Rounding
		result = Rounding::roundPriceWeight(result, precision, 1, precision, Configuration::getInstance()->getCurrencyRoundingMethod());
	else
		// SOF-5333 - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
		result = Rounding::roundPerWMPriceWeight(result, precision, 1, precision);

	if (traceOn)
		Debug::trace("MetSig.divideAndRoundPortions", string("divideAndRoundPortions(): dividend=") + longtostring(dividend) + "  divisor=" + inttostring(divisor) + "  precision=" + inttostring(precision) + "  result=" + longtostring(result));
	return result;
}


// Originally from ByCountCalcs.cpp
// Used by MetSig::byCountCalcTotalPrice()
long MetSig::calcByCountCost(const long qty_purch, const long deal_qty, const long deal_price, const long single_price, const short method)
{
	long even_deal_cost = 0, remainder_cost = 0, discount, hi_count, overflow;
	bool traceOn = Debug::isTracing("MetSig.bycountcalcs");
	string hdr = "calcByCountCost(): ";
	if (traceOn)
	{
		string sep = ", ";
		Debug::trace("MetSig.bycountcalcs", hdr + longtostring(qty_purch) + sep + longtostring(deal_qty) + sep + longtostring(deal_price) + sep + longtostring(single_price) + sep + inttostring(method));
	}

	if (deal_qty > 0)
	{
		even_deal_cost = (qty_purch / deal_qty) * deal_price;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "even_deal_cost=" + longtostring(even_deal_cost));

		if (method == 1)
		{
			overflow = qty_purch % deal_qty;
			discount = (single_price * deal_qty) - deal_price;
			hi_count = deal_qty - discount;

			if (traceOn)
				Debug::trace("MetSig.bycountcalcs", hdr + "method==1  overflow=" + longtostring(overflow) + "  discount=" + longtostring(discount) + "  hi_count=" + longtostring(hi_count));

			if ( hi_count < overflow )
				remainder_cost = (hi_count * single_price) + ((overflow - hi_count) * (single_price - 1));
			else
				remainder_cost = overflow * single_price;
		}
		else
		{
			remainder_cost = (qty_purch % deal_qty) * single_price;

			if (traceOn)
				Debug::trace("MetSig.bycountcalcs", hdr + "method != 1:  remainder_cost = (qty_purch % deal_qty) * single_price");
		}
	}

	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "remainder_cost = " + longtostring(remainder_cost) + "  returning = (even_deal_cost + remainder_cost) = " + longtostring(even_deal_cost + remainder_cost));
	return (even_deal_cost + remainder_cost);
}


// Originally from ByCountCalcs.cpp
// Used by MetSig::byCountCalcTotalPrice()
long MetSig::calcByCountCost2(const long qty_purch, const long deal_qty, const long deal_price, const long single_price)
{
	long theCost = 0, singleCost = 0;
	long part1 = 0, part2 = 0, part3 = 0;

	if (deal_qty > 0)
	{
		HobString myDebug("calcByCountCost2(");
		bool traceOn = Debug::isTracing("MetSig.bycountcalcs");
		string hdr = "calcByCountCost2(): ";
		if (traceOn)
		{
			string sep = ", ";
			Debug::trace("MetSig.bycountcalcs", hdr + longtostring(qty_purch) + sep + longtostring(deal_qty) + sep + longtostring(deal_price) + sep + longtostring(single_price));
		}

		part1 = (qty_purch / deal_qty) * deal_price;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "part1 = (qty_purch / deal_qty) * deal_price = " + longtostring(part1));

		singleCost = (qty_purch % deal_qty) * single_price;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "singleCost = (qty_purch % deal_qty) * single_price = " + longtostring(singleCost));

		part2 = singleCost / 100L;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "part2 = singleCost / 100 = " + longtostring(part2));

		if ((singleCost % 100L) != 0)
			part3 = 1;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "part3 = (singleCost % 100L) != 0) = " + longtostring(part3));

		theCost = part1 + part2 + part3;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "theCost = part1 + part2 + part3 = " + longtostring(theCost));
	}
	return theCost;
}


// Originally from ByCountCalcs.cpp
// Used by MetSig::byCountCalcTotalPrice()
long MetSig::calcGCD(long num1, long num2)
{
	bool traceOn = Debug::isTracing("MetSig.bycountcalcs");
	string hdr = "calcGCD(): ";

	if ((num1 == 0) && (num2 == 0))
		return 1;

	long temp;
	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "num1=" + longtostring(num1) + "  num2=" + longtostring(num2));

	while (num2 != 0)
	{
		temp = num1  % num2;
		num1 = num2;
		num2 = temp;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "num1=" + longtostring(num1) + "  num2=" + longtostring(num2));
	}
	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "returning num1=" + longtostring(num1));
	return num1;
}


// Originally from ByCountCalcs.cpp
// Used by MetSig::byCountCalcTotalPrice()
long MetSig::calcSinglePrice(const long deal_qty, const long deal_price)
{
	bool traceOn = Debug::isTracing("MetSig.bycountcalcs");
	string hdr = "calcSinglePrice(): ";
	if (traceOn)
		Debug::trace("MetSig_ByCountCalcs", hdr + "deal_qty=" + longtostring(deal_qty) + "  deal_price=" + longtostring(deal_price));

	long temp = 0;

	if (deal_qty > 0)
	{
		temp = deal_price / deal_qty;
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "temp = deal_price / deal_qty = " + longtostring(temp));

		if ((deal_price % deal_qty) != 0)
		{
			temp = temp + 1;
			if (traceOn)
				Debug::trace("MetSig.bycountcalcs", hdr + "deal_price % deal_qty != 0, increment temp = " + longtostring(temp));
		}
	}
	return (temp);
}


// Originally from ByCountCalcs.cpp
// Used by Product::setOpModeByCount() and FrequentShopper::calculateDiscounts()
HobPrice MetSig::byCountCalcTotalPrice(const int originalByCount, int byCount, const HobPrice unitPrice, const HobPrice exceptionPrice)
{
	Util::CLockCriticalSection lock(metSigCS_byCountCalcTotalPrice);

	bool traceOn = Debug::isTracing("MetSig.bycountcalcs");
	string hdr = "byCountCalcTotalPrice(): ";
	HobString myDebug;

	long great_comm_div = 1;
	long single_price;
	long deal_qty;
	long deal_price;
	short method = Configuration::getInstance()->getByCountCalculationMethod();

	if (byCount == 0)
		byCount = originalByCount;

	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "byCount=" + inttostring(byCount) + "  Unit price=" + longtostring(unitPrice));

	HobPrice totalPrice = 0;

	if (exceptionPrice == 0)
		great_comm_div = calcGCD(originalByCount, unitPrice);
	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "great_comm_div=" + longtostring(great_comm_div));

	deal_price = unitPrice / great_comm_div;
	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "deal_price=" + longtostring(deal_price));

	deal_qty = originalByCount / great_comm_div;
	if (traceOn)
		Debug::trace("MetSig.bycountcalcs", hdr + "deal_qty=" + longtostring(deal_qty));

	if (deal_price || (byCount == 0))
	{
		if (exceptionPrice == 0)
		{
			if ((deal_qty > deal_price) || (method == 3))
			{
				single_price = calcSinglePrice(deal_qty, (deal_price * 100L));
				totalPrice = calcByCountCost2(byCount, deal_qty, deal_price, single_price);
			}
			else
			{
				single_price = calcSinglePrice(deal_qty, deal_price);
				totalPrice = calcByCountCost(byCount, deal_qty, deal_price, single_price, method);
			}
		}
		else
		{
			method = 2;

			single_price = exceptionPrice;
			totalPrice = calcByCountCost(byCount, deal_qty, deal_price, single_price, method);
		}
		if (traceOn)
			Debug::trace("MetSig.bycountcalcs", hdr + "single_price=" + longtostring(single_price) + "  totalPrice=" + longtostring(totalPrice));
	}
	return totalPrice;
}


// Used for converting a received price value with an explicit decimal point to a price value with an implied decimal point
// Originally in-line code that I converted to a method during the MetSig project; code that used getPriceDivisor()
// Used by HfcProductUpdateValuesMsg CTOR, Configuration::setSecurityLblUnitPrice(), Configuration::setSecurityLblTtlPrice(),  CuttingTest::updateField( fieldOffsetCuttingTestPrimalCost ),
//         Product::applyRulesToReceivedFields(), Product::updateField( fieldOffsetTotalPriceMin ), Product::updateField( fieldOffsetTotalPriceMax ),
//         RemotePrintRequest::updateField( fieldOffsetRemotePrintUnitPrice ), Softkey::parseActionParameter( Action_ApplyDiscount_CentsOff ), Total::updateField( fieldOffsetTotalPrice )
HobPrice MetSig::determineImplicitDecimalPrice(const string priceStr)
{
	Util::CLockCriticalSection lock(metSigCS_determineImplicitDecimalPrice);

	HobPrice resultPrice = 0;
	if ( priceStr.find(".") == string::npos )
		resultPrice = Long::valueOf(priceStr);
	else
	{
		double dPrice = Double::valueOf(priceStr);
		dPrice *= Configuration::getInstance()->getPriceDivisor();
		dPrice = Rounding::hobround(dPrice);
		resultPrice = (HobPrice)dPrice;
	}

	Debug::trace("MetSig.price", string("determineImplicitDecimalPrice  input=") + priceStr + "  result=" + longtostring(resultPrice));
	return resultPrice;
}



/***************************************************************************************************************************
*****
*****   Weight-related routines
*****
***************************************************************************************************************************/

/*
Search for

 / 10
 + 5
hobround
getRawCounts
setRawCounts
getCounts
getBaseCounts
setCounts
getRelativeCounts
getWeight
setTotalNetWeight
setFinalNetWeight


valuemax
dualrange
dualrangedirection
WeightDivisor
WeightPrecision
1000.0, 1000

not going to worry about any calculations in CuttingTestReport
not going to worry about any calculations in FrequentShopper
*/


// MetSig is keeping track of the current weigh mode (Avoir or metric)
// Originally from Configuration.cpp
WeighMode MetSig::getScaleWeighMode()
{
	return currentWeighMode;
}

// Originally from Configuration.cpp
void MetSig::setScaleWeighMode(const WeighMode mode)
{
	Util::CLockCriticalSection lock(metSigCS_setScaleWeighMode);

	currentWeighMode = mode;
}

// Originally from Configuration.cpp
// Used by many
bool MetSig::isScaleAvoir()
{
	return (currentWeighMode == WeighMode_Avoir) ? true : false;
}

// Originally from Configuration.cpp
// Used by many
bool MetSig::isScaleMetric()
{
	return (currentWeighMode == WeighMode_Metric) ? true : false;
}


// Originally from Configuration.cpp and CANWeigherManager.cpp (combined them)
// Used by AutoWeighEvolWrapperMode::handleWeigherMessage(), CANWeigherManager::handleWeigherEvent(), and WeigherManager::handleWeigherEvent() methods
bool MetSig::isWeigherDualRangeCapable(const WeigherModel model)
{
	Util::CLockCriticalSection lock(metSigCS_isWeigherDualRangeCapable);

	WeigherModel myModel = model;
	if (myModel == endWeigherModel)  // 'endWeigherModel' is the default if nothing is passed in
	{
		if ( SessionManagerHelper::getInstance()->isWeigherAttached() )
			myModel = Configuration::getInstance()->getWeigherModel();
		else
			return true;
	}

	if (myModel == WmKPCWeigher || myModel == WmHLX1 || myModel == WmAWS || myModel == WmSASCALE2 || myModel == WmNone)
		return true;
	else
		return false;
}


// Originally from Configuration.cpp
// Used by PrinterManager::updateFormPostImageFields(), HfcProductUpdateValuesMsg::determineDisplayValues(), BarCodeNumber::buildEANBarcodeString(),
//         BarCodeNumber::buildGS1BarcodeStringPart(), CfgScale::getFieldAsString(  fieldOffsetCfgIsSingleRangeWeigher  ), Configuration::formatWeight(), Configuration::getEffectiveWeightPrecision()
bool MetSig::isWeigherConfiguredForSingleRange()
{
	UINT32 scaleFactor = Configuration::getInstance()->getWeigherScaleFactor();

	if (MetSig::isScaleMetric())
	{
		return (scaleFactor == 5 || scaleFactor == 0) ? true : false;
	}
	else if (MetSig::isWeigherDualRangeCapable())
	{
		return (scaleFactor == 1 || scaleFactor == 0) ? true : false;
	}
	else
		return true;
}


// Originally from Configuration.cpp
// Used by OperatingMode::getMaxAWSPackageWeight(), GCDSession::noMessageFromMessengerProcessingImpl(), HfcProductUpdateValuesMsg::determineDisplayValues()
bool MetSig::isWeigherConfiguredForDualRange()
{
	return !isWeigherConfiguredForSingleRange();
}


// MetSig is keeping track of the current dual-range direction
//        (up - if weight was previously zero and current weight hasn't gone past the dual range limit, or 
//         down - if weight has gone past the dual-range limit and we haven't returned to zero)
// Originally, Session maintained dualRangeDirection, getter was available in Session.h
// Used by no one currently, all usages use the method in WeigherStatus
// Also previously, and still currently, exists in WeigherStatus (which is in MetSig)
WgDualRangeDirection MetSig::getDualRangeDirection()
{
	return currentDualRangeDirection;
}


void MetSig::setDualRangeDirection(const WgDualRangeDirection direction)
{
	Util::CLockCriticalSection lock(metSigCS_setDualRangeDirection);

	Debug::trace("MetSig.dualrange", string("setDualRangeDirection - current dualRangeDirection=") + inttostring(currentDualRangeDirection) + "  new dualRangeDirection=" + inttostring(direction));
	currentDualRangeDirection = direction;
}


// Originally in-line code in Session.cpp when handling a weigherStatus, that I converted to a method during the MetSig project
// Used by Session::handleWeigherMessage( handling WgStatus ) and Product::updateTotalValuesData()
bool MetSig::checkToSwitchDualRangeDirection(HfcWeigherStatus *newWeight, bool hasCurrentProductInOperateMode)
{
	Util::CLockCriticalSection lock(metSigCS_checkToSwitchDualRangeDirection);

	bool changedDualRangeDirection = false;

	//	Dual range is only available on the HLX and new USB weigher peripheral with dual-range load cell. (haberrc)
	Debug::trace("MetSig.dualrange", string("checkToSwitchDualRangeDirection - Session handling WgStatus - current dualRangeDirection=") + inttostring(currentDualRangeDirection));

	if (MetSig::isWeigherConfiguredForDualRange())
	{
		newWeight->setDualRangeEnabled(true);

		// We can reset the direction to DualRangeUp here, before allowing the ProductManager to handle the weight,
		// since we only have to consider a stable zero to switch from DualRangeDown to DualRangeUp (tare plays no role in this).
		if (currentDualRangeDirection == DualRangeDown)
		{
			if (newWeight->isZeroStableWeight())
			{
				currentDualRangeDirection = DualRangeUp;
				changedDualRangeDirection = true;
				Debug::trace("MetSig.dualrange", "checkToSwitchDualRangeDirection - switching from dual range down to dual range up");
			}
			newWeight->setDualRangeDirection(currentDualRangeDirection);
		}
		else  // == DualRangeUp
		{
			// if using gross weight to determine when to switch from dualRangeUp to dualRangeDown mode (when the weight goes over the dual range limit)
			// then we can make that switch from up to down right now.  This will help the rounding of tare when in the dualRangeDown mode;
			// the mode will be set correctly before total values are updated in the current product transaction.  If we are using net weight
			// to determine the switch over from dualRangeUp to dualRangeDown mode, that will have to be checked when the Product updates
			// the total values, a part of the logic in the productManager->handleWeight() call.

			HobWeight weight = newWeight->getWeight();

			Configuration *config = Configuration::getInstance();
			if (!config->isNetWeightDualRangeLimitCheck() || !hasCurrentProductInOperateMode)
			{
				// FT-1388: dual range limit is at or above
				if ((MetSig::isScaleMetric() && weight >= config->toIntCfgI(CfgInternal::DUALRANGELIMITMETRIC))
					|| (MetSig::isScaleAvoir() && weight >= config->toIntCfgI(CfgInternal::DUALRANGELIMITAVOIR)))
				{
					Debug::trace("MetSig.dualrange", "checkToSwitchDualRangeDirection - switching from dual range up to dual range down");
					currentDualRangeDirection = DualRangeDown;
					changedDualRangeDirection = true;
					newWeight->setDualRangeDirection(currentDualRangeDirection);
				}
			}
		}
	}

	Debug::trace("MetSig.dualrange", string("checkToSwitchDualRangeDirection - leaving method - current dualRangeDirection=") + inttostring(currentDualRangeDirection));
	return changedDualRangeDirection;
}


// Originally in-line code in Product.cpp when handling a weigherStatus, that I converted to a method during the MetSig project
// Used by Product::updateTotalValuesData( )
bool MetSig::checkToSwitchDualRangeDirectionOnNetWeight(HfcWeigherStatus *weigherStatus, const HobWeight netWeight)
{
	Util::CLockCriticalSection lock(metSigCS_checkToSwitchDualRangeDirectionOnNetWeight);

	bool changedDualRangeDirection = false;

	/* Here is where we can determine if we have to switch the dualRangeDirection from up to down because the weight goes over the dualRangeLimit,
	* when we are using the net weight as the weight for the limit check.  When we are using the gross weight, the Session has already determined
	* in which direction we are moving, up or down, since in this case, the tare doesn't play any part in determining if we are over the dual range limit.
	* We now know the net weight, and can use it to determine if we are past the dualRangeLimit if the configuration parameter that indicates
	* that we should use the net weight to make this determination is set.
	*/

	if (weigherStatus->isDualRangeEnabled() && weigherStatus->getDualRangeDirection() == DualRangeUp)
	{
		// if the dual range direction is going up, considering 3 decimal places in the weight, then
		// check if we have gone past the range limit.  If we have, then set the flag to indicate
		// we are now going down, in which case, we will only use 2 decimal places until we reach
		// a zero, stable weight before we consider using 3 decimal places again.
		Configuration *config = Configuration::getInstance();
		if (config->isNetWeightDualRangeLimitCheck())
		{
			// FT-1388: dual range limit is at or above
			if ((MetSig::isScaleMetric() && netWeight >= config->toIntCfgI(CfgInternal::DUALRANGELIMITMETRIC))
				|| (MetSig::isScaleAvoir() && netWeight >= config->toIntCfgI(CfgInternal::DUALRANGELIMITAVOIR)))
			{
				Debug::trace("MetSig.dualrange", "checkToSwitchDualRangeDirectionOnNetWeight - using net weight, and switching from dual range up to dual range down");

				currentDualRangeDirection = DualRangeDown;
				weigherStatus->setDualRangeDirection(DualRangeDown);
				changedDualRangeDirection = true;
			}
		}
	}
	return changedDualRangeDirection;
}



// Originally from Product.cpp
// Used by Product::getFieldAsString(  fieldOffsetTareAValue, fieldOffsetTareBValue  )
HobWeight MetSig::applyScaleFactorToWeight(HobWeight weight, UINT32 scaleFactor)
{
	Util::CLockCriticalSection lock(metSigCS_applyScaleFactorToWeight);

	// This is what the code did before 4/4/07, when W&M said the tare was always rounded up, and when in dualRange Down mode,
	// we need to round the tare up to .01 (2 decimal places, not 3)
#if 0
	// Round the weight passed in (tare, netWeight, ...) based on the scale factor
	if (scaleFactor == 5)
	{
		//Debug::trace("Product.applyScaleFactorToWeight", HobString("applyScaleFactorToWeight() - Applying scalefactor of 5"));
		// Adjust the weight to the scale factor; 5g or 0.005lb increments
		long remain = weight % 5;
		if (remain > 2)
			weight += (5 - remain);
		else
			weight -= remain;
	}
	else if (scaleFactor == 2)
	{
		//Debug::trace("Product.applyScaleFactorToWeight", HobString("applyScaleFactorToWeight() - Applying scalefactor of 2"));
		// Adjust the weight to the scale factor; 2g increments
		long remain = weight % 2;
		if (remain == 1)
			weight += 1;
	}
#endif

	bool isTracing = Debug::isTracing("MetSig.applyScaleFactorToWeight");

	// Round the weight passed in (tare, netWeight, ...) based on the scale factor
	if (isTracing)
	Debug::trace("MetSig.applyScaleFactorToWeight", string("applyScaleFactorToWeight() - inputs: weight=") + longtostring(weight) + "  scaleFactor=" + inttostring((int)scaleFactor));

	if (MetSig::isScaleAvoir())
	{
		if (scaleFactor == 5)
		{
			// Adjust the weight to the scale factor; 0.005lb increments, always rounding up!
			long remain = weight % 5;
			if (remain)
				weight += (5 - remain);
		}
		else
		{
			// Adjust the weight to the scale factor; 0.01lb increments, always rounding up!
			long remain = weight % 10;
			if (remain)
				weight += (10 - remain);
		}
	}
	else // metric
	{
		if (scaleFactor == 2)
		{
			// Adjust the weight to the scale factor; 2g increments
			long remain = weight % 2;
			if (remain == 1)
				weight += 1;
		}
		else // (scaleFactor == 5)
		{
			// Adjust the weight to the scale factor; 5g or 0.005lb increments
			long remain = weight % 5;
			if (remain)
				weight += (5 - remain);
		}
	}

	if (isTracing)
		Debug::trace("MetSig.applyScaleFactorToWeight", string("applyScaleFactorToWeight() - output: weight=") + Long::toString(weight));
	return weight;
}


// Originally from Configuration.cpp
// Used by ProductManager::handleMessage( event = ProductAddItemToAccumulatedItems ), ProductManager::handleWeight(), and ProductManager::handleProductPrintKey1( for handling RequireFixedWeightVerification )
HobWeight MetSig::weightCountsToFixedWeight(const HobWeight weight)
{
	Util::CLockCriticalSection lock(metSigCS_weightCountsToFixedWeight);

	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return 0;

	HobWeight fixedWeight = 0;

	if (MetSig::isScaleMetric())
	{
		// weight is in kg, and when formatting for fixed weight display (see formatFixedWeightValue() in this class), it is still in kg, so just use the weight passed in. Tenths of a gram is not supported.
		fixedWeight = weight;
	}
	else
	{
		// 1 lb = 1000 counts
		// weight is in pounds (with 3 implied decimal places.
		// Divide by 1000 to get to decimal value of pounds, then multiply by 16 to get to ounces, or 160 to get to tenths of an ounce (with 1 implied decimal place)
		HobWeight lbsMultiple = 16 * (cfg->isFixedWeightInTenths() ? 10 : 1);
		double theWeight = ((double)weight / 1000) * lbsMultiple;
		fixedWeight = (HobWeight)theWeight;

		if (Debug::isTracing("MetSig.weightCountsToFixedWeight"))
			Debug::trace("MetSig.weightCountsToFixedWeight", string("input weight=") + longtostring(weight) + "  double(theWeight)=" + Double::toString(theWeight, 20, 20));
	}

	if (Debug::isTracing("MetSig.weightCountsToFixedWeight"))
		Debug::trace("MetSig.weightCountsToFixedWeight", string("  resulting rounded fixedWeight=") + longtostring(fixedWeight));
	return fixedWeight;
}

// Originally from Configuration.cpp
// Used by ProductManager::handleMessage( event = ProductAddItemToAccumulatedItems ), ProductManager::handleWeight(), and ProductManager::handleProductPrintKey1( for handling RequireFixedWeightVerification )
HobWeight MetSig::fixedWeightToWeightCounts(const HobWeight fixedWeight)
{
	HobWeight counts = 0;

	if (MetSig::isScaleMetric())
	{
		//  The fixed weight is in grams, which is the same value of the weight with 3 implied decimal places, so no conversion needed.
		counts = fixedWeight;
	}
	else
	{
		// 1 lb = 1000 counts
		// Take the ounces passed in and divide by 16 (or the tenths of an ounce with 1 impled decimal place and divide by 160) to get the equivalent weight in pounds.
		// Then multiply by 1000 to get to 3 implied decimal places, and round by adding the .5
		// Since counts is a HobWeight (long), any final decimal places will be truncated off, which is why I rounded by adding the .5
		HobWeight lbsMultiple = 16 * (Configuration::getInstance()->isFixedWeightInTenths() ? 10 : 1);
		double theWeight = (double)fixedWeight / lbsMultiple;
		counts = (long)((theWeight * 1000) + .5);

		if (Debug::isTracing("MetSig.fixedWeightToWeightCounts"))
			Debug::trace("MetSig.fixedWeightToWeightCounts", string("input fixedWeight=") + longtostring(fixedWeight) + "  double(theWeight)=" + Double::toString(theWeight, 20, 20));
	}

	if (Debug::isTracing("MetSig.fixedWeightToWeightCounts"))
		Debug::trace("MetSig.fixedWeightToWeightCounts", string("  resulting rounded counts=") + longtostring(counts));
	return counts;
}

// Originally in-line code that I converted to a method during the MetSig project
// Used by Configuration::formatFixedWeightValue()
HobWeight MetSig::fixedWeightOuncesToDecimalWeight(const HobWeight fixedWeight)
{
	// This is for Avoir mode only
	HobWeight decimalWeight = 0;

	// 1 lb = 1000 counts
	// Take the ounces passed in and divide by 16 (or the tenths of an ounce with 1 implied decimal place and divide by 160) to get the equivalent weight in pounds.
	// Then multiply by 1000 to get to 3 implied decimal places
	HobWeight lbsMultiple = 16 * (Configuration::getInstance()->isFixedWeightInTenths() ? 10 : 1);

	double dTempWeight = (fixedWeight * Power::power((double)10, (double)Configuration::getInstance()->getWeightPrecision())) / lbsMultiple;
	decimalWeight = Rounding::roundPerWM(dTempWeight);
	if (Configuration::getInstance()->getEffectiveWeightPrecision() == 2)
		decimalWeight /= 10;	// Truncate the weight to 2-decimal digits; don't round since 18 oz = 1.125 lbs, rounded to 1.13 lbs for 2-decimal places, but 1.13 is more than 18 oz.

	if (Debug::isTracing("MetSig.fixedWeightOuncesToDecimalWeight"))
		Debug::trace("MetSig.fixedWeightOuncesToDecimalWeight", string("input fixedWeight=") + longtostring(fixedWeight) + "  double(dTempWeight)=" + Double::toString(dTempWeight, 20, 20) + "  resulting rounded decimalWeight=" + longtostring(decimalWeight));
	return decimalWeight;
}


// Originally in-line code that I converted to a method during the MetSig project
// Used by Configuration::formatWeight(  for MetricRuleWeight  )
HobWeight MetSig::ouncesToPoundCounts(HobWeight ounces, bool isInTenthsOfAnOunce)
{
	double ouncesPerPound = (isInTenthsOfAnOunce) ? 160.0 : 16.0;
	double myPounds = ounces / ouncesPerPound * 10000;

	if (Debug::isTracing("MetSig.ouncesToPoundCounts"))
		Debug::trace("MetSig.ouncesToPoundCounts", string("input ounces=") + longtostring(ounces) + "  double(myPounds)=" + Double::toString(myPounds, 20, 20) + "  resulting rounded pounds=" + longtostring((HobWeight)myPounds));
	return (HobWeight)myPounds;
}


// Originally from Configuration.cpp
// Used by Configuration::formatWeight(  for MetricRuleWeight  )
HobWeight MetSig::ouncesToGrams(const HobWeight ounces)
{
	Util::CLockCriticalSection lock(metSigCS_ouncesToGrams);

	double tempGrams = 0;
	if (ounces)
	{
		if (Configuration::getInstance()->isFixedWeightInTenths())
			tempGrams = ((double)ounces * 283495.0) / 100000;
		else
			tempGrams = ((double)ounces * 283495.0) / 10000;
	}
	HobWeight lTempGrams = Rounding::roundPerWM(tempGrams);

	if ( Debug::isTracing("MetSig.ouncesToGrams") )
		Debug::trace("MetSig.ouncesToGrams", string("input ounces=") + longtostring(ounces) + "  double(tempGrams)=" + Double::toString(tempGrams, 20, 20) + "  resulting rounded grams=" + longtostring(lTempGrams));
	return lTempGrams;
}


// Originally from Configuration.cpp
// Used by Configuration::formatWeight(  for MetricRuleWeight  )
HobWeight MetSig::gramsToOunces(const HobWeight grams)
{
	// The output will automatically be in tenths of an ounce.
	Util::CLockCriticalSection lock(metSigCS_gramsToOunces);

	double tempOunces = 0;
	if (grams)
		tempOunces = ((double)grams / 283495.0) * 100000;

	HobWeight lTempOunces = Rounding::roundPerWM(tempOunces);

	if (Debug::isTracing("MetSig.gramsToOunces"))
		Debug::trace("MetSig.gramsToOunces", string("input grams=") + longtostring(grams) + "  double(tempOunces)=" + Double::toString(tempOunces, 20, 20) + "  resulting rounded ounces=" + longtostring(lTempOunces));
	return lTempOunces;
}


// Originally from Configuration.cpp
// Used by Configuration::formatVolume(  for MetricRuleWeight  )
HobWeight MetSig::fluidOuncesToLiters(const HobWeight totalOunces)
{
	Util::CLockCriticalSection lock(metSigCS_ouncesToLiters);

	double tempMl = 0;
	if (totalOunces)
	{
		if (Configuration::getInstance()->isFixedWeightInTenths())
			tempMl = ((double)totalOunces * 295735.0) / 100000;
		else
			tempMl = ((double)totalOunces * 295735.0) / 10000;
	}
	HobWeight lTempMl = Rounding::roundPerWM(tempMl);

	if (Debug::isTracing("MetSig.fluidOuncesToLiters"))
		Debug::trace("MetSig.fluidOuncesToLiters", string("input totalOunces=") + longtostring(totalOunces) + "  double(tempMl)=" + Double::toString(tempMl, 20, 20) + "  resulting rounded liters=" + longtostring(lTempMl));
	return lTempMl;
}

// Originally in-line code that I converted to a method during the MetSig project
// Used by Configuration::formatFixedVolumeValue()
long MetSig::fluidOuncesToGallons(const HobWeight fluidOunces)
{
	double gallons = (fluidOunces * 100.0) / 1280;
	long lGallons = Rounding::roundPerWM(gallons);

	if (Debug::isTracing("MetSig.fluidOuncesToGallons"))
		Debug::trace("MetSig.fluidOuncesToGallons", string("input fluidOunces=") + longtostring(fluidOunces) + "  resulting rounded gallons=" + longtostring(lGallons));
	return lGallons;
}

// Originally in-line code that I converted to a method during the MetSig project
// Used by Configuration::formatFixedVolumeValue()
long MetSig::fluidOuncesToQuarts(const HobWeight fluidOunces)
{
	double quarts = (fluidOunces * 100.0) / 320;
	long lQuarts = Rounding::roundPerWM(quarts);

	if (Debug::isTracing("MetSig.fluidOuncesToQuarts"))
		Debug::trace("MetSig.fluidOuncesToQuarts", string("input fluidOunces=") + longtostring(fluidOunces) + "  resulting rounded quarts=" + longtostring(lQuarts));
	return lQuarts;
}

// Originally in-line code that I converted to a method during the MetSig project
// Used by Configuration::formatFixedVolumeValue()
long MetSig::fluidOuncesToPints(const HobWeight fluidOunces)
{
	double pints = (fluidOunces * 100.0) / 160;
	long lPints = Rounding::roundPerWM(pints);

	if (Debug::isTracing("MetSig.fluidOuncesToPints"))
		Debug::trace("MetSig.fluidOuncesToPints", string("input fluidOunces=") + longtostring(fluidOunces) + "  resulting rounded pints=" + longtostring(lPints));
	return lPints;
}


// Originally from Product.cpp
// Used by Product::printCurrentTransaction(), Product::initProductTransaction()
// This function receives a fixed weight in ounces or grams and returns the same
// weight in pounds or kilos with 'getWeightPrecision' implicit decimal places.
HobWeight MetSig::BRConvertFixedWeightToTotalWeight(const HobWeight fixedWeight, const bool fluidOz)
{
	Util::CLockCriticalSection lock(metSigCS_BRConvertFixedWeightToTotalWeight);

	Configuration *config = Configuration::getInstance();
	short weightPrecision = config->getWeightPrecision();
	HobWeight returnWeight = 0;

	if (MetSig::isScaleMetric())
	{
		double grPerKg = 1000.0;
		returnWeight = (HobWeight)Rounding::hobround(Power::power(10, weightPrecision) * fixedWeight / grPerKg);
	}
	else
	{
		double ozPerUnit = fluidOz ? 128.0 : 16.0;
		if ( !fluidOz && config->isFixedWeightInTenths() )
			ozPerUnit *= 10.0;

		returnWeight = (HobWeight)Rounding::hobround(Power::power(10, weightPrecision) * fixedWeight / ozPerUnit);
	}

	if (Debug::isTracing("MetSig.BRConvertFixedWeightToTotalWeight"))
		Debug::trace("MetSig.BRConvertFixedWeightToTotalWeight", string("input fixedWeight=") + longtostring(fixedWeight) + "  is fluidOz=" + booltostring(fluidOz) + "  returnWeight=" + longtostring(returnWeight));
	return returnWeight;
}


// Originally in-line code that I converted to a method during the MetSig project
// Used by PrinterManager::updateFormPostImageFields( for Digimarc ),
//         BarCodeNumber::buildEANBarcodeString( for weight characters, 'W' ), BarCodeNumber::buildGS1BarcodeStringPart(  for weight characters, 'W' ),
//         Configuration::formatWeight( for MetricRuleWeight ) 3 times, Configuration::formatWeight( for DualRangeModeDown ),
HobWeight MetSig::divideWeightForSingleRange(const HobWeight weight)
{
	return weight / 10;

}

// Originally in-line code that I converted to a method during the MetSig project
// Used by Product::getFieldAsString( for fieldOffsetTareAValue, fieldOffsetTareBValue, fieldOffsetExceptionPriceTare, fieldOffsetWeightMin, and fieldOffsetWeightMax when getExternalWeightPrecision() = 2 )
double MetSig::divideWeightForSingleRangeD(const HobWeight weight)
{
	return weight / 10.0;

}

// Originally in-line code that I converted to a method during the MetSig project
// Used for converting a received weight value with an explicit decimal point to a weight value with an implied decimal point
// Used by Configuration::setTareSetback(), Configuration::setMaxTare(), ConfigurationManager::handleWeigherConfig( when receive MinWeightToPrint via comm (cat3),
//         CuttingTest::updateField( for fieldOffsetCuttingTestPrimalWeight ), Product::applyRulesToReceivedFields( for fieldOffsetTareAValue ),
//         Product::updateField( for fieldOffsetTareAValue, fieldOffsetTareBValue, fieldOffsetWeightMin, and fieldOffsetWeightMax ), Total::updateField( for fieldOffsetTotalWeight )
HobWeight MetSig::determineImplicitDecimalWeight(const string weightStr)
{
	Util::CLockCriticalSection lock(metSigCS_determineImplicitDecimalWeight);

	HobWeight resultWeight = 0;
	if (weightStr.find(".") == string::npos)
		resultWeight = Long::valueOf(weightStr);
	else
	{
		double dWeight = Double::valueOf(weightStr);
		dWeight *= Configuration::getInstance()->getWeightDivisor();
		dWeight = Rounding::hobround(dWeight);
		resultWeight = (HobWeight)dWeight;
	}

	Debug::trace("MetSig.weight", string("determineImplicitDecimalWeight  input=") + weightStr + "  result=" + longtostring(resultWeight));
	return resultWeight;
}


// Originally in-line code that I converted to a method during the MetSig project
// Used for converting a received keyboard net weight value with an explicit decimal point to a keyboard net weight value with an implied decimal point
// Used by Product::updateField( for fieldOffsetKeyboardNetWeight )
HobWeight MetSig::determineImplicitDecimalKeyboardNetWeight(const string weightStr)
{
	Util::CLockCriticalSection lock(metSigCS_determineImplicitDecimalKeyboardNetWeight);

	HobWeight resultWeight = 0;
	if (weightStr.find(".") == string::npos)
		resultWeight = Long::valueOf(weightStr);
	else
	{
		double dWeight = Double::valueOf(weightStr);
		dWeight *= ( Power::power((double)10.0, (double)Configuration::getInstance()->getKeyboardNetWeightPrecision()) );
		dWeight = Rounding::hobround(dWeight);
		resultWeight = (HobWeight)dWeight;
	}

	Debug::trace("MetSig.weight", string("determineImplicitDecimalKeyboardNetWeight  input=") + weightStr + "  result=" + longtostring(resultWeight));
	return resultWeight;
}


// Originally in-line code that I converted to a method during the MetSig project
// Used by Softkey::determineActionParameter( for Action_LoadTareSoftkey ), and SoftkeyManager::handleExecute( for Action_LoadTareSoftkey )
double MetSig::roundWeightTo2DecimalsThenBackTo3(const double weight)
{
	Util::CLockCriticalSection lock(metSigCS_roundWeightTo2DecimalsThenBackTo3);

	// Need to round the preset tare flashkey's value to 2 decimal places, then make it 3 implied decimal places again.
	// The subsequent call to formatWeight will get rid of the third decimal place.
	double temp = weight;
	temp /= 10.0;
	temp = Rounding::hobround(temp);
	temp *= 10.0;

	Debug::trace("MetSig.roundWeightTo2DecimalsThenBackTo3", "roundWeightTo2DecimalsThenBackTo3 - input=" + doubletostring(weight) + "  result=" + doubletostring(temp));
	return temp;
}


// Originally in-line code in Product.cpp that I converted to a method during the MetSig project
// Used by Product::updateTotalValuesData(), Product::updateOldTotalValuesData()
void MetSig::applyTare(HobWeight &netWeight, HobWeight &totalTare, const HfcWeigherStatus *weigherStatus)
{
	if (!weigherStatus)
		return;

	Util::CLockCriticalSection lock(metSigCS_applyTare);

	bool traceOn = Debug::isTracing("MetSig.netweight");
	if (traceOn)
		Debug::trace("MetSig.netweight", string("applyTare: input netweight=") + longtostring(netWeight) + "  input totalTare=" + longtostring(totalTare));

	// Per Gary Lameris, 5/30/06, the tare setback does not need to be rounded, since
	// it is entered at the scale and can't be sent down via comm.  It is assumed that
	// it will be entered at the scale already rounded to the precision of the weigher;
	// in 5g, 2g, or .005lb increments.  Thus, we only need to round the total tare.
	totalTare = MetSig::applyScaleFactorToWeight(totalTare, weigherStatus->getEffectiveScaleFactor());

	netWeight -= totalTare;

	if (traceOn)
		Debug::trace("MetSig.netweight", string("applyTare: new netWeight=") + longtostring(netWeight) + "  new totalTare=" + longtostring(totalTare));
}


// Originally in-line code in Product.cpp that I converted to a method during the MetSig project
// Used by Product::updateTotalValuesData(), Product::updateOldTotalValuesData()
void MetSig::applyTareSetback(HobWeight &netWeight, const HobWeight totalTare)
{
	Util::CLockCriticalSection lock(metSigCS_applyTareSetback);

	if ( SessionManagerHelper::getInstance()->isModeCorrectForApplyingTareSetback() || testingMetSig == true)
	{
		// Note: TareSetback affects the netWeight.  The tare shown on the screen is the value stored in
		//       the product record (at least that's what I've seen with an Ultima on a UWS).
		//       This should be added to the net weight only if a wrapper is attached!!
		//       Harry says this should only be applied when in AUTO_WEIGH_CYCLE.
		//       TareSetback is only applied if TareSetback is less than the current tare.

		HobWeight setbackTare = Configuration::getInstance()->getTareSetback();

		bool traceOn = Debug::isTracing("MetSig.netweight");
		if (traceOn)
			Debug::trace("MetSig.netweight", string("applyTareSetback: input netweight=") + longtostring(netWeight) + "  input totalTare=" + longtostring(totalTare) + " setbackTare=" + longtostring(setbackTare));

		if ((totalTare - setbackTare) > 0)
			netWeight += setbackTare;

		if (traceOn)
			Debug::trace("MetSig.netweight", string("applyTareSetback: new netWeight=") + longtostring(netWeight));
	}
}


// Originally in-line code in Product.cpp that I converted to a method during the MetSig project
// Used by Product::updateTotalValuesData(), Product::updateOldTotalValuesData()
void MetSig::applyProportionalTare(HobWeight &netWeight, HobWeight &totalTare, const int proportionalTare, const HfcWeigherStatus *weigherStatus)
{
	if (!weigherStatus)
		return;

	Util::CLockCriticalSection lock(metSigCS_applyProportionalTare);

	bool traceOn = Debug::isTracing("MetSig.netweight");
	if (traceOn)
		Debug::trace("MetSig.netweight", string("applyProportionalTare: inputs:  netweight=") + longtostring(netWeight) + "  totalTare=" + longtostring(totalTare) + "  proportionalTare=" + inttostring(proportionalTare));

	HobWeight propWeight = (HobWeight)(netWeight * ((double)proportionalTare / (pow(10.0, PROP_TARE_DEC_PRECISION) * 100.0)));

	// Have to apply the scale factor the proportional tare also.
	propWeight = MetSig::applyScaleFactorToWeight(propWeight, weigherStatus->getEffectiveScaleFactor());

	totalTare += propWeight;
	netWeight -= propWeight;

	if (traceOn)
		Debug::trace("MetSig.netweight", string("applyProportionalTare:  propWeight=") + longtostring(propWeight) +
										"  new netWeight=" + longtostring(netWeight) + "  new totalTare=" + longtostring(totalTare));
}


// Originally in-line code in Product.cpp that I converted to a method during the MetSig project
// Used by Product::updateTotalValuesData()
HobPrice MetSig::calculateRandomWeightTotalPrice(const bool hasKbdNetWeightLabel, const HobPrice unitPrice, const long priceModifier, const HobWeight netWeight)
{
	Util::CLockCriticalSection lock(metSigCS_calculateRandomWeightTotalPrice);

	Configuration *config = Configuration::getInstance();
	if (!config)
		return 0;

	bool traceOn = Debug::isTracing("MetSig.calcprice");
	if (traceOn)
		Debug::trace("MetSig.calcprice", string("calculateRandomWeightTotalPrice - inputs:  hasKbdNetWeightLabel=") + booltostring(hasKbdNetWeightLabel) + "  unitPrice=" + longtostring(unitPrice) + "  priceModifier=" + longtostring(priceModifier) + "  netWeight=" + longtostring(netWeight));


	HobValue64 newUnitPrice = unitPrice;
	if (config->isPoundsForPricingEnabled() && !hasKbdNetWeightLabel)
	{
		newUnitPrice = (unitPrice * 10) / priceModifier;
		if (traceOn)
			Debug::trace("MetSig.calcprice", string("calculateRandomWeightTotalPrice - updating total price, PoundsForPricingEnabled - newUnitPrice=") + longlongtostring(newUnitPrice));
	}
	else if (config->isFractionalPricingEnabled() && !hasKbdNetWeightLabel && priceModifier != UNIT_PRICING)
	{
		if (MetSig::isScaleMetric())
		{
			if (priceModifier == config->getHundredGramsFor())
				newUnitPrice = unitPrice * 10;
		}
		else
		{
			if (priceModifier == config->getQuarterPoundFor())
				newUnitPrice = unitPrice * 4;
			else if (priceModifier == config->getHalfPoundFor())
				newUnitPrice = unitPrice * 2;
		}
		if (traceOn)
			Debug::trace("MetSig.calcprice", string("calculateRandomWeightTotalPrice - updating total price, FractionalPricingEnabled, newUnitPrice=") + longlongtostring(newUnitPrice));
	}

	HobValue64 netWt64 = (HobValue64)netWeight;
	HobValue64 unitPrice64 = (HobValue64)newUnitPrice;
	HobValue64 totalPrice64 = netWt64 * unitPrice64;
	if (traceOn)
	{
		HobString msgDebug("totalPrice = netWeight * unitPrice = ");
		msgDebug.append(util_string::int64tostring(netWt64));
		msgDebug.append(" * ");
		msgDebug.append(util_string::int64tostring(unitPrice64));
		msgDebug.append(" = ");
		msgDebug.append(util_string::int64tostring(totalPrice64));
		Debug::trace("MetSig.calcprice", msgDebug);

		msgDebug = "testTotalPrice = ";
		msgDebug.append(util_string::int64tostring((HobValue64)netWeight * (HobValue64)newUnitPrice));
		Debug::trace("MetSig.calcprice", msgDebug);
	}

	if (config->isPoundsForPricingEnabled() && !hasKbdNetWeightLabel)
	{
		totalPrice64 /= 10;
		if (traceOn)
			Debug::trace("MetSig.calcprice", string("calculateRandomWeightTotalPrice - updating total price, PoundsForPricingEnabled, totalPrice=") + int64tostring(totalPrice64));
	}
	// HTA-170
	short precision = (hasKbdNetWeightLabel) ? config->getKeyboardNetWeightPrecision() : config->getWeightPrecision();

	HobValue64 roundedTotalPrice64;
	if (config->isInternationalRoundingLicensed())
	{
		// International Rounding
		roundedTotalPrice64 = Rounding::roundPriceWeight(totalPrice64, config->getPricePrecision(), precision, config->getPricePrecision(), config->getCurrencyRoundingMethod());
	}
	else
	{
		// SOF-5333 - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
		roundedTotalPrice64 = Rounding::roundPerWMPriceWeight(totalPrice64, config->getPricePrecision(), precision, config->getPricePrecision());
	}

	// End HTA-170
	// line below is before HTA-170
	//HobValue64 roundedTotalPrice64 =  Rounding::RoundConventionalPriceWeight(totalPrice64, scaleConfig->getPricePrecision(), scaleConfig->getWeightPrecision(), scaleConfig->getPricePrecision());
	if (traceOn)
		Debug::trace("MetSig.calcprice", string("calculateRandomWeightTotalPrice - roundedTotalPrice64=") + int64tostring(roundedTotalPrice64));

	HobValue roundedTotalPrice = (HobValue)roundedTotalPrice64;

	if (traceOn)
		Debug::trace("MetSig.calcprice", string("updating total price - totalvalue=") + int64tostring(totalPrice64) + "  roundedTotalPrice=" + longtostring(roundedTotalPrice));

	return roundedTotalPrice;
}


// Originally in-line code in Product.cpp that I converted to a method during the MetSig project
// Used by Product::updateOldTotalValuesData()
HobPrice MetSig::calculateRandomWeightOldTotalPrice(const bool hasKbdNetWeightLabel, const HobPrice unitPrice, const long priceModifier, const HobWeight netWeight)
{
	Util::CLockCriticalSection lock(metSigCS_calculateRandomWeightOldTotalPrice);

	Configuration *config = Configuration::getInstance();
	if (!config)
		return 0;

	bool traceOn = Debug::isTracing("MetSig.calcprice");

	HobValue newUnitPrice = unitPrice;
	if (config->isPoundsForPricingEnabled() && !hasKbdNetWeightLabel)
		newUnitPrice = (newUnitPrice * 10) / priceModifier;

	if (traceOn)
		Debug::trace("MetSig.calcprice", "updated old unit price is: " + longtostring(newUnitPrice));

	HobValue totalPrice = netWeight * newUnitPrice;
	if (config->isPoundsForPricingEnabled() && !hasKbdNetWeightLabel)
		totalPrice /= 10;

	if (traceOn)
		Debug::trace("MetSig.calcprice", "old total price before rounding is: " + longtostring(totalPrice));

	HobValue roundedTotalPrice;
	if (config->isInternationalRoundingLicensed())
	{
		// International Rounding
		roundedTotalPrice = Rounding::roundPriceWeight(totalPrice, config->getPricePrecision(), config->getWeightPrecision(), config->getPricePrecision(), config->getCurrencyRoundingMethod());
	}
	else
	{
		// SOF-5333 - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using
		roundedTotalPrice = Rounding::roundPerWMPriceWeight(totalPrice, config->getPricePrecision(), config->getWeightPrecision(), config->getPricePrecision());
	}

	return roundedTotalPrice;
}



// private
MetSig::MetSig()
{
}



/***************************************************************************************************************************
*****
*****   Testing methods
*****
***************************************************************************************************************************/

void MetSig::test_allMetSig()
{
	if (!Debug::isTracing("MetSig.test"))
		return;

	if (MetSig::testingMetSig)
		return;

	MetSig::testingMetSig = true;

	Debug::trace("MetSig.test", "******************** MetSig::test_allMetSig() START ********************");

	// Price calculation routines

	MetSig::test_calculateUnitPriceRounded();
	MetSig::test_calculateUnitPriceNoFractionalCents();
	MetSig::test_calcFixedWeightPricePerUnit();
	MetSig::test_divideAndRoundPortions();

	MetSig::test_byCountCalcTotalPrice();

	MetSig::test_determineImplicitDecimalPrice();


	// Weigher-related and weight calculation routines

	MetSig::test_isWeigherDualRangeCapable();

	MetSig::test_checkToSwitchDualRangeDirection();
	MetSig::test_checkToSwitchDualRangeDirectionOnNetWeight();

	MetSig::test_applyScaleFactorToWeight();

	MetSig::test_weightCountsToFixedWeight();
	MetSig::test_fixedWeightToWeightCounts();
	MetSig::test_fixedWeightOuncesToDecimalWeight();
	MetSig::test_ouncesToPoundCounts();

	MetSig::test_ouncesToGrams();
	MetSig::test_gramsToOunces();
	MetSig::test_fluidOuncesToLiters();
	MetSig::test_fluidOuncesToGallons();
	MetSig::test_fluidOuncesToQuarts();
	MetSig::test_fluidOuncesToPints();

	MetSig::test_BRConvertFixedWeightToTotalWeight();

	MetSig::test_determineImplicitDecimalWeight();
	MetSig::test_determineImplicitDecimalKeyboardNetWeight();

	MetSig::test_roundWeightTo2DecimalsThenBackTo3();

	MetSig::test_applyTare();
	MetSig::test_applyTareSetback();
	MetSig::test_applyProportionalTare();
	MetSig::test_calculateRandomWeightTotalPrice();
	MetSig::test_calculateRandomWeightOldTotalPrice();

	MetSig::testingMetSig = false;

	Debug::trace("MetSig.testAll", "******************** MetSig::test_allMetSig() END ********************");
}

void MetSig::test_calculateUnitPriceRounded()
{
	// HobPrice calculateUnitPriceRounded(const HobPrice unitPriceToUse, const int byCountToUse);
	
	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_calculateUnitPriceRounded() START ********************");

	bool tracing = Debug::isTracing("MetSig.calculateUnitPriceRounded");
	if (!tracing)
		Debug::addTraceCategory("MetSig.calculateUnitPriceRounded");

	calculateUnitPriceRounded(1, 1);
	calculateUnitPriceRounded(12, 1);
	calculateUnitPriceRounded(123, 1);
	calculateUnitPriceRounded(1234, 1);
	calculateUnitPriceRounded(12345, 1);
	calculateUnitPriceRounded(123456, 1);
	calculateUnitPriceRounded(1234567, 1);
	calculateUnitPriceRounded(12345678, 1);
	calculateUnitPriceRounded(123456789, 1);
	calculateUnitPriceRounded(1234567890, 1);

	calculateUnitPriceRounded(1, 2);
	calculateUnitPriceRounded(12, 2);
	calculateUnitPriceRounded(123, 2);
	calculateUnitPriceRounded(1234, 2);
	calculateUnitPriceRounded(12345, 2);
	calculateUnitPriceRounded(123456, 2);
	calculateUnitPriceRounded(1234567, 2);
	calculateUnitPriceRounded(12345678, 2);
	calculateUnitPriceRounded(123456789, 2);
	calculateUnitPriceRounded(1234567890, 2);

	calculateUnitPriceRounded(1, 3);
	calculateUnitPriceRounded(12, 3);
	calculateUnitPriceRounded(123, 3);
	calculateUnitPriceRounded(1234, 3);
	calculateUnitPriceRounded(12345, 3);
	calculateUnitPriceRounded(123456, 3);
	calculateUnitPriceRounded(1234567, 3);
	calculateUnitPriceRounded(12345678, 3);
	calculateUnitPriceRounded(123456789, 3);
	calculateUnitPriceRounded(1234567890, 3);

	calculateUnitPriceRounded(1, 4);
	calculateUnitPriceRounded(12, 4);
	calculateUnitPriceRounded(123, 4);
	calculateUnitPriceRounded(1234, 4);
	calculateUnitPriceRounded(12345, 4);
	calculateUnitPriceRounded(123456, 4);
	calculateUnitPriceRounded(1234567, 4);
	calculateUnitPriceRounded(12345678, 4);
	calculateUnitPriceRounded(123456789, 4);
	calculateUnitPriceRounded(1234567890, 4);

	calculateUnitPriceRounded(1, 5);
	calculateUnitPriceRounded(12, 5);
	calculateUnitPriceRounded(123, 5);
	calculateUnitPriceRounded(1234, 5);
	calculateUnitPriceRounded(12345, 5);
	calculateUnitPriceRounded(123456, 5);
	calculateUnitPriceRounded(1234567, 5);
	calculateUnitPriceRounded(12345678, 5);
	calculateUnitPriceRounded(123456789, 5);
	calculateUnitPriceRounded(1234567890, 5);

	calculateUnitPriceRounded(1, 6);
	calculateUnitPriceRounded(12, 6);
	calculateUnitPriceRounded(123, 6);
	calculateUnitPriceRounded(1234, 6);
	calculateUnitPriceRounded(12345, 6);
	calculateUnitPriceRounded(123456, 6);
	calculateUnitPriceRounded(1234567, 6);
	calculateUnitPriceRounded(12345678, 6);
	calculateUnitPriceRounded(123456789, 6);
	calculateUnitPriceRounded(1234567890, 6);

	calculateUnitPriceRounded(1, 7);
	calculateUnitPriceRounded(12, 7);
	calculateUnitPriceRounded(123, 7);
	calculateUnitPriceRounded(1234, 7);
	calculateUnitPriceRounded(12345, 7);
	calculateUnitPriceRounded(123456, 7);
	calculateUnitPriceRounded(1234567, 7);
	calculateUnitPriceRounded(12345678, 7);
	calculateUnitPriceRounded(123456789, 7);
	calculateUnitPriceRounded(1234567890, 7);

	calculateUnitPriceRounded(1, 8);
	calculateUnitPriceRounded(12, 8);
	calculateUnitPriceRounded(123, 8);
	calculateUnitPriceRounded(1234, 8);
	calculateUnitPriceRounded(12345, 8);
	calculateUnitPriceRounded(123456, 8);
	calculateUnitPriceRounded(1234567, 8);
	calculateUnitPriceRounded(12345678, 8);
	calculateUnitPriceRounded(123456789, 8);
	calculateUnitPriceRounded(1234567890, 8);

	calculateUnitPriceRounded(1, 9);
	calculateUnitPriceRounded(12, 9);
	calculateUnitPriceRounded(123, 9);
	calculateUnitPriceRounded(1234, 9);
	calculateUnitPriceRounded(12345, 9);
	calculateUnitPriceRounded(123456, 9);
	calculateUnitPriceRounded(1234567, 9);
	calculateUnitPriceRounded(12345678, 9);
	calculateUnitPriceRounded(123456789, 9);
	calculateUnitPriceRounded(1234567890, 9);

	calculateUnitPriceRounded(1, 0);
	calculateUnitPriceRounded(12, 0);
	calculateUnitPriceRounded(123, 0);
	calculateUnitPriceRounded(1234, 0);
	calculateUnitPriceRounded(12345, 0);
	calculateUnitPriceRounded(123456, 0);
	calculateUnitPriceRounded(1234567, 0);
	calculateUnitPriceRounded(12345678, 0);
	calculateUnitPriceRounded(123456789, 0);
	calculateUnitPriceRounded(1234567890, 0);


	HobPrice unitPriceToUse = 0;
	int byCountToUse = 0;
	for (byCountToUse = 0; byCountToUse < 100; byCountToUse++)
		for (unitPriceToUse = 0; unitPriceToUse < 10001; unitPriceToUse++)
			calculateUnitPriceRounded(unitPriceToUse, byCountToUse);
	
	if (!tracing)
		Debug::removeTraceCategory("MetSig.calculateUnitPriceRounded");

	Debug::trace("MetSig.test", "******************** MetSig::test_calculateUnitPriceRounded() END ********************");
}

void MetSig::test_calculateUnitPriceNoFractionalCents()
{
	// bool calculateUnitPriceNoFractionalCents(HobPrice *result, const HobPrice unitPriceToUse, const int byCountToUse);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_calculateUnitPriceNoFractionalCents() START ********************");

	bool tracing = Debug::isTracing("MetSig.calculateUnitPriceNoFractionalCents");
	if (!tracing)
		Debug::addTraceCategory("MetSig.calculateUnitPriceNoFractionalCents");

	bool result = false;
	HobPrice resultPrice = 0;
	HobPrice unitPriceToUse = 0;
	int byCountToUse = 0;

	for (byCountToUse = 0; byCountToUse < 100; byCountToUse++)
		for (unitPriceToUse = 0; unitPriceToUse < 10001; unitPriceToUse++)
			result = calculateUnitPriceNoFractionalCents(&resultPrice, unitPriceToUse, byCountToUse);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.calculateUnitPriceNoFractionalCents");

	Debug::trace("MetSig.test", "******************** MetSig::test_calculateUnitPriceNoFractionalCents() END ********************");
}

void MetSig::test_calcFixedWeightPricePerUnit()
{
	// HobPrice calcFixedWeightPricePerUnit(const HobPrice thePrice, const HobWeight theFixedWeight);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_calcFixedWeightPricePerUnit() START ********************");

	bool tracing = Debug::isTracing("MetSig.calcFixedWeightPricePerUnit");
	if (!tracing)
		Debug::addTraceCategory("MetSig.calcFixedWeightPricePerUnit");

	HobPrice resultPrice = 0;
	HobPrice thePrice = 0;
	HobWeight theFixedWeight = 0;
/*
	MetSig::setScaleWeighMode(WeighMode_Metric);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.calcFixedWeightPricePerUnit", "Metric and FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theFixedWeight = 0; theFixedWeight < 100; theFixedWeight++)
		for (thePrice = 0; thePrice < 10001; thePrice++)
			resultPrice = calcFixedWeightPricePerUnit(thePrice, theFixedWeight);

	MetSig::setScaleWeighMode(WeighMode_Metric);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.calcFixedWeightPricePerUnit", "Metric and FIXED_WEIGHT_IN_TENTHS");
	for (theFixedWeight = 0; theFixedWeight < 100; theFixedWeight++)
		for (thePrice = 0; thePrice < 10001; thePrice++)
			resultPrice = calcFixedWeightPricePerUnit(thePrice, theFixedWeight);
*/
	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.calcFixedWeightPricePerUnit", "Avoir and FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theFixedWeight = 0; theFixedWeight < 100; theFixedWeight++)
		for (thePrice = 0; thePrice < 1001; thePrice++)
			resultPrice = calcFixedWeightPricePerUnit(thePrice, theFixedWeight);
/*
	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.calcFixedWeightPricePerUnit", "Avoir and FIXED_WEIGHT_IN_TENTHS");
	for (theFixedWeight = 0; theFixedWeight < 100; theFixedWeight++)
		for (thePrice = 0; thePrice < 10001; thePrice++)
			resultPrice = calcFixedWeightPricePerUnit(thePrice, theFixedWeight);
*/
	if (!tracing)
		Debug::removeTraceCategory("MetSig.calcFixedWeightPricePerUnit");

	Debug::trace("MetSig.test", "******************** MetSig::test_calcFixedWeightPricePerUnit() END ********************");
}

void MetSig::test_divideAndRoundPortions()
{
	// HobValue divideAndRoundPortions(const long dividend, const int divisor, const short precision);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_divideAndRoundPortions() START ********************");

	bool tracing = Debug::isTracing("MetSig.divideAndRoundPortions");
	if (!tracing)
		Debug::addTraceCategory("MetSig.divideAndRoundPortions");

	HobPrice resultValue = 0;
	HobPrice thePriceorWeight = 0;
	HobWeight portions = 0;

	Debug::trace("MetSig.divideAndRoundPortions", "Precision = 2");
	for (portions = 1; portions < 100; portions++)
		for (thePriceorWeight = 0; thePriceorWeight < 30001; thePriceorWeight++)
			resultValue = divideAndRoundPortions(thePriceorWeight, portions, 2);

	Debug::trace("MetSig.divideAndRoundPortions", "Precision = 3");
	for (portions = 1; portions < 100; portions++)
		for (thePriceorWeight = 0; thePriceorWeight < 30001; thePriceorWeight++)
			resultValue = divideAndRoundPortions(thePriceorWeight, portions, 3);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.divideAndRoundPortions");

	Debug::trace("MetSig.test", "******************** MetSig::test_divideAndRoundPortions() END ********************");
}

void MetSig::test_byCountCalcTotalPrice()
{
	// HobPrice byCountCalcTotalPrice(const int originalByCount, const int byCount, const HobPrice unitPrice, const HobPrice exceptionPrice);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_byCountCalcTotalPrice() START ********************");

	bool tracing = Debug::isTracing("MetSig.bycountcalcs");
	if (!tracing)
		Debug::addTraceCategory("MetSig.bycountcalcs");

	int originalByCount = 0;
	int byCount = 0;
	HobPrice unitPrice = 0;
	HobPrice resultPrice = 0;

	Configuration::getInstance()->setByCountCalculationMethod(1);
	Debug::trace("MetSig.bycountcalcs", "setByCountCalculationMethod=1, exceptionPrice = 0");
	for (originalByCount = 0; originalByCount < 100; originalByCount++)
		for (byCount = 0; byCount < 100; byCount++)
			for (unitPrice = 0; unitPrice < 10001; unitPrice++)
			resultPrice = byCountCalcTotalPrice(originalByCount, byCount, unitPrice, 0);

	Configuration::getInstance()->setByCountCalculationMethod(2);
	Debug::trace("MetSig.bycountcalcs", "setByCountCalculationMethod=2, exceptionPrice = 0");
	for (originalByCount = 0; originalByCount < 100; originalByCount++)
		for (byCount = 0; byCount < 100; byCount++)
			for (unitPrice = 0; unitPrice < 10001; unitPrice++)
				resultPrice = byCountCalcTotalPrice(originalByCount, byCount, unitPrice, 0);

	Configuration::getInstance()->setByCountCalculationMethod(3);
	Debug::trace("MetSig.bycountcalcs", "setByCountCalculationMethod=3, exceptionPrice = 0");
	for (originalByCount = 0; originalByCount < 100; originalByCount++)
		for (byCount = 0; byCount < 100; byCount++)
			for (unitPrice = 0; unitPrice < 10001; unitPrice++)
				resultPrice = byCountCalcTotalPrice(originalByCount, byCount, unitPrice, 0);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.bycountcalcs");

	Debug::trace("MetSig.test", "******************** MetSig::test_byCountCalcTotalPrice() END ********************");
}

void MetSig::test_determineImplicitDecimalPrice()
{
	// HobPrice determineImplicitDecimalPrice(const std::string priceStr);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_determineImplicitDecimalPrice() START ********************");

	bool tracing = Debug::isTracing("MetSig.price");
	if (!tracing)
		Debug::addTraceCategory("MetSig.price");

	MetSig::determineImplicitDecimalPrice("-1");
	MetSig::determineImplicitDecimalPrice("-1.0");
	MetSig::determineImplicitDecimalPrice("-1.00");
	MetSig::determineImplicitDecimalPrice("-1.000");
	MetSig::determineImplicitDecimalPrice("-1.0000");
	MetSig::determineImplicitDecimalPrice("1");
	MetSig::determineImplicitDecimalPrice("12");
	MetSig::determineImplicitDecimalPrice("123");
	MetSig::determineImplicitDecimalPrice("1234");
	MetSig::determineImplicitDecimalPrice("12345");
	MetSig::determineImplicitDecimalPrice("123456");
	MetSig::determineImplicitDecimalPrice("1234567");
	MetSig::determineImplicitDecimalPrice("12345678");
	MetSig::determineImplicitDecimalPrice("123456789");
	MetSig::determineImplicitDecimalPrice("1234567890");
	MetSig::determineImplicitDecimalPrice("12345678901");
	MetSig::determineImplicitDecimalPrice("123456789012");
	MetSig::determineImplicitDecimalPrice("0123");
	MetSig::determineImplicitDecimalPrice("123.");
	MetSig::determineImplicitDecimalPrice("123.0");
	MetSig::determineImplicitDecimalPrice("123.00");
	MetSig::determineImplicitDecimalPrice("123.000");
	MetSig::determineImplicitDecimalPrice("123.001");
	MetSig::determineImplicitDecimalPrice("123.004");
	MetSig::determineImplicitDecimalPrice("123.005");
	MetSig::determineImplicitDecimalPrice("123.006");
	MetSig::determineImplicitDecimalPrice("123.4");
	MetSig::determineImplicitDecimalPrice("123.45");
	MetSig::determineImplicitDecimalPrice("123.450");
	MetSig::determineImplicitDecimalPrice("123.451");
	MetSig::determineImplicitDecimalPrice("123.452");
	MetSig::determineImplicitDecimalPrice("123.453");
	MetSig::determineImplicitDecimalPrice("123.454");
	MetSig::determineImplicitDecimalPrice("123.455");
	MetSig::determineImplicitDecimalPrice("123.456");
	MetSig::determineImplicitDecimalPrice("123.457");
	MetSig::determineImplicitDecimalPrice("123.458");
	MetSig::determineImplicitDecimalPrice("123.459");
	MetSig::determineImplicitDecimalPrice("-0");
	MetSig::determineImplicitDecimalPrice("0");
	MetSig::determineImplicitDecimalPrice("0.0");
	MetSig::determineImplicitDecimalPrice("0.00");
	MetSig::determineImplicitDecimalPrice("0.000");
	MetSig::determineImplicitDecimalPrice("0.001");
	MetSig::determineImplicitDecimalPrice("0.004");
	MetSig::determineImplicitDecimalPrice("0.005");
	MetSig::determineImplicitDecimalPrice("0.006");
	
	if (!tracing)
		Debug::removeTraceCategory("MetSig.price");

	Debug::trace("MetSig.test", "******************** MetSig::test_determineImplicitDecimalPrice() END ********************");
}


// Weigher-related and weight calculation routines

void MetSig::test_isWeigherDualRangeCapable()
{
	// bool isWeigherDualRangeCapable(const WeigherModel model = endWeigherModel);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_isWeigherDualRangeCapable() START ********************");

	bool tracing = Debug::isTracing("MetSig.isWeigherDualRangeCapable");
	if (!tracing)
		Debug::addTraceCategory("MetSig.isWeigherDualRangeCapable");
		
	for (int model = WmNone; model <= endWeigherModel; model++)
		Debug::trace("MetSig.isWeigherDualRangeCapable", string("enum value=") + inttostring(model) + "  result=" + booltostring( isWeigherDualRangeCapable( (WeigherModel)model) ));
	
	if (!tracing)
		Debug::removeTraceCategory("MetSig.isWeigherDualRangeCapable");

	Debug::trace("MetSig.test", "******************** MetSig::test_isWeigherDualRangeCapable() END ********************");
}

void MetSig::test_checkToSwitchDualRangeDirection()
{
	// bool checkToSwitchDualRangeDirection(HfcWeigherStatus *newWeight, bool hasCurrentProductInOperateMode);
	
	bool tracing = Debug::isTracing("MetSig.dualrange");
	if (!tracing)
		Debug::addTraceCategory("MetSig.dualrange");

	
	if (!tracing)
		Debug::removeTraceCategory("MetSig.dualrange");
}

void MetSig::test_checkToSwitchDualRangeDirectionOnNetWeight()
{
	// bool checkToSwitchDualRangeDirectionOnNetWeight(HfcWeigherStatus *weigherStatus, const HobWeight netWeight);
	
	bool tracing = Debug::isTracing("MetSig.dualrange");
	if (!tracing)
		Debug::addTraceCategory("MetSig.dualrange");

	
	if (!tracing)
		Debug::removeTraceCategory("MetSig.dualrange");
}

void MetSig::test_applyScaleFactorToWeight()
{
	// HobWeight applyScaleFactorToWeight(HobWeight weight, UINT32 scaleFactor);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_applyScaleFactorToWeight() START ********************");

	bool tracing = Debug::isTracing("MetSig.applyScaleFactorToWeight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.applyScaleFactorToWeight");

	HobWeight resultWeight = 0;
	HobWeight theWeight = 0;

	MetSig::setScaleWeighMode(WeighMode_Metric);
	Debug::trace("MetSig.applyScaleFactorToWeight", "Metric, scalefactor=5");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = applyScaleFactorToWeight(theWeight, 5);

	MetSig::setScaleWeighMode(WeighMode_Metric);
	Debug::trace("MetSig.applyScaleFactorToWeight", "Metric, scalefactor=2");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = applyScaleFactorToWeight(theWeight, 2);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Debug::trace("MetSig.applyScaleFactorToWeight", "Avoir, scalefactor=1");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = applyScaleFactorToWeight(theWeight, 1);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Debug::trace("MetSig.applyScaleFactorToWeight", "Avoir, scalefactor=5");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = applyScaleFactorToWeight(theWeight, 5);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.applyScaleFactorToWeight");

	Debug::trace("MetSig.test", "******************** MetSig::test_applyScaleFactorToWeight() END ********************");
}

void MetSig::test_weightCountsToFixedWeight()
{
	// HobWeight weightCountsToFixedWeight(const HobWeight weight);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_weightCountsToFixedWeight() START ********************");

	bool tracing = Debug::isTracing("MetSig.weightCountsToFixedWeight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.weightCountsToFixedWeight");

	HobWeight resultWeight = 0;
	HobWeight theWeight = 0;

	for (theWeight = 0; theWeight < 10001; theWeight++)
		resultWeight = weightCountsToFixedWeight(theWeight);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.weightCountsToFixedWeight");

	Debug::trace("MetSig.test", "******************** MetSig::test_weightCountsToFixedWeight() END ********************");
}

void MetSig::test_fixedWeightToWeightCounts()
{
	// HobWeight fixedWeightToWeightCounts(const HobWeight fixedWeight);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_fixedWeightToWeightCounts() START ********************");

	bool tracing = Debug::isTracing("MetSig.fixedWeightToWeightCounts");
	if (!tracing)
		Debug::addTraceCategory("MetSig.fixedWeightToWeightCounts");

	HobWeight resultWeight = 0;
	HobWeight theWeight = 0;

	MetSig::setScaleWeighMode(WeighMode_Metric);
	Debug::trace("MetSig.fixedWeightToWeightCounts", "Metric");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = fixedWeightToWeightCounts(theWeight);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.fixedWeightToWeightCounts", "Avoir, FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = fixedWeightToWeightCounts(theWeight);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.fixedWeightToWeightCounts", "Avoir, FIXED_WEIGHT_IN_TENTHS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = fixedWeightToWeightCounts(theWeight);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fixedWeightToWeightCounts");

	Debug::trace("MetSig.test", "******************** MetSig::test_fixedWeightToWeightCounts() END ********************");
}

void MetSig::test_fixedWeightOuncesToDecimalWeight()
{
	// HobWeight fixedWeightOuncesToDecimalWeight(const HobWeight fixedWeight);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_fixedWeightOuncesToDecimalWeight() START ********************");

	bool tracing = Debug::isTracing("MetSig.fixedWeightOuncesToDecimalWeight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.fixedWeightOuncesToDecimalWeight");

	// Need to consider: if isScaleAvoir() && isWeigherConfiguredForSingleRange()
	HobWeight resultWeight = 0;
	HobWeight theWeight = 0;

	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.fixedWeightOuncesToDecimalWeight", "FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = fixedWeightOuncesToDecimalWeight(theWeight);

	// Need to consider: if isScaleAvoir() && isWeigherConfiguredForSingleRange()
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.fixedWeightOuncesToDecimalWeight", "FIXED_WEIGHT_IN_TENTHS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = fixedWeightOuncesToDecimalWeight(theWeight);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fixedWeightOuncesToDecimalWeight");

	Debug::trace("MetSig.test", "******************** MetSig::test_fixedWeightOuncesToDecimalWeight() END ********************");
}

void MetSig::test_ouncesToPoundCounts()
{
	// HobWeight ouncesToPoundCounts(HobWeight ounces, bool isInTenthsOfAnOunce);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_ouncesToPoundCounts() START ********************");

	bool tracing = Debug::isTracing("MetSig.ouncesToPoundCounts");
	if (!tracing)
		Debug::addTraceCategory("MetSig.ouncesToPoundCounts");

	// Need to consider: if isScaleAvoir() && isWeigherConfiguredForSingleRange()
	HobWeight resultWeight = 0;
	HobWeight theWeight = 0;

	// Need to consider: if isScaleAvoir() && isWeigherConfiguredForSingleRange()
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.ouncesToPoundCounts", "isInTenthsOfAnOunce = false, so FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = ouncesToPoundCounts(theWeight, false);

	// Need to consider: if isScaleAvoir() && isWeigherConfiguredForSingleRange()
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.ouncesToPoundCounts", "isInTenthsOfAnOunce = true, so FIXED_WEIGHT_IN_TENTHS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = ouncesToPoundCounts(theWeight, true);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fixedWeightOuncesToDecimalWeight");

	Debug::trace("MetSig.test", "******************** MetSig::test_ouncesToPoundCounts() END ********************");
}

void MetSig::test_ouncesToGrams()
{
	// HobWeight ouncesToGrams(const HobWeight ounces);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_ouncesToGrams() START ********************");

	bool tracing = Debug::isTracing("MetSig.ouncesToGrams");
	if (!tracing)
		Debug::addTraceCategory("MetSig.ouncesToGrams");

	HobWeight result = 0;
	HobWeight ounces = 0;
	for (ounces = 0; ounces < 1001; ounces++)
		result = ouncesToGrams(ounces);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.ouncesToGrams");

	Debug::trace("MetSig.test", "******************** MetSig::test_ouncesToGrams() END ********************");
}

void MetSig::test_gramsToOunces()
{
	// HobWeight gramsToOunces(const HobWeight grams);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_gramsToOunces() START ********************");

	bool tracing = Debug::isTracing("MetSig.gramsToOunces");
	if (!tracing)
		Debug::addTraceCategory("MetSig.gramsToOunces");

	HobWeight result = 0;
	HobWeight grams = 0;
	for (grams = 0; grams < 1001; grams++)
		result = gramsToOunces(grams);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.gramsToOunces");

	Debug::trace("MetSig.test", "******************** MetSig::test_gramsToOunces() END ********************");
}

void MetSig::test_fluidOuncesToLiters()
{
	// HobWeight fluidOuncesToLiters(const HobWeight totalOunces);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToLiters() START ********************");

	bool tracing = Debug::isTracing("MetSig.fluidOuncesToLiters");
	if (!tracing)
		Debug::addTraceCategory("MetSig.fluidOuncesToLiters");

	HobWeight result = 0;
	HobWeight ounces = 0;
	for (ounces = 0; ounces < 1001; ounces++)
		result = fluidOuncesToLiters(ounces);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fluidOuncesToLiters");

	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToLiters() END ********************");
}

void MetSig::test_fluidOuncesToGallons()
{
	// long fluidOuncesToGallons(const HobWeight fluidOunces);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToGallons() START ********************");

	bool tracing = Debug::isTracing("MetSig.fluidOuncesToGallons");
	if (!tracing)
		Debug::addTraceCategory("MetSig.fluidOuncesToGallons");

	HobWeight result = 0;
	HobWeight ounces = 0;
	for (ounces = 0; ounces < 1001; ounces++)
		result = fluidOuncesToGallons(ounces);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fluidOuncesToGallons");

	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToGallons() END ********************");
}

void MetSig::test_fluidOuncesToQuarts()
{
	// long fluidOuncesToQuarts(const HobWeight fluidOunces);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToQuarts() START ********************");

	bool tracing = Debug::isTracing("MetSig.fluidOuncesToQuarts");
	if (!tracing)
		Debug::addTraceCategory("MetSig.fluidOuncesToQuarts");

	HobWeight result = 0;
	HobWeight ounces = 0;
	for (ounces = 0; ounces < 1001; ounces++)
		result = fluidOuncesToQuarts(ounces);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fluidOuncesToQuarts");
	 
	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToQuarts() END ********************");
}

void MetSig::test_fluidOuncesToPints()
{
	// long fluidOuncesToPints(const HobWeight fluidOunces);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToPints() START ********************");

	bool tracing = Debug::isTracing("MetSig.fluidOuncesToPints");
	if (!tracing)
		Debug::addTraceCategory("MetSig.fluidOuncesToPints");

	HobWeight result = 0;
	HobWeight ounces = 0;
	for (ounces = 0; ounces < 1001; ounces++)
		result = fluidOuncesToPints(ounces);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.fluidOuncesToPints");

	Debug::trace("MetSig.test", "******************** MetSig::test_fluidOuncesToPints() END ********************");
}

void MetSig::test_BRConvertFixedWeightToTotalWeight()
{
	// HobWeight BRConvertFixedWeightToTotalWeight(const HobWeight fixedWeight, const bool fluidOz);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_BRConvertFixedWeightToTotalWeight() START ********************");

	bool tracing = Debug::isTracing("MetSig.BRConvertFixedWeightToTotalWeight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.BRConvertFixedWeightToTotalWeight");

	HobWeight resultWeight = 0;
	HobWeight theWeight = 0;

	MetSig::setScaleWeighMode(WeighMode_Metric);
	Debug::trace("MetSig.BRConvertFixedWeightToTotalWeight", "Metric");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = BRConvertFixedWeightToTotalWeight(theWeight, false);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.BRConvertFixedWeightToTotalWeight", "Avoir, fluidOz, FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = BRConvertFixedWeightToTotalWeight(theWeight, true);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.BRConvertFixedWeightToTotalWeight", "Avoir, fluidOz, FIXED_WEIGHT_IN_TENTHS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = BRConvertFixedWeightToTotalWeight(theWeight, true);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_WHOLE_UNITS);
	Debug::trace("MetSig.BRConvertFixedWeightToTotalWeight", "Avoir, not fluidOz, FIXED_WEIGHT_IN_WHOLE_UNITS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = BRConvertFixedWeightToTotalWeight(theWeight, false);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	Configuration::getInstance()->setFixedWeightUnits(Configuration::FIXED_WEIGHT_IN_TENTHS);
	Debug::trace("MetSig.BRConvertFixedWeightToTotalWeight", "Avoir, not fluidOz, FIXED_WEIGHT_IN_TENTHS");
	for (theWeight = 0; theWeight < 1001; theWeight++)
		resultWeight = BRConvertFixedWeightToTotalWeight(theWeight, false);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.BRConvertFixedWeightToTotalWeight");

	Debug::trace("MetSig.test", "******************** MetSig::test_BRConvertFixedWeightToTotalWeight() END ********************");
}

void MetSig::test_determineImplicitDecimalWeight()
{
	// HobWeight determineImplicitDecimalWeight(const std::string weightStr);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_determineImplicitDecimalWeight() START ********************");

	bool tracing = Debug::isTracing("MetSig.weight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.weight");

	MetSig::determineImplicitDecimalWeight("-1");
	MetSig::determineImplicitDecimalWeight("-1.0");
	MetSig::determineImplicitDecimalWeight("-1.00");
	MetSig::determineImplicitDecimalWeight("-1.000");
	MetSig::determineImplicitDecimalWeight("-1.0000");
	MetSig::determineImplicitDecimalWeight("1");
	MetSig::determineImplicitDecimalWeight("12");
	MetSig::determineImplicitDecimalWeight("123");
	MetSig::determineImplicitDecimalWeight("1234");
	MetSig::determineImplicitDecimalWeight("12345");
	MetSig::determineImplicitDecimalWeight("123456");
	MetSig::determineImplicitDecimalWeight("1234567");
	MetSig::determineImplicitDecimalWeight("12345678");
	MetSig::determineImplicitDecimalWeight("123456789");
	MetSig::determineImplicitDecimalWeight("1234567890");
	MetSig::determineImplicitDecimalWeight("12345678901");
	MetSig::determineImplicitDecimalWeight("123456789012");
	MetSig::determineImplicitDecimalWeight("0123");
	MetSig::determineImplicitDecimalWeight("123.");
	MetSig::determineImplicitDecimalWeight("123.0");
	MetSig::determineImplicitDecimalWeight("123.00");
	MetSig::determineImplicitDecimalWeight("123.000");
	MetSig::determineImplicitDecimalWeight("123.001");
	MetSig::determineImplicitDecimalWeight("123.004");
	MetSig::determineImplicitDecimalWeight("123.005");
	MetSig::determineImplicitDecimalWeight("123.006");
	MetSig::determineImplicitDecimalWeight("123.4");
	MetSig::determineImplicitDecimalWeight("123.45");
	MetSig::determineImplicitDecimalWeight("123.450");
	MetSig::determineImplicitDecimalWeight("123.451");
	MetSig::determineImplicitDecimalWeight("123.452");
	MetSig::determineImplicitDecimalWeight("123.453");
	MetSig::determineImplicitDecimalWeight("123.454");
	MetSig::determineImplicitDecimalWeight("123.455");
	MetSig::determineImplicitDecimalWeight("123.456");
	MetSig::determineImplicitDecimalWeight("123.457");
	MetSig::determineImplicitDecimalWeight("123.458");
	MetSig::determineImplicitDecimalWeight("123.459");
	MetSig::determineImplicitDecimalWeight("-0");
	MetSig::determineImplicitDecimalWeight("0");
	MetSig::determineImplicitDecimalWeight("0.0");
	MetSig::determineImplicitDecimalWeight("0.00");
	MetSig::determineImplicitDecimalWeight("0.000");
	MetSig::determineImplicitDecimalWeight("0.001");
	MetSig::determineImplicitDecimalWeight("0.004");
	MetSig::determineImplicitDecimalWeight("0.005");
	MetSig::determineImplicitDecimalWeight("0.006");

	if (!tracing)
		Debug::removeTraceCategory("MetSig.weight");

	Debug::trace("MetSig.test", "******************** MetSig::test_determineImplicitDecimalWeight() END ********************");
}

void MetSig::test_determineImplicitDecimalKeyboardNetWeight()
{
	// HobWeight determineImplicitDecimalKeyboardNetWeight(const std::string weightStr);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_determineImplicitDecimalKeyboardNetWeight() START ********************");

	bool tracing = Debug::isTracing("MetSig.weight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.weight");

	MetSig::determineImplicitDecimalKeyboardNetWeight("-1");
	MetSig::determineImplicitDecimalKeyboardNetWeight("-1.0");
	MetSig::determineImplicitDecimalKeyboardNetWeight("-1.00");
	MetSig::determineImplicitDecimalKeyboardNetWeight("-1.000");
	MetSig::determineImplicitDecimalKeyboardNetWeight("-1.0000");
	MetSig::determineImplicitDecimalKeyboardNetWeight("1");
	MetSig::determineImplicitDecimalKeyboardNetWeight("12");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123");
	MetSig::determineImplicitDecimalKeyboardNetWeight("1234");
	MetSig::determineImplicitDecimalKeyboardNetWeight("12345");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123456");
	MetSig::determineImplicitDecimalKeyboardNetWeight("1234567");
	MetSig::determineImplicitDecimalKeyboardNetWeight("12345678");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123456789");
	MetSig::determineImplicitDecimalKeyboardNetWeight("1234567890");
	MetSig::determineImplicitDecimalKeyboardNetWeight("12345678901");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123456789012");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0123");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.0");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.00");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.000");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.001");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.004");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.005");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.006");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.4");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.45");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.450");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.451");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.452");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.453");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.454");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.455");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.456");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.457");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.458");
	MetSig::determineImplicitDecimalKeyboardNetWeight("123.459");
	MetSig::determineImplicitDecimalKeyboardNetWeight("-0");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.0");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.00");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.000");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.001");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.004");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.005");
	MetSig::determineImplicitDecimalKeyboardNetWeight("0.006");

	if (!tracing)
		Debug::removeTraceCategory("MetSig.weight");

	Debug::trace("MetSig.test", "******************** MetSig::test_determineImplicitDecimalKeyboardNetWeight() END ********************");
}

void MetSig::test_roundWeightTo2DecimalsThenBackTo3()
{
	// double roundWeightTo2DecimalsThenBackTo3(const double weight);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_roundWeightTo2DecimalsThenBackTo3() START ********************");

	bool tracing = Debug::isTracing("MetSig.roundWeightTo2DecimalsThenBackTo3");
	if (!tracing)
		Debug::addTraceCategory("MetSig.roundWeightTo2DecimalsThenBackTo3");

	double result = 0;
	double weight = 0;
	for (weight = 0.0; weight < 30000.0; weight+=0.1)
		result = roundWeightTo2DecimalsThenBackTo3(weight);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.roundWeightTo2DecimalsThenBackTo3");

	Debug::trace("MetSig.test", "******************** MetSig::test_roundWeightTo2DecimalsThenBackTo3() END ********************");
}

void MetSig::test_applyTare()
{
	// void applyTare(HobWeight &netWeight, HobWeight &totalTare, const HfcWeigherStatus *weigherStatus);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_applyTare() START ********************");

	bool tracing = Debug::isTracing("MetSig.netweight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.netweight");

	HobWeight netWeight = 0;
	HobWeight totalTare = 0;
	HfcWeigherStatus ws;

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Avoir, scalefactor=1, dualRangeDirection=UP");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	ws.setScaleFactor(5);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Avoir, scalefactor=5, dualRangeDirection=UP");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Avoir, scalefactor=1, dualRangeDirection=DOWN");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Avoir);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Metric, scalefactor=5, dualRangeDirection=DOWN");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Metric);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Metric, scalefactor=5, dualRangeDirection=UP");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Metric);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Metric, scalefactor=2, dualRangeDirection=UP");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Metric);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Metric, scalefactor=5, dualRangeDirection=DOWN");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	MetSig::setScaleWeighMode(WeighMode_Metric);
	ws.setScaleFactor(1);
	ws.setDualRangeEnabled(true);
	ws.setDualRangeDirection(DualRangeUp);

	Debug::trace("MetSig.netweight", "Avoir, scalefactor=2, dualRangeDirection=DOWN");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTare(netWeight, totalTare, &ws);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.netweight");

	Debug::trace("MetSig.test", "******************** MetSig::test_applyTare() END ********************");
}

void MetSig::test_applyTareSetback()
{
	// void applyTareSetback(HobWeight &netWeight, const HobWeight totalTare);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_applyTareSetback() START ********************");

	if (Debug::isTracing("MetSig.test_applyTareSetback"))
		MetSig::testingMetSig = true;

	bool tracing = Debug::isTracing("MetSig.netweight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.netweight");

	HobWeight netWeight = 0;
	HobWeight totalTare = 0;

	Configuration::getInstance()->setTareSetback("5");
	Debug::trace("MetSig.netweight", "TareSetback=5");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTareSetback(netWeight, totalTare);

	Configuration::getInstance()->setTareSetback("10");
	Debug::trace("MetSig.netweight", "TareSetback=10");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTareSetback(netWeight, totalTare);

	Configuration::getInstance()->setTareSetback("25");
	Debug::trace("MetSig.netweight", "TareSetback=25");
	for (netWeight = 0; netWeight < 1001; netWeight++)
		for (totalTare = 0; totalTare < 1001; totalTare++)
			applyTareSetback(netWeight, totalTare);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.netweight");

	if (Debug::isTracing("MetSig.test_applyTareSetback"))
		MetSig::testingMetSig = false;

	Debug::trace("MetSig.test", "******************** MetSig::test_applyTareSetback() END ********************");
}

void MetSig::test_applyProportionalTare()
{
	// void applyProportionalTare(HobWeight &netWeight, HobWeight &totalTare, const int proportionalTare, const HfcWeigherStatus *weigherStatus);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_applyProportionalTare() START ********************");

	bool tracing = Debug::isTracing("MetSig.netweight");
	if (!tracing)
		Debug::addTraceCategory("MetSig.netweight");

	HobWeight netWeight = 0;
	HobWeight totalTare = 0;
	int propTare = 0;
	HfcWeigherStatus ws;

	for (int i = 0; i < 6; i++)
	{
		Debug::trace("MetSig.netweight", "test_applyProportionalTare, propTare=" + inttostring(propTare));

		MetSig::setScaleWeighMode(WeighMode_Avoir);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Avoir, scalefactor=1, dualRangeDirection=UP");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Avoir);
		ws.setScaleFactor(5);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Avoir, scalefactor=5, dualRangeDirection=UP");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Avoir);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Avoir, scalefactor=1, dualRangeDirection=DOWN");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Avoir);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Metric, scalefactor=5, dualRangeDirection=DOWN");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Metric);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Metric, scalefactor=5, dualRangeDirection=UP");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Metric);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Metric, scalefactor=2, dualRangeDirection=UP");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Metric);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Metric, scalefactor=5, dualRangeDirection=DOWN");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		MetSig::setScaleWeighMode(WeighMode_Metric);
		ws.setScaleFactor(1);
		ws.setDualRangeEnabled(true);
		ws.setDualRangeDirection(DualRangeUp);

		Debug::trace("MetSig.netweight", "Avoir, scalefactor=2, dualRangeDirection=DOWN");
		for (netWeight = 0; netWeight < 1001; netWeight++)
			for (totalTare = 0; totalTare < 1001; totalTare++)
				applyProportionalTare(netWeight, totalTare, propTare, &ws);

		if (propTare = 0)
			propTare = 10;
		else
			propTare *= 10;
	}

	if (!tracing)
		Debug::removeTraceCategory("MetSig.netweight");

	Debug::trace("MetSig.test", "******************** MetSig::test_applyProportionalTare() END ********************");
}

void MetSig::test_calculateRandomWeightTotalPrice()
{
	// HobPrice calculateRandomWeightTotalPrice(const bool hasKbdNetWeightLabel, const HobPrice unitPrice, const long priceModifier, const HobWeight netWeight);

	if (!Debug::isTracing("MetSig.test"))
		return;
	Debug::trace("MetSig.test", "******************** MetSig::test_calculateRandomWeightTotalPrice() START ********************");

	bool tracing = Debug::isTracing("MetSig.calcprice");
	if (!tracing)
		Debug::addTraceCategory("MetSig.calcprice");

	/*
	HobPrice unitPrice = 0;
	HobWeight netWeight = 0;
	long priceModifier = 1;

	while (true)
	{
		Debug::trace("MetSig.calcprice", string("test_calculateRandomWeightTotalPrice - hasKbdNetWeightLabel=false, priceModifier=") + longtostring(priceModifier));

		for (unitPrice = 0; unitPrice < 1001; unitPrice++)
			for (netWeight = 0; netWeight < 1001; netWeight++)
				calculateRandomWeightTotalPrice(false, unitPrice, priceModifier, netWeight);

		if (priceModifier < 8)
			priceModifier++;
		else if (priceModifier == 9)
			priceModifier = 25;
		else if (priceModifier == 25)
			priceModifier = 50;
		else if (priceModifier == 50)
			priceModifier = 100;
		else
			break;
	}

	priceModifier = 1;

	while (true)
	{
		Debug::trace("MetSig.calcprice", string("test_calculateRandomWeightTotalPrice - hasKbdNetWeightLabel=true, priceModifier=") + longtostring(priceModifier));

		for (unitPrice = 0; unitPrice < 1001; unitPrice++)
			for (netWeight = 0; netWeight < 1001; netWeight++)
				calculateRandomWeightTotalPrice(true, unitPrice, priceModifier, netWeight);

		if (priceModifier < 8)
			priceModifier++;
		else if (priceModifier == 9)
			priceModifier = 25;
		else if (priceModifier == 25)
			priceModifier = 50;
		else if (priceModifier == 50)
			priceModifier = 100;
		else
			break;
	}
	*/

	calculateRandomWeightTotalPrice(false, 100, 1, 0);
	calculateRandomWeightTotalPrice(false, 4, 1, 10);
	calculateRandomWeightTotalPrice(false, 5, 1, 10);
	calculateRandomWeightTotalPrice(false, 15, 1, 10);
	calculateRandomWeightTotalPrice(false, 82, 1, 31);
	calculateRandomWeightTotalPrice(false, 85, 1, 31);
	calculateRandomWeightTotalPrice(false, 89, 1, 31);
	calculateRandomWeightTotalPrice(false, 83, 1, 32);
	calculateRandomWeightTotalPrice(false, 89, 1, 32);
	calculateRandomWeightTotalPrice(false, 7, 1, 151);
	calculateRandomWeightTotalPrice(false, 70, 1, 151);
	calculateRandomWeightTotalPrice(false, 700, 1, 151);
	calculateRandomWeightTotalPrice(false, 7929, 1, 250);
	calculateRandomWeightTotalPrice(false, 79, 1, 254);
	calculateRandomWeightTotalPrice(false, 86, 1, 254);
	calculateRandomWeightTotalPrice(false, 399, 1, 254);
	calculateRandomWeightTotalPrice(false, 9999, 1, 254);
	calculateRandomWeightTotalPrice(false, 9999, 1, 1254);
	calculateRandomWeightTotalPrice(false, 183, 1, 1503);
	calculateRandomWeightTotalPrice(false, 59, 1, 2067);
	calculateRandomWeightTotalPrice(false, 89, 1, 2451);

	calculateRandomWeightTotalPrice(false, 100, 1, 0);
	calculateRandomWeightTotalPrice(false, 4, 1, 100);
	calculateRandomWeightTotalPrice(false, 5, 1, 100);
	calculateRandomWeightTotalPrice(false, 15, 1, 100);
	calculateRandomWeightTotalPrice(false, 82, 1, 310);
	calculateRandomWeightTotalPrice(false, 85, 1, 310);
	calculateRandomWeightTotalPrice(false, 89, 1, 310);
	calculateRandomWeightTotalPrice(false, 83, 1, 320);
	calculateRandomWeightTotalPrice(false, 89, 1, 320);
	calculateRandomWeightTotalPrice(false, 7, 1, 1510);
	calculateRandomWeightTotalPrice(false, 70, 1, 1510);
	calculateRandomWeightTotalPrice(false, 700, 1, 1510);
	calculateRandomWeightTotalPrice(false, 7929, 1, 2500);
	calculateRandomWeightTotalPrice(false, 79, 1, 2540);
	calculateRandomWeightTotalPrice(false, 86, 1, 2540);
	calculateRandomWeightTotalPrice(false, 399, 1, 2540);
	calculateRandomWeightTotalPrice(false, 9999, 1, 2540);
	calculateRandomWeightTotalPrice(false, 9999, 1, 12540);
	calculateRandomWeightTotalPrice(false, 183, 1, 15030);
	calculateRandomWeightTotalPrice(false, 59, 1, 20670);
	calculateRandomWeightTotalPrice(false, 89, 1, 24510);

	if (!tracing)
		Debug::removeTraceCategory("MetSig.calcprice");

	Debug::trace("MetSig.test", "******************** MetSig::test_calculateRandomWeightTotalPrice() END ********************");
}

void MetSig::test_calculateRandomWeightOldTotalPrice()
{
	// HobPrice calculateRandomWeightOldTotalPrice(const bool hasKbdNetWeightLabel, const HobPrice unitPrice, const long priceModifier, const HobWeight netWeight);
}
