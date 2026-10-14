/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FrequentShopper.h"
#include "MetSig.h"
#include "Configuration.h"
#include "Rounding.h"
#include "Product.h"
#include "ProductTransaction.h"

#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Power.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FrequentShopper::dataType = 0;
std::string FrequentShopper::dataTypeName("FrequentShopper"); 
#endif


FrequentShopper::FrequentShopper(const FrequentShopper& obj): junkParameter(obj.junkParameter),
	scaleConfig(obj.scaleConfig), totalPrice1(obj.totalPrice1), totalPrice2(obj.totalPrice2),
	totalPrice3(obj.totalPrice3), totalSavings1(obj.totalSavings1), totalSavings2(obj.totalSavings2),
	totalSavings3(obj.totalSavings3), unitPrice1(obj.unitPrice1), unitPrice2(obj.unitPrice2),
	unitPrice3(obj.unitPrice3), unitPrice(obj.unitPrice), totalPrice(obj.totalPrice), 
	netWeight(obj.netWeight), recalculatedWeight(obj.recalculatedWeight), 
	discountValue(obj.discountValue), discountFormula(obj.discountFormula), discountType(obj.discountType)
{

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

FrequentShopper::FrequentShopper()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

FrequentShopper::~FrequentShopper()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void FrequentShopper::init()
{
	Debug::trace("FrequentShopper", HobString("init()"));
	scaleConfig = Configuration::getInstance();

	totalPrice1 	= 0;
	totalPrice2 	= 0;
	totalPrice3 	= 0;
	totalSavings1   = 0;
	totalSavings2   = 0;
	totalSavings3   = 0;
	unitPrice1  	= 0;
	unitPrice2  	= 0;
	unitPrice3  	= 0;

	unitPrice			= 0;
	totalPrice			= 0;
	netWeight			= 0;
	recalculatedWeight	= 0;
	discountValue		= 0;
	discountFormula		= IBM_FS_FORMULA;
	discountType		= Discount_price_by_weight;

	show();
}


HobPrice FrequentShopper::getTotalPrice1() const
{
	return totalPrice1;
}
void FrequentShopper::setTotalPrice1(HobPrice totalPrice)
{
	totalPrice1 = totalPrice;
	Debug::trace("FrequentShopper", HobString("setTotalPrice1():   totalPrice1 = ") + HobString(Long::toString(totalPrice1)));
}

HobPrice FrequentShopper::getTotalPrice2() const
{
	return totalPrice2;
}
void FrequentShopper::setTotalPrice2(HobPrice totalPrice)
{
	totalPrice2 = totalPrice;
}

HobPrice FrequentShopper::getTotalPrice3() const
{
	return totalPrice3;
}
void FrequentShopper::setTotalPrice3(HobPrice totalPrice)
{
	totalPrice3 = totalPrice;
}

HobPrice FrequentShopper::getTotalSavings1() const
{
	return totalSavings1;
}
void FrequentShopper::setTotalSavings1(HobPrice totalSavings)
{
	totalSavings1 = totalSavings;
	Debug::trace("FrequentShopper", HobString("setTotalSavings1():   totalSavings1 = ") + HobString(Long::toString(totalSavings1)));
}

HobPrice FrequentShopper::getTotalSavings2() const
{
	return totalSavings2;
}
void FrequentShopper::setTotalSavings2(HobPrice totalSavings)
{
	totalSavings2 = totalSavings;
}

HobPrice FrequentShopper::getTotalSavings3() const
{
	return totalSavings3;
}
void FrequentShopper::setTotalSavings3(HobPrice totalSavings)
{
	totalSavings3 = totalSavings;
}

//``````````````````````````````````````````````````````````````````````````````````````

HobPrice FrequentShopper::getUnitPrice1() const
{
	return unitPrice1;
}
void FrequentShopper::setUnitPrice1(HobPrice unitPrice)
{
	unitPrice1 = unitPrice;
	Debug::trace("FrequentShopper", HobString("setUnitPrice1():   unitPrice1 = ") + HobString(Long::toString(unitPrice1)));
}

HobPrice FrequentShopper::getUnitPrice2() const
{
	return unitPrice2;
}
void FrequentShopper::setUnitPrice2(HobPrice unitPrice)
{
	unitPrice2 = unitPrice;
}

HobPrice FrequentShopper::getUnitPrice3() const
{
	return unitPrice3;
}
void FrequentShopper::setUnitPrice3(HobPrice unitPrice)
{
	unitPrice3 = unitPrice;
}

/*-*********************************************************************
*
*   Function name:  recalculateWeight
*
************************************************************************
*
*   Synopsis:   VOID recalculateWeight()
*
*   Description:
*       This function recalculates the weight of a package by
*       dividing the total price by the unit price.
*
*   Parameter List: None
*   Returns:        UDWORD containing the recalculated weight
*
*   Example Usage:  recalculateWeight();
*
*   Requirements:  None
*
*   Assumptions:
*       uif_cb->op_plu->price has been calculated
*
*   Side Effects:  none
*
*********************************************************************-*/

void FrequentShopper::recalculateWeight(int byCount)
{
	bool traceOn = Debug::isTracing("FrequentShopper.RecalcWt");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "recalculateWeight:  ";

	// Note that the netweight will have 3-implied decimal places since all weights internally to the application have 3-implied decimal places.
	short roundingWeightPrecision = getRoundingWeightPrecision(netWeight);

	// FT-1754
	if ( (byCount == 0) && (discountFormula == RII_FS_FORMULA) )
	{
		if ( scaleConfig->isInternationalRoundingLicensed() )
		{
			// International Rounding
			recalculatedWeight = Rounding::roundPriceWeight(netWeight, 0, scaleConfig->getWeightPrecision(), roundingWeightPrecision, scaleConfig->getCurrencyRoundingMethod() );
		}
		else
		{
			// Standard conventional rounding that we have been using
			recalculatedWeight = Rounding::roundConventionalPriceWeight(netWeight, 0, scaleConfig->getWeightPrecision(), roundingWeightPrecision);
		}

		// If we rounded the weight to two decimal places, have to multiply by 10 to get back to 3-implied decimal places for internal use.
		if ( 2 == roundingWeightPrecision )
			recalculatedWeight *= 10;
	}
	else if ( (byCount == 0) && (discountType == Special_price) )
	{
		recalculatedWeight = Rounding::roundDownPriceWeight(netWeight, 0, scaleConfig->getWeightPrecision(), roundingWeightPrecision);
		// If we rounded the weight to two decimal places, have to multiply by 10 to get back to 3-implied decimal places for internal use.
		if ( 2 == roundingWeightPrecision )
			recalculatedWeight *= 10;
	}
	else
	{
		short decimalPlaces = scaleConfig->getEffectiveWeightPrecision() + 1;
		if (discountFormula == NCR_FS_FORMULA)
			++decimalPlaces;
		HobValue64 lmultiplier = Power::power(10, decimalPlaces);
		HobValue64 newExtendedTotalPrice = totalPrice * lmultiplier;
		HobWeight newWeight = (HobWeight) (newExtendedTotalPrice / unitPrice);
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" value = (totalPrice * lmultiplier) / unitPrice = (").append(Long::toString(totalPrice));
			msgDebug2.append(" * ").append(util_string::int64tostring(lmultiplier));
			msgDebug2.append(") / ").append(Long::toString(unitPrice));
			msgDebug2.append(" = ").append(Long::toString(newWeight));
			Debug::trace("FrequentShopper.RecalcWt", msgDebug2);
		}
		if (discountFormula == NCR_FS_FORMULA)
		{
			newWeight = newWeight * 10;
			if (traceOn)
				Debug::trace("FrequentShopper.RecalcWt", msgDebug + "NCR_FS_FORMULA:  Before rounding, multiply the weight by 10.  newWeight = " + Long::toString(newWeight));
		}

		if ( scaleConfig->isInternationalRoundingLicensed() )
		{
			// International Rounding
			recalculatedWeight = Rounding::roundPriceWeight(newWeight, 0, decimalPlaces, roundingWeightPrecision, scaleConfig->getCurrencyRoundingMethod());
		}
		else
		{
			// Standard conventional rounding that we have been using
			recalculatedWeight = Rounding::roundConventionalPriceWeight(newWeight, 0, decimalPlaces, roundingWeightPrecision);
		}

		if (traceOn)
		{
			if ( scaleConfig->isInternationalRoundingLicensed() )
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append(" recalculatedWeight = INTERNATIONAL RoundPriceWeight(").append(Long::toString(newWeight));
				msgDebug2.append(", 0, ").append(Integer::toString(decimalPlaces));
				msgDebug2.append(", ").append(Integer::toString(roundingWeightPrecision));
				msgDebug2.append(") = ").append(Long::toString(recalculatedWeight));
				Debug::trace("FrequentShopper.Calc", msgDebug2);
			}
			else
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append(" recalculatedWeight = RoundConventionalPriceWeight(").append(Long::toString(newWeight));
				msgDebug2.append(", 0, ").append(Integer::toString(decimalPlaces));
				msgDebug2.append(", ").append(Integer::toString(roundingWeightPrecision));
				msgDebug2.append(") = ").append(Long::toString(recalculatedWeight));
				Debug::trace("FrequentShopper.Calc", msgDebug2);
			}
		}

		if (discountFormula == NCR_FS_FORMULA)
		{
			recalculatedWeight = recalculatedWeight / 10;
			if (traceOn)
				Debug::trace("FrequentShopper.RecalcWt", msgDebug + "NCR_FS_FORMULA:  After rounding, divide the weight by 10.  recalculatedWeight = " + Long::toString(recalculatedWeight));
		}

		// If we rounded the weight to two decimal places, have to multiply by 10 to get back to 3-implied decimal places for internal use.
		if ( 2 == roundingWeightPrecision )
			recalculatedWeight *= 10;
	}
	Debug::trace("FrequentShopper.RecalcWt", HobString("recalculatedWeight = ") + HobString(Long::toString(recalculatedWeight)));
}

void FrequentShopper::calculatePackageDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculatePackageDiscount:  ";

	//  TDP = (Total price - Package discount)
	if (totalPrice > discountValue)
		*fsTotalPrice = totalPrice - discountValue;
	else
		*fsTotalPrice = 0;
	*fsSavings = discountValue;

	if (Debug::isTracing("FrequentShopper.Calc"))
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsTotalPrice = ").append(Long::toString(*fsTotalPrice));
		msgDebug2.append(", fsSavings = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
}

void FrequentShopper::calculateICLSASIDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculateICLSASIDiscount:  ";
	HobPrice  discountPrice;

	//  TDP = ((Total price * Discount price) / Unit price)->rm
	if (discountType == Cents_off_unit_price)
		discountPrice = unitPrice - discountValue;
	else
		discountPrice = discountValue;

	// The * 10 is a factor to allow us to apply rounding since this calculation involves prices only.
	HobValue64 testValue64 = ((HobValue64) totalPrice * (HobValue64) discountPrice * 10) / (HobValue64) unitPrice;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" testValue64 = (totalPrice * discountPrice * 10) / unitPrice = (").append(Long::toString(totalPrice));
		msgDebug2.append(" * ").append(Long::toString(discountPrice));
		msgDebug2.append(" * 10) / ").append(Long::toString(unitPrice));
		msgDebug2.append(" = ").append(util_string::int64tostring(testValue64));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	*fsTotalPrice = (HobPrice) Rounding::roundPriceWeight(testValue64, scaleConfig->getPricePrecision() + 1, 0, scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsTotalPrice = roundPriceWeight(").append(util_string::int64tostring(testValue64));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision() + 1));
		msgDebug2.append(", 0, ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(scaleConfig->toStringCfg(fieldOffsetCfgFSRoundingMethod));
		msgDebug2.append(") = ").append(Long::toString(*fsTotalPrice));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
	*fsSavings = totalPrice - *fsTotalPrice; 
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsSavings = totalPrice - fsTotalPrice = ").append(Long::toString(totalPrice));
		msgDebug2.append(" - ").append(Long::toString(*fsTotalPrice));
		msgDebug2.append(" = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
}

void FrequentShopper::calculateRIIDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculateRIIDiscount:  ";
	HobPrice  discountPrice;

	//  TDP = (NW * (Unit price - Cents off))->rm
	if(discountType == Cents_off_unit_price)
		discountPrice = unitPrice - discountValue;
	else
		discountPrice = discountValue;

	HobValue64 testValue64 = (HobValue64) recalculatedWeight * (HobValue64) discountPrice;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" testValue64 = recalculatedWeight * discountPrice = ").append(Long::toString(recalculatedWeight));
		msgDebug2.append(" * ").append(Long::toString(discountPrice));
		msgDebug2.append(" = ").append(util_string::int64tostring(testValue64));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	*fsTotalPrice = (HobPrice) Rounding::roundPriceWeight(testValue64, scaleConfig->getPricePrecision(), scaleConfig->getWeightPrecision(), scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsTotalPrice = roundPriceWeight(").append(util_string::int64tostring(testValue64));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getWeightPrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(scaleConfig->toStringCfg(fieldOffsetCfgFSRoundingMethod));
		msgDebug2.append(") = ").append(Long::toString(*fsTotalPrice));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	*fsSavings = totalPrice - *fsTotalPrice; 
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsSavings = totalPrice - fsTotalPrice = ").append(Long::toString(totalPrice));
		msgDebug2.append(" - ").append(Long::toString(*fsTotalPrice));
		msgDebug2.append(" = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
}

void FrequentShopper::calculateIBMDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculateIBMDiscount:  ";
	HobPrice  centsOffPrice;

	//  TS = (RW * (Unit price - Discount price))->rm
	if(discountType == Cents_off_unit_price)
		centsOffPrice = discountValue;
	else
		centsOffPrice = unitPrice - discountValue;

	HobValue64 testValue64 = (HobValue64) recalculatedWeight * (HobValue64) centsOffPrice;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" testValue64 = recalculatedWeight * centsOffPrice = ").append(Long::toString(recalculatedWeight));
		msgDebug2.append(" * ").append(Long::toString(centsOffPrice));
		msgDebug2.append(" = ").append(util_string::int64tostring(testValue64));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	*fsSavings = (HobPrice) Rounding::roundPriceWeight(testValue64, scaleConfig->getPricePrecision(), scaleConfig->getWeightPrecision(), scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsSavings = roundPriceWeight(").append(util_string::int64tostring(testValue64));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getWeightPrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(scaleConfig->toStringCfg(fieldOffsetCfgFSRoundingMethod));
		msgDebug2.append(") = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	*fsTotalPrice = totalPrice - *fsSavings; 
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsTotalPrice = totalPrice - fsSavings = ").append(Long::toString(totalPrice));
		msgDebug2.append(" - ").append(Long::toString(*fsSavings));
		msgDebug2.append(" = ").append(Long::toString(*fsTotalPrice));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
}

void FrequentShopper::calculateIBMEM2Discount(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculateIBMEM2Discount:  ";
	HobPrice  discountPrice;

	//  TDP = (RW * Discount price)->rm
	if(discountType == Cents_off_unit_price)
		discountPrice = unitPrice - discountValue;
	else
		discountPrice = discountValue;

	HobValue64 testValue64 = (HobValue64) recalculatedWeight * (HobValue64) discountPrice;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" testValue64 = recalculatedWeight * discountPrice = ").append(Long::toString(recalculatedWeight));
		msgDebug2.append(" * ").append(Long::toString(discountPrice));
		msgDebug2.append(" = ").append(util_string::int64tostring(testValue64));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	*fsTotalPrice = (HobPrice) Rounding::roundPriceWeight(testValue64, scaleConfig->getPricePrecision(), scaleConfig->getWeightPrecision(), scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsTotalPrice = roundPriceWeight(").append(util_string::int64tostring(testValue64));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getWeightPrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(scaleConfig->toStringCfg(fieldOffsetCfgFSRoundingMethod));
		msgDebug2.append(") = ").append(Long::toString(*fsTotalPrice));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
	*fsSavings = totalPrice - *fsTotalPrice; 
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsSavings = totalPrice - fsTotalPrice = ").append(Long::toString(totalPrice));
		msgDebug2.append(" - ").append(Long::toString(*fsTotalPrice));
		msgDebug2.append(" = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
}

void FrequentShopper::calculateAdvertisedPrice(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculateAdvertisedPrice:  ";

	//	TDP = (NW * Regular price)->rm
	HobValue64 testValue64 = (HobValue64) netWeight * (HobValue64) discountValue;
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" testValue64 = netWeight * discountValue = ").append(Long::toString(netWeight));
		msgDebug2.append(" * ").append(Long::toString(discountValue));
		msgDebug2.append(" = ").append(util_string::int64tostring(testValue64));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
	*fsTotalPrice = (HobPrice) Rounding::roundPriceWeight(testValue64, scaleConfig->getPricePrecision(), scaleConfig->getWeightPrecision(), scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsTotalPrice = roundPriceWeight(").append(util_string::int64tostring(testValue64));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getWeightPrecision()));
		msgDebug2.append(", ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(scaleConfig->toStringCfg(fieldOffsetCfgFSRoundingMethod));
		msgDebug2.append(") = ").append(Long::toString(*fsTotalPrice));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
	*fsSavings = *fsTotalPrice - totalPrice; 
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsSavings = fsTotalPrice - totalPrice = ").append(Long::toString(*fsTotalPrice));
		msgDebug2.append(" - ").append(Long::toString(totalPrice));
		msgDebug2.append(" = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
}

void FrequentShopper::calculatePercentDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings)
{
	bool traceOn = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "calculatePercentDiscount:  ";

	//	TS = (Total price * Percentage)->rm
	int decimalPlaces = scaleConfig->getPercentagePrecision();
	long divisor = Power::power(10, decimalPlaces);

	HobValue64 tempVal;

	// FT-155 Global Percent Off
	if( scaleConfig->toBoolCfg( fieldOffsetCfgEnableGlobalPercentOff ) )
	{
		// Use the Global Percentage Off value ... even if it is Zero
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" Using Global Percentage Off");
			HobValue tempGlobalPercentageOff = scaleConfig->toIntCfg( fieldOffsetCfgGlobalPercentageOff );
			msgDebug2.append(": Percent Off=");
			msgDebug2.append( Long::toString( tempGlobalPercentageOff ) );
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}

		HobValue64 globalPercentageOff = scaleConfig->toIntCfg( fieldOffsetCfgGlobalPercentageOff );
		tempVal = ( (double) ( totalPrice * globalPercentageOff ) / 10000 );

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" tempVal=");
			//msgDebug2.append( Long::toString( tempVal ) );
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}
	}
	else
	{
		// Use the percent discount for the PLU
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" Using PLU Percent Off Discount");
			msgDebug2.append(": Percent Off=");
			msgDebug2.append( Long::toString( discountValue ) );
			msgDebug2.append(": divisor=");
			msgDebug2.append( Long::toString( divisor ) );
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}

		tempVal = (HobValue)((totalPrice * discountValue) / divisor);

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" tempVal=");
			//msgDebug2.append( Long::toString( tempVal ) );
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		HobString onOrOff = (discountType == Percent_added_net_value) ? "Added" : "Off";
		msgDebug2.append(" Percent ").append(onOrOff);
		msgDebug2.append("  : tempVal = ").append(Long::toString(tempVal));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	if (scaleConfig->getPricePrecision() == 0)
		decimalPlaces = 0;

	*fsSavings = Rounding::roundPriceWeight(tempVal, decimalPlaces + 2, 0, scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(" fsSavings = roundPriceWeight(").append(Long::toString(tempVal));
		msgDebug2.append(", ").append(Integer::toString(decimalPlaces + 2));
		msgDebug2.append(", 0, ").append(Integer::toString(scaleConfig->getPricePrecision()));
		msgDebug2.append(", ").append(scaleConfig->toStringCfg(fieldOffsetCfgFSRoundingMethod));
		msgDebug2.append(") = ").append(Long::toString(*fsSavings));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}

	if(discountType == Percent_added_net_value)
	{
		*fsTotalPrice = totalPrice + *fsSavings;	//  Percent_added_net_value 
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" fsTotalPrice = totalPrice + fsSavings = ").append(Long::toString(totalPrice));
			msgDebug2.append(" + ").append(Long::toString(*fsSavings));
			msgDebug2.append(" = ").append(Long::toString(*fsTotalPrice));
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}
	}
	else
	{
		*fsTotalPrice = totalPrice - *fsSavings;	//  Percent_off_net_value 
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(" fsTotalPrice = totalPrice - fsSavings = ").append(Long::toString(totalPrice));
			msgDebug2.append(" - ").append(Long::toString(*fsSavings));
			msgDebug2.append(" = ").append(Long::toString(*fsTotalPrice));
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}
	}
}


void FrequentShopper::setupFrequentShopperData(short pluDiscountType)
{
	short discountMethod = (short)scaleConfig->toIntCfg(fieldOffsetCfgFSDiscountMethod);
	switch (discountMethod)
	{
		case IBM_EM2_DISCOUNT_METHOD:
			discountFormula = IBM_EM2_FS_FORMULA;
			discountType = Discount_price_by_weight;
			break;
		case IBM_CENTS_OFF_DISCOUNT_METHOD:
			discountFormula = IBM_FS_FORMULA;
			discountType = Cents_off_unit_price;
			break;
		case PERCENT_DISCOUNT_METHOD:
			discountFormula = PERCENT_OFF_FS_FORMULA;
			discountType = Percent_off_net_value;
			break;
		case RII_DISCOUNT_METHOD:
			discountFormula = RII_FS_FORMULA;
			discountType = Cents_off_unit_price;
			break;
		case NCR_DISCOUNT_METHOD:
			discountFormula = NCR_FS_FORMULA;
			discountType = Discount_price_by_weight;
			break;
		case SPECIAL_SAVINGS:
			discountFormula = SPECIAL_SAVINGS_FORMULA;
			discountType = Special_price;
			break;
		case PERCENT_ADDED_METHOD:
			discountFormula = PERCENT_ADDED_FS_FORMULA;
			discountType = Percent_added_net_value;
			break;
		case ICL_SASI_DISCOUNT_METHOD:
			discountFormula = ICL_SASI_FS_FORMULA;
			discountType = Discount_price_by_weight;
			break;

		case FLEX_DISCOUNT_METHOD_PER_PLU:
		case FLEX_DISCOUNT_METHOD_PER_SCALE:
			if((discountMethod == FLEX_DISCOUNT_METHOD_PER_SCALE) ||
				((discountMethod == FLEX_DISCOUNT_METHOD_PER_PLU) && (pluDiscountType == Default_dept_type)))
			{
				discountType = scaleConfig->getDefaultDiscountType();
			}
			else
			{
				discountType = pluDiscountType;
			}

			if(discountType == Discount_per_package)
			{
				discountFormula = PACKAGE_DISCOUNT_FS_FORMULA;
			}
			else
			if(discountType == Percent_off_net_value)
			{
				discountFormula = PERCENT_OFF_FS_FORMULA;
			}
			else
			if(discountType == Percent_added_net_value)
			{
				discountFormula = PERCENT_ADDED_FS_FORMULA;
			}
			else
			if(discountType == Special_price)
			{
				discountFormula = SPECIAL_SAVINGS_FORMULA;
			}
			else
			{
				discountFormula = scaleConfig->getDefaultDiscountFormula();

				//	Check for IBM_ACE_DISCOUNT_FS_FORMULA as the true formula depends on the discountType.
				if (discountFormula == IBM_ACE_DISCOUNT_FS_FORMULA)
				{
					Debug::trace("FrequentShopper", "setupFrequentShopperData():  discountFormula = IBM_ACE_DISCOUNT_FS_FORMULA. discountType = " + HobString(Integer::toString(discountType)));
					if(discountType == Cents_off_unit_price)
					{
						Debug::trace("FrequentShopper", "setupFrequentShopperData():  IBM_ACE. discountFormula now = IBM_FS_FORMULA.");
						discountFormula = IBM_FS_FORMULA;
					}
					else	//	If not Cents_off_unit_price, assume Discount_price_by_weight.
					{
						Debug::trace("FrequentShopper", "setupFrequentShopperData():  IBM_ACE. discountFormula now = IBM_EM2_FS_FORMULA.");
						discountFormula = IBM_EM2_FS_FORMULA;
					}
				}
			}
			break;

		default:
			// Don't set this anymore, setting configuration should be done using the CfgFrequentShopper class.
			//scaleConfig->setDiscountMethod(IBM_DISCOUNT_METHOD);
			discountFormula = IBM_FS_FORMULA;
			discountType = Discount_price_by_weight;
			break;
	}
	if (Debug::isTracing("FrequentShopper"))
	{
		Debug::trace("FrequentShopper", HobString("setupFrequentShopperData():  discountMethod = ") + HobString(Integer::toString(discountMethod)) + 
					HobString("\tpluDiscountType = ") + HobString(Integer::toString(pluDiscountType)) + 
					HobString("\tdiscountFormula = ") + HobString(Integer::toString(discountFormula)) + 
					HobString("\tdiscountType = ") + HobString(Integer::toString(discountType)));
	}
}

/*-*********************************************************************
*
*   Function Name:  calculate_discounts
*
************************************************************************
*
*   Synopsis:   VOID calculate_discounts()
*
*   Description:
*       This function calculates the discounted amount owed and the
*       amount saved off the regular price and assigns the values to
*       the discounted_total_price and discount_customer_savings
*       fields, respectively, in the op_plu data structure.  Whether
*       rounding or truncating is used on the results is set in service
*       mode.
*
*   Parameter List: None
*   Returns:        Nothing
*
*   Example Usage:  calculate_discounts();
*
*   Requirements:  recalculateWeight
*
*   Assumptions:
*       uif_cb->op_plu->price has been calculated
*
*   Side Effects:  none
*
*********************************************************************-*/
/*
*/
void FrequentShopper::calculateDiscounts(const Product *product)
{
	HobPrice fsTotalPrice = 0, fsSavings = 0, fsUnitPrice = 0;
	bool traceOnFSC = Debug::isTracing("FrequentShopper.Calc");
	HobString msgDebug("calculateDiscounts():");

	if (!product)
		return;

	const ProductTransaction *productTransaction = product->getCurrentTransaction();
	if (!productTransaction)
		return;

	Debug::trace("FrequentShopper", msgDebug);

	//	Clear all of the fields for all three levels		
	setTotalPrice1(fsTotalPrice);
	setTotalPrice2(fsTotalPrice);
	setTotalPrice3(fsTotalPrice);
	setTotalSavings1(fsSavings);
	setTotalSavings2(fsSavings);
	setTotalSavings3(fsSavings);
	setUnitPrice1(fsUnitPrice);
	setUnitPrice2(fsUnitPrice);
	setUnitPrice3(fsUnitPrice);

	HobString discType("");

	if (traceOnFSC)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("   unitPrice = ").append(Long::toString(unitPrice));
		msgDebug2.append(",  totalPrice = ").append(Long::toString(totalPrice));
		msgDebug2.append(",  netWeight = ").append(Long::toString(netWeight));
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
	unitPrice = productTransaction->getUnitPrice();
	if (unitPrice <= 0)
		return;
	netWeight = productTransaction->getTotalNetWeight();
	if ((netWeight <= 0) && product->isRandomWeight())
		return;
	totalPrice = productTransaction->getTotalPrice();
	if (totalPrice <= 0)
		return;

	setupFrequentShopperData(product->getDiscountType());

	//  WE DON'T NEED WEIGHTS FOR PACKAGE DISCOUNT OR PERCENTAGES
	//  SO SKIP THIS IN ANY OF THESE CASES.
	if( (discountFormula != PACKAGE_DISCOUNT_FS_FORMULA) &&
		(discountFormula != PERCENT_ADDED_FS_FORMULA) &&
		(discountFormula != PERCENT_OFF_FS_FORMULA) )
	{
		//	We have the exact weight available.  A front-end POS
		//	system, however, will not.  It will calculate the
		//	discounted amount owed and amount saved by first
		//	recalculating the weight.  We have to come up with
		//	the same discount amount owed and amount saved, so
		//	we must use the same (recalculated) weight.

		recalculateWeight(productTransaction->getByCount());

		// For Advertised price (SPECIAL_SAVINGS_FORMULA) we don't use recalculatedWeight but
		// the netWeight. We will then assign the recalculatedWeight to the netWeight bearing in
		// mind the fact that the former always uses 2 decimal places, but our weight use 3.
		if ((discountFormula == SPECIAL_SAVINGS_FORMULA) && product->isFixedWtOrFluidOz())
			netWeight = recalculatedWeight * Power::power(10, scaleConfig->getWeightPrecision() - getRoundingWeightPrecision(netWeight));
	}
	if (traceOnFSC)
	{
		discType = getDiscountTypeString();
		
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("   unitPrice = ").append(Long::toString(unitPrice));
		msgDebug2.append("   totalPrice = ").append(Long::toString(totalPrice));
		msgDebug2.append("   netWeight = ").append(Long::toString(netWeight));
		msgDebug2.append("   discountType = ").append(discType);
		Debug::trace("FrequentShopper.Calc", msgDebug2);
	}
	int theRound, endRounds = (discountFormula == SPECIAL_SAVINGS_FORMULA) ? 1 : 3;
	for(theRound = 0; theRound < endRounds; theRound++)
	{
		switch(theRound)
		{
			case 0:
				discountValue = product->getDiscountValue1();
				break;
			case 1:
				discountValue = product->getDiscountValue2();
				break;
			case 2:
				discountValue = product->getDiscountValue3();
				break;
		}
		if (traceOnFSC)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("   Round = ").append(Integer::toString(theRound));
			msgDebug2.append("   discountValue = ").append(Long::toString(discountValue));
			Debug::trace("FrequentShopper.Calc", msgDebug2);
		}
		if(((discountValue <= 0) || ((unitPrice == 0) || (totalPrice == 0)) ||
			(((discountType == Cents_off_unit_price) || (discountType == Discount_price_by_weight) ||
			  (discountType == Discount_per_package)) && (unitPrice < discountValue)) ||
			((discountType == Special_price) &&	(unitPrice > discountValue))) &&
		   /* HT-155 */
		   ( ( !scaleConfig->toBoolCfg( fieldOffsetCfgEnableGlobalPercentOff ) ) ) ||
		   ( ( !product->hasFrequentShopperLabel() ) ) )
		{
			fsSavings = 0;
			fsTotalPrice = 0;
			fsUnitPrice = 0;
		}
		else
		{
			if(	( scaleConfig->toBoolCfg( fieldOffsetCfgEnableGlobalPercentOff ) ) && 
				( product->hasFrequentShopperLabel() ) )
			{
				// HT-155: If Global Percent Off is enabled, it overrides any other discounts that are set up

				discountValue = scaleConfig->toIntCfg( fieldOffsetCfgGlobalPercentageOff );

				calculatePercentDiscount(&fsTotalPrice, &fsSavings);
			}
			else
			{
				if (product->isByCount() && ((discountFormula != PACKAGE_DISCOUNT_FS_FORMULA) &&
											(discountFormula != PERCENT_ADDED_FS_FORMULA) &&
											(discountFormula != PERCENT_OFF_FS_FORMULA)))
				{
					int newByCount = productTransaction->getByCount();
					int origByCount = product->getOriginalByCount();
					HobPrice tempDiscountValue;

					if(discountType == Cents_off_unit_price)
						tempDiscountValue = unitPrice - discountValue;
					else
						tempDiscountValue = discountValue;

					if (traceOnFSC)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("ByCount: None of the usual FS formulas will be used here.");
						Debug::trace("FrequentShopper.Calc", msgDebug2);
					}
					fsTotalPrice = MetSig::byCountCalcTotalPrice(origByCount, newByCount, tempDiscountValue, product->getExceptionPrice());
					fsSavings = totalPrice - fsTotalPrice;
				}
				else
				{
					switch(discountFormula)
					{
						case PACKAGE_DISCOUNT_FS_FORMULA:
							calculatePackageDiscount(&fsTotalPrice, &fsSavings);
							break;

						case ICL_SASI_FS_FORMULA:
							calculateICLSASIDiscount(&fsTotalPrice, &fsSavings);
							break;

						case RII_FS_FORMULA:
							calculateRIIDiscount(&fsTotalPrice, &fsSavings);
							break;

						case IBM_EM2_FS_FORMULA:
							calculateIBMEM2Discount(&fsTotalPrice, &fsSavings);
							break;

						case PERCENT_ADDED_FS_FORMULA:
						case PERCENT_OFF_FS_FORMULA:
							calculatePercentDiscount(&fsTotalPrice, &fsSavings);
							break;

						case SPECIAL_SAVINGS_FORMULA:
							calculateAdvertisedPrice(&fsTotalPrice, &fsSavings);
							break;

						case IBM_FS_FORMULA:
						case NCR_FS_FORMULA:
						default:				//	IBM_DISCOUNT_METHOD
							calculateIBMDiscount(&fsTotalPrice, &fsSavings);
							break;
					}
				}
			}
		//	if (product->isRandomWeight())		//	PTR 1148
				fsUnitPrice = calculateDiscountUnitPrice(product, fsTotalPrice);
		}

		switch(theRound)
		{
			case 0:
				setTotalPrice1(fsTotalPrice);
				setTotalSavings1(fsSavings);
				setUnitPrice1(fsUnitPrice);
				break;
			case 1:
				setTotalPrice2(fsTotalPrice);
				setTotalSavings2(fsSavings);
				setUnitPrice2(fsUnitPrice);
				break;
			case 2:
				setTotalPrice3(fsTotalPrice);
				setTotalSavings3(fsSavings);
				setUnitPrice3(fsUnitPrice);
				break;
		}

		if (Debug::isTracing("FrequentShopper.CalculatedValues"))
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("   Frequent Shopper Level ").append(Integer::toString(theRound)).append(":  ");
			msgDebug2.append("   Calculated UnitPrice = ").append(Long::toString(fsUnitPrice));
			msgDebug2.append("   Calculated TotalPrice = ").append(Long::toString(fsTotalPrice));
			msgDebug2.append("   Calculated Savings = ").append(Long::toString(fsSavings));
			Debug::trace("FrequentShopper.CalculatedValues", msgDebug2);
		}
	}
	show();
}

HobString FrequentShopper::getDiscountTypeString() const
{
	HobString string;
	switch(discountType)
	{
		case Default_dept_type:
			string = HobString("Default");
			break;
		case Cents_off_unit_price:
			string = HobString("Cents off");
			break;
		case Discount_price_by_weight:
			string = HobString("Discount price");
			break;
		case Percent_off_net_value:
			string = HobString("Percent off");
			break;
		case Special_price:
			string = HobString("Advertised price");
			break;
		case Percent_added_net_value:
			string = HobString("Percent added");
			break;
		case Discount_per_package:
			string = HobString("Package Discount");
			break;
	}
	return string;
}

HobPrice FrequentShopper::calculateDiscountUnitPrice(const Product *product, HobPrice fsTotalPrice)
{
#ifdef FRACTIONAL_AND_POUNDS_FOR
	int			fractional = 1;
#endif
	int			pounds_for = 1;
	long		multiplier = Power::power(10, Configuration::getInstance()->getWeightPrecision());
	HobPrice	fsUnitPrice = 0;

	Debug::trace("FrequentShopper.Calc", HobString("calculateDiscountUnitPrice() ===================    BEGIN    ================ ")); 
	if(product->isFixedWeight())
	{
		//	PTR 1148
		fsUnitPrice =  product ? MetSig::calcFixedWeightPricePerUnit(fsTotalPrice, product->getFixedWeight()) : 0;
	}
	else if(product->isRandomWeight())
	{
#ifdef FRACTIONAL_AND_POUNDS_FOR
		//	Finish this when we handle Fractional and Pounds for pricing
//		if(product->isRandomWeight())
//		{
			if(FRACTIONAL_PRICING)
			{
				fractional = price_modifier & 0xf;
				if(fractional == 0xA)
				{
					if (scaleIsMetric)
						fractional = 100;
				}
			}
			else if((POUNDS_FOR_PRICING) && (!scaleIsMetric))
			{
				pounds_for = (price_modifier >> 4) & 0xF;
			}
//		}
#endif

		if (discountType == Cents_off_unit_price)
		{
			fsUnitPrice = (unitPrice > discountValue) ? (unitPrice - discountValue) : 0;
		}
		else if ((discountType == Percent_off_net_value) ||
			(discountType == Percent_added_net_value) ||
			(discountType == Discount_per_package))
		{
			/* due to rounding errors (loss of number precision with low weight)
			in discount_total_price and price, we need to calculate the
			price (frequent shopper card price) using the total
			price and the discount percentage. ats 5242001 */

			//	PTR 1148 - Don't calculate the Discount Unit price for a byCount as we won't print it anyway.
//			if (product->isByCount())
//			{
//				//	Calculate this on a byCount ONLY if byCount == 1 & Weight on a byCount == true.
//				if (product->isByCountWithWeight() && (netWeight > 0))
//					fsUnitPrice = divideAndRound((fsTotalPrice * pounds_for * multiplier), netWeight, Rounding::CONVENTIONAL_ROUNDING);
//				if (Debug::isTracing("FrequentShopper.Calc"))
//					Debug::trace("FrequentShopper.Calc", HobString("ByCount:  fsUnitPrice = ") + HobString(Long::toString(fsUnitPrice))); 
//			}
//			else
			if (!product->isByCount())
			{
				if (discountType == Percent_off_net_value)
				{
					fsUnitPrice = unitPrice - calculateCentsOff(unitPrice, discountValue);
					if (Debug::isTracing("FrequentShopper.Calc"))
						Debug::trace("FrequentShopper.Calc", HobString("Percent_off:  fsUnitPrice = ") + HobString(Long::toString(fsUnitPrice))); 
				}
				else if (discountType == Percent_added_net_value)
				{
					fsUnitPrice = unitPrice + calculateCentsOff(unitPrice, discountValue);
					if (Debug::isTracing("FrequentShopper.Calc"))
						Debug::trace("FrequentShopper.Calc", HobString("Percent_added:  fsUnitPrice = ") + HobString(Long::toString(fsUnitPrice))); 
				}
				else if (discountType == Discount_per_package)
				{
					fsUnitPrice = divideAndRound((fsTotalPrice * pounds_for * multiplier), netWeight, Rounding::CONVENTIONAL_ROUNDING);
					if (Debug::isTracing("FrequentShopper.Calc"))
						Debug::trace("FrequentShopper.Calc", HobString("PackageDiscount:  fsUnitPrice = ") + HobString(Long::toString(fsUnitPrice))); 
				}
			}
		}
		else
		{
			fsUnitPrice = discountValue;
		}
	}
	Debug::trace("FrequentShopper.Calc", HobString("calculateDiscountUnitPrice() ===================     END     ================ ")); 
	return fsUnitPrice;
}


HobPrice FrequentShopper::calculateCentsOff(HobPrice unitPrice, HobPrice discountValue)
{
	//	Called for Percent_off_net_value & Percent_added_net_value
	HobPrice centsOff;
	int decimalPlaces;
	// HT-155
	if( scaleConfig->toBoolCfg( fieldOffsetCfgEnableGlobalPercentOff ) )
		decimalPlaces = 8;
	else
		decimalPlaces = scaleConfig->getPricePrecision() + scaleConfig->getPercentagePrecision() + 2;

	Debug::trace("FrequentShopper.Calc", HobString("calculateCentsOff()  decimalPlaces=") + HobString( Long::toString( decimalPlaces ) ) );
	Debug::trace("FrequentShopper.Calc", HobString("calculateCentsOff()  discountValue=") + HobString( Long::toString( discountValue ) ) );

	centsOff = unitPrice * discountValue;
	
	Debug::trace("FrequentShopper.Calc", HobString("calculateCentsOff()  Before rounding:  centsOff = ") + HobString(Long::toString(centsOff))); 
	if(centsOff)
	{
		centsOff = Rounding::roundPriceWeight(centsOff, decimalPlaces, 0, scaleConfig->getPricePrecision(), (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingMethod));
		Debug::trace("FrequentShopper.Calc", HobString("calculateCentsOff()   After rounding:  centsOff = ") + HobString(Long::toString(centsOff))); 
	}
	return(centsOff);
}

long FrequentShopper::divideAndRound(long dividend, int divisor, short rounding_method)
{
	long result;
	long roundingAddend;

	roundingAddend = GetRoundingAddend(rounding_method);
	Debug::trace("FrequentShopper.Calc", HobString("divideAndRound():  result =  (((") + HobString(Long::toString(dividend)) + HobString(" * 10) / ") + HobString(Integer::toString(divisor)) + HobString(") + ") + HobString(Integer::toString(roundingAddend)) + HobString(") / 10"));
	result = (((dividend * 10) / divisor) + roundingAddend) / 10;
	Debug::trace("FrequentShopper.Calc", HobString("divideAndRound():  result =  ") + HobString(Long::toString(result))); 
	return(result);
}


int FrequentShopper::GetRoundingAddend(short rounding_method)
{
	int roundingAddend = 0;
	
	if(rounding_method == Rounding::ROUND_UP)
		roundingAddend = 9;
	else
	if(rounding_method == Rounding::CONVENTIONAL_ROUNDING)
		roundingAddend = 5;

	return(roundingAddend);
}


void FrequentShopper::show()
{
	if (Debug::isTracing("FrequentShopper.show1"))
	{
		Debug::trace("FrequentShopper.show1", HobString("  totalPrice1   =  ") + HobString(Long::toString(totalPrice1))); 
		Debug::trace("FrequentShopper.show1", HobString("  totalSavings1 =  ") + HobString(Long::toString(totalSavings1))); 
		Debug::trace("FrequentShopper.show1", HobString("  unitPrice1    =  ") + HobString(Long::toString(unitPrice1)));
	} 
	if (Debug::isTracing("FrequentShopper.show2"))
	{
		Debug::trace("FrequentShopper.show2", HobString("  totalPrice2   =  ") + HobString(Long::toString(totalPrice2))); 
		Debug::trace("FrequentShopper.show2", HobString("  totalSavings2 =  ") + HobString(Long::toString(totalSavings2))); 
		Debug::trace("FrequentShopper.show2", HobString("  unitPrice2    =  ") + HobString(Long::toString(unitPrice2)));
	} 
	if (Debug::isTracing("FrequentShopper.show3"))
	{
		Debug::trace("FrequentShopper.show3", HobString("  totalPrice3   =  ") + HobString(Long::toString(totalPrice3))); 
		Debug::trace("FrequentShopper.show3", HobString("  totalSavings3 =  ") + HobString(Long::toString(totalSavings3))); 
		Debug::trace("FrequentShopper.show3", HobString("  unitPrice3    =  ") + HobString(Long::toString(unitPrice3)));
	} 
}


short FrequentShopper::getRoundingWeightPrecision(HobWeight netWeight)
{
	// SOF-3864
	// As of 5/18/2016, all weighers are 3-implied decimal places, even truly single range weighers (which are only used in wrapping systems (Access)!
	//
	// The "Advertised Price" frequent shopper method computes discount internally and has no
	// dependancy on POS device (we are not trying to match the precision of a POS)... in 2016 we
	// made change to store all weights internally as three decimal digits so here we should always
	// use 3 decimal digits for rounding.
	const int discountMethod = scaleConfig->toIntCfg(fieldOffsetCfgFSDiscountMethod);

	if ( SPECIAL_SAVINGS_FORMULA == discountFormula )
		return 3;

	//	This method was created because of the difference in weight precision between the EPP and the HLX.
	//	The HLX in Avoirdupois is set for 3 decimal places. The EPP uses 2, as well as Ultima and Quantum.
	//	The problem arises at the front end in the store - the POS system. Currently all POS systems are set
	//	for 2 decimal weight precision. If you have a store with EPP's, Quantums and HLX's, they will all need
	//	to match the front end. The decision was made by Dave Miller to set the rounding weight precision
	//	to 2 for the HLX in Avoirdupois.
	short precision = scaleConfig->getEffectiveWeightPrecision();
	if (precision == 3 && MetSig::isScaleAvoir())
	{
		// FT-861
		if ( 2 == (short)scaleConfig->toIntCfg(fieldOffsetCfgFSRoundingWeightPrecision) ||
		     ( MetSig::isScaleMetric() && netWeight >= Configuration::getInstance()->toIntCfgI(CfgInternal::DUALRANGELIMITMETRIC) ) ||
		     ( MetSig::isScaleAvoir() && netWeight >= Configuration::getInstance()->toIntCfgI(CfgInternal::DUALRANGELIMITAVOIR) ) )
			precision = 2;
	}
	return precision;
}
