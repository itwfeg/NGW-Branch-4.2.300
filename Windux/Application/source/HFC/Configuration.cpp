/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include <math.h>

#include "Configuration.h"
#include "CfgScale.h"
#include "MetSig.h"
#include "CfgItem.h"
#include "HobDateTime.h"
#include "HobTokenizerCSV.h"
#include "ConfigurationManager.h"
#include "SessionManager.h"
#include "Session.h"
#include "LabelApplier.h"
#include "Product.h"
#include "ProductTransaction.h"
#include "HfcWeigherStatus.h"
#include "HfcWeigherConfig.h"
#include "FpmManager.h"
#include "CuttingTest.h"
#include "BackupRestoreEvent.h"

#include "DBAccess.h"

#include "DBManager.h"
#include "DBConfigurationsRM.h"
#include "DBConfiguration.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Bool.h"
#include "util/e_string/utilstring.h"
#include "properties/properties.h"

#include "MultiLanguageText.h"

#include "PythonScripter.h"



using namespace std;
using namespace edb;


Configuration *Configuration::instance = 0;

int Configuration::_printToScreenBarcodeFilenameCtr_ = 1;



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Configuration::dataType = 0;
std::string Configuration::dataTypeName("Configuration"); 
#endif



Configuration *Configuration::getInstance()
{
	if (instance == 0)
		instance = new Configuration();
	return instance;
}


/******************************************************************************
 *
 * Formatting Routines
 *
 *****************************************************************************/
std::string Configuration::formatHobPrice(const HobPrice& price, const bool useDefaultDecimalSeparator, int flags)
{
	bool traceOn = Debug::isTracing("Configuration.formatPrice");

	if ( traceOn )
		Debug::trace("Configuration.formatPrice", "Price: " + longtostring(price) + " Flags: " + inttostring(flags));

	HobString result;

	if ( flags & UseOldStyle )
	{
		formatPrice(price, result, useDefaultDecimalSeparator, flags & AppendPerWeightPFFlag ? AppendPerWeightText : 0, (flags & SignedValuePFFlag) == SignedValuePFFlag);
	}
	else
	{
		formatDecimalValue(price, result,
				flags & SecondaryCurrencyPFFlag ? getSecondaryMonetaryDecimalDigits() : getPrimaryMonetaryDecimalDigits(),
				useDefaultDecimalSeparator, (flags & SignedValuePFFlag) == SignedValuePFFlag, true);

		if ( flags & UseMonetarySymbolPFFlag )
		{
			if ( getMonetaryRule() == CfgCurrency::MonetaryRuleTrailing )
				result.append(flags & SecondaryCurrencyPFFlag ? getSecondaryMonetarySymbol() : getPrimaryMonetarySymbol());
			else
				result.insert(0, flags & SecondaryCurrencyPFFlag ? getSecondaryMonetarySymbol() : getPrimaryMonetarySymbol());
		}

		if ( flags & AppendPerWeightPFFlag )
		{
			if ( MetSig::isScaleMetric() )
				result.append("/kg");
			else
				result.append("/lb");
		}
	}

	if ( traceOn )
		Debug::trace("Configuration.formatPrice", "result: '" + result + "'");

	return result;
}


void Configuration::formatPrice(const HobPrice price, HobString &result, const bool useDefaultDecimalSeparator, const short formatIndicator, bool isSigned)
{
	bool traceOn = Debug::isTracing("Configuration.formatPrice");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "price = ";
		msgDebug.append(Long::toString(price)).append("  formatIndicator = ");
		msgDebug.append(Integer::toString(formatIndicator));
		Debug::trace("Configuration.formatPrice", msgDebug);
	}

//	switch (formatIndicator)
//	{
//		default:
//		{
			formatDecimalValue(price, result, getPricePrecision(), useDefaultDecimalSeparator, isSigned);
//			break;
//		}
//	}

	if (formatIndicator == AppendPerWeightText)
	{
		if ( MetSig::isScaleMetric() )
			result.append("/kg");
		else
			result.append("/lb");
	}

	if (traceOn)
	{
		msgDebug = "result = [";
		msgDebug.append(result).append("]\n");
		Debug::trace("Configuration.formatPrice", msgDebug);
	}
}


// Returns the print method ultimately used to format the unit price string.  For example, if NoFractionalCents is the user-configured method, and the division results in fractional cents,
// then we go to the backup method (internal config parameter 'unitPricePrintExactCalcBackupMethod').  If this backup method is to use the traditional approach, the word 'For', we don't want
// to display that on the UI, since it never has.  So checking the return value indicates that the traditional method was used, so we can blank out the UI value, but still print it on the label.
UnitPricePrintMethods Configuration::formatUnitPriceForPrinting(const Product *pCurrentProduct, const ProductTransaction *pCurrentProductTransaction, HobString &result)
{
	UnitPricePrintMethods method = UnitPricePrintMethodUnknown;
	result = "";

	if (!pCurrentProduct || !pCurrentProductTransaction)
		return UnitPricePrintMethodUnknown;

	hfc::MultiLanguageText internationalize;
	Configuration *pConfig = Configuration::getInstance();

	// This code is originally from PrinterManager, when determining the UNIT_PRICE data to be post-imaged, in updateFormPostImageFields().

	if (pCurrentProduct->isByCount())
	{
		Debug::trace("Configuration.formatUnitPrice", "formatting unit price for by count item");
		bool useTraditionalMethod = true;

		if ( pCurrentProductTransaction->getByCount() == 1 && pConfig->toBoolCfg(fieldOffsetCfgPFWeightOnByCountPrinted) )
		{
			result = "";
			return BlankUnitPrice;
		}

		int byCountToUse = 0;
		// If the operator ever changes the price from what it was in the database (or if it's forced price) we won't recalculate the total price using the by count value,
		// whether it was changed or not.  The by count value for the unit price string will be the current by count value, which is in the product object.
		if ( pCurrentProduct->isUnitPriceChanged() )
			byCountToUse = pCurrentProduct->getByCount();
		else
		{
			if ( pCurrentProduct->isByCountForced() )
				byCountToUse = 1;
			else if ( pCurrentProduct->getExceptionPrice() > 0 )
				// When we have an exception price, the current value of the by count is always used in the unit price string.
				byCountToUse = pCurrentProduct->getByCount();
			else
				byCountToUse = pCurrentProductTransaction->getOldByCount();
		}

		HobPrice unitPriceToUse = 0;
		// If the operator ever changes the price from what it was in the database (or if it's forced price) we won't recalculate the total price using the by count value,
		// whether it was changed or not.  The unit price value for the unit price string will be the current price value ("Total Price" field on the screen), which is in the 
		// current value in the item's transaction object.
		if ( pCurrentProduct->isUnitPriceChanged() )
			unitPriceToUse = pCurrentProductTransaction->getTotalPrice();
		else
		{
			// Exception price is not used if it's a forced by count item, so only use the total price in the transaction if we have an exception price, and it's not a forced by count.
			if (!pCurrentProduct->isByCountForced() && pCurrentProduct->getExceptionPrice() > 0)
				unitPriceToUse = pCurrentProductTransaction->getTotalPrice();
			else
				unitPriceToUse = pCurrentProductTransaction->getUnitPrice();
		}

		// If we've enabled, through internal config, the additional methods of printing the unit price on the label, then print the correct one.
		method = (UnitPricePrintMethods) pConfig->toIntCfg(fieldOffsetCfgUnitPricePrintMethod);

		bool stillNeedToFormat = true;
		while (stillNeedToFormat)
		{
			stillNeedToFormat = false;  // assume we're going to format the unit price string on this pass

			if (std::string::npos != pConfig->toStringCfgI(fieldOffsetCfgIUnitPricePrintOptionsEnabled).find(inttostring(method)))
			{
				Debug::trace("Configuration.formatUnitPrice", string("Unit Price Print Method = ") + inttostring((int)method));

				switch ( method )
				{
					case NoFractionalCents:
					{
						// if unit price calculates with no fractional cents, print it; otherwise, use today's functionality
						HobPrice resultUnitPrice = 0;
						bool noFractionalCents = MetSig::calculateUnitPriceNoFractionalCents(&resultUnitPrice, unitPriceToUse, byCountToUse);
						if (noFractionalCents)
						{
							useTraditionalMethod = false;
							result = pConfig->formatHobPrice(resultUnitPrice, false, (pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));
							Debug::trace("Configuration.formatUnitPrice", string("NoFractionalCents: ") + result);
						}
						else
						{
							if ( method != pConfig->toIntCfgI(fieldOffsetCfgIunitPricePrintExactCalcBackupMethod) )
							{
								// Use the alternate method when we have fractional cents
								stillNeedToFormat = true;  // go back through and format it for the alternate method
								method = (UnitPricePrintMethods) pConfig->toIntCfgI(fieldOffsetCfgIunitPricePrintExactCalcBackupMethod);
							}
						}
						break;
					}

					case BCSlashPrice:
					case BCForPrice:
					{
						// print it either like competition (Toledo) does; '4 / $1.99', or '4 For $1.99'
						useTraditionalMethod = false;

						string priceString = pConfig->formatHobPrice(unitPriceToUse, false, (pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));
						if (byCountToUse >= 1)
							result = inttostring(byCountToUse).append((method == BCSlashPrice) ? " / " : " For ").append(priceString);
						Debug::trace("Configuration.formatUnitPrice", string((method == BCSlashPrice) ? "BCSlashPrice: " : "BCForPrice: ") + result);
						break;
					}

					case BlankUnitPrice:
					{
						useTraditionalMethod = false;
						result = "";
						break;
					}

					case RoundedUnitPrice:
					{
						useTraditionalMethod = false;
						double unitPriceD = ((double)unitPriceToUse) / byCountToUse;
						HobPrice roundedUnitPrice = MetSig::calculateUnitPriceRounded(unitPriceToUse, byCountToUse);
						result = pConfig->formatHobPrice(roundedUnitPrice, false, pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0);
						break;
					}

					default:
					{
						// remember that method = 1 is the method that has been used for years, and if the value is anything other than the known alternate methods,
						// then use this long-standing existing method, by leaving var useTraditionalMethod set to true
						break;
					}
				}
			}
		}

		if (useTraditionalMethod)
		{
			method = ForText;

			// FT-716, FT-718 [begin]
			// switched code to use what was in printer manager; original code commented below
			// this code also used for accumulate items... so if you get some sort of bug that accumulate
			// items was printing unit price for by count and now it is not this is why... furthermore that
			// behavior was (most likley) incorrect
			if ((pCurrentProductTransaction->getByCount() > 1) || (pCurrentProductTransaction->getByCount() == 1 && pConfig->toBoolCfg(fieldOffsetCfgPFOneOnByCountPrinted)))
			{
				result = internationalize("For");
			}
			/*
				if ( pCurrentProductTransaction->getByCount() > 1 ) // && Configuration::getInstance()->printingActualUnitPriceForByCountItems()
				{
					double unitPrice = ((double) pCurrentProductTransaction->getTotalPrice()) / pCurrentProductTransaction->getByCount();
					HobValue roundedUnitPrice = Rounding::roundPerWM(unitPrice);
					result = pConfig->formatHobPrice(roundedUnitPrice, false, pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0);
				}
				else
					result = internationalize("For");
			*/
			// FT-716, FT-718 [end]
		}
	}
	else if (pCurrentProduct->isRandomWeight())
	{
		if (MetSig::isScaleAvoir() && pConfig->isPoundsForPricingEnabled() && pCurrentProductTransaction->getPriceModifier() > 1)
		{
			result = Integer::toString(pCurrentProductTransaction->getPriceModifier());
			result.append(internationalize("lb_For"));
		}
		else if (MetSig::isScaleAvoir() && pConfig->isFractionalPricingEnabled() && pCurrentProductTransaction->getPriceModifier() > 1)
		{
			result = pConfig->formatHobPrice(pCurrentProductTransaction->getUnitPrice(), false,
				(pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));
			result.append("/\n");
			if ( 50 == pCurrentProductTransaction->getPriceModifier() )
				result.append("1/2lb");
			else if ( 25 == pCurrentProductTransaction->getPriceModifier() )
				result.append("1/4lb");
		}
		else if (MetSig::isScaleMetric() && pConfig->isFractionalPricingEnabled() && pCurrentProductTransaction->getPriceModifier() > 1)
		{
			result = pConfig->formatHobPrice(pCurrentProductTransaction->getUnitPrice(), false,
				(pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));
			result.append("/\n");
			result.append("100g");
		}
		else
		{
//								pConfig->formatPrice(pCurrentProductTransaction->getUnitPrice(), unitPriceString, AppendPerWeightText);
			result = pConfig->formatHobPrice(pCurrentProductTransaction->getUnitPrice(), false,
												Configuration::AppendPerWeightPFFlag | (pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));

//								if (printedFields.isCurrencySymbolPrinted() == true)
//								{
//									unitPriceString.insert(0, pConfig->getMonetarySymbol());
//								}
		}
	}
	else if (pCurrentProduct->isFixedWtOrFluidOz())
	{
		if (pConfig->toBoolCfg(fieldOffsetCfgPFUnitPricePrinted) && pCurrentProductTransaction->getFixedWeight() > 0)
		{
			result = pConfig->formatHobPrice(pCurrentProductTransaction->getFixedWeightUnitPrice(), false,
												Configuration::AppendPerWeightPFFlag | (pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0));
//								pConfig->formatPrice(pCurrentProductTransaction->getFixedWeightUnitPrice(), unitPriceString, AppendPerWeightText);
//								if (printedFields.isCurrencySymbolPrinted() == true)
//								{
//									unitPriceString.insert(0, pConfig->getMonetarySymbol());
//								}
		}
		else if (pCurrentProductTransaction->getByCount() > 0)
			result = internationalize("For");
	}
	return method;
}

short Configuration::digitsPrecision()
{
	// So anyone anywhere can find out. Lots of places use this same code in here and could be directed this way
	Configuration *pConfig = Configuration::getInstance();
	short digits = 0;

	int flags = (pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted) ? Configuration::UseMonetarySymbolPFFlag : 0);
	digits = flags & SecondaryCurrencyPFFlag ? getSecondaryMonetaryDecimalDigits() : getPrimaryMonetaryDecimalDigits();

	return digits;
}

void Configuration::formatValue(const HobValue value, HobString &result, const short formatIndicator, const bool useDefaultDecimalSeparator)
{
	formatDecimalValue(value, result, formatIndicator, useDefaultDecimalSeparator);
}

void Configuration::formatWeight(const HobWeight weight, HobString &result, const bool useDefaultDecimalSeparator, const short formatIndicator)
{
	bool traceOn = Debug::isTracing("Configuration.formatWeight");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "formatWeight():  ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(",  weight = ").append(Long::toString(weight));
		msgDebug2.append(",  formatIndicator = ").append(Integer::toString(formatIndicator));
		Debug::trace("Configuration.formatWeight", msgDebug2);
	}

	short theFormatIndicator = formatIndicator;
	bool appendWeightTextFlag = false;
	if (theFormatIndicator & AppendWeightText)
		appendWeightTextFlag = true;
	theFormatIndicator &= ~AppendWeightText;

	// HT-176: fix weigher precision (revert HA-226)
	if ( theFormatIndicator == 0 )
		if ( MetSig::isWeigherConfiguredForSingleRange() )
			theFormatIndicator = DualRangeModeDown;

	HobString tempStr;
	if (traceOn)
	{
		Debug::trace("Configuration.formatWeight", "formatweight(): theFormatIndicator = " + Integer::toString(theFormatIndicator));
	}

	switch (theFormatIndicator)
	{
		case PrecisionPlusOne:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "PrecisionPlusOne");
			formatDecimalValue(weight, result, getWeightPrecision((short)PrecisionPlusOne), useDefaultDecimalSeparator, true);
			break;
		}
		case FixedWeightText:
		{
			// from format_bakery_weight_form(), in load_form.c (Quantum)
			/***********************************************/
			/* Net weights can be expressed in two ways.   */
			/* 1). As whole ounces as in 15 OZ             */
			/* 2). Or tenths of an ounce as in 15.4 OZ     */
			/*                                             */
			/* In addition the weight can be printed in    */
			/* four different formats.                     */
			/* 1). < 1 lb no tenths as "12 OZ"             */
			/* 2). < 1 lb in tenths  as "12.4 OZ"          */
			/* 3). > 1 lb to 4 lb as "35 OZ (2 LB 3 Oz)"   */
			/* 4). > 4 lbs as "4 LB 3 OZ"                  */
			/*                                             */
			/* If formats 3 or 4 above are used and tenths */
			/* of an ounce is configured on then the value */
			/* printed will be rounded to the nearest ounce*/
			/*                                             */
			/***********************************************/

			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "FixedWeightText");

			if (toBoolCfg(fieldOffsetCfgPFNetWtOnFixedWeightAlways))
				result.append("NET WT ");

			if ( MetSig::isScaleMetric() )
			{
				// HT-503 - g not printing if < 1000 g. Only prints kg. Also always print NET WT on front of string

				if (weight < 1000)
				{
					result.append(Long::toString(weight));
					result.append(" g ");
				}
				else
				{
					HobString tmpStr;
					formatFixedWeightValue(weight, tmpStr, useDefaultDecimalSeparator);
					result.append(tmpStr);
				}
			}
			else
			{
				HobString tmpStr;
				formatFixedWeightValue(weight, tmpStr, useDefaultDecimalSeparator);

//METSIG??  NO, just used to determine which format to use
				HobWeight lbsMultiple = 16 * (isFixedWeightInTenths() ? 10 : 1);

				if ((weight < lbsMultiple) || (weight >(4 * lbsMultiple)))
				{
					result.append(tmpStr);
				}
				else
				{
					HobString myWeightStr;
					if (isFixedWeightInTenths())
						formatDecimalValue(weight, myWeightStr, 1, true);
					else
						myWeightStr = Long::toString(weight);
					result.append(myWeightStr);
					result.append(" oz");

					result.append(" (");
					result.append(tmpStr);
					result.append(")");
				}
			}
			break;
		}
		case MetricRuleWeight:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "MetricRuleWeight");
			if (toBoolCfg(fieldOffsetCfgPFNetWtOnFixedWeightAlways))
				result.append("NET WT ");

			if (MetSig::isScaleMetric())
			{
				if (weight < 1000)
				{
					result.append(Long::toString(weight));
					result.append(" g (");
				}
				else
				{
					formatDecimalValue(weight, result, getEffectiveWeightPrecision(), useDefaultDecimalSeparator, true);
					result.append(" kg (");
				}

				HobWeight ounces = MetSig::gramsToOunces(weight);
				// remember, the calculated ounces will be in tenths of an ounce.
				if (ounces < 160)   // (ounces < 1lb)
				{
					HobString ouncesStr;
					formatDecimalValue(ounces, ouncesStr, 1, useDefaultDecimalSeparator, true);
					result.append(ouncesStr);
					result.append(" oz)");
				}
				else
				{
//METSIG??  NO, setting up the counts of a temporary weigherStatus for a fixed weight to get the 'weight' 
// Is this next line even correct?????????????????  The variable 'weight' is the metric weight.
					double myPounds = weight / 160.0 * 10000;
					HobWeight wght = myPounds;

					HfcWeigherStatus ws;
					ws.setupToDetermineMetricRuleWeight(wght, WeighMode_Avoir);

					wght = ws.getWeightUsingWeighMode(WeighMode_Avoir);
					if ( 2 == getEffectiveWeightPrecision() )
						wght = MetSig::divideWeightForSingleRange(wght);
					//HobValue myLPounds = Rounding::roundPerWM(myPounds);
					HobString poundsStr;
					formatDecimalValue(wght, poundsStr, getEffectiveWeightPrecision(), useDefaultDecimalSeparator, true);

					result.append(poundsStr);
					result.append(" lb)");
				}
			}
			else
			{
				if (Configuration::getInstance()->isFixedWeightInTenths())
				{
					if (weight < 160)
					{
						formatDecimalValue(weight, result, 1, useDefaultDecimalSeparator, true);
						result.append(" oz (");
					}
					else
					{
						HobWeight wght = MetSig::ouncesToPoundCounts(weight, true);

						HfcWeigherStatus ws;
						ws.setupToDetermineMetricRuleWeight(wght, WeighMode_Avoir);

						wght = ws.getWeightUsingWeighMode(WeighMode_Avoir);
						if (2 == getEffectiveWeightPrecision())
							wght = MetSig::divideWeightForSingleRange(wght);
						//HobValue myLPounds = Rounding::roundPerWM(myPounds);
						HobString poundsStr;
						formatDecimalValue(wght, poundsStr, getEffectiveWeightPrecision(), useDefaultDecimalSeparator, true);

						result.append(poundsStr);
						result.append(" lb (");
					}
				}
				else
				{
					if (weight < 16)
					{
						result.append(Long::toString(weight));
						result.append(" oz (");
					}
					else
					{
						HobWeight wght = MetSig::ouncesToPoundCounts(weight, false);

						HfcWeigherStatus ws;
						ws.setupToDetermineMetricRuleWeight(wght, WeighMode_Avoir);

						wght = ws.getWeightUsingWeighMode(WeighMode_Avoir);
						if (2 == getEffectiveWeightPrecision())
							wght = MetSig::divideWeightForSingleRange(wght);

						HobString poundsStr;
						formatDecimalValue(wght, poundsStr, getEffectiveWeightPrecision(), useDefaultDecimalSeparator, true);

						result.append(poundsStr);
						result.append(" lb (");
					}
				}

				HobWeight grams = MetSig::ouncesToGrams(weight);
				if (grams < 1000)
				{
					result.append(Long::toString(grams));
					result.append(" g)");
				}
				else
				{
					//	PTR 725. Regardless of whether the scale is Avoir or Metric - we want a metric format.

//METSIG??  NO, setting up the counts of a temporary weigherStatus for a fixed weight to get the 'weight' 
					HfcWeigherStatus ws;
					ws.setupToDetermineMetricRuleWeight(grams * 10, WeighMode_Metric);

					HobWeight wght = ws.getWeightUsingWeighMode(WeighMode_Metric);
					HobString kgStr;
					formatDecimalValue(wght, kgStr, 3, useDefaultDecimalSeparator, true);

					result.append(kgStr);
					result.append(" kg)");
				}
			}
			break;
		}
		case DualRangeModeDown:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "DualRangeModeDown");

			// The weight coming in always has three implied decimal places.  If we are only supposed to display
			// two decimal places then we divide the weight by 10 to remove the third decimal place.
			if ( MetSig::isScaleMetric() )
				formatDecimalValue(weight, result, getWeightPrecision(), useDefaultDecimalSeparator, true);
			else
				formatDecimalValue((HobValue)( MetSig::divideWeightForSingleRange(weight) ), result, getWeightPrecision()-1, useDefaultDecimalSeparator, true);

			break;
		}
		case KeyboardNetWeight:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "KeyboardNetWeight");

			formatDecimalValue(weight, result, getKeyboardNetWeightPrecision() , useDefaultDecimalSeparator, true);
			
			/*
			 //HTA-100 removed the -1 in line below and replaced with the code above
				//formatDecimalValue((HobValue)(weight/10), result, getWeightPrecision()-1, useDefaultDecimalSeparator, true);
			*/
			break;
		}

		//	This came from vxWorks Access because the browser was calculating the fixed weight to display, sometimes incorrectly.
		case OperateFixedWeightText:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "OperateFixedWeightText");

			/********************************************************/
			/*	This new format provides the Fixed weight in ounces	*/
			/*	as well as the formatted Net weight in pounds and	*/
			/*	ounces. This is done by separating the two strings	*/
			/*	with an exclamation point (!). So a fixed weight of */
			/*	18 ounces would be formatted as "18!1 lb 2 oz".		*/
			/*	This was done so the JavaScript would not have to	*/
			/*	calculate the pounds and ounces for the second Net	*/
			/*	weight field on the Run screen (as floating point	*/
			/*	calculations did not always come out correctly).	*/
			/********************************************************/

			if (MetSig::isScaleMetric())
			{
				if (traceOn)
					Debug::trace("Configuration.formatWeight", msgDebug + "Metric - Calling Long::toString(" + Long::toString(weight) + ")");
				result = Long::toString(weight);
			}
			else
			{
				if (isFixedWeightInTenths())
				{
					if (traceOn)
						Debug::trace("Configuration.formatWeight", msgDebug + "Avoir - Calling formatDecimalValue(" + Long::toString(weight) + ", result, 1)");
					formatDecimalValue(weight, result, 1, useDefaultDecimalSeparator);
				}
				else
				{
					if (traceOn)
						Debug::trace("Configuration.formatWeight", msgDebug + "Calling Long::toString(" + Long::toString(weight) + ")");
					result = Long::toString(weight);
				}
			}

			result.append("!");

			HobString tmpStr;
			formatFixedWeightValue(weight, tmpStr, useDefaultDecimalSeparator);
			result.append(tmpStr);

			break;
		}
		default:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "default");
			// HA-226 Weigher displaying 0.0 lbs

			formatDecimalValue( ( ( getEffectiveWeightPrecision() == 2 ) ? ((HobValue)(weight/10)) : weight ), result, getEffectiveWeightPrecision(), useDefaultDecimalSeparator, true );
			break;
		}
	}

	if (appendWeightTextFlag)
	{
		if (MetSig::isScaleMetric())
			result.append(" kg");
		else
			result.append(" lb");
	}

	if (traceOn)
	{
		msgDebug = "result = [";
		msgDebug.append(result).append("]\n");
		Debug::trace("Configuration.formatWeight", msgDebug);
	}
}


void Configuration::formatDecimalValue(const HobValue value, HobString &result, const short precision, const bool useDefaultDecimalSeparator, const bool isSigned, const bool leadingZero)
{
	bool traceOn = Debug::isTracing("Configuration.formatValue");
	if (traceOn)
		Debug::trace("Configuration.formatValue", string("formatDecimalValue: value=") + longtostring(value) + "  precision=" + inttostring(precision) + "  useDefaultDecimalSeparator=" + booltostring(isSigned) + "  leadingZero=" + booltostring(leadingZero));

	int pos;

	HobValue tmpValue = value;
	if (tmpValue < 0)
		tmpValue = -value;

	result = Long::toString(tmpValue);
	pos = (int)result.length() - precision;

	if (pos < 0)
	{
		result.insert((std::string::size_type) 0, -pos, '0');
		pos = 0;
	}

	if (!useDefaultDecimalSeparator)
		result.insert(pos, HobString(1, getDecimalSeparator()));
	else
		result.insert(pos, DefaultDecimalSeparator);

	if (leadingZero && (pos == 0))
		result.insert(0, "0");

	if (isSigned && (value < 0))
		result.insert(0, "-");

	if (traceOn)
		Debug::trace("Configuration.formatValue", string("formatDecimalValue: result=") + result);
}


void Configuration::formatVolume(const HobWeight totalOunces, HobString &result, const short formatIndicator, const bool useDefaultDecimalSeparator, const bool maxAtPints)
{
	//	In the Quantum code, it only seems to print Volume in pints and ounces, even if the total volume is in gallons.
	//	maxAtPints will be used for this purpose. By default, it is false.
	bool traceOn = Debug::isTracing("Configuration.formatWeight");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "formatVolume():  ";
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(",  totalOunces = ").append(Long::toString(totalOunces));
		msgDebug2.append(",  formatIndicator = ").append(Integer::toString(formatIndicator));
		msgDebug2.append(",  maxAtPints = ").append(Bool::toString(maxAtPints));
		msgDebug2.append(",  isScaleMetric() = ").append(Bool::toString(MetSig::isScaleMetric()));
		Debug::trace("Configuration.formatWeight", msgDebug2);
	}

	if (MetSig::isScaleMetric() && (formatIndicator != OperateFixedWeightText))
		return;

	HobString tmpStr;

	switch (formatIndicator)
	{
		case FixedWeightText:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "FixedWeightText");
			formatFixedVolumeValue (totalOunces, tmpStr, formatIndicator, maxAtPints, useDefaultDecimalSeparator);
			result.append(tmpStr);
			break;
		}
		case OperateFixedWeightText:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "OperateFixedWeightText");

			if ( isFixedWeightInTenths() )
			{
				if (traceOn)
					Debug::trace("Configuration.formatWeight", msgDebug + "Calling formatDecimalValue(" + Long::toString(totalOunces) + ", result, 1)");
				formatDecimalValue(totalOunces, result, 1, useDefaultDecimalSeparator);
			}
			else
			{
				if (traceOn)
					Debug::trace("Configuration.formatWeight", msgDebug + "Calling Long::toString(" + Long::toString(totalOunces) + ")");
				result = Long::toString(totalOunces);
			}
			result.append("!");
			formatFixedVolumeValue (totalOunces, tmpStr, formatIndicator, maxAtPints, useDefaultDecimalSeparator);
			result.append(tmpStr);
			break;
		}
		case MetricRuleWeight:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "MetricRuleWeight");
			formatFixedVolumeValue (totalOunces, tmpStr, formatIndicator, maxAtPints, useDefaultDecimalSeparator);
			result.append(tmpStr);
			result.append(" [");

			HobWeight liters = MetSig::fluidOuncesToLiters(totalOunces);
			if (liters < 1000)
			{
				result.append(Long::toString(liters));
				result.append(" ml]");
			}
			else
			{
				HobString mlStr;
				formatDecimalValue(liters, mlStr, 3, useDefaultDecimalSeparator, true);
				result.append(mlStr);
				result.append(" L]");
			}
			break;
		}
		default:
		{
			if (traceOn)
				Debug::trace("Configuration.formatWeight", msgDebug + "default");
			formatDecimalValue(totalOunces, result, getWeightPrecision(), useDefaultDecimalSeparator, true);
			break;
		}
	}
}


/**********************************************************************
 *  formatVolume - Modified from Quantum code
 *--------------------------------------------------------------------*
 *                                                                  
 *  Description:                                                      
 *  Converts a weight in fluid ounces into a Avoirdupois fluid weight 
 *  in gallons, quarts or pints and ounces.    
 *  It prints GAL if fluid ounces >= 128.   
 *  It prints QT if fluid ounces >= 32 < 128.
 *  It prints PT if fluid ounces > 16 < 32.
 *                                                                  
 **********************************************************************/

void Configuration::formatFixedVolumeValue(const HobWeight totalOunces, HobString &result, const short formatIndicator, bool maxAtPints, const bool useDefaultDecimalSeparator)
{
	HobWeight tempFlOz = 0;
	HobWeight ounces = 0;

	if (MetSig::isScaleMetric())
	{
		//	Format for kg instead of fluid.
		formatDecimalValue(totalOunces, result, getWeightPrecision(), useDefaultDecimalSeparator, true);
		result.append(" kg");
	}
	else
	{
		if(isFixedWeightInTenths())
			tempFlOz = totalOunces;
		else
			tempFlOz = totalOunces * 10;

		if (!maxAtPints && tempFlOz >= 1280)
		{
			/* Convert to gallons */
			if (formatIndicator != MetricRuleWeight)
				result.append(Long::toString(tempFlOz / 1280));
			else
			{
				HobString tempStr;
				long lTempGal = MetSig::fluidOuncesToGallons(tempFlOz);
				formatDecimalValue(lTempGal, tempStr, 2, useDefaultDecimalSeparator, true);
				result.append(tempStr);
			}
			result.append(" gal");
			if (formatIndicator != OperateFixedWeightText)
				result.append(" ");
			else
			{
				result.append("lon");
				if ((tempFlOz / 1280) > 1)
					result.append("s");
				result.append("\n");
			}
			ounces = tempFlOz % 1280;
		}
		else if (!maxAtPints && tempFlOz >= 320)
		{
			/* Convert to quarts */
			if (formatIndicator != MetricRuleWeight)
				result.append(Long::toString(tempFlOz / 320));
			else
			{
				HobString tempStr;
				long lTempQts = MetSig::fluidOuncesToQuarts(tempFlOz);
				formatDecimalValue(lTempQts, tempStr, 2, useDefaultDecimalSeparator, true);
				result.append(tempStr);
			}
			result.append(" q");
			if (formatIndicator != OperateFixedWeightText)
				result.append("t ");
			else
			{
				result.append("uart");
				if ((tempFlOz / 320) > 1)
					result.append("s");
				result.append("\n");
			}
			ounces = tempFlOz % 320;
		}
		else if (tempFlOz >= 160)
		{
			/* Convert to pints */
			if (formatIndicator != MetricRuleWeight)
				result.append(Long::toString(tempFlOz / 160));
			else
			{
				HobString tempStr;
				long lTempPints = MetSig::fluidOuncesToPints(tempFlOz);
				formatDecimalValue(lTempPints, tempStr, 2, useDefaultDecimalSeparator, true);
				result.append(tempStr);
			}
			result.append(" p");
			if (formatIndicator != OperateFixedWeightText)
				result.append("t ");
			else
			{
				result.append("int");
				if ((tempFlOz / 160) > 1)
					result.append("s");
				result.append("\n");
			}
			ounces = tempFlOz % 160;
		}
		else
		{
			/* No conversion needed */
			ounces = tempFlOz;
		}
		if ((formatIndicator != MetricRuleWeight) || (tempFlOz < 160))
		{
			if (isFixedWeightInTenths())
			{
				HobString tempStr;
				formatDecimalValue(ounces, tempStr, 1, useDefaultDecimalSeparator, true);
				result.append(tempStr);
			}
			else
			{
				ounces /= 10;
				result.append(Long::toString(ounces));
			}
		}
		if ((formatIndicator != MetricRuleWeight) || (tempFlOz < 160))
			result.append(" fl oz");
	}
}


void Configuration::formatFixedWeightValue (const HobWeight weight, HobString &result, const bool useDefaultDecimalSeparator)
{
	if (MetSig::isScaleMetric())
	{
		formatDecimalValue(weight, result, getWeightPrecision(), useDefaultDecimalSeparator, true);
		result.append(" kg");
	}
	else
	{
		HobString tempStr;

		if ( toIntCfg(fieldOffsetCfgPFFixedWeightFormat) == FIXED_WEIGHT_FORMAT_DECIMAL )
		{
			HobWeight lTempWeight = MetSig::fixedWeightOuncesToDecimalWeight(weight);
			formatDecimalValue(lTempWeight, result, getEffectiveWeightPrecision(), useDefaultDecimalSeparator, true);
			result.append(" lb");
		}
		else
		{
			HobWeight lbsMultiple = 16 * (isFixedWeightInTenths() ? 10 : 1);
			if ( weight >= lbsMultiple )
			{
				result.append(Long::toString(weight / lbsMultiple));
				result.append(" lb ");
				if (isFixedWeightInTenths())
				{
					formatDecimalValue((weight % lbsMultiple), tempStr, 1, useDefaultDecimalSeparator, true);
					result.append(tempStr);
				}
				else
				{
					result.append(Long::toString(weight % lbsMultiple));
				}
			}
			else
			{
				if (isFixedWeightInTenths())
				{
					formatDecimalValue((weight), tempStr, 1, useDefaultDecimalSeparator, true);
					result.append(tempStr);
				}
				else
				{
					result.append(Long::toString(weight));
				}
			}
			result.append(" oz");
		}
	}
}

//METSIG??  NO, used for formatting a weight for display, not for calculating a weight
//*****************************************************************************
//	The number of digits to the right of the decimal point for weight values.
//	The value returned depends on the setting of the scaleWeighMode.
//*****************************************************************************
short Configuration::getEffectiveWeightPrecision(short formatIndicator) const
{
	short offset = (formatIndicator == PrecisionPlusOne) ? 1 : 0;
	Debug::trace("Configuration.formatWeight", "getEffectiveWeightPrecision(" + Integer::toString(formatIndicator) + "):  offset = " + Integer::toString(offset));

	// This returns the number of decimal places that we format a weight to:
	// Metric? -> 3 decimal places
	// Avoir? -> if single-range mode then 2, dual-range mode = 3

	if ( MetSig::isScaleAvoir() && MetSig::isWeigherConfiguredForSingleRange() )
		return 2 + offset;
	return 3 + offset;
}

//METSIG??  NO, this is not a property of a weigher, it's how many decimal places we use and store for weights in the application
//*****************************************************************************
//	The number of digits to the right of the decimal point for weight values.
//	The value returned depends on the setting of the scaleWeighMode.
//*****************************************************************************
short Configuration::getWeightPrecision(short formatIndicator) const
{
	short offset = (formatIndicator == PrecisionPlusOne) ? 1 : 0;
	Debug::trace("Configuration.formatWeight", "getWeightPrecision(" + Integer::toString(formatIndicator) + "):  offset = " + Integer::toString(offset));

	// As of 5/18/2016, all weighers are 3-implied decimal places, even truly single range weighers (which are only used in wrapping systems (Access)!

	// No longer need to check if is metric or avoir weighing.  Both now use 3 decimal places.
	Debug::trace("Configuration.formatWeight", "getWeightPrecision():  all weighers are 3-implied decimal places, even truly single range weighers:  return 3 + offset");
		
	return 3 + offset;
}

short Configuration::getKeyboardNetWeightPrecision() const
{
	return ( MetSig::isScaleMetric() ) ? 3 : (short) Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIKeyboardNetWeightPrecision);
}

/******************************************************************************
 *
 * End of Formatting Routines
 *
 *****************************************************************************/

 // SOF-1593 - user needs to opt-in for saving continuous label stock.
// Added additional check for the config param that gets set when user opts-in to save continuous stock
bool Configuration::isConserveContinuousStockByNotSizingEnabled()
{
	bool result = false;
	if ( !isPrepackScale() && toBoolCfgI(fieldOffsetCfgIConserveContinuousStockByNotSizing) && toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock) )
		result = true;
	Debug::trace("PrinterManager.PrinterSize", string("isConserveContinuousStockByNotSizingEnabled():  returning ") + Bool::toString(result));
	return result;
}

//SOF-3875
bool Configuration::isConserveLinerlessStock()
{
	bool result = false;
	if ( !isPrepackScale() && toBoolCfgI(fieldOffsetCfgIsConserveLinerLess) && toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock) )
		result = true;
	Debug::trace("PrinterManager.PrinterSize", string("fieldOffsetCfgIsConserveLinerLess():  returning ") + Bool::toString(result));
	return result;
}

bool Configuration::isSelfServiceScale() const
{
	Session *pcs = SessionManager::getInstance()->getPrimaryConsoleSession();
	
	if ( pcs && pcs->getCurrentMode() == OpModeSelfService )
		return true;
	else
		return cfgInternal.isSelfServiceScale();
}

bool Configuration::isPrepackScale() const
{
	if ( isSelfServiceScale() )
		return false;
	else
		return cfgInternal.isPrepackScale();
}



void Configuration::getIPAndGatewayAddresses(std::string &ipAddressString, std::string &gatewayAddressString)
{
	// This code was in VersionInfo.cpp - serialize() to send the IP address to the Help-About screen.
	// EmailManager also needs the IP address for the subject line in email alerts, so I moved this here so the code can be reused.

	// The caller must send in two strings in which the addresses will be filled in by this method.

	ipAddressString = "";
	gatewayAddressString = "";

	// need to link in Iphlpapi.lib

	PIP_ADAPTER_INFO AdapterAddresses = NULL;
	ULONG OutBufferLength = 0;
	ULONG RetVal = 0, i;

#if defined ( __linux__ )
	AdapterAddresses = new IP_ADAPTER_INFO;					
#endif

	for (i = 0; i < 4; i++)
	{
		RetVal = PAL_absclass::PAL_getAdaptersInfo(AdapterAddresses, &OutBufferLength);
		if  (RetVal != ERROR_BUFFER_OVERFLOW)
			break ;

		if (AdapterAddresses != NULL)
			free(AdapterAddresses);

		AdapterAddresses = (PIP_ADAPTER_INFO) malloc(OutBufferLength);
		if  (AdapterAddresses == NULL)
		{
			RetVal = PAL_absclass::PAL_getLastError();
			break ;
		}
	}

	if (RetVal == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO AdapterList = AdapterAddresses;
		if  (AdapterList)
		{
			//printf( "\tFriendly name: %s\n" , AdapterList->AdapterName);
			//printf( "\tDescription: %s\n" , AdapterList->Description);

			ipAddressString = AdapterList->IpAddressList.IpAddress.String;
			gatewayAddressString = AdapterList->GatewayList.IpAddress.String;
		}
	}

	if (AdapterAddresses)
	{
		delete AdapterAddresses;
		AdapterAddresses = 0;
	}
}




bool Configuration::isDualLanguageEnabled() const
{
	return !( (getSecondaryCountryCode().empty() || getSecondaryCountryCode() == getPrimaryCountryCode()) &&
			(getSecondaryLanguageCode().empty() || getSecondaryLanguageCode() == getPrimaryLanguageCode()) );
}


short Configuration::getHorizLabelSkew(PrinterType prType) const
{
	int rv = toIntCfg(fieldOffsetCfgHorizLabelSkew1);

	if (prType == SecondPrinter)
		rv = toIntCfg(fieldOffsetCfgHorizLabelSkew2);
	else if (prType == ThirdPrinter)
		rv = toIntCfg(fieldOffsetCfgHorizLabelSkew3);

	return (short)rv;
}


void Configuration::setHorizLabelSkew(PrinterType prType, short labelSkew)
{
	if (prType == FirstPrinter)
		setHorizLabelSkew1(labelSkew);
	else if (prType == SecondPrinter)
		setHorizLabelSkew2(labelSkew);
	else if (prType == ThirdPrinter)
		setHorizLabelSkew3(labelSkew);
}


int Configuration::determineHourForQuery() const
{
	/* See CfgSelfService.h for a description/definition
	 * of the different values this method uses to determine the hour.
	 * Refer to the definition of 'productStatsHoursMethod'.
	 */
	switch (getProductStatsHoursMethod())
	{
		case 0:
		{
			return 0;
			break;
		}

		case 1:
		{
			HobDateTime hdt;
			return hdt.getHour24();
			break;
		}

		case 2:
		{
			HobDateTime hdt;
			return hdt.getDayOfWeek();
			break;
		}

		case 3:
		{
			HobDateTime hdt;
			return (int)((hdt.getDayOfWeek() * 24) + hdt.getHour24());
			break;
		}

		case 4:
		{
			HobDateTime hdt;
			return (hdt.getHour24() < 12) ? 0 : 1;
			break;
		}

		default:
		{
			return 0;
			break;
		}
	}
	return 0; /* just in case you get here */
}


string Configuration::getLicensedPackages( bool recheck )
{
	Util::CLockCriticalSection guard( cfgLicensingCS_ );

	if (( licensedPackagesInit_ ) && ( !recheck ))
		Debug::trace("Configuration.license","Already have licenses loaded");
	else if (!toBoolCfgI(CfgInternal::PYTHONENABLED))
		Debug::trace("Configuration.license","Cannot load licenses, python not enabled");
	else
	{
		string licensedPackageStringsBackdoor = toStringCfgI(CfgInternal::LICENSEDPACKAGESTRINGS);

		string licensedPackageStrings = toStringCfgI(CfgInternal::LICENSEPACKAGESTRINGS);
		HobTokenizerCSV tok(&licensedPackageStrings, BrowserFormatSeparator);
		HobString licensedPackageString;

		if (licensedPackageStringsBackdoor == "ALL")
			licensedPackages_ = licensedPackageStrings;
		else if ( !licensedPackageStringsBackdoor.empty() )
			licensedPackages_ = licensedPackageStringsBackdoor;
		else
		{
			while (tok.next(&licensedPackageString) != -1 && licensedPackageString.size())
			{
				const string pythonCommand(PythonScripter::buildPythonCall("isProductLicensed.py", -1, licensedPackageString));
				string licenseCallOutput;
				int result = PAL_absclass::PAL_executeWithResults(pythonCommand, licenseCallOutput);
				Debug::trace("Configuration.license", "called python script to see if " + licensedPackageString + " is licensed - result: " + inttostring(result) + " data: *" + licenseCallOutput + "*");

				if (result <= 0)
					Debug::trace("Configuration.license", licensedPackageString + " is NOT licensed, licensedPackages: " + licensedPackages_);
				else
				{
					std::size_t found = licensedPackages_.find(licensedPackageString + ",");
					if(found == std::string::npos)
					{
						if (!licensedPackages_.empty())
							licensedPackages_.append(",");

						licensedPackages_.append(licensedPackageString);
					}

					Debug::trace("Configuration.license", licensedPackageString + " is licensed, licensedPackages: " + licensedPackages_);
				}
			}
		}

		licensedPackagesInit_ = true;

		if ( isPrepackScale() )
		{
			std::string catpattern = "SCALE_CAT3_AUDITTRAIL";
			size_t len = catpattern.length() + 1;		// to pick up the comma
			std::string::size_type pos = licensedPackages_.find(catpattern);

			//while (pos != std::string::npos)		// use this if we have more than one license string and test pos again in loop
			if (pos != std::string::npos)
			{
				// SOF - 2823 Cat 3 audit trail can be licensed to a Prepack scale - so dont allow it and remove it if found
				Debug::trace("Configuration.license", "Remove SCALE_CAT3_AUDITTRAIL to allowable licenses if we are a Prepack scale");
				licensedPackages_.erase(pos, len);
			}
		}
	}
		
	return licensedPackages_;
}

bool Configuration::isPackageLicensed(const string packageName)
{
	const string licensedPackages( getLicensedPackages() );
	Debug::trace("Configuration.license", "Checking license " + packageName + " in " + licensedPackages);

	bool result( ( licensedPackages.find(packageName) == string::npos ) ? false : true );

	// Code checker required specific hardware
	if (( result ) && ( "SCALE_CODE_CHECKER" == packageName ))
	{
		WeigherModel model = getWeigherModel();

		if (model != WmKPCWeigher)
		{
			Debug::trace("Configuration.license", "disabling code checker due to unsupported hardware");
			result = false;
		}
	}

	return result;
}

bool Configuration::isInternationalRoundingLicensed()
{
	bool licensed = isPackageLicensed("SCALE_INTERNATIONAL_ROUNDING");
	Debug::trace("Configuration.license", string("ROUNDING is") + ((licensed) ? "" : " not") + " licensed");
	return licensed;
}

bool Configuration::isProductionPlanningLicensed()
{
	bool licensed = isPackageLicensed("SCALE_FPP");
	Debug::trace("Configuration.license", string("FPP is") + ((licensed) ? "" : " not") + " licensed");
	return licensed;
}

bool Configuration::isTransactionalGCDLicensed()
{
	bool licensed = isPackageLicensed("SCALE_TRANS_GCD");
	Debug::trace("Configuration.license", string("Trans GCD is") + ((licensed) ? "" : " not") + " licensed");
	return licensed;
}

bool Configuration::isOnlineOrderingLicensesd()
{
	bool licensed = isPackageLicensed("SCALE_ONLINE_ORDERING");
	Debug::trace("Configuration.license", string("Online Ordering is") + ((licensed) ? "" : " not licensed") + " licensed");
	return licensed;
}

bool Configuration::isCAT3AuditTrailLicensed()
{
	bool licensed = isPackageLicensed("SCALE_CAT3_AUDITTRAIL");

	// SOF-2823 cat3, even if it is licensed, cannot be legal on a prepack scale. Catch where loaded too
	if (licensed && !isPrepackScale())
		licensed = true;
	else
		licensed = false;

	Debug::trace("Configuration.license", string("CAT3 AuditTrail is") + ((licensed) ? "" : " not") + " licensed");
	return licensed;
}

bool Configuration::isRemotePrintingLicensed()
{
	bool licensed = isPackageLicensed("SCALE_REMOTE_PRINTING");
	Debug::trace("Configuration.license", string("Remote Printing is") + ((licensed) ? "" : " not") + " licensed");
	return licensed;
}

bool Configuration::isDigimarcLicensed()
{
	const bool licensed( isPackageLicensed("SCALE_DIGIMARC") );
	Debug::trace("Configuration.license", string("Digimarc is") + ((licensed) ? "" : " not") + " licensed");
	return licensed;
}

bool Configuration::isDotWearLicensed()
{
	return isPackageLicensed("SCALE_CODE_CHECKER");
}

bool Configuration::isDotWearEnabled()
{
	const bool isEnabled( isCodeCheckerEnabled() );
	//Debug::trace("Configuration.license", string("Code Checker is enabled is ") + ( (isEnabled) ? "TRUE" : " FALSE"));
	return isEnabled;
}

bool Configuration::isThreeDigitTare()
{
	return ( getTareEntryPrecision() == 2 ) ? false : true;
}

/******************************************************************************
 *
 * End of "Other" Routines
 *
 *****************************************************************************/



//Protected
Configuration::Configuration()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	traceCfg_ = false;
	checkTracingCfgI();
	checkTracingCfg();
	init();
}

//Protected
Configuration::~Configuration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	int end = (int)cfgItems_.size();
	for (int i = 0; i < end; i++)
	{
		if (cfgItems_[i])
			delete cfgItems_[i];
	}
}

//private
void Configuration::init()
{
	Util::CLockCriticalSection lock( cfgLoadCS_ );

	tempValue_ = "";

	licensedPackagesInit_ = false;

	if ((int)cfgItems_.size() != endCfgFieldOffsets)
	{
		cfgItems_.resize(endCfgFieldOffsets);
		if (traceCfg_)
			Debug::trace("Configuration", "resized cfgItems vector, new size is " + inttostring(cfgItems_.size()));
	}

	/*****************************************************************************
	 *  CfgScale - Configuration parameters applicable to the entire scale.
	 ****************************************************************************/
 
	cfgItems_[fieldOffsetCfgStoreId]                   = new CfgItem("storeId",                   fieldOffsetCfgStoreId,                   CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgStoreName]                 = new CfgItem("storeName",                 fieldOffsetCfgStoreName,                 CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgStoreGraphic]              = new CfgItem("storeGraphicName",          fieldOffsetCfgStoreGraphic,              CfgItem::STRING,  "");
// No longer used as of 9-19-17- claim back later - 
//	cfgItems_[fieldOffsetCfgDeptName]                  = new CfgItem("deptName",                  fieldOffsetCfgDeptName,                  CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgPrimaryCountryCode]        = new CfgItem("primaryCountryCode",        fieldOffsetCfgPrimaryCountryCode,        CfgItem::STRING,  "US");
	cfgItems_[fieldOffsetCfgPrimaryLanguageCode]       = new CfgItem("primaryLanguageCode",       fieldOffsetCfgPrimaryLanguageCode,       CfgItem::STRING,  "en");
	cfgItems_[fieldOffsetCfgSecondaryCountryCode]      = new CfgItem("secondaryCountryCode",      fieldOffsetCfgSecondaryCountryCode,      CfgItem::STRING,  "MX");
	cfgItems_[fieldOffsetCfgSecondaryLanguageCode]     = new CfgItem("secondaryLanguageCode",     fieldOffsetCfgSecondaryLanguageCode,     CfgItem::STRING,  "es");
	cfgItems_[fieldOffsetCfgLengthUnits]               = new CfgItem("lengthUnits",               fieldOffsetCfgLengthUnits,               CfgItem::INT,     "0");  // defined as short  // 0 = U.S. (inches), 1 = metric (centimeters)
	cfgItems_[fieldOffsetCfgTemperatureUnits]          = new CfgItem("temperatureUnits",          fieldOffsetCfgTemperatureUnits,          CfgItem::INT,     "0");  // defined as short  // 0 = Fahrenheit, 1 = Celsius
	//cfgItems_[]                                      = new CfgItem("currencyUnits",             ,                                        CfgItem::STRING,  "Dollars");  // defined as HobString  // Dollars, Pesos, Reels, ...
	//cfgItems_[]                                      = new CfgItem("monetarySymbol",            ,                                        CfgItem::STRING,  "$");  // defined as HobString  // automatically set based on the setting of currencyUnits
	cfgItems_[fieldOffsetCfgDateFormat]                = new CfgItem("dateDisplayFormat",         fieldOffsetCfgDateFormat,                CfgItem::INT,     Integer::toString(FormatDate_mmmDDYY));  // defined as short  // See the static const shorts above TIMEFORMAT_*** for the values of the formats defined.
	cfgItems_[fieldOffsetCfgTimeFormat]                = new CfgItem("timeDisplayFormat",         fieldOffsetCfgTimeFormat,                CfgItem::INT,     Integer::toString(HourFormat_AMPM));  // defined as short  // See the static const shorts above TIMEFORMAT_*** for the values of the formats defined.

	cfgItems_[fieldOffsetCfgUnitPriceSecurityLabel]    = new CfgItem("securityLblUnitPrice",      fieldOffsetCfgUnitPriceSecurityLabel,    CfgItem::INT,     "0");  // long  // defined as HobPrice
	cfgItems_[fieldOffsetCfgTotalPriceSecurityLabel]   = new CfgItem("securityLblTtlPrice",       fieldOffsetCfgTotalPriceSecurityLabel,   CfgItem::INT,     "0");  // long  // defined as HobPrice
	cfgItems_[fieldOffsetCfgFractionalPricingEnabled]  = new CfgItem("fractionalPricingEnabled",  fieldOffsetCfgFractionalPricingEnabled,  CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgPoundsForPricingEnabled]   = new CfgItem("lbsForPricingEnabled",      fieldOffsetCfgPoundsForPricingEnabled,   CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgBycountCalcMethod]         = new CfgItem("byCountCalcMethod",         fieldOffsetCfgBycountCalcMethod,         CfgItem::INT,     "3");  // defined as short  // See USM pg 2-16:  Current values set via "Set up normal scale operation" screen are: 1, 2, 3  (but what are their meanings)
	cfgItems_[fieldOffsetCfgClearPluAfterPrinting]     = new CfgItem("clearPluAfterPrinting",     fieldOffsetCfgClearPluAfterPrinting,     CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgBeepOnStableWeight]        = new CfgItem("beepOnStableWeight",        fieldOffsetCfgBeepOnStableWeight,        CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgBeeperVolumeLevel]         = new CfgItem("beeperVolumeLevel",         fieldOffsetCfgBeeperVolumeLevel,         CfgItem::INT,     "5");  // defined as short
	
	cfgItems_[fieldOffsetCfgFixedWeightUnits]          = new CfgItem("fixedWeightUnits",          fieldOffsetCfgFixedWeightUnits,          CfgItem::INT,     Integer::toString(FIXED_WEIGHT_IN_WHOLE_UNITS));  // defined as short  // The fixedWeightUnits mode can be set to one of:  0 = one tenth of an ounce, 1 = whole ounces,
	cfgItems_[fieldOffsetCfgWeigherAttached]           = new CfgItem("weigherAttached",           fieldOffsetCfgWeigherAttached,           CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgTareSetback]               = new CfgItem("tareSetback",               fieldOffsetCfgTareSetback,               CfgItem::INT,     "0");     // HobWeight
	cfgItems_[fieldOffsetCfgMaxTare]                   = new CfgItem("maxTare",                   fieldOffsetCfgMaxTare,                   CfgItem::INT,     "9995");  // HobWeight
	cfgItems_[fieldOffsetCfgPlatterTareEnabled]        = new CfgItem("platterTareEnabled",        fieldOffsetCfgPlatterTareEnabled,        CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgChangeZeroTareOnlyOnce]    = new CfgItem("changeZeroTareOnlyOnce",    fieldOffsetCfgChangeZeroTareOnlyOnce,    CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgChangeForcedTareOnlyOnce]  = new CfgItem("changeForcedTareOnlyOnce",  fieldOffsetCfgChangeForcedTareOnlyOnce,  CfgItem::BOOL,    "0");

	cfgItems_[fieldOffsetCfgMinWeightToPrintLabel]     = new CfgItem("minWeightToPrint",          fieldOffsetCfgMinWeightToPrintLabel,     CfgItem::INT,     "20");  // defined as HobWeight  // default to 0.020 pounds
	cfgItems_[fieldOffsetCfgMinWeightToPrintLabel]->setPersistent(false);

	cfgItems_[fieldOffsetCfgWeighMode] = new CfgItem("scaleWeighMode", fieldOffsetCfgWeighMode, CfgItem::INT, Integer::toString(WeighMode_Avoir));  // defined as the enum WeighMode (Weigher.h)  // 0=Metric, 1=Avoir
	// This has been commented out so that HTe-ScaleManagement can have access to this variable.
	// NOTE:  This is NOT the official setting for the scaleMode.  It is kept in the weigher eep.
	// This configuration is updated with the value from the weigher eep any time that the weigher
	// configuration is read from the weigher (APC/FPC), via the WeigherInquire message.
	//cfgItems_[fieldOffsetCfgWeighMode]->setPersistent(false);

	cfgItems_[fieldOffsetCfgWeigherModel] = new CfgItem("weigherModel", fieldOffsetCfgWeigherModel, CfgItem::INT, Integer::toString(WmNone));  // defined as the enum WeigherModel
	cfgItems_[fieldOffsetCfgWeigherModel]->setPersistent(false);

	cfgItems_[fieldOffsetCfgNetWeightDualRangeLimitCheck] = new CfgItem("netWeightDualRangeLimitCheck", fieldOffsetCfgNetWeightDualRangeLimitCheck, CfgItem::BOOL, "1");
	cfgItems_[fieldOffsetCfgNetWeightDualRangeLimitCheck]->setPersistent(false);

	cfgItems_[fieldOffsetCfgValueMaxEnabled] = new CfgItem("valueMaxEnabled", fieldOffsetCfgValueMaxEnabled, CfgItem::INT, "0");

	/**
	* The number of implied digits to the right of the decimal point for weight values.
	* This is used when reading in and writing out weight values.
	* Starting with the LX scale, we will keep 3 implied decimal digits internally, but
	* we may have to only write out (comm, backup/restore) 2 digits for compatibility with other software.
	* i.e. tare, total weight, ...
	*/
	cfgItems_[fieldOffsetCfgExternalWeightPrecision]     = new CfgItem("externalWeightPrecision",     fieldOffsetCfgExternalWeightPrecision,     CfgItem::INT,  "2");  // defined as short

	cfgItems_[fieldOffsetCfgTareEntryPrecision]          = new CfgItem("tareEntryPrecision",          fieldOffsetCfgTareEntryPrecision,          CfgItem::INT,  "3");  // defined as short
	cfgItems_[fieldOffsetCfgTareEntryPrecision]->setPersistent(false);   // as of April 2017, no longer configurable, per W&M

	cfgItems_[fieldOffsetCfgUserRezeroWeightLimit]       = new CfgItem("userRezeroWeightLimit",       fieldOffsetCfgUserRezeroWeightLimit,       CfgItem::INT,  "10");  // 0.010 lbs for Avoir mode (= 2d of 30 lbs), by default  // Which value is sent to and received from the browser depends on the weigher mode.
	cfgItems_[fieldOffsetCfgWeigherWakeupSensorAttached] = new CfgItem("weigherWakeupSensorAttached", fieldOffsetCfgWeigherWakeupSensorAttached, CfgItem::BOOL, "1");  //	For a CLA/Weigher conveyor setup on the EPCP.

	cfgItems_[fieldOffsetCfgOperateLoginRequired]        = new CfgItem("loginRequired",               fieldOffsetCfgOperateLoginRequired,        CfgItem::BOOL, "0");

	/* This property sets the amount of time, in minutes, afterwhich the user
	 * will be automatically logged out if they haven't had activity
	 * (pressed a key, looked up an item, not within a run, etc.)
	 *
	 * Note:  Just placing an item on the weigher is not considered activity.
	 * The scale will still display weights when a user is not logged in.
	 *
	 * This would most likely be implemented by a Session.  If the Session
	 * hasn't received any message from the associated Console/Browser,
	 * the logged-in user gets logged out after the set number of minutes.
	 */
	cfgItems_[fieldOffsetCfgInactivityAutoLogoutTime]        = new CfgItem("autoLogoutTime",                   fieldOffsetCfgInactivityAutoLogoutTime,        CfgItem::INT,     "0");  // in minutes

	cfgItems_[fieldOffsetCfgAutoLookupProduct]               = new CfgItem("autoLookupProduct",                fieldOffsetCfgAutoLookupProduct,               CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgAutoLookupNumOfDigits]           = new CfgItem("autoLookupNumDigits",              fieldOffsetCfgAutoLookupNumOfDigits,           CfgItem::INT,     "4");  // defined as short
	cfgItems_[fieldOffsetCfgAutoLookupOverrideDigit]         = new CfgItem("autoLookupOverrideDigit",          fieldOffsetCfgAutoLookupOverrideDigit,         CfgItem::STRING,  "0");
	cfgItems_[fieldOffsetCfgAutoDisplayProductNotes]         = new CfgItem("autoDisplayProductNotes",          fieldOffsetCfgAutoDisplayProductNotes,         CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgAutoDisplaySuggestiveSelling]    = new CfgItem("autoDisplaySuggestiveSelling",     fieldOffsetCfgAutoDisplaySuggestiveSelling,    CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgAutoDisplayCookingNotes]         = new CfgItem("autoDisplayCookingNotes",          fieldOffsetCfgAutoDisplayCookingNotes,         CfgItem::BOOL,    "0");

	cfgItems_[fieldOffsetCfgHorizLabelSkew1]                 = new CfgItem("horizLabelSkew1",                  fieldOffsetCfgHorizLabelSkew1,                 CfgItem::INT,     "5");  // defined as short  // Primary printe  //	Horizontal position (on display), Label skew (internal)
	cfgItems_[fieldOffsetCfgHorizLabelSkew2]                 = new CfgItem("horizLabelSkew2",                  fieldOffsetCfgHorizLabelSkew2,                 CfgItem::INT,     "5");  // defined as short  // Second printer
	cfgItems_[fieldOffsetCfgHorizLabelSkew3]                 = new CfgItem("horizLabelSkew3",                  fieldOffsetCfgHorizLabelSkew3,                 CfgItem::INT,     "5");  // defined as short  // Third printer

	cfgItems_[fieldOffsetCfgCollectingTransactions]          = new CfgItem("collectTransactions",              fieldOffsetCfgCollectingTransactions,          CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgCollectingTransactionsDays]      = new CfgItem("collectTransactionsDays",          fieldOffsetCfgCollectingTransactionsDays,      CfgItem::INT,     "8");  // 0 = indefinately
	cfgItems_[fieldOffsetCfgTrackingOperatorChanges]         = new CfgItem("trackingOperatorChanges",          fieldOffsetCfgTrackingOperatorChanges,         CfgItem::BOOL,    "0");

	cfgItems_[fieldOffsetCfgOpAccessToBrowserMode]           = new CfgItem("opAccessToBrowserMode",            fieldOffsetCfgOpAccessToBrowserMode,           CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgOpAccessToDesktopMode]           = new CfgItem("opAccessToDesktopMode",            fieldOffsetCfgOpAccessToDesktopMode,           CfgItem::BOOL,    "0");

	cfgItems_[fieldOffsetCfgAutoLoadOldFirmwareVersions]     = new CfgItem("autoLoadOldFirmwareVersions",      fieldOffsetCfgAutoLoadOldFirmwareVersions,     CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgAutoSearchNewFirmwareVersions]   = new CfgItem("autoSearchNewFirmwareVersions",    fieldOffsetCfgAutoSearchNewFirmwareVersions,   CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgNewFirmwareVersionSearchTime]    = new CfgItem("newFirmwareVersionSearchTime",     fieldOffsetCfgNewFirmwareVersionSearchTime,    CfgItem::STRING,  "05:00 AM");  //	12-hour format: "HH:MM AM"

	cfgItems_[fieldOffsetCfgForceUniqueProdClassNums]        = new CfgItem("forceUniqueProdClassNums",         fieldOffsetCfgForceUniqueProdClassNums,        CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgEnablePowerSaverMode]            = new CfgItem("enablePowerSaverMode",             fieldOffsetCfgEnablePowerSaverMode,            CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgPowerSaverModeTimeout]           = new CfgItem("powerSaverModeTimeout",            fieldOffsetCfgPowerSaverModeTimeout,           CfgItem::INT,     "10");  // defined as short  //	0 - 120 Minutes
	cfgItems_[fieldOffsetCfgDefaultBrowserHomePage]          = new CfgItem("defaultBrowserHomePage",           fieldOffsetCfgDefaultBrowserHomePage,          CfgItem::STRING,  "defaultBrowserHomepage.html");
	cfgItems_[fieldOffsetCfgDefaultRunScreenHomePage]        = new CfgItem("defaultRunScreenHomePage",         fieldOffsetCfgDefaultRunScreenHomePage,        CfgItem::STRING,  "defaultBrowserHomepage.html");
	cfgItems_[fieldOffsetCfgDefaultProductRecordType]        = new CfgItem("defaultProductRecordType",         fieldOffsetCfgDefaultProductRecordType,        CfgItem::INT,     Integer::toString(Product::PRODUCTTYPE_RANDOMWEIGHT));  // Product::ProductType
	
	cfgItems_[fieldOffsetCfgDefWrFilmWidth]                  = new CfgItem("defaultFilmWidth",                 fieldOffsetCfgDefWrFilmWidth,                  CfgItem::INT,     Integer::toString(AutoWidthSelect));    // defined as short
	cfgItems_[fieldOffsetCfgDefWrFilmTension]                = new CfgItem("defaultFilmTension",               fieldOffsetCfgDefWrFilmTension,                CfgItem::INT,     Integer::toString(MediumFilmTension));  // defined as short
	cfgItems_[fieldOffsetCfgDefWrFilmTensionFrontToBack]     = new CfgItem("defaultFilmTensionFrontToBack",    fieldOffsetCfgDefWrFilmTensionFrontToBack,     CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgDefWrFilmTensionLeftToRight]     = new CfgItem("defaultFilmTensionLeftToRight",    fieldOffsetCfgDefWrFilmTensionLeftToRight,     CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgDefWrIntakeMode]                 = new CfgItem("defaultIntakeMode",                fieldOffsetCfgDefWrIntakeMode,                 CfgItem::INT,     Integer::toString(WrapperIntakeMode::NormalIntake));  // defined as short
	cfgItems_[fieldOffsetCfgDefWrLabelPlacement]             = new CfgItem("defaultLabelPlacement",            fieldOffsetCfgDefWrLabelPlacement,             CfgItem::INT,     Integer::toString(LocRight));  // defined as short
	cfgItems_[fieldOffsetCfgDefWrSecLabelPlacement]			 = new CfgItem("secondary defaultLabelPlacement",  fieldOffsetCfgDefWrSecLabelPlacement,          CfgItem::INT,     Integer::toString(LocLeftOfPrimary));  // defined as short
	cfgItems_[fieldOffsetCfgDefWrApplierRotation]            = new CfgItem("defaultApplierRotation",           fieldOffsetCfgDefWrApplierRotation,            CfgItem::INT,     "0");  // defined as short
	cfgItems_[fieldOffsetCfgDefWrSecApplierRotation]         = new CfgItem("secondary defaultApplierRotation", fieldOffsetCfgDefWrSecApplierRotation,         CfgItem::INT,     "0");  // defined as short

	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnSun]           = new CfgItem("sealerBeltTimeOnSun",              fieldOffsetCfgWrSealerBeltTimeOnSun,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffSun]          = new CfgItem("sealerBeltTimeOffSun",             fieldOffsetCfgWrSealerBeltTimeOffSun,          CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnMon]           = new CfgItem("sealerBeltTimeOnMon",              fieldOffsetCfgWrSealerBeltTimeOnMon,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffMon]          = new CfgItem("sealerBeltTimeOffMon",             fieldOffsetCfgWrSealerBeltTimeOffMon,          CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnTue]           = new CfgItem("sealerBeltTimeOnTue",              fieldOffsetCfgWrSealerBeltTimeOnTue,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffTue]          = new CfgItem("sealerBeltTimeOffTue",             fieldOffsetCfgWrSealerBeltTimeOffTue,          CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnWed]           = new CfgItem("sealerBeltTimeOnWed",              fieldOffsetCfgWrSealerBeltTimeOnWed,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffWed]          = new CfgItem("sealerBeltTimeOffWed",             fieldOffsetCfgWrSealerBeltTimeOffWed,          CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnThu]           = new CfgItem("sealerBeltTimeOnThu",              fieldOffsetCfgWrSealerBeltTimeOnThu,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffThu]          = new CfgItem("sealerBeltTimeOffThu",             fieldOffsetCfgWrSealerBeltTimeOffThu,          CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnFri]           = new CfgItem("sealerBeltTimeOnFri",              fieldOffsetCfgWrSealerBeltTimeOnFri,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffFri]          = new CfgItem("sealerBeltTimeOffFri",             fieldOffsetCfgWrSealerBeltTimeOffFri,          CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOnSat]           = new CfgItem("sealerBeltTimeOnSat",              fieldOffsetCfgWrSealerBeltTimeOnSat,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgWrSealerBeltTimeOffSat]          = new CfgItem("sealerBeltTimeOffSat",             fieldOffsetCfgWrSealerBeltTimeOffSat,          CfgItem::STRING,  "");

	cfgItems_[fieldOffsetCfgDbBackupFrequency]               = new CfgItem("dbBackupFrequency",                fieldOffsetCfgDbBackupFrequency,               CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgAutoCompressDB]                  = new CfgItem("autoCompressDB",                   fieldOffsetCfgAutoCompressDB,                  CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgCompressDBTime]                  = new CfgItem("compressDBTime",                   fieldOffsetCfgCompressDBTime,                  CfgItem::STRING,  "04:00 AM");

	cfgItems_[fieldOffsetCfgCOOLTrackingNumberEnabled]       = new CfgItem("COOLTrackingNumberEnabled",        fieldOffsetCfgCOOLTrackingNumberEnabled,       CfgItem::BOOL,    "0");   // I'm changing it to read as it is saved, and not the opposite, so that existing databases will be read correctly
	cfgItems_[fieldOffsetCfgDontChangeCurrentCOOL]           = new CfgItem("dontChangeCurrentCOOLViaComm",     fieldOffsetCfgDontChangeCurrentCOOL,           CfgItem::BOOL,    "0");   // Default is false. If true, the tag data does not override the current COOL text.

	cfgItems_[fieldOffsetCfgPrimaryExpirationPageNote]       = new CfgItem("primaryExpirationPageNote",        fieldOffsetCfgPrimaryExpirationPageNote,       CfgItem::STRING,  "Check for expiring product in the case daily.");  // The note on the Expiration Label Page telling the operator what to do with about-to-expire products.
	cfgItems_[fieldOffsetCfgSecondExpirationPageNote]        = new CfgItem("secondExpirationPageNote",         fieldOffsetCfgSecondExpirationPageNote,        CfgItem::STRING,  "Check for expiring product in the case daily.");

	cfgItems_[fieldOffsetCfgCheckNetworkConnectivity]        = new CfgItem("checkingNetworkConnectivity",      fieldOffsetCfgCheckNetworkConnectivity,        CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgOfflineDateTime]                 = new CfgItem("offlineDateTime",                  fieldOffsetCfgOfflineDateTime,                 CfgItem::STRING,  "");

	cfgItems_[fieldOffsetCfgCommunicationHistory]            = new CfgItem("communicationHistory",             fieldOffsetCfgCommunicationHistory,            CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgCommunicationItemReceived]       = new CfgItem("communicationItemReceived",        fieldOffsetCfgCommunicationItemReceived,       CfgItem::STRING,  "");

	//cfgItems_[]                                            = new CfgItem("daysToKeepOperatorChanges",        ,                                              CfgItem::INT,     "0");  // enum commented out in CfgBase.h
	cfgItems_[fieldOffsetCfgPopupKeypadEnabled]              = new CfgItem("popupKeypadEnabled",               fieldOffsetCfgPopupKeypadEnabled,              CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgPopupKeypadStyle]                = new CfgItem("popupKeypadStyle",                 fieldOffsetCfgPopupKeypadStyle,                CfgItem::INT,     "2");
	cfgItems_[fieldOffsetCfgPopupKeypadLocation]             = new CfgItem("popupKeypadLocation",              fieldOffsetCfgPopupKeypadLocation,             CfgItem::INT,     "0");  // 0=right 1=left

	cfgItems_[fieldOffsetCfgOperatorLoginKeyboardStyle]      = new CfgItem("operatorLoginKeyboardStyle",       fieldOffsetCfgOperatorLoginKeyboardStyle,      CfgItem::INT,     "2");  // 1 = full qwerty keyboard, 2 = numeric popup keypad

	cfgItems_[fieldOffsetCfgCapacityBarLocation]             = new CfgItem("capacityBarLocation",              fieldOffsetCfgCapacityBarLocation,             CfgItem::INT,     "0");  // 0=top 1=bottom 

	cfgItems_[fieldOffsetCfgPingPrimary]                     = new CfgItem("pingPrimary",                      fieldOffsetCfgPingPrimary,                     CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgPingGateway]                     = new CfgItem("pingGateway",                      fieldOffsetCfgPingGateway,                     CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgPingTarget]                      = new CfgItem("pingTarget",                       fieldOffsetCfgPingTarget,                      CfgItem::BOOL,    "0");  //	PTR 673
	cfgItems_[fieldOffsetCfgPingInterval]                    = new CfgItem("pingInterval",                     fieldOffsetCfgPingInterval,                    CfgItem::STRING,  "00:30");  // Set to ping every 30 minutes (HH:MM)
	cfgItems_[fieldOffsetCfgPingTargetIPAddress]             = new CfgItem("pingTargetIPAddress",              fieldOffsetCfgPingTargetIPAddress,             CfgItem::STRING,  "0.0.0.0");  //	IP address other than the Gateway to ping for Network Connectivity.

	cfgItems_[fieldOffsetCfgRebootUnprotectedAllowed]        = new CfgItem("rebootUnprotectedAllowed",         fieldOffsetCfgRebootUnprotectedAllowed,        CfgItem::BOOL,    "0");  // Does the 'reboot unprotected' menu selection appear on the Supervisor menu or not.
	cfgItems_[fieldOffsetCfgAutoCreateRFIDOperator]          = new CfgItem("autoCreateRFIDOperator",           fieldOffsetCfgAutoCreateRFIDOperator,          CfgItem::BOOL,    "1");

	cfgItems_[fieldOffsetCfgIgnoreUltimaFormatLabelTypes]    = new CfgItem("ignoreUltimaFormatLabelTypes",     fieldOffsetCfgIgnoreUltimaFormatLabelTypes,    CfgItem::BOOL,    "1");

	cfgItems_[fieldOffsetCfgReportsOffsetTop]                = new CfgItem("reportsOffsetTop",                 fieldOffsetCfgReportsOffsetTop,                CfgItem::INT,     "0");  //	0-9999. Adds offset to top margin for printing reports only.  //	Reports offsets allow reports to print after (top) or before (bottom) a preprinted image on the label (CCD 9/09).
	cfgItems_[fieldOffsetCfgReportsOffsetBottom]             = new CfgItem("reportsOffsetBottom",              fieldOffsetCfgReportsOffsetBottom,             CfgItem::INT,     "0");  //	0-9999. Adds offset to bottom margin for printing reports only.

	cfgItems_[fieldOffsetCfgShowDeleteAllFiles]              = new CfgItem("showDeleteAllFiles",               fieldOffsetCfgShowDeleteAllFiles,              CfgItem::BOOL,    "0");  //	Whether or not to show 'Delete All files' in the Supervisor menu (Albertson's request)
	cfgItems_[fieldOffsetCfgShowPrinterSettings]             = new CfgItem("showPrinterSettings",              fieldOffsetCfgShowPrinterSettings,             CfgItem::BOOL,    "1");  //	Whether or not to show 'Printer Settings' in the Operate menu (Albertson's request)

	cfgItems_[fieldOffsetCfgRemoteAccessPermitted]           = new CfgItem("remoteAccessPermitted",            fieldOffsetCfgRemoteAccessPermitted,           CfgItem::BOOL,    "0");   //	Whether or not to allow a remote session.
	cfgItems_[fieldOffsetCfgRemoteAccessRequiresLogin]       = new CfgItem("remoteAccessRequiresLogin",        fieldOffsetCfgRemoteAccessRequiresLogin,       CfgItem::BOOL,    "0");  //	Carried forward from HLX, but should be deprecated; using "remoteAccessLoginMethod" instead.
	cfgItems_[fieldOffsetCfgRemoteAccessLoginMethod]         = new CfgItem("remoteAccessLoginMethod",          fieldOffsetCfgRemoteAccessLoginMethod,         CfgItem::INT,     "0");  // The method for how a remote user logs in:  0=no login required, 1=Operator ID required, 2=UserName and Password required
	cfgItems_[fieldOffsetCfgRemoteAccessUsername]            = new CfgItem("remoteAccessUsername",             fieldOffsetCfgRemoteAccessUsername,            CfgItem::STRING,  "");   // Username needed to remotely connect to the scale
	cfgItems_[fieldOffsetCfgRemoteAccessPassword]            = new CfgItem("remoteAccessPassword",             fieldOffsetCfgRemoteAccessPassword,            CfgItem::STRING,  "");   // Password needed to remotely connect to the scale

	cfgItems_[fieldOffsetCfgBackdateByDays]                  = new CfgItem("backdateByDays",                   fieldOffsetCfgBackdateByDays,                  CfgItem::BOOL,    "1");  //	Default is true. If false, backdate by date.

	cfgItems_[fieldOffsetCfgSystemFont]                      = new CfgItem("systemFont",                       fieldOffsetCfgSystemFont,                      CfgItem::STRING,  "");   // font name (and point size?)
	cfgItems_[fieldOffsetCfgStartScreenHome]                 = new CfgItem("startScreenHome",                  fieldOffsetCfgStartScreenHome,                 CfgItem::INT,     "1");  //	0=browser (left), 1=plu entry (middle), ... other?
	cfgItems_[fieldOffsetCfgBrowserHasAddressBar]            = new CfgItem("browserHasAddressBar",             fieldOffsetCfgBrowserHasAddressBar,            CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgDefaultSearchMethod]             = new CfgItem("defaultSearchMethod",              fieldOffsetCfgDefaultSearchMethod,             CfgItem::INT,     "0");  //	0=description, 1=plu number
	cfgItems_[fieldOffsetCfgResetStatisticsDays]             = new CfgItem("resetStatisticsDays",              fieldOffsetCfgResetStatisticsDays,             CfgItem::INT,     "10");  //	0 = don't reset, 1+ = number of days statistics are kept
	cfgItems_[fieldOffsetCfgLeftRunscreenContent]            = new CfgItem("leftRunscreenContent",             fieldOffsetCfgLeftRunscreenContent,            CfgItem::INT,     "32");  //	The values for this field are defined in the UI code, look in shared/sessionHandler/widgets/dynamiccontentwidget.h
	cfgItems_[fieldOffsetCfgMiddleRunscreenDynamicContent]   = new CfgItem("runscreenDynamicContent",          fieldOffsetCfgMiddleRunscreenDynamicContent,   CfgItem::INT,     "4");  //	The values for this field are defined in the UI code, look in shared/sessionHandler/widgets/dynamiccontentwidget.h
	cfgItems_[fieldOffsetCfgMiddleRunscreenKeypadContent]    = new CfgItem("runscreenKeypadContent",           fieldOffsetCfgMiddleRunscreenKeypadContent,    CfgItem::INT,     "2");  //	The values for this field are defined in the UI code, look in shared/sessionHandler/widgets/dynamiccontentwidget.h
	cfgItems_[fieldOffsetCfgRightScreenContent]              = new CfgItem("rightScreenContent",               fieldOffsetCfgRightScreenContent,              CfgItem::INT,     "0");  //	The values for this field are defined in the UI code, look in shared/sessionHandler/widgets/dynamiccontentwidget.h

	cfgItems_[fieldOffsetCfgMostRecentPlanogramGraphic]      = new CfgItem("mostRecentPlanogramGraphic",       fieldOffsetCfgMostRecentPlanogramGraphic,      CfgItem::STRING,  "");  // This is not written out to comm, it currently is for internal use only.
	cfgItems_[fieldOffsetCfgMostRecentPlanogramGraphicCRC]   = new CfgItem("mostRecentPlanogramGraphicCRC",    fieldOffsetCfgMostRecentPlanogramGraphicCRC,   CfgItem::INT,     "0");  // This is not written out to comm, it currently is for internal use only.
	cfgItems_[fieldOffsetCfgMostRecentPlanogramAcknowledged] = new CfgItem("mostRecentPlanogramAcknowledged",  fieldOffsetCfgMostRecentPlanogramAcknowledged, CfgItem::BOOL,    "1");  // This is not written out to comm, it currently is for internal use only.

	loadStoreGraphic();
	
	updatedPrimaryLocale_ = false;
	updatedSecondaryLocale_ = false;

	cfgItems_[fieldOffsetCfgDatabaseContainsOperators]      = new CfgItem("dbContainsOpers",                fieldOffsetCfgDatabaseContainsOperators,      CfgItem::BOOL,        "0");
	cfgItems_[fieldOffsetCfgDatabaseContainsOperators]->setPersistent(false);

	cfgItems_[fieldOffsetCfgEnableDisplayNetworkDetails]    = new CfgItem("enableDisplayNetworkDetails",    fieldOffsetCfgEnableDisplayNetworkDetails,    CfgItem::BOOL,        "0");
	cfgItems_[fieldOffsetCfgDisableOperatorFlashkeyEditor]  = new CfgItem("disableOperatorFlashkeyEditor",  fieldOffsetCfgDisableOperatorFlashkeyEditor,  CfgItem::BOOL,        "0");
	cfgItems_[fieldOffsetCfgDisableLeftRunscreen]           = new CfgItem("disableLeftRunscreen",           fieldOffsetCfgDisableLeftRunscreen,           CfgItem::BOOL,        "0");

	// This is last mode the user was in, so we return them to that mode the next time the scale is booted.
	cfgItems_[fieldOffsetCfgMostRecentRunMode]              = new CfgItem("mostRecentRunMode",              fieldOffsetCfgMostRecentRunMode,              CfgItem::INT,         Integer::toString(OpModeManualWeigh));  // defined as the enum WeigherModel

	cfgItems_[fieldOffsetCfgDisplayTransTypeIndicator]      = new CfgItem("displayTransTypeIndicator",      fieldOffsetCfgDisplayTransTypeIndicator,      CfgItem::BOOL,        "0");
	cfgItems_[fieldOffsetCfgNowServingQueueIPAddress]       = new CfgItem("nowServingQueueIPAddress",       fieldOffsetCfgNowServingQueueIPAddress,       CfgItem::STRING,      "0.0.0.0");
	cfgItems_[fieldOffsetCfgNowServingSysType]              = new CfgItem("nowServingSysType",              fieldOffsetCfgNowServingSysType,              CfgItem::INT,         "0");


	/*****************************************************************************
	 *  CfgFrequentShopper
	 ****************************************************************************/

	cfgItems_[fieldOffsetCfgFSRoundingMethod]   = new CfgItem("fsRoundingMethod",   fieldOffsetCfgFSRoundingMethod,   CfgItem::INT,  Integer::toString(Rounding::CONVENTIONAL_ROUNDING));
	cfgItems_[fieldOffsetCfgFSDiscountMethod]   = new CfgItem("fsDiscountMethod",   fieldOffsetCfgFSDiscountMethod,   CfgItem::INT,  Integer::toString(FLEX_DISCOUNT_METHOD_PER_SCALE));
	cfgItems_[fieldOffsetCfgFSDiscountType]     = new CfgItem("fsDiscountType",     fieldOffsetCfgFSDiscountType,     CfgItem::INT,  Integer::toString(Discount_price_by_weight));
	cfgItems_[fieldOffsetCfgFSDiscountFormula]  = new CfgItem("fsDiscountFormula",  fieldOffsetCfgFSDiscountFormula,  CfgItem::INT,  Integer::toString(IBM_FS_FORMULA));

	// Note: fieldOffsetCfgFSPerScale is not saved in the database, it refers to the discount method; so the name and value don't matter, since this CfgItem's value will never be used.
	cfgItems_[fieldOffsetCfgFSPerScale]         = new CfgItem("",                   fieldOffsetCfgFSPerScale,         CfgItem::INT,  "0");
	cfgItems_[fieldOffsetCfgFSPerScale]->setPersistent(false);
	
	cfgItems_[fieldOffsetCfgFSTotalPriceText1]  = new CfgItem("totalPriceText1",    fieldOffsetCfgFSTotalPriceText1,  CfgItem::STRING,  "");	//	Primary language string for configurable 'U-Pay' text.
	cfgItems_[fieldOffsetCfgFSTotalPriceText2]  = new CfgItem("totalPriceText2",    fieldOffsetCfgFSTotalPriceText2,  CfgItem::STRING,  "");	//	Secondary language string for configurable 'U-Pay' text.
	cfgItems_[fieldOffsetCfgFSSavingsText1]     = new CfgItem("savingsText1",       fieldOffsetCfgFSSavingsText1,     CfgItem::STRING,  "");	//	Primary language string for configurable 'U-Save' text.
	cfgItems_[fieldOffsetCfgFSSavingsText2]     = new CfgItem("savingsText2",       fieldOffsetCfgFSSavingsText2,     CfgItem::STRING,  "");	//	Secondary language string for configurable 'U-Save' text.
	cfgItems_[fieldOffsetCfgFSUnitPriceText1]   = new CfgItem("unitPriceText1",     fieldOffsetCfgFSUnitPriceText1,   CfgItem::STRING,  "");	//	Primary language string for configurable 'Club price' text.
	cfgItems_[fieldOffsetCfgFSUnitPriceText2]   = new CfgItem("unitPriceText2",     fieldOffsetCfgFSUnitPriceText2,   CfgItem::STRING,  "");	//	Secondary language string for configurable 'Club price' text.

	cfgItems_[fieldOffsetCfgFSRoundingWeightPrecision]  = new CfgItem("fsRoundingWeightPrecision",  fieldOffsetCfgFSRoundingWeightPrecision,  CfgItem::INT,  "2");	//	How many decimal places in a weight value to use when calculating fs unit price.


	/*****************************************************************************
	 *  CfgOperatorAccess
	 ****************************************************************************/

	cfgItems_[fieldOffsetCfgOAPriceModifiable]         = new CfgItem("priceModifiable",       fieldOffsetCfgOAPriceModifiable,         CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOAByCountModifiable]       = new CfgItem("byCountModifiable",     fieldOffsetCfgOAByCountModifiable,       CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOATareModifiable]          = new CfgItem("tareModifiable",        fieldOffsetCfgOATareModifiable,          CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOAFixedWeightModifiable]   = new CfgItem("fixedWghtModifiable",   fieldOffsetCfgOAFixedWeightModifiable,   CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOAShelfLifeModifiable]     = new CfgItem("shelfLifeModifiable",   fieldOffsetCfgOAShelfLifeModifiable,     CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOAProductLifeModifiable]   = new CfgItem("prodLifeModifiable",    fieldOffsetCfgOAProductLifeModifiable,   CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOALabelTypeModifiable]     = new CfgItem("labelTypeModifiable",   fieldOffsetCfgOALabelTypeModifiable,     CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOADateTimeModifiable]      = new CfgItem("dateTimeModifiable",    fieldOffsetCfgOADateTimeModifiable,      CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgOAPortionsModifiable]      = new CfgItem("portionsModifiable",    fieldOffsetCfgOAPortionsModifiable,      CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgOAPriceAccessLevel]        = new CfgItem("priceAccessLevel",      fieldOffsetCfgOAPriceAccessLevel,        CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOAByCountAccessLevel]      = new CfgItem("byCountAccessLevel",    fieldOffsetCfgOAByCountAccessLevel,      CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOATareAccessLevel]         = new CfgItem("tareAccessLevel",       fieldOffsetCfgOATareAccessLevel,         CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOAFixedWeightAccessLevel]  = new CfgItem("fixedWghtAccessLevel",  fieldOffsetCfgOAFixedWeightAccessLevel,  CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOAShelfLifeAccessLevel]    = new CfgItem("shelfLifeAccessLevel",  fieldOffsetCfgOAShelfLifeAccessLevel,    CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOAProductLifeAccessLevel]  = new CfgItem("prodLifeAccessLevel",   fieldOffsetCfgOAProductLifeAccessLevel,  CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOALabelTypeAccessLevel]    = new CfgItem("labelTypeAccessLevel",  fieldOffsetCfgOALabelTypeAccessLevel,    CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOADateTimeAccessLevel]     = new CfgItem("dateTimeAccessLevel",   fieldOffsetCfgOADateTimeAccessLevel,     CfgItem::INT,   "1");
	cfgItems_[fieldOffsetCfgOAPortionsAccessLevel]     = new CfgItem("portionsAccessLevel",   fieldOffsetCfgOAPortionsAccessLevel,     CfgItem::INT,   "1");


	/*****************************************************************************
	 *  CfgPrintedFields
	 ****************************************************************************/

	cfgItems_[fieldOffsetCfgPFSellByDatePrinted]           = new CfgItem("sellByDatePrinted",           fieldOffsetCfgPFSellByDatePrinted,           CfgItem::BOOL,  "1");
	//  When the user is configuring the fields to print on the label, via Supervisor, this field should not show if the 'sellByDate' is set to NO.
	cfgItems_[fieldOffsetCfgPFSellByDateJulian]            = new CfgItem("sellByDateJulian",            fieldOffsetCfgPFSellByDateJulian,            CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgPFPackedOnDatePrinted]         = new CfgItem("packedOnDatePrinted",         fieldOffsetCfgPFPackedOnDatePrinted,         CfgItem::BOOL,  "1");
	//  When the user is configuring the fields to print on the label, via Supervisor, this field should not show if the 'packedOnDate' is set to NO.
	cfgItems_[fieldOffsetCfgPFPackedOnDateJulian]          = new CfgItem("packedOnDateJulian",          fieldOffsetCfgPFPackedOnDateJulian,          CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgPFProductLifeDatePrinted]      = new CfgItem("prodLifeDatePrinted",         fieldOffsetCfgPFProductLifeDatePrinted,      CfgItem::BOOL,  "1");
	//  When the user is configuring the fields to print on the label, via Supervisor, this field should not show if the 'productLifeDate' is set to NO.
	cfgItems_[fieldOffsetCfgPFProductLifeDateJulian]       = new CfgItem("prodLifeDateJulian",          fieldOffsetCfgPFProductLifeDateJulian,       CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgPFUnitPricePrinted]            = new CfgItem("unitPricePrinted",            fieldOffsetCfgPFUnitPricePrinted,            CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgPFByCountPrinted]              = new CfgItem("byCountPrinted",              fieldOffsetCfgPFByCountPrinted,              CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgPFBarCodePrinted]              = new CfgItem("barCodePrinted",              fieldOffsetCfgPFBarCodePrinted,              CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgPFCurrencySymbolPrinted]       = new CfgItem("currencySymbolPrinted",       fieldOffsetCfgPFCurrencySymbolPrinted,       CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgPFTextNamesPreprinted]         = new CfgItem("textNamesPreprinted",         fieldOffsetCfgPFTextNamesPreprinted,         CfgItem::BOOL,  "1");
	//	Mutually exclusive with oneOnByCountPrinted.
	cfgItems_[fieldOffsetCfgPFWeightOnByCountPrinted]      = new CfgItem("wghtOnByCountPrinted",        fieldOffsetCfgPFWeightOnByCountPrinted,      CfgItem::BOOL,  "0");
	//	Print "1 For" on a by count label. Mutually exclusive with weightOnByCountPrinted.
	cfgItems_[fieldOffsetCfgPFOneOnByCountPrinted]         = new CfgItem("oneOnByCountPrinted",         fieldOffsetCfgPFOneOnByCountPrinted,         CfgItem::BOOL,  "1");
	//  When the user is configuring the fields to print on the label, via Supervisor, this field should not show if 'barCode' is set to NO.
	cfgItems_[fieldOffsetCfgPFSecondCheckDigit]            = new CfgItem("secondCheckDigit",            fieldOffsetCfgPFSecondCheckDigit,            CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgPFPrintFirstLetterOfTotalType] = new CfgItem("printFirstLetterOfTotalType", fieldOffsetCfgPFPrintFirstLetterOfTotalType, CfgItem::INT,   Integer::toString((int)NO_TOTALTYPES));
	//	Print "NET WT" always on a fixed weight label.
	cfgItems_[fieldOffsetCfgPFNetWtOnFixedWeightAlways]    = new CfgItem("netWtOnFixedWtAlwaysPrinted", fieldOffsetCfgPFNetWtOnFixedWeightAlways,    CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgPFFixedWeightFormat]           = new CfgItem("fixedWeightFormat",           fieldOffsetCfgPFFixedWeightFormat,           CfgItem::INT,   Integer::toString((int)FIXED_WEIGHT_FORMAT_STANDARD));


	/****************************************************************************
	*  CfgSelfService
	****************************************************************************/


	/****************************************************************************
	*  CfgEmail
	****************************************************************************/


	/****************************************************************************
	*  CfgSynchronization
	****************************************************************************/

	cfgItems_[fieldOffsetCfgScaleSynchronizationType]    = new CfgItem("scaleSynchronizationType",    fieldOffsetCfgScaleSynchronizationType,    CfgItem::INT,    Integer::toString(ScaleSynchronizationTypeStandAlone));
	cfgItems_[fieldOffsetCfgPrimaryScale]                = new CfgItem("primaryScale",                fieldOffsetCfgPrimaryScale,                CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgSyncConfigurations]          = new CfgItem("syncConfigurations",          fieldOffsetCfgSyncConfigurations,          CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncEmailAlerts]             = new CfgItem("syncEmailAlerts",             fieldOffsetCfgSyncEmailAlerts,             CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncProductDefaults]         = new CfgItem("syncProductDefaults",         fieldOffsetCfgSyncProductDefaults,         CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncClasses]                 = new CfgItem("syncClasses",                 fieldOffsetCfgSyncClasses,                 CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncExpandedTexts]           = new CfgItem("syncExpandedTexts",           fieldOffsetCfgSyncExpandedTexts,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSpecialMessages]         = new CfgItem("syncSpecialMessages",         fieldOffsetCfgSyncSpecialMessages,         CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncNutritionFacts]          = new CfgItem("syncNutritionFacts",          fieldOffsetCfgSyncNutritionFacts,          CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncProductNotes]            = new CfgItem("syncProductNotes",            fieldOffsetCfgSyncProductNotes,            CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncMarqueeTexts]            = new CfgItem("syncMarqueeTexts",            fieldOffsetCfgSyncMarqueeTexts,            CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncTransactions]            = new CfgItem("syncTransactions",            fieldOffsetCfgSyncTransactions,            CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncTransactionTypes]        = new CfgItem("syncTransactionTypes",        fieldOffsetCfgSyncTransactionTypes,        CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCategories]              = new CfgItem("syncCategories",              fieldOffsetCfgSyncCategories,              CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSpecials]                = new CfgItem("syncSpecials",                fieldOffsetCfgSyncSpecials,                CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolCategories]          = new CfgItem("syncCoolCategories",          fieldOffsetCfgSyncCoolCategories,          CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolCountries]           = new CfgItem("syncCoolCountries",           fieldOffsetCfgSyncCoolCountries,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolProductionSteps]     = new CfgItem("syncCoolProductionSteps",     fieldOffsetCfgSyncCoolProductionSteps,     CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolPreTexts]            = new CfgItem("syncCoolPreTexts",            fieldOffsetCfgSyncCoolPreTexts,            CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolTexts]               = new CfgItem("syncCoolTexts",               fieldOffsetCfgSyncCoolTexts,               CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolListTexts]           = new CfgItem("syncCoolListTexts",           fieldOffsetCfgSyncCoolListTexts,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCoolLists]               = new CfgItem("syncCoolLists",               fieldOffsetCfgSyncCoolLists,               CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCategoryCountries]       = new CfgItem("syncCategoryCountries",       fieldOffsetCfgSyncCategoryCountries,       CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCategoryProductionSteps] = new CfgItem("syncCategoryProductionSteps", fieldOffsetCfgSyncCategoryProductionSteps, CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCategoryPreTexts]        = new CfgItem("syncCategoryPreTexts",        fieldOffsetCfgSyncCategoryPreTexts,        CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncProducts]                = new CfgItem("syncProducts",                fieldOffsetCfgSyncProducts,                CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncExpirationLabels]        = new CfgItem("syncExpirationLabels",        fieldOffsetCfgSyncExpirationLabels,        CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncLabels]                  = new CfgItem("syncLabels",                  fieldOffsetCfgSyncLabels,                  CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncLabelTexts]              = new CfgItem("syncLabelTexts",              fieldOffsetCfgSyncLabelTexts,              CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncOperatorNotes]           = new CfgItem("syncOperatorNotes",           fieldOffsetCfgSyncOperatorNotes,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncOperators]               = new CfgItem("syncOperators",               fieldOffsetCfgSyncOperators,               CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSoftkeyGroups]           = new CfgItem("syncSoftkeyGroups",           fieldOffsetCfgSyncSoftkeyGroups,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSoftkeyStyles]           = new CfgItem("syncSoftkeyStyles",           fieldOffsetCfgSyncSoftkeyStyles,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSoftkeyFeatures]         = new CfgItem("syncSoftkeyFeatures",         fieldOffsetCfgSyncSoftkeyFeatures,         CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSoftkeys]                = new CfgItem("syncSoftkeys",                fieldOffsetCfgSyncSoftkeys,                CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncLabelGraphics]           = new CfgItem("syncLabelGraphics",           fieldOffsetCfgSyncLabelGraphics,           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncCategoryGraphics]        = new CfgItem("syncCategoryGraphics",        fieldOffsetCfgSyncCategoryGraphics,        CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncProductGraphics]         = new CfgItem("syncProductGraphics",         fieldOffsetCfgSyncProductGraphics,         CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSoftkeyGraphics]         = new CfgItem("syncSoftkeyGraphics",         fieldOffsetCfgSyncSoftkeyGraphics,         CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSyncSpecialsGraphics]        = new CfgItem("syncSpecialsGraphics",        fieldOffsetCfgSyncSpecialsGraphics,        CfgItem::BOOL,   "0");


	/****************************************************************************
	*  CfgPrepack
	****************************************************************************/

	cfgItems_[fieldOffsetCfgEnableSellBy]                   = new CfgItem("enableSellBy",              fieldOffsetCfgEnableSellBy,                    CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgEnableBestBefore]               = new CfgItem("enableBestBefore",          fieldOffsetCfgEnableBestBefore,                CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnablePackedOn]                 = new CfgItem("enablePackedOn",            fieldOffsetCfgEnablePackedOn,                  CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableProdImg]                  = new CfgItem("enableProdImg",             fieldOffsetCfgEnableProdImg,                   CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableUpcNumber]                = new CfgItem("enableUpcNumber",           fieldOffsetCfgEnableUpcNumber,                 CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgEnableFilmWidth]                = new CfgItem("enableFilmWidth",           fieldOffsetCfgEnableFilmWidth,                 CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableFilmTension]              = new CfgItem("enableFilmTension",         fieldOffsetCfgEnableFilmTension,               CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableWrapTypes]                = new CfgItem("enableWrapTypes",           fieldOffsetCfgEnableWrapTypes,                 CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgEnableCountryOfOrigin]          = new CfgItem("enableCountryOfOrigin",     fieldOffsetCfgEnableCountryOfOrigin,           CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableTrackingNumber]           = new CfgItem("enableTrackingNumber",      fieldOffsetCfgEnableTrackingNumber,            CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableRunTotals]                = new CfgItem("enableRunTotals",           fieldOffsetCfgEnableRunTotals,                 CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableProductionEntry]          = new CfgItem("enableProductionEntry",     fieldOffsetCfgEnableProductionEntry,           CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgProductionEntryContinuousPrint] = new CfgItem("productionEntryContPrint",  fieldOffsetCfgProductionEntryContinuousPrint,  CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableProductionPlanning]       = new CfgItem("enableProductionPlanning",  fieldOffsetCfgEnableProductionPlanning,        CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgProductionPlanningFrame]        = new CfgItem("productionPlanningFrame",   fieldOffsetCfgProductionPlanningFrame,         CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgProductionPlanningUpdate]       = new CfgItem("productionPlanningUpdate",  fieldOffsetCfgProductionPlanningUpdate,        CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgEnableSelfServe]                = new CfgItem("enableSelfServe",           fieldOffsetCfgEnableSelfServe,                 CfgItem::BOOL,    "0");

	// The number of plu's to lookup (on each side of the requested plu number) when the user performs a 'Search by Plu Range'.
	cfgItems_[fieldOffsetCfgPluSearchRange]                 = new CfgItem("pluSearchRange",            fieldOffsetCfgPluSearchRange,                  CfgItem::INT,     "10");

	// When putting a product into a grid cell, should we append the name that is displayed with the product's plu number: eg. (PLU 1234).
	cfgItems_[fieldOffsetCfgAppendingPluNumberToName]       = new CfgItem("appendingPluNumberToName",  fieldOffsetCfgAppendingPluNumberToName,        CfgItem::BOOL,    "1");

	// If the short description field in the product record (used for the description used in the Self-Service products/categories grid) is NULL for the corresponding language,
	// then use this number of characters from the description (not including font escape sequences). If 0, then use the entire description; don't truncate it.
	cfgItems_[fieldOffsetCfgNumOfDescCharsToUse]            = new CfgItem("numOfDescCharsToUse",       fieldOffsetCfgNumOfDescCharsToUse,             CfgItem::INT,     "0");

	cfgItems_[fieldOffsetCfgRotationOverrideSetting1]       = new CfgItem("rotOverrideSetting1",       fieldOffsetCfgRotationOverrideSetting1,        CfgItem::INT,     Integer::toString(ROSNone));
	cfgItems_[fieldOffsetCfgRotationOverrideValue1]         = new CfgItem("rotOverrideValue1",         fieldOffsetCfgRotationOverrideValue1,          CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgRotationOverrideSetting2]       = new CfgItem("rotOverrideSetting2",       fieldOffsetCfgRotationOverrideSetting2,        CfgItem::INT,     Integer::toString(ROSNone));
	cfgItems_[fieldOffsetCfgRotationOverrideValue2]         = new CfgItem("rotOverrideValue2",         fieldOffsetCfgRotationOverrideValue2,          CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgRotationOverrideSetting3]       = new CfgItem("rotOverrideSetting3",       fieldOffsetCfgRotationOverrideSetting3,        CfgItem::INT,     Integer::toString(ROSNone));
	cfgItems_[fieldOffsetCfgRotationOverrideValue3]         = new CfgItem("rotOverrideValue3",         fieldOffsetCfgRotationOverrideValue3,          CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgPrimaryLabelStockRotated]       = new CfgItem("primaryLabelStockRotated",   fieldOffsetCfgPrimaryLabelStockRotated,        CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgSecondaryLabelStockRotated]     = new CfgItem("secondaryLabelStockRotated", fieldOffsetCfgSecondaryLabelStockRotated,      CfgItem::INT,     "0");

	/****************************************************************************
	*  CfgMarquee
	****************************************************************************/


	/****************************************************************************
	*  CfgCommInterface
	****************************************************************************/


	/****************************************************************************
	*  CfgCurrency
	****************************************************************************/


	/*****************************************************************************
	 *  CfgGCD
	 ****************************************************************************/

	cfgItems_[fieldOffsetCfgCustomerDisplay]                = new CfgItem("customerDisplay",              fieldOffsetCfgCustomerDisplay,                 CfgItem::INT,     "2");  // GCD as customer display by default
	cfgItems_[fieldOffsetCfgGCDLayoutNumber]                = new CfgItem("gcdLayoutNumber",              fieldOffsetCfgGCDLayoutNumber,                 CfgItem::INT,     "1");
	cfgItems_[fieldOffsetCfgGCDColorSchemeNumber]           = new CfgItem("gcdColorSchemeNumber",         fieldOffsetCfgGCDColorSchemeNumber,            CfgItem::INT,     "1");
	cfgItems_[fieldOffsetCfgGCDAllowOperReset]              = new CfgItem("gcdAllowOperReset",            fieldOffsetCfgGCDAllowOperReset,               CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgGCDVolume]                      = new CfgItem("gcdVolume",                    fieldOffsetCfgGCDVolume,                       CfgItem::INT,     "60");   // default setting volume is 60% of Full System Volume
	cfgItems_[fieldOffsetCfgGCDPlaybackWaitTime]            = new CfgItem("gcdPlaybackWaitTime",          fieldOffsetCfgGCDPlaybackWaitTime,             CfgItem::INT,     "30");   // default playback wait time 
	cfgItems_[fieldOffsetCfgGCDMediaServerName]             = new CfgItem("gcdMediaServerName",           fieldOffsetCfgGCDMediaServerName,              CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgGCDMediaServerIP]               = new CfgItem("gcdMediaServerIP",             fieldOffsetCfgGCDMediaServerIP,                CfgItem::STRING,  "0.0.0.0");
	cfgItems_[fieldOffsetCfgGCDFtpUserName]                 = new CfgItem("gcdFtpUser",                   fieldOffsetCfgGCDFtpUserName,                  CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgGCDFtpPassword]                 = new CfgItem("gcdFtpPassword",               fieldOffsetCfgGCDFtpPassword,                  CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgGCDFtpDirectory]                = new CfgItem("gcdFtpDir",                    fieldOffsetCfgGCDFtpDirectory,                 CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgGCDPlaylistFile]                = new CfgItem("gcdPlaylistFile",              fieldOffsetCfgGCDPlaylistFile,                 CfgItem::STRING,  "playlist.txt");
	cfgItems_[fieldOffsetCfgGCDAutoSearch]                  = new CfgItem("gcdAutoSearch",                fieldOffsetCfgGCDAutoSearch,                   CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgGCDSearchHour]                  = new CfgItem("gcdSearchHour",                fieldOffsetCfgGCDSearchHour,                   CfgItem::STRING,  "12");
	cfgItems_[fieldOffsetCfgGCDSearchMinute]                = new CfgItem("gcdSearchMinute",              fieldOffsetCfgGCDSearchMinute,                 CfgItem::STRING,  "00");
	cfgItems_[fieldOffsetCfgGCDSearchAmOrPM]                = new CfgItem("gcdSearchAmOrPm",              fieldOffsetCfgGCDSearchAmOrPM,                 CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgGCDIsDnsEnabled]                = new CfgItem("gcdIsDnsEnabled",              fieldOffsetCfgGCDIsDnsEnabled,                 CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgGCDDownloadProtocol]            = new CfgItem("gcdDownloadProtocol",          fieldOffsetCfgGCDDownloadProtocol,             CfgItem::STRING,  "SFTP");
	cfgItems_[fieldOffsetCfgGCDLanguage]                    = new CfgItem("gcdLanguage",                  fieldOffsetCfgGCDLanguage,                     CfgItem::INT,     Integer::toString(GcdLanguage_English));
	cfgItems_[fieldOffsetCfgGCDPrintWithoutCustomerDisplay] = new CfgItem("printWithoutCustomerDisplay",  fieldOffsetCfgGCDPrintWithoutCustomerDisplay,  CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgGCDUseTransactionalLayout]      = new CfgItem("gcdUseTransactionalLayout",    fieldOffsetCfgGCDUseTransactionalLayout,       CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgGCDIdleAdvertType]              = new CfgItem("gcdIdleAdvertType",            fieldOffsetCfgGCDIdleAdvertType,               CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgGCDIdleWebsiteAddress]          = new CfgItem("gcdIdleWebsiteAddress",        fieldOffsetCfgGCDIdleWebsiteAddress,           CfgItem::STRING,  "");
	cfgItems_[fieldOffsetCfgGCDProductAdvertType]           = new CfgItem("gcdProductAdvertType",         fieldOffsetCfgGCDProductAdvertType,            CfgItem::INT,     "0");

	/*****************************************************************************
	 *  CfgAccumulateItems
	 ****************************************************************************/

	cfgItems_[fieldOffsetCfgEnableAccumItems]               = new CfgItem("enableAccumItems",               fieldOffsetCfgEnableAccumItems,               CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableItemStripMode]            = new CfgItem("enableItemStripMode",            fieldOffsetCfgEnableItemStripMode,            CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgAccumItemsReturnToStartScreen]  = new CfgItem("accumItemsReturnToStartScreen",  fieldOffsetCfgAccumItemsReturnToStartScreen,  CfgItem::BOOL,    "1");
	cfgItems_[fieldOffsetCfgEnableAccumItemsTotalsOnly]     = new CfgItem("enableAccumItemsTotalsOnly",     fieldOffsetCfgEnableAccumItemsTotalsOnly,     CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgAccumItemsEANFormatString]      = new CfgItem("accumItemsEANFormatString",      fieldOffsetCfgAccumItemsEANFormatString,      CfgItem::STRING,  "XXEEEEEPPPPP");
	cfgItems_[fieldOffsetCfgAccumItemsEANPrefix]            = new CfgItem("accumItemsEANPrefix",            fieldOffsetCfgAccumItemsEANPrefix,            CfgItem::STRING,  "99");
	cfgItems_[fieldOffsetCfgAccumItemsEANCode]              = new CfgItem("accumItemsEANCode",              fieldOffsetCfgAccumItemsEANCode,              CfgItem::STRING,  "12345");
	cfgItems_[fieldOffsetCfgAccumItemsScaleId]              = new CfgItem("accumItemsScaleId",              fieldOffsetCfgAccumItemsScaleId,              CfgItem::INT,     "1");
	cfgItems_[fieldOffsetCfgAccumItemsTransactionNumber]    = new CfgItem("accumItemsTransactionNumber",    fieldOffsetCfgAccumItemsTransactionNumber,    CfgItem::INT,     "1");
	cfgItems_[fieldOffsetCfgAccumItemsUPCProductNumber]     = new CfgItem("accumItemsUPCProductNumber ",    fieldOffsetCfgAccumItemsUPCProductNumber,     CfgItem::INT,     "6636");
	cfgItems_[fieldOffsetCfgAccumBarcodeUseFirstItem]       = new CfgItem("accumBarcodeUseFirstItem",       fieldOffsetCfgAccumBarcodeUseFirstItem,       CfgItem::BOOL,    "0");
	
	cfgItems_[fieldOffsetCfgReadTransactionsCompatibility]  = new CfgItem("readTransactionsCompatibility",  fieldOffsetCfgReadTransactionsCompatibility,  CfgItem::BOOL,    "0");
	// For readTransactionsCompatibility, if True, then write out transaction data for RT77 using Quantum compatible tags and data.
	// The main reason for this is that the HLX has internally set, unique transaction ID's.
	// In the Quantum, transaction IDs are not unique (each item in the Accumulated Item list gets the same transaction ID).
	// For the HLX, the transaction's transactionGroup attribute serves this purpose.
	// So, when writing out transaction data from the RT77 request, write out the transaction group number as the transaction ID.
	// Also, a slightly different set of fields is written out, based on fields sent from a Quantum.
	//
	// If False, write the HLX transaction's serialized set of fields,
	// and don't write out the transaction group number as the transaction ID.




	cfgItems_[fieldOffsetCfgVirtualPrintingOverride2ndPrinter] = new CfgItem("virtualPrintingOverride2ndPrinter", fieldOffsetCfgVirtualPrintingOverride2ndPrinter, CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgVirtualPrintingOverride3rdPrinter] = new CfgItem("virtualPrintingOverride3rdPrinter", fieldOffsetCfgVirtualPrintingOverride3rdPrinter, CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgDisplaySelfServeStartScreen]       = new CfgItem("displaySelfServeStartScreen",       fieldOffsetCfgDisplaySelfServeStartScreen,       CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgBestSellersOnSelfServeMainScreen]  = new CfgItem("bestSellersOnSelfServeMainScreen",  fieldOffsetCfgBestSellersOnSelfServeMainScreen,  CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgEnableLabelLength_1_000]           = new CfgItem("enableLabelLength_1_000",           fieldOffsetCfgEnableLabelLength_1_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_1_500]           = new CfgItem("enableLabelLength_1_500",           fieldOffsetCfgEnableLabelLength_1_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_1_750]           = new CfgItem("enableLabelLength_1_750",           fieldOffsetCfgEnableLabelLength_1_750,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_2_000]           = new CfgItem("enableLabelLength_2_000",           fieldOffsetCfgEnableLabelLength_2_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_2_375]           = new CfgItem("enableLabelLength_2_375",           fieldOffsetCfgEnableLabelLength_2_375,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_2_500]           = new CfgItem("enableLabelLength_2_500",           fieldOffsetCfgEnableLabelLength_2_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_3_000]           = new CfgItem("enableLabelLength_3_000",           fieldOffsetCfgEnableLabelLength_3_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_3_500]           = new CfgItem("enableLabelLength_3_500",           fieldOffsetCfgEnableLabelLength_3_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_4_000]           = new CfgItem("enableLabelLength_4_000",           fieldOffsetCfgEnableLabelLength_4_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_4_500]           = new CfgItem("enableLabelLength_4_500",           fieldOffsetCfgEnableLabelLength_4_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_5_000]           = new CfgItem("enableLabelLength_5_000",           fieldOffsetCfgEnableLabelLength_5_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_5_500]           = new CfgItem("enableLabelLength_5_500",           fieldOffsetCfgEnableLabelLength_5_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_6_000]           = new CfgItem("enableLabelLength_6_000",           fieldOffsetCfgEnableLabelLength_6_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_6_500]           = new CfgItem("enableLabelLength_6_500",           fieldOffsetCfgEnableLabelLength_6_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_7_000]           = new CfgItem("enableLabelLength_7_000",           fieldOffsetCfgEnableLabelLength_7_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_7_500]           = new CfgItem("enableLabelLength_7_500",           fieldOffsetCfgEnableLabelLength_7_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_8_000]           = new CfgItem("enableLabelLength_8_000",           fieldOffsetCfgEnableLabelLength_8_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_8_500]           = new CfgItem("enableLabelLength_8_500",           fieldOffsetCfgEnableLabelLength_8_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_9_000]           = new CfgItem("enableLabelLength_9_000",           fieldOffsetCfgEnableLabelLength_9_000,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_9_500]           = new CfgItem("enableLabelLength_9_500",           fieldOffsetCfgEnableLabelLength_9_500,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_10_00]           = new CfgItem("enableLabelLength_10_00",           fieldOffsetCfgEnableLabelLength_10_00,           CfgItem::BOOL,  "1");
	cfgItems_[fieldOffsetCfgEnableLabelLength_Continuous]      = new CfgItem("enableLabelLength_Continuous",      fieldOffsetCfgEnableLabelLength_Continuous,      CfgItem::BOOL,  "1");

	cfgItems_[fieldOffsetCfgContinuousStockIgnoreLabelTaken]   = new CfgItem("continuousStockIgnoreLabelTaken",   fieldOffsetCfgContinuousStockIgnoreLabelTaken,   CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgChoseToUseContinuousStock]         = new CfgItem("choseToUseContinuousStock",         fieldOffsetCfgChoseToUseContinuousStock,         CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgWeightRequiredForMeal]             = new CfgItem("weightRequiredForMeal",             fieldOffsetCfgWeightRequiredForMeal,             CfgItem::BOOL,  "1");

	cfgItems_[fieldOffsetCfgSuggestiveSellStyle]               = new CfgItem("suggestiveSellStyle",               fieldOffsetCfgSuggestiveSellStyle,               CfgItem::INT,   "0");
	cfgItems_[fieldOffsetCfgSuggestiveSellFrequency]           = new CfgItem("suggestiveSellFrequency",           fieldOffsetCfgSuggestiveSellFrequency,           CfgItem::INT,   "1");

	cfgItems_[fieldOffsetCfgAlwaysEnableVirtualKeyboard]       = new CfgItem("alwaysEnableVirtualKeyboard",       fieldOffsetCfgAlwaysEnableVirtualKeyboard,       CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgShowSelectAllWhenVoidingTrans]     = new CfgItem("showSelectAllWhenVoidingTrans",     fieldOffsetCfgShowSelectAllWhenVoidingTrans,     CfgItem::BOOL,  "1");

	cfgItems_[fieldOffsetCfgPeriodicOfflineBeep]               = new CfgItem("periodicOfflineBeep",               fieldOffsetCfgPeriodicOfflineBeep,               CfgItem::INT,   "0");

	cfgItems_[fieldOffsetCfgEnableGlobalPercentOff]            = new CfgItem("enableGlobalPercentOff",            fieldOffsetCfgEnableGlobalPercentOff,            CfgItem::BOOL,  "0");
	cfgItems_[fieldOffsetCfgGlobalPercentageOff]               = new CfgItem("globalPercentageOff",               fieldOffsetCfgGlobalPercentageOff,               CfgItem::INT,   "00000");

	cfgItems_[fieldOffsetCfgEnableAccumItemsAndFlashkeys]      = new CfgItem("enableAccumItemsAndFlashkeys",      fieldOffsetCfgEnableAccumItemsAndFlashkeys,      CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgRequireFixedWeightVerification]    = new CfgItem("requireFixedWeightVerification",    fieldOffsetCfgRequireFixedWeightVerification,    CfgItem::BOOL,  "0");

	cfgItems_[fieldOffsetCfgAutoBackupDBFrequency]             = new CfgItem("autoBackupDBFrequency",             fieldOffsetCfgAutoBackupDBFrequency,             CfgItem::INT,   "24");

	// This has been added so that HTe-ScaleManagement can have access to this information, the FPC version.
	// NOTE:  This is only for informational purposes.  It will be set to the most current version info supplied by the FPC.
	// You can not send this into the scale and expect the FPC to be set to this version.  If you send it in via comm or backup/restore, it is ignored.
	cfgItems_[fieldOffsetCfgFPCVersion] = new CfgItem("FPCVersion", fieldOffsetCfgFPCVersion, CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgFPCWeigherVersion] = new CfgItem("FPCWeigherVersion", fieldOffsetCfgFPCWeigherVersion, CfgItem::STRING, "");

	// This has been added for writing out with configuration to comm (RTB9).  It is not stored in the database, rather, it is read from the SPI-flash.
	// It will not be updated by being sent in via comm or restore/merge.
	cfgItems_[fieldOffsetCfgScaleModel] = new CfgItem("scaleModel", fieldOffsetCfgScaleModel, CfgItem::STRING, "HT-Series");
	cfgItems_[fieldOffsetCfgScaleModel]->setPersistent(false);


	cfgItems_[fieldOffsetCfgCutTestOptionEnabled]         = new CfgItem("cutTestOptionEnabled",          fieldOffsetCfgCutTestOptionEnabled,          CfgItem::INT,     Integer::toString(CutTestOption_Classic));		// 1 = historical cut tests, 2 = "Option 1" cut tests (for Hobart Mexico)
	cfgItems_[fieldOffsetCfgMaxSuspendedCutTestsOverall]  = new CfgItem("maxSuspendedCutTestsOverall",   fieldOffsetCfgMaxSuspendedCutTestsOverall,   CfgItem::INT,     "40");
	cfgItems_[fieldOffsetCfgMaxSuspendedCutTestsOperator] = new CfgItem("maxSuspendedCutTestsOperator",  fieldOffsetCfgMaxSuspendedCutTestsOperator,  CfgItem::INT,     "20");
	cfgItems_[fieldOffsetCfgSecondsToAutoSuspendCutTest]  = new CfgItem("secondsToAutoSuspendCutTest",   fieldOffsetCfgSecondsToAutoSuspendCutTest,   CfgItem::INT,     "0");
	cfgItems_[fieldOffsetCfgNextCutTestNumber]            = new CfgItem("nextCutTestNumber",             fieldOffsetCfgNextCutTestNumber,             CfgItem::INT,     "1");

	cfgItems_[fieldOffsetCfgBottomScreenContent]          = new CfgItem("bottomScreenContent",           fieldOffsetCfgBottomScreenContent,           CfgItem::INT,     "0");  // 0 = nothing, 1 = Build-A-Meal, 2 = Cut Test Meat Sheet
																																			  // This has been added duee to Hy-Vee requesting that the Search feature be removed from the Operator's ability to use it.
	cfgItems_[fieldOffsetCfgEnableSearchMenuButton]       = new CfgItem("enableSearchMenuButton",        fieldOffsetCfgEnableSearchMenuButton,        CfgItem::BOOL,    "1");

	cfgItems_[fieldOffsetCfgEnableHelpPrompts]            = new CfgItem("enableHelpPrompts",             fieldOffsetCfgEnableHelpPrompts,             CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgEnableUseCharacterSearching]  = new CfgItem("enableUseCharacterSearching",   fieldOffsetCfgEnableUseCharacterSearching,   CfgItem::BOOL,    "0");

	cfgItems_[fieldOffsetCfgUseSecondTare]                = new CfgItem("useSecondTare",                 fieldOffsetCfgUseSecondTare,                 CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgTrackingSupervisorChanges]    = new CfgItem("trackingSupervisorChanges",     fieldOffsetCfgTrackingSupervisorChanges,     CfgItem::BOOL,    "0");
	cfgItems_[fieldOffsetCfgVariableShelfLifeTime]        = new CfgItem("variableShelfLifeTime",         fieldOffsetCfgVariableShelfLifeTime,         CfgItem::STRING,  "00:00 AM");	// POS 52.1: HT-365

	cfgItems_[fieldOffsetCfgSelfServeDisplayTareOnRW]           = new CfgItem("selfServeDisplayTareOnRW",       fieldOffsetCfgSelfServeDisplayTareOnRW,         CfgItem::BOOL, "0");
	cfgItems_[fieldOffsetDoNotPrintNutrifacts]                  = new CfgItem("doNotPrintNutrifacts",           fieldOffsetDoNotPrintNutrifacts,                CfgItem::BOOL, "0");
	
	cfgItems_[fieldOffsetCfgDigimarcEnabled]					= new CfgItem("digimarcEnabled",				fieldOffsetCfgDigimarcEnabled,					CfgItem::BOOL,	"1");
    cfgItems_[fieldOffsetCfgDigimarcDotDensity]					= new CfgItem("digimarcDotDensity",				fieldOffsetCfgDigimarcDotDensity,				CfgItem::INT,	"40" );
    cfgItems_[fieldOffsetCfgDigimarcDotSize]					= new CfgItem("digimarcDotSize",				fieldOffsetCfgDigimarcDotSize,					CfgItem::INT,	"1" );
	cfgItems_[fieldOffsetCfgDigimarcWatermarkRes]				= new CfgItem("digimarcWatermarkRes",			fieldOffsetCfgDigimarcWatermarkRes,				CfgItem::INT,	"75" );
	cfgItems_[fieldOffsetCfgDigimarcExpandedFreshEnabled]		= new CfgItem("digimarcExpandedFreshEnabled",	fieldOffsetCfgDigimarcExpandedFreshEnabled,		CfgItem::BOOL,	"1");

	cfgItems_[fieldOffsetCfgReduceGS1Length]		            = new CfgItem("reduceGS1Length",				fieldOffsetCfgReduceGS1Length,					CfgItem::BOOL,	"0");

	cfgItems_[fieldOffsetCfgDisableHooks]		                = new CfgItem("disableHooks",					fieldOffsetCfgDisableHooks,						CfgItem::BOOL,	"0");

	cfgItems_[fieldOffsetCfgCameraSystemTrustLevel]				= new CfgItem("cameraSystemTrustLevel",			fieldOffsetCfgCameraSystemTrustLevel,			CfgItem::INT,	"0" );

	/*****************************************************************************
	 *  CfgFTP
	 ****************************************************************************/

	cfgItems_[fieldOffsetCfgFTPServerIporHostName]                 = new CfgItem("FTPServerIPorHostName",                 fieldOffsetCfgFTPServerIporHostName,                 CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgFTPUserName]                           = new CfgItem("FTPUserName",                           fieldOffsetCfgFTPUserName,                           CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgFTPPassword]                           = new CfgItem("FTPPassword",                           fieldOffsetCfgFTPPassword,                           CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgFTPSubDir]                             = new CfgItem("FTPSubDirectory",                       fieldOffsetCfgFTPSubDir,                             CfgItem::STRING, "");
	// UPDATE NOW NEW FIELDS
	cfgItems_[fieldOffsetCfgFTPstartTime]                          = new CfgItem("FTPstartTime",                          fieldOffsetCfgFTPstartTime,                          CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgFTPuseAutomatic]                       = new CfgItem("FTPUseAutomatic",                       fieldOffsetCfgFTPuseAutomatic,                       CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPmonday]                             = new CfgItem("FTPmonday",                             fieldOffsetCfgFTPmonday,                             CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPtuesday]                            = new CfgItem("FTPtuesday",                            fieldOffsetCfgFTPtuesday,                            CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPwednesday]                          = new CfgItem("FTPwednesday",                          fieldOffsetCfgFTPwednesday,                          CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPthursday]                           = new CfgItem("FTPthursday",                           fieldOffsetCfgFTPthursday,                           CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPfriday]                             = new CfgItem("FTPfriday",                             fieldOffsetCfgFTPfriday,                             CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPsaturday]                           = new CfgItem("FTPsaturday",                           fieldOffsetCfgFTPsaturday,                           CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPsunday]                             = new CfgItem("FTPsunday",                             fieldOffsetCfgFTPsunday,                             CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPdownloadFirmwareOnly]               = new CfgItem("FTPdownloadFirwmwareOnly",              fieldOffsetCfgFTPdownloadFirmwareOnly,               CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPdownloadInstallFirmware]            = new CfgItem("FTPdownloadInstallFirmware",            fieldOffsetCfgFTPdownloadInstallFirmware,            CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPdailyUpdates]                       = new CfgItem("FTPdailyUpdates",                       fieldOffsetCfgFTPdailyUpdates,                       CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPhourlyUpdates]                      = new CfgItem("FTPhourlyUpdates",                      fieldOffsetCfgFTPhourlyUpdates,                      CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTP15MinuteUpdates]                    = new CfgItem("FTP15MinuteUpdates",                    fieldOffsetCfgFTP15MinuteUpdates,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdatePLUData]                      = new CfgItem("FTPUpdatePLUData",                      fieldOffsetCfgFTPUpdatePLUData,                      CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateLabelData]                    = new CfgItem("FTPUpdateLabelData",                    fieldOffsetCfgFTPUpdateLabelData,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateFlashkeyData]                 = new CfgItem("FTPUpdateFlashkeyData",                 fieldOffsetCfgFTPUpdateFlashkeyData,                 CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateOperatorData]                 = new CfgItem("FTPUpdateOperatorData",                 fieldOffsetCfgFTPUpdateLabelData,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdatePlaylists]                    = new CfgItem("FTPUpdatePlaylists",                    fieldOffsetCfgFTPUpdatePlaylists,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateAlerts]                       = new CfgItem("FTPUpdateAlerts",                       fieldOffsetCfgFTPUpdateAlerts,                       CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateScaleConfiguration]           = new CfgItem("FTPUpdateScaleConfiguration",           fieldOffsetCfgFTPUpdateScaleConfiguration,           CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateSynchronizationConfiguration] = new CfgItem("FTPUpdateSynchronizationConfiguration", fieldOffsetCfgFTPUpdateSynchronizationConfiguration, CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateStoreNameNumber]              = new CfgItem("FTPUpdateStoreNameNumber",              fieldOffsetCfgFTPUpdateStoreNameNumber,              CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateVideos]                       = new CfgItem("FTPUpdateVideos",                       fieldOffsetCfgFTPUpdateVideos,                       CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateImages]                       = new CfgItem("FTPUpdateImages",                       fieldOffsetCfgFTPUpdateImages,                       CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateAudio]                        = new CfgItem("FTPUpdateAudio",                        fieldOffsetCfgFTPUpdateAudio,                        CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPUpdateFirmware]                     = new CfgItem("FTPUpdateFirmware",                     fieldOffsetCfgFTPUpdateFirmware,                     CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendPLUData]                        = new CfgItem("FTPSendPLUData",                        fieldOffsetCfgFTPSendPLUData,                        CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendLabelData]                      = new CfgItem("FTPSendLabelData",                      fieldOffsetCfgFTPSendLabelData,                      CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendFlashkeyData]                   = new CfgItem("FTPSendFlashkeyData",                   fieldOffsetCfgFTPSendFlashkeyData,                   CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendOperatorData]                   = new CfgItem("FTPSendOperatorData",                   fieldOffsetCfgFTPSendLabelData,                      CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendPlaylists]                      = new CfgItem("FTPSendPlaylists",                      fieldOffsetCfgFTPSendPlaylists,                      CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendAlerts]                         = new CfgItem("FTPUSendAlerts",                        fieldOffsetCfgFTPSendAlerts,                         CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendScaleConfiguration]             = new CfgItem("FTPSendScaleConfiguration",             fieldOffsetCfgFTPSendScaleConfiguration,             CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendSynchronizationConfiguration]   = new CfgItem("FTPSendSynchronizationConfiguration",   fieldOffsetCfgFTPSendSynchronizationConfiguration,   CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendStoreNameNumber]                = new CfgItem("FTPSendStoreNameNumber",                fieldOffsetCfgFTPSendStoreNameNumber,                CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendVideos]                         = new CfgItem("FTPSendVideos",                         fieldOffsetCfgFTPSendVideos,                         CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendImages]                         = new CfgItem("FTPSendImages",                         fieldOffsetCfgFTPSendImages,                         CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendAudio]                          = new CfgItem("FTPSendAudio",                          fieldOffsetCfgFTPSendAudio,                          CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPSendFirmware]                       = new CfgItem("FTPSendFirmware",                       fieldOffsetCfgFTPSendFirmware,                       CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPAllowOperatorSendToDept]            = new CfgItem("FTPAllowOperatorSendToDept",            fieldOffsetCfgFTPAllowOperatorSendToDept,            CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgFTPAllowOperatorUpdateNow]             = new CfgItem("FTPAllowOperatorUpdateNow",             fieldOffsetCfgFTPAllowOperatorUpdateNow,             CfgItem::STRING, "0");
	// END UPDATE NOW NEW FIELDS


	cfgItems_[fieldOffsetCfgSelfServeWeightRequiredToSelectItem]   = new CfgItem("selfServeWeightRequiredToSelectItem",   fieldOffsetCfgSelfServeWeightRequiredToSelectItem,   CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSelfServeByCountItemsAllowed]          = new CfgItem("selfServeByCountItemsAllowed",          fieldOffsetCfgSelfServeByCountItemsAllowed,          CfgItem::BOOL,   "1");

	cfgItems_[fieldOffsetCfgPrinter1LabelLength]                   = new CfgItem("printer1LabelLength",                   fieldOffsetCfgPrinter1LabelLength,                   CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgPrinter2LabelLength]                   = new CfgItem("printer2LabelLength",                   fieldOffsetCfgPrinter2LabelLength,                   CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgPrinter3LabelLength]                   = new CfgItem("printer3LabelLength",                   fieldOffsetCfgPrinter3LabelLength,                   CfgItem::INT,    "0");

	cfgItems_[fieldOffsetCfgDefaultSoftkeyFontPointSize]           = new CfgItem("defaultSoftkeyFontPointSize",           fieldOffsetCfgDefaultSoftkeyFontPointSize,           CfgItem::INT,    "12");

	cfgItems_[fieldOffsetCfgDiagServiceContactInfo]                = new CfgItem("diagServiceContactInfo",                fieldOffsetCfgDiagServiceContactInfo,                CfgItem::STRING, "1.888.4HOBART (1.888.446.2278)");
	cfgItems_[fieldOffsetCfgDiagGatewayDeviceAddress]              = new CfgItem("diagGatewayDeviceAddress",              fieldOffsetCfgDiagGatewayDeviceAddress,              CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel]    = new CfgItem("diagGatewayDeviceStatusMsgLogLevel",	  fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel,	   CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgValueMaxDiagCollectionDisabled]        = new CfgItem("valueMaxDiagCollectionDisabled",        fieldOffsetCfgValueMaxDiagCollectionDisabled,        CfgItem::BOOL,   "0");

	cfgItems_[fieldOffsetCfgSelfServePrintToScreen]                = new CfgItem("selfServePrintToScreen",                fieldOffsetCfgSelfServePrintToScreen,                CfgItem::BOOL,   "1");				// // Amazon GO - Print To Screen

	cfgItems_[fieldOffsetCfgVoidTransactionsViewHours]             = new CfgItem("voidTransactionsViewHours",             fieldOffsetCfgVoidTransactionsViewHours,             CfgItem::INT,    "8");   // SOF-43

	cfgItems_[fieldOffsetCfgSelfServeEnablePlaylists]              = new CfgItem("selfServeEnablePlaylists",              fieldOffsetCfgSelfServeEnablePlaylists,              CfgItem::BOOL, "0");
	cfgItems_[fieldOffsetCfgSelfServePlaylistFile]                 = new CfgItem("selfServePlaylistFile",                 fieldOffsetCfgSelfServePlaylistFile,                 CfgItem::STRING, "" );

	// This is last dept (deptNum) that was/is active, so we return them to that dept the next time the scale is booted.
	cfgItems_[fieldOffsetCfgMostRecentActiveDeptNum]               = new CfgItem("mostRecentActiveDeptNum",               fieldOffsetCfgMostRecentActiveDeptNum,               CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgUseCurrentDeptnumWhenNotReceived]      = new CfgItem("useCurrentDeptnumWhenNotReceived",      fieldOffsetCfgUseCurrentDeptnumWhenNotReceived,      CfgItem::BOOL,   "0");

	cfgItems_[fieldOffsetCfgUnitPricePrintMethod]                  = new CfgItem("unitPricePrintMethod",                  fieldOffsetCfgUnitPricePrintMethod,                  CfgItem::INT,    "1");

	// Since this is not stored to the database, it is always initialized to false, "0", upon scale boot (per Cherie B. 10/1/2018)
	cfgItems_[fieldOffsetCfgPick5Active]                           = new CfgItem("pick5Active",                           fieldOffsetCfgPick5Active,                           CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgPick5Active]->setPersistent(false);

	cfgItems_[fieldOffsetCfgIsCutterEnabled]                       = new CfgItem("cutterEnabled",                         fieldOffsetCfgIsCutterEnabled,                       CfgItem::BOOL,   "1"); //SOF-3870
	cfgItems_[fieldOffsetCfgIsCodeCheckerEnabled]                  = new CfgItem("codeCheckerEnabled",                    fieldOffsetCfgIsCodeCheckerEnabled,                  CfgItem::BOOL,   "1");
	cfgItems_[fieldOffsetCfgVirtualCombinePrint]				   = new CfgItem("combinedVirtualPrint",  				  fieldOffsetCfgVirtualCombinePrint,               	   CfgItem::BOOL,   "0"); //PDD-146 SOF-3848                           
	cfgItems_[fieldOffsetCfgIsConserveLinerLess]				   = new CfgItem("conserveLinerLessStock",  		      fieldOffsetCfgIsConserveLinerLess,               	   CfgItem::BOOL,   "0"); //SOF-3875                           
	cfgItems_[fieldOffsetCfgAllowSupervisorDotWear]                = new CfgItem("AllowSupervisorDotWear",                fieldOffsetCfgAllowSupervisorDotWear,                CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgAllowOperatorDotWear]                  = new CfgItem("AllowOperatorDotWear",                  fieldOffsetCfgAllowOperatorDotWear,                  CfgItem::BOOL,   "0");

	cfgItems_[fieldOffsetCfgPriceChangedLabelIndicator]            = new CfgItem("priceChangedLabelIndicator",            fieldOffsetCfgPriceChangedLabelIndicator,            CfgItem::STRING, "");

	cfgItems_[fieldOffsetCfgAutoUpdateUPCBarcodeOver99]            = new CfgItem("AutoUpdateUPCBarcodeOver99",           fieldOffsetCfgAutoUpdateUPCBarcodeOver99,             CfgItem::BOOL, "0");
	cfgItems_[fieldOffsetCfgBarcodeTypeOver99]                     = new CfgItem("barcodeTypeOver99",                    fieldOffsetCfgBarcodeTypeOver99,                      CfgItem::INT, "");
	// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
	//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
	//                        Zero means don't change the width of the alternate barcode from the width of the original UPC-A barcode in the label type
	//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
	cfgItems_[fieldOffsetCfgBarcodeWidthOver99]                    = new CfgItem("barcodeWidthOver99",                   fieldOffsetCfgBarcodeWidthOver99,                     CfgItem::INT, "0");
	cfgItems_[fieldOffsetBarcodeOffsetFromLeftOver99]              = new CfgItem("barcodeOffsetFromLeftOver99",          fieldOffsetBarcodeOffsetFromLeftOver99,               CfgItem::INT, "0");

	// These four are kept in a separate Properties file.
	cfgItems_[fieldOffsetCfgSNMPTrapsIp]                           = new CfgItem("snmpTrapsIP",                           fieldOffsetCfgSNMPTrapsIp,                           CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgSNMPTrapsIp]->setPersistent(false);
	cfgItems_[fieldOffsetCfgSNMPTrapsPort]                         = new CfgItem("snmpTrapsPort",                         fieldOffsetCfgSNMPTrapsPort,                         CfgItem::STRING, "162");
	cfgItems_[fieldOffsetCfgSNMPTrapsPort]->setPersistent(false);
	cfgItems_[fieldOffsetCfgSNMPVariablesPort]                     = new CfgItem("snmpVariablesPort",                     fieldOffsetCfgSNMPVariablesPort,                     CfgItem::STRING, "161");
	cfgItems_[fieldOffsetCfgSNMPVariablesPort]->setPersistent(false);
	// Kind of an exception case, this next parameter is considered to be a string, not bool, because the toStringCfg method deals with this offset as an exception case,
	// since it has to go to a separate properties file to get its value.
	cfgItems_[fieldOffsetCfgSNMPEnabled]                           = new CfgItem("snmpEnabled",                           fieldOffsetCfgSNMPEnabled,                           CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSNMPEnabled]->setPersistent(false);

	cfgItems_[fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel]     = new CfgItem("handsFreeDeviceMinLoginPowerLevel",     fieldOffsetCfgHandsFreeDeviceMinLoginPowerLevel,     CfgItem::INT,    "-86");

	cfgItems_[fieldOffsetCfgEnableSearchOnKeypad]		= new CfgItem("enableSearchOnKeypad",		fieldOffsetCfgEnableSearchOnKeypad,			CfgItem::BOOL, "0");
	cfgItems_[fieldOffsetCfgEnableEnhancedCenterScreen] = new CfgItem("enableEnhancedCenterScreen", fieldOffsetCfgEnableEnhancedCenterScreen,	CfgItem::BOOL, "0");
	cfgItems_[fieldOffsetCfgEnhancedCenterHomePage]		= new CfgItem("enhancedCenterHomePage",		fieldOffsetCfgEnhancedCenterHomePage,		CfgItem::STRING, "");

	cfgItems_[fieldOffsetCfgSelfServeIgnorePrinterErrors]          = new CfgItem("selfServeIgnorePrinterErrors",         fieldOffsetCfgSelfServeIgnorePrinterErrors,           CfgItem::INT, Integer::toString(SelfServOption_PrintToLabel));
	cfgItems_[fieldOffsetCfgSelfServeSearchKeypadFlashkeys]        = new CfgItem("selfServeSearchKeypadFlashkeys",       fieldOffsetCfgSelfServeSearchKeypadFlashkeys,         CfgItem::BOOL,   "1");
	cfgItems_[fieldOffsetCfgSelfServeDisplayPluLabel]              = new CfgItem("selfServeDisplayPluLabel",             fieldOffsetCfgSelfServeDisplayPluLabel,               CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgEnableUPTagPrefix]				   	   = new CfgItem("enableUPTagPrefix",			fieldOffsetCfgEnableUPTagPrefix,			CfgItem::BOOL, "0");   // Default is false. If true, the tag data does not override the current COOL text.

	cfgItems_[fieldOffsetCfgMinSweetheartWeight] = new CfgItem("minSweetheartWeight", fieldOffsetCfgMinSweetheartWeight, CfgItem::INT, "0");  // with 3 implied decimal places

	cfgItems_[fieldOffsetCfgOnlineOrderingEnabled] = new CfgItem("onlineOrderingEnabled", fieldOffsetCfgOnlineOrderingEnabled, CfgItem::BOOL, "1");   // Default is true - enabled
	cfgItems_[fieldOffsetCfgOnlineOrderingServer]  = new CfgItem("onlineOrderingServer",  fieldOffsetCfgOnlineOrderingServer,  CfgItem::STRING, "");
	cfgItems_[fieldOffsetCfgOnlineOrderingZone]    = new CfgItem("onlineOrderingZone",    fieldOffsetCfgOnlineOrderingZone,    CfgItem::INT, "0");

	cfgItems_[fieldOffsetCfgSelfServeDisplayBarcodeLabel]          = new CfgItem("selfServeDisplayBarcodeLabel",         fieldOffsetCfgSelfServeDisplayBarcodeLabel,           CfgItem::BOOL,   "1");

	// pjr383 TODO - check these types as I have changed them around a bit
	cfgItems_[fieldOffsetCfgSelfServeStartState]					= new CfgItem("selfServeStartState",			fieldOffsetCfgSelfServeStartState,              CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgSelfServeItemEntryState]				= new CfgItem("selfServeItemEntryState",		fieldOffsetCfgSelfServeItemEntryState,          CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgSelfServeSearchState]					= new CfgItem("selfServeSearchState",			fieldOffsetCfgSelfServeSearchState,             CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgSelfServePrintState]					= new CfgItem("selfServePrintState",			fieldOffsetCfgSelfServePrintState,              CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgSelfServeAfterPrintingState]            = new CfgItem("selfServeAfterPrintingState",    fieldOffsetCfgSelfServeAfterPrintingState,      CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgSelfServeStartScreenClickable]          = new CfgItem("selfServeStartScreenClickable",	fieldOffsetCfgSelfServeStartScreenClickable,	CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSelfServeIncludeKeypad]					= new CfgItem("selfServeIncludeKeypad",			fieldOffsetCfgSelfServeIncludeKeypad,           CfgItem::BOOL,   "1");
	cfgItems_[fieldOffsetCfgSelfServeHideMenuBar]                   = new CfgItem("selfServeHideMenuBar",           fieldOffsetCfgSelfServeHideMenuBar,             CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgSelfServePrintOnStableWeight]           = new CfgItem("selfServePrintOnStableWeight",   fieldOffsetCfgSelfServePrintOnStableWeight,     CfgItem::BOOL,   "0");
	cfgItems_[fieldOffsetCfgEnableVariableShelfLifeTime]            = new CfgItem("enableVariableShelfLifeTime",    fieldOffsetCfgEnableVariableShelfLifeTime,      CfgItem::BOOL,   "0");
	
	// SOF-4769: Scheduled Merge
	cfgItems_[fieldOffsetCfgCommScheduledUseAutomaticMerges] = new CfgItem("CommScheduledUseAutomaticMerges",            fieldOffsetCfgCommScheduledUseAutomaticMerges,        CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledMonday]             = new CfgItem("CommScheduledMonday",                        fieldOffsetCfgCommScheduledMonday,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledTuesday]            = new CfgItem("CommScheduledTuesday",                       fieldOffsetCfgCommScheduledTuesday,                   CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledWednesday]          = new CfgItem("CommScheduledWednesday",                     fieldOffsetCfgCommScheduledWednesday,                 CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledThursday]           = new CfgItem("CommScheduledThursday",                      fieldOffsetCfgCommScheduledThursday,                  CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledFriday]             = new CfgItem("CommScheduledFriday",                        fieldOffsetCfgCommScheduledFriday,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledSaturday]           = new CfgItem("CommScheduledSaturday",                      fieldOffsetCfgCommScheduledSaturday,                  CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledSunday]             = new CfgItem("CommScheduledSunday",                        fieldOffsetCfgCommScheduledSunday,                    CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledDailyUpdates]       = new CfgItem("CommScheduledDailyUpdates",                  fieldOffsetCfgCommScheduledDailyUpdates,              CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledStartTime]          = new CfgItem("CommScheduledStartTime",                     fieldOffsetCfgCommScheduledStartTime,                 CfgItem::STRING, "00:00 AM");
	cfgItems_[fieldOffsetCfgCommScheduledHourlyUpdates]      = new CfgItem("CommScheduledHourlyUpdates",                 fieldOffsetCfgCommScheduledHourlyUpdates,             CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledExecMergeAfterComm] = new CfgItem("CommScheduledExecMergeAfterComm",            fieldOffsetCfgCommScheduledExecMergeAfterComm,        CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgCommScheduledMergeHistory]       = new CfgItem("CommScheduledMergeHistory",                  fieldOffsetCfgCommScheduledMergeHistory,              CfgItem::STRING, "N/A");

	cfgItems_[fieldOffsetCfgWrapperCameraEnabled]            = new CfgItem("wrapperCameraEnabled",                       fieldOffsetCfgWrapperCameraEnabled,                   CfgItem::BOOL, "1");

	//PJR413 (SOF-4772)
	cfgItems_[fieldOffsetCfgSelfServeFixedPlu]               = new CfgItem("selveServeFixedPlu",                         fieldOffsetCfgSelfServeFixedPlu,                      CfgItem::INT,	"0" );
	cfgItems_[fieldOffsetCfgSelfServeEnableFreshServe]       = new CfgItem("selfServeEnableFreshServe",                  fieldOffsetCfgSelfServeEnableFreshServe,              CfgItem::BOOL,   "0" );
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyType0]          = new CfgItem("selfServeFlashkeyType0",                     fieldOffsetCfgSelfServeFlashkeyType0,                 CfgItem::STRING, "1" );
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyType1]			 = new CfgItem("selfServeFlashkeyType1",					 fieldOffsetCfgSelfServeFlashkeyType1,                 CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyType2]			 = new CfgItem("selfServeFlashkeyType2",                     fieldOffsetCfgSelfServeFlashkeyType2,                 CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyType3]			 = new CfgItem("selfServeFlashkeyType3",                     fieldOffsetCfgSelfServeFlashkeyType3,                 CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyType4]			 = new CfgItem("selfServeFlashkeyType4",                     fieldOffsetCfgSelfServeFlashkeyType4,                 CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyText0]          = new CfgItem("selfServeFlashkeyText0",                     fieldOffsetCfgSelfServeFlashkeyText0,                 CfgItem::STRING, "Best Sellers" );
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyText1]			 = new CfgItem("selfServeFlashkeyText1",                     fieldOffsetCfgSelfServeFlashkeyText1,                 CfgItem::STRING, "Fruits");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyText2]			 = new CfgItem("selfServeFlashkeyText2",                     fieldOffsetCfgSelfServeFlashkeyText2,                 CfgItem::STRING, "Vegetables");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyText3]			 = new CfgItem("selfServeFlashkeyText3",                     fieldOffsetCfgSelfServeFlashkeyText3,                 CfgItem::STRING, "Organics");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyText4]			 = new CfgItem("selfServeFlashkeyText4",                     fieldOffsetCfgSelfServeFlashkeyText4,                 CfgItem::STRING, "Others");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyCatNum0]        = new CfgItem("selfServeFlashkeyCatNum0",                   fieldOffsetCfgSelfServeFlashkeyCatNum0,               CfgItem::STRING, "0" );
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyCatNum1]		 = new CfgItem("selfServeFlashkeyCatNum1",                   fieldOffsetCfgSelfServeFlashkeyCatNum1,               CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyCatNum2]		 = new CfgItem("selfServeFlashkeyCatNum2",                   fieldOffsetCfgSelfServeFlashkeyCatNum2,               CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyCatNum3]		 = new CfgItem("selfServeFlashkeyCatNum3",                   fieldOffsetCfgSelfServeFlashkeyCatNum3,               CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeyCatNum4]		 = new CfgItem("selfServeFlashkeyCatNum4",                   fieldOffsetCfgSelfServeFlashkeyCatNum4,               CfgItem::STRING, "0");
	cfgItems_[fieldOffsetCfgSelfServeFreshServeLabelOffset]  = new CfgItem("SelfServeFreshServeLabelOffset",             fieldOffsetCfgSelfServeFreshServeLabelOffset,         CfgItem::INT,    "0");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeySecText0]		 = new CfgItem("selfServeFlashkeySecText0",                  fieldOffsetCfgSelfServeFlashkeySecText0,              CfgItem::STRING, "Más vendidos");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeySecText1]       = new CfgItem("selfServeFlashkeySecText1",                  fieldOffsetCfgSelfServeFlashkeySecText1,              CfgItem::STRING, "Frutas");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeySecText2]       = new CfgItem("selfServeFlashkeySecText2",                  fieldOffsetCfgSelfServeFlashkeySecText2,              CfgItem::STRING, "Verduas");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeySecText3]       = new CfgItem("selfServeFlashkeySecText3",                  fieldOffsetCfgSelfServeFlashkeySecText3,              CfgItem::STRING, "Orgánico");
	cfgItems_[fieldOffsetCfgSelfServeFlashkeySecText4]       = new CfgItem("selfServeFlashkeySecText4",                  fieldOffsetCfgSelfServeFlashkeySecText4,              CfgItem::STRING, "Otro");
	cfgItems_[fieldOffsetCfgReadBarcodeFromCloud]			 = new CfgItem("readBarcodeFromCloud",			             fieldOffsetCfgReadBarcodeFromCloud,				   CfgItem::BOOL, "0");
	cfgItems_[fieldOffsetCfgBarcodeServerAddress]			 = new CfgItem("barcodeServerAddress",			             fieldOffsetCfgBarcodeServerAddress,				   CfgItem::STRING, "example: url/scale/v1/{countryCode}/{storeNbr}/mips/generate-barcode");

	cfgItems_[fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag] = new CfgItem("ignoreVendorAndProductNumTagsWhenUpcNumTag", fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag, CfgItem::BOOL, "0");

	cfgItems_[fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight] = new CfgItem("allowKeyboardNetWeightItemAsRandomWeight", fieldOffsetAllowKeyboardNetWeightItemAsRandomWeight, CfgItem::BOOL, "0");

}

bool Configuration::isClearPluAfterPrinting() const
{
	bool result = true;
	if (isWrapper())
		result = false;
	else
		result = toBoolCfg(fieldOffsetCfgClearPluAfterPrinting); 
	Debug::trace("Configuration.clearplu", string("isClearPluAfterPrinting result=") + Bool::toString(result));
	return result;
}

void Configuration::load(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	Util::CLockCriticalSection lock( cfgLoadCS_ );

	HobString dbVal;

	int end = (int)cfgItems_.size();
	for (int i = 0; i < end; i++)
	{
		if (cfgItems_[i])
		{
			if (loadField(dbAccess, cfgItems_[i]->getName(), dbVal))
			{
				cfgItems_[i]->setValue(dbVal);
				cfgItems_[i]->setModified(false);
				cfgItems_[i]->setDefaulted(false);
			}
		}
	}


	if ( cfgItems_[fieldOffsetCfgEnableProductionPlanning] && !isProductionPlanningLicensed() )
	{
		Debug::trace("Configuration.load", "load() - ProductionPlanning is not licensed, setting fpp to be not enabled");
		cfgItems_[fieldOffsetCfgEnableProductionPlanning]->setValue("0");
		cfgItems_[fieldOffsetCfgEnableProductionPlanning]->setModified(false);
		cfgItems_[fieldOffsetCfgEnableProductionPlanning]->setDefaulted(false);
	}

	cfgGenericLabelText.load(dbAccess);
	cfgSelfService.load(dbAccess);
	cfgEmail.load(dbAccess);
	cfgCommInterface.load(dbAccess);
	cfgCurrency.load(dbAccess);
	cfgMarquee.load(dbAccess);

	updatedPrimaryLocale_ = true;
	updatedSecondaryLocale_ = true;
	setupLanguages();
}


// This method returns true if the value was loaded; false if any errors encountered.
bool Configuration::loadField(DBAccess *dbAccess, const std::string &fieldName, std::string &value)
{
	bool ret = false;
	value = "";

	if (!dbAccess)
		return ret;

	bool isTracing = Debug::isTracing("Configuration.load");

	Util::CLockCriticalSection lock( cfgLoadCS_ );

	DBConfigurationsRM *dbConfigurationsRM = dbAccess->getDBConfigurationsRM();
	if (!dbConfigurationsRM)
	{
		if (isTracing)
			Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString("  could not get DBConfigurationsRM"));
		return ret;
	}	

	try
	{		
		DBConfiguration *cfgRec = dbConfigurationsRM->findName(fieldName);
		if (cfgRec)
		{
			if (cfgRec->isRecordValid())
			{
				if (cfgRec->isValueNull())
				{
					if (isTracing)
						Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" not set because both value fields are null"));
				}
				else
				{
					value = cfgRec->getValue();
					if (isTracing)
						Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" set from value: ") + value);
				}

				ret = true;
			}
			else if (isTracing)
				Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" not set due to db record not valid!!!"));

			delete cfgRec;
			cfgRec = (DBConfiguration *)NULL;
			return ret;
		}
		else if (isTracing)
			Debug::trace("Configuration.load", HobString("loadField() - ") + fieldName + HobString(" not set due to db record not found!!!"));
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Configuration::loadField() - ") + fieldName + HobString(" - ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return ret;
}


void Configuration::updateConfigurationFromUI(const list<CfgFieldOffsets> &cfgList, const string &receivedData)
{
	Debug::trace("Configuration.Update", HobString("updateConfigurationFromUI - receivedData: ") + HobString(receivedData));

	HobString temp(receivedData);
	list<CfgFieldOffsets>::const_iterator fieldIter = cfgList.begin();

	HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	HobString token;

	while ((tok.next(&token) != -1) && (fieldIter != cfgList.end()))
	{
		if (token.size())
			updateField(*fieldIter, token);

		fieldIter++;
	}
}


void Configuration::updateField(int fieldCode, const string &value)
{
	if (fieldCode <= 0 || fieldCode >= INTERNAL_CONFIGURATION_FIRST_FIELDCODE)
		return;

	HobString tempValue = value;
	tempValue.unQuoteForCSV();

	if (fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
	{
		if (traceCfg_)
			Debug::trace("Configuration", "Configuration updateField " + inttostring(fieldCode) + " " + cfgItems_[fieldCode]->getName() + " from: " + cfgItems_[fieldCode]->getValue() + " to: " + tempValue);

		switch (fieldCode)
		{
			//  CfgScale
			case fieldOffsetCfgStoreName:
				setStoreName(tempValue);
				break;
			case fieldOffsetCfgStoreGraphic:
				setStoreGraphicName(tempValue);
				break;
			case fieldOffsetCfgUnitPriceSecurityLabel:
				setSecurityLblUnitPrice(tempValue);
				break;
			case fieldOffsetCfgTotalPriceSecurityLabel:
				setSecurityLblTtlPrice(tempValue);
				break;
			case fieldOffsetCfgTareSetback:
				setTareSetback(tempValue);
				break;
			case fieldOffsetCfgMaxTare:
				setMaxTare(tempValue);
				break;
			case fieldOffsetCfgMinSweetheartWeight:
				setMinSweetheartWeight(tempValue);
				break;
			case fieldOffsetCfgMinWeightToPrintLabel:
				setMinWeightToPrint(tempValue);
				break;
			case fieldOffsetCfgWeighMode:
				//	Conform to Quantum poundsOrMetric: 1 = pounds, 0 = metric
				setScaleWeighMode( (Bool::valueOf(tempValue)) ? WeighMode_Avoir : WeighMode_Metric );
				break;
			case fieldOffsetCfgWeigherModel:
				setWeigherModel(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgIsScaleMetric:
				setScaleWeighMode( (Bool::valueOf(tempValue)) ? WeighMode_Metric : WeighMode_Avoir );
				break;
			case fieldOffsetCfgUserRezeroWeightLimit:
				setUserRezeroWeightLimit(tempValue);
				break;
			case fieldOffsetCfgValueMaxEnabled:
				setValueMaxEnabled(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgIsCodeCheckerEnabled:
				setCodeCheckerEnabled(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgAllowSupervisorDotWear:
				setAllowSupervisorDotWear(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgAllowOperatorDotWear:
				setAllowOperatorDotWear(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgIsCutterEnabled:
				setCutterEnabled(Integer::valueOf(tempValue));
				break;

			//	collectingProductTotals, collectingHourlyTotals, collectingOperatorTotals, (trackingOperatorChanges - but not for Fastrack)
			//	and collectingCOOLTotals are no longer supported individually because of Transactions. To support
			//	legacy database backups, we will set the collectingTransactions flag if the unsupported flag is set.
			//	These unsupported flags will NOT clear the collectingTransactions flag. [CCD APR 2011 - PT 546]
			//
			//  With Fastrack, we are again keeping track of trackingOperatorChanges independently of tracking transactions,
			//  even though both are stored in the transactions database table.
			case fieldOffsetCfgCollectingProductTotals:
			case fieldOffsetCfgCollectingHourlyTotals:
			case fieldOffsetCfgCollectingOperatorTotals:
			//case fieldOffsetCfgTrackingOperatorChanges:   as noted above, I'm commenting this one out for Fastrack
			//case fieldOffsetCfgTrackingSupervisorChanges: as noted above, I'm commenting this one out for Fastrack 
			case fieldOffsetCfgCollectingCOOLTotals:
			{
				bool isSet = Bool::valueOf(tempValue);
				if (isSet)
					setCfg(fieldOffsetCfgCollectingTransactions, tempValue);
				break;
			}
			
			case fieldOffsetCfgDefaultProductRecordType:
				setDefaultProductRecordType(Integer::valueOf(tempValue));
				break;

			case fieldOffsetCfgWrSealerBeltTimeOnSun:
			case fieldOffsetCfgWrSealerBeltTimeOffSun:
			case fieldOffsetCfgWrSealerBeltTimeOnMon:
			case fieldOffsetCfgWrSealerBeltTimeOffMon:
			case fieldOffsetCfgWrSealerBeltTimeOnTue:
			case fieldOffsetCfgWrSealerBeltTimeOffTue:
			case fieldOffsetCfgWrSealerBeltTimeOnWed:
			case fieldOffsetCfgWrSealerBeltTimeOffWed:
			case fieldOffsetCfgWrSealerBeltTimeOnThu:
			case fieldOffsetCfgWrSealerBeltTimeOffThu:
			case fieldOffsetCfgWrSealerBeltTimeOnFri:
			case fieldOffsetCfgWrSealerBeltTimeOffFri:
			case fieldOffsetCfgWrSealerBeltTimeOnSat:
			case fieldOffsetCfgWrSealerBeltTimeOffSat:
				setSealerBeltTime(fieldCode, tempValue);
				break;

			case fieldOffsetCfgRemoteAccessRequiresLogin:
				setCfg(fieldCode, tempValue);
				if (Bool::valueOf(tempValue) && toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) == RemoteAccessLoginMethod_None)
					setCfg(fieldOffsetCfgRemoteAccessLoginMethod, inttostring(RemoteAccessLoginMethod_OperatorID));
				else if (!Bool::valueOf(tempValue) && toIntCfg(fieldOffsetCfgRemoteAccessLoginMethod) != RemoteAccessLoginMethod_None)
					setCfg(fieldOffsetCfgRemoteAccessLoginMethod, inttostring(RemoteAccessLoginMethod_None));
				break;

			case fieldOffsetCfgRemoteAccessLoginMethod:
				setCfg(fieldCode, tempValue);
				if (Integer::valueOf(tempValue) != RemoteAccessLoginMethod_None)
					setCfg(fieldOffsetCfgRemoteAccessRequiresLogin, Bool::toString(true));
				else
					setCfg(fieldOffsetCfgRemoteAccessRequiresLogin, Bool::toString(false));
				break;

			case fieldOffsetCfgPingInterval:
			{
				if ( tempValue.find(":") == string::npos )
				{
					// data is in Quantum format if it doesn't contain a ':'.
					// Quantum format is hhmm, not zero filled, so 1 hr 30 min would come in as '130', 15 min would come in as '15'.
					while (tempValue.length() < 4)
						tempValue.insert(0, "0");
					tempValue.insert(2, ":");
				}
				if ( tempValue.find(":") != string::npos )
				{
					int hours = Integer::valueOf(tempValue);
					int minutes = Integer::valueOf( tempValue.substr(tempValue.find(":")+1) );
					if (0 == hours && 0 == minutes)
						tempValue = toStringCfgI(CfgInternal::DEFAULTPINGINTERVAL);

					setCfg(fieldOffsetCfgPingInterval, tempValue);
				}
				break;
			}

			//  CfgFrequentShopper
			case fieldOffsetCfgFSPerScale:
				if (Bool::valueOf(tempValue) == true)
					setPerScale();
				else
					setPerPlu();
				break;
			case fieldOffsetCfgFSDiscountMethod:
				setDiscountMethod(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgFSRoundingMethod:
				setRoundingMethod(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgFSDiscountType:
				setDefaultDiscountType(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgFSDiscountFormula:
				setDefaultDiscountFormula(Integer::valueOf(tempValue));
				break;


			//  CfgPrintedFields
			case fieldOffsetCfgPFWeightOnByCountPrinted:
				setWeightOnByCountPrinted(Bool::valueOf(tempValue));
				break;
			case fieldOffsetCfgPFOneOnByCountPrinted:
				setOneOnByCountPrinted(Bool::valueOf(tempValue));
				break;


			//  CfgPrepack
			case fieldOffsetCfgPluSearchRange:
				setPluSearchRange(Integer::valueOf(tempValue));
				break;
			case fieldOffsetCfgEnableProductionPlanning:
				if ( isProductionPlanningLicensed() )
					setCfg(fieldOffsetCfgEnableProductionPlanning, tempValue);
				break;

			//	HA-79:  Add the Rotation override settings to Configuration.
			case fieldOffsetCfgRotationOverrideSetting1:
				setRotationOverrideSetting1(value);
				break;
			case fieldOffsetCfgRotationOverrideSetting2:
				setRotationOverrideSetting2(value);
				break;
			case fieldOffsetCfgRotationOverrideSetting3:
				setRotationOverrideSetting3(value);
				break;

			//	HA-79:  Add the Rotation override settings to Configuration.
			case fieldOffsetCfgRotationOverrideValue1:
				setRotationOverrideValue1(value);
				break;
			case fieldOffsetCfgRotationOverrideValue2:
				setRotationOverrideValue2(value);
				break;
			case fieldOffsetCfgRotationOverrideValue3:
				setRotationOverrideValue3(value);
				break;
			case fieldOffsetCfgPrimaryLabelStockRotated:
				Debug::trace("Configuration.Update", HobString("Updating primary pre print: ") + HobString(value));
				setPrimaryLabelPrePrint(value);
				break;
			case fieldOffsetCfgSecondaryLabelStockRotated:
				Debug::trace("Configuration.Update", HobString("Updating secondary pre print: ") + HobString(value));
				setSecondaryLabelPrePrint(value);
				break;
			// CfgGCD
			case fieldOffsetCfgGCDLanguage:
				if ( !tempValue.compare(FRENCHLANGNAME) )
					setCfg(fieldCode, inttostring((int)GcdLanguage_French));
				else if ( !tempValue.compare(SPANISHLANGNAME) )
					setCfg(fieldCode, inttostring((int)GcdLanguage_Spanish));
				else
					setCfg(fieldCode, inttostring((int)GcdLanguage_English));
				break;
/*
			case fieldOffsetCfgGCDProductAdvertType:
				setPluSearchRange(Integer::valueOf(tempValue));
				break;
*/
			case fieldOffsetCfgDatabaseContainsOperators:
				// No one can set this config param.
				break;

			case fieldOffsetCfgExternalWeightPrecision:
				setCfg( fieldOffsetCfgExternalWeightPrecision, ( (MetSig::isScaleMetric()) ? "3" : tempValue ) );
				break;

			case fieldOffsetCfgFSRoundingWeightPrecision:
				setCfg( fieldOffsetCfgFSRoundingWeightPrecision, ( (MetSig::isScaleMetric()) ? "3" : ( (getEffectiveWeightPrecision() == 2) ? "2" : tempValue ) ) );
				break;

			case fieldOffsetCfgFPCVersion:
			case fieldOffsetCfgFPCWeigherVersion:
			case fieldOffsetCfgPrinter1LabelLength:
			case fieldOffsetCfgPrinter2LabelLength:
			case fieldOffsetCfgPrinter3LabelLength:
			case fieldOffsetCfgTareEntryPrecision:   // as of April 2017, no longer configurable, per W&M
				// You can't set these from the outside, i.e., via HTe, comm, backup/restore.
				break;

			case fieldOffsetCfgSNMPTrapsIp:
			case fieldOffsetCfgSNMPTrapsPort:
			case fieldOffsetCfgSNMPVariablesPort:
			case fieldOffsetCfgSNMPEnabled:
			{
				// These four are kept in a separate Properties file.
				// open up a properties file (scaleSettings) and read and write the SNMP values

				Properties *scaleprops = new util_env::Properties(toStringCfgI(fieldOffsetCfgIScaleSettingsFile));
				if (scaleprops)
				{
					std::string keyName = getName(fieldCode);
					if (keyName.size())
					{
						const string *s = scaleprops->getProperty(keyName);
						if (!s || s->compare(tempValue) != 0)
						{
							scaleprops->setProperty(keyName, tempValue);
							scaleprops->save();
						}
					}
					delete scaleprops;
					scaleprops = 0;
				}
				break;
			}

			default:
				setCfg(fieldCode, tempValue);
		}
	}
	else
	{
		if (fieldCode < endCfgSSFieldOffsets)
			cfgSelfService.updateField(fieldCode, tempValue);
		else if (fieldCode < endCfgNetworkFieldOffsets)
			cfgNetwork.updateField(fieldCode, tempValue);
		else if (fieldCode < endCfgEmailFieldOffsets)
			cfgEmail.updateField(fieldCode, tempValue);
		else if (fieldCode < endCfgMarqueeFieldOffsets)
			cfgMarquee.updateField(fieldCode, tempValue);
		else if (fieldCode < endCfgCommFieldOffsets)
			cfgCommInterface.updateField(fieldCode, tempValue);
		else if (fieldCode < endCfgCurrencyFieldOffsets)
			cfgCurrency.updateField(fieldCode, tempValue);
	}
}


bool Configuration::isSaveNeeded(const list<CfgFieldOffsets> &cfgList) const
{
	bool saveNeeded = false;

	list<CfgFieldOffsets>::const_iterator fieldIter = cfgList.begin();
	while (!saveNeeded && fieldIter != cfgList.end())
	{
		if (isModifiedCfg(*fieldIter))
			saveNeeded = true;
		fieldIter++;
	}

	return saveNeeded;
}


void Configuration::save(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	Util::CLockCriticalSection lock( cfgSaveCS_ );

	if (cfgItems_[fieldOffsetCfgPrimaryCountryCode] && cfgItems_[fieldOffsetCfgPrimaryCountryCode]->isModified())
		updatedPrimaryLocale_ =  true;
	if (cfgItems_[fieldOffsetCfgPrimaryLanguageCode] && cfgItems_[fieldOffsetCfgPrimaryLanguageCode]->isModified())
		updatedPrimaryLocale_ =  true;

	if (cfgItems_[fieldOffsetCfgSecondaryCountryCode] && cfgItems_[fieldOffsetCfgSecondaryCountryCode]->isModified())
		updatedSecondaryLocale_ =  true;
	if (cfgItems_[fieldOffsetCfgSecondaryLanguageCode] && cfgItems_[fieldOffsetCfgSecondaryLanguageCode]->isModified())
		updatedSecondaryLocale_ =  true;

	int end = (int)cfgItems_.size();
	for (int i = 0; i < end; i++)
	{
		if (cfgItems_[i] && cfgItems_[i]->isModified() && cfgItems_[i]->isPersistent())
		{
			saveField(dbAccess, cfgItems_[i]->getName(), cfgItems_[i]->getValue());
			cfgItems_[i]->setModified(false);
			cfgItems_[i]->setDefaulted(false);
		}
	}


	cfgSelfService.save(dbAccess);
	cfgNetwork.save(dbAccess);
	cfgEmail.save(dbAccess);
	cfgCommInterface.save(dbAccess);
	cfgCurrency.save(dbAccess);
	cfgMarquee.save(dbAccess);
	//genericLabelText.save(dbAccess);
	//cfgInternal.save();
	
	sendRebootMessage();

	setupLanguages();
}

void Configuration::setupLanguages()
{
	if (updatedPrimaryLocale_)
	{
		writeSystemLangFile();

		updatedPrimaryLocale_ = false;

		hfc::MultiLanguageText::updateLanguageFiles();

		edb::DBManager::getInstance()->setDatabaseLanguage(PrimaryLanguage, toStringCfg(fieldOffsetCfgPrimaryLanguageCode) + "_" + toStringCfg(fieldOffsetCfgPrimaryCountryCode));

		// FT-1095
		// fix crash during upgrade, no session yet...
		if (SessionManager::getInstance()->getPrimaryConsoleSession())
			SessionManager::getInstance()->getPrimaryConsoleSession()->modifiedLanguageNumber(PrimaryLanguage);
	}

	if (updatedSecondaryLocale_)
	{
		updatedSecondaryLocale_ = false;

		hfc::MultiLanguageText::updateLanguageFiles();

		edb::DBManager::getInstance()->setDatabaseLanguage(SecondaryLanguage, toStringCfg(fieldOffsetCfgSecondaryLanguageCode) + "_" + toStringCfg(fieldOffsetCfgSecondaryCountryCode));

		// FT-1095
		// fix crash during upgrade, no session yet...
		if (SessionManager::getInstance()->getPrimaryConsoleSession())
			SessionManager::getInstance()->getPrimaryConsoleSession()->modifiedLanguageNumber(SecondaryLanguage);
	}
}

void Configuration::sendRebootMessage()
{
	Configuration* cfg = Configuration::getInstance();

	Session* primaryConsoleSession = SessionManager::getInstance()->getPrimaryConsoleSession();

	// We have a new FS UI scale that use a very specific set of configurations. 
	// If the FS UI is enabled, then we cant allow changes to self serv configs
	bool tester = (cfg->toBoolCfg(fieldOffsetCfgSelfServeEnableFreshServe));

	if (isSelfServiceScale())
		return;

	if (primaryConsoleSession && cfg && (cfg->toBoolCfg(fieldOffsetCfgSelfServeEnableFreshServe)))
	{
		Debug::trace("Configuration.CfgSelfService", "save() COMM or Restore: FS UI in effect so set default values");

		if ((0 != cfg->toIntCfg(fieldOffsetCfgSelfServeStartState)) ||
			(0 != cfg->toIntCfg(fieldOffsetCfgSelfServeSearchState)) ||
			(3 != cfg->toIntCfg(fieldOffsetCfgSelfServeItemEntryState)) ||
			(true == cfg->toBoolCfg(fieldOffsetCfgDisplaySelfServeStartScreen)) ||
			(false == cfg->toBoolCfg(fieldOffsetCfgSelfServeWeightRequiredToSelectItem)) ||
			(true == cfg->toBoolCfg(fieldOffsetCfgSelfServeEnablePlaylists)) ||
			(true == cfg->toBoolCfg(fieldOffsetCfgBestSellersOnSelfServeMainScreen)) ||
			(false == cfg->toBoolCfg(fieldOffsetCfgEnableUseCharacterSearching)) ||
			(false == cfg->toBoolCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel)) ||
			(0 != cfg->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors)) ||
			(false == cfg->toBoolCfg(fieldOffsetCfgSelfServeStartScreenClickable)) ||
			(false == cfg->toBoolCfg(fieldOffsetCfgSelfServeIncludeKeypad)) ||
			(0 != cfg->toIntCfg(fieldOffsetCfgSelfServeFixedPlu)) ||
			(false == cfg->toBoolCfg(fieldOffsetCfgSelfServeByCountItemsAllowed)) ||
			(true == cfg->toBoolCfg(fieldOffsetCfgSelfServeDisplayTareOnRW)) ||
			(true == cfg->toBoolCfg(fieldOffsetCfgEnableHelpPrompts)) ||
			(true == cfg->toBoolCfg(fieldOffsetCfgSelfServeHideMenuBar)))
		{
			if (0 != cfg->toIntCfg(fieldOffsetCfgSelfServeStartState))
				Debug::trace("Configuration.CfgSelfService", "SelfServeStartState != 0 so set to 0 and REBOOT");
			if (0 != cfg->toIntCfg(fieldOffsetCfgSelfServeSearchState))
				Debug::trace("Configuration.CfgSelfService", "SelfServeSearchState != 0 so set to 0 and REBOOT");
			if (3 != cfg->toIntCfg(fieldOffsetCfgSelfServeItemEntryState))
				Debug::trace("Configuration.CfgSelfService", "SelfServeItemEntryState != 3 so set to 3 and REBOOT");
			if (true == cfg->toBoolCfg(fieldOffsetCfgDisplaySelfServeStartScreen))
				Debug::trace("Configuration.CfgSelfService", "DisplaySelfServeStartScreene == true so set to false and REBOOT");
			if (false == cfg->toBoolCfg(fieldOffsetCfgSelfServeWeightRequiredToSelectItem))
				Debug::trace("Configuration.CfgSelfService", "SelfServeWeightRequired == false so set to true and REBOOT");
			if (true == cfg->toBoolCfg(fieldOffsetCfgSelfServeEnablePlaylists))
				Debug::trace("Configuration.CfgSelfService", "SelfServeEnablePlaylists == true so set to false and REBOOT");
			if (true == cfg->toBoolCfg(fieldOffsetCfgBestSellersOnSelfServeMainScreen))
				Debug::trace("Configuration.CfgSelfService", "BestSellersOnSelfServeMainScreen == true so set to false and REBOOT");
			if (false == cfg->toBoolCfg(fieldOffsetCfgEnableUseCharacterSearching))
				Debug::trace("Configuration.CfgSelfService", "EnableUseCharacterSearching == false so set to true and REBOOT");
			if (false == cfg->toBoolCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel))
				Debug::trace("Configuration.CfgSelfService", "SelfServeDisplayBarcodeLabel == false so set to true and REBOOT");
			if (0 != cfg->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors))
				Debug::trace("Configuration.CfgSelfService", "SelfServeIgnorePrinterErrors != 0 so set to 0 and REBOOT");
			if (false == cfg->toBoolCfg(fieldOffsetCfgSelfServeStartScreenClickable))
				Debug::trace("Configuration.CfgSelfService", "SelfServeStartScreenClickable == false so set to true and REBOOT");
			if (false == cfg->toBoolCfg(fieldOffsetCfgSelfServeIncludeKeypad))
				Debug::trace("Configuration.CfgSelfService", "SelfServeIncludeKeypad == false so set to true and REBOOT");
			if (0 != cfg->toIntCfg(fieldOffsetCfgSelfServeFixedPlu))
				Debug::trace("Configuration.CfgSelfService", "SelfServeFixedPlu != 0 so set to 0 and REBOOT");
			if (false == cfg->toBoolCfg(fieldOffsetCfgSelfServeByCountItemsAllowed))
				Debug::trace("Configuration.CfgSelfService", "SelfServeByCountItemsAllowed == false so set to true and REBOOT");
			if (true == cfg->toBoolCfg(fieldOffsetCfgSelfServeDisplayTareOnRW))
				Debug::trace("Configuration.CfgSelfService", "SelfServeDisplayTareOnRW == true so set to false and REBOOT");
			if (true == cfg->toBoolCfg(fieldOffsetCfgEnableHelpPrompts))
				Debug::trace("Configuration.CfgSelfService", "EnableHelpPrompts == true so set to false and REBOOT");
			if (true == cfg->toBoolCfg(fieldOffsetCfgSelfServeHideMenuBar))
				Debug::trace("Configuration.CfgSelfService", "SelfServeHideMenuBar == true so set to false and REBOOT");

			cfg->setCfg(fieldOffsetCfgSelfServeStartState, "0");							// int  0-
			cfg->setCfg(fieldOffsetCfgSelfServeSearchState, "0");							// int  0-
			cfg->setCfg(fieldOffsetCfgSelfServeItemEntryState, "3");						// int  0-
			cfg->setCfg(fieldOffsetCfgDisplaySelfServeStartScreen, "0");					// bool 1-
			cfg->setCfg(fieldOffsetCfgSelfServeWeightRequiredToSelectItem, "1");			// bool 0
			cfg->setCfg(fieldOffsetCfgSelfServeEnablePlaylists, "0");						// bool 0				
			cfg->setCfg(fieldOffsetCfgBestSellersOnSelfServeMainScreen, "0");				// bool 0			
			cfg->setCfg(fieldOffsetCfgEnableUseCharacterSearching, "1");					// bool 0
			cfg->setCfg(fieldOffsetCfgSelfServeDisplayBarcodeLabel, "1");					// bool 1
			cfg->setCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors, "0");					// int  0
			cfg->setCfg(fieldOffsetCfgSelfServeStartScreenClickable, "1");					// bool 0
			cfg->setCfg(fieldOffsetCfgSelfServeIncludeKeypad, "1");							// bool 1
			cfg->setCfg(fieldOffsetCfgSelfServeFixedPlu, "0");								// int  0
			cfg->setCfg(fieldOffsetCfgSelfServeByCountItemsAllowed, "1");					// bool 1
			cfg->setCfg(fieldOffsetCfgSelfServeDisplayTareOnRW, "0");						// bool 0
			cfg->setCfg(fieldOffsetCfgEnableHelpPrompts, "0");								// bool 0
			cfg->setCfg(fieldOffsetCfgSelfServeHideMenuBar, "0");							// bool 0

			Debug::trace("Configuration.CfgSelfService", "save() COMM or Restore: FS UI Configs RESET");

			primaryConsoleSession->addDialogMessage(ErrBackupRestore, BackupRestoreSelfServeModified, HfcResponse::RESPONSESTYLE_REBOOT_CANCEL);			// 109022
		}
	}
}

// New for Fastrack, the ability for the application to save an individual configuration parameter, as long as it is in the cfgItems_ vector
void Configuration::saveField(int fieldCode, DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	if (fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode] && cfgItems_[fieldCode]->isModified() && cfgItems_[fieldCode]->isPersistent())
	{
		if (traceCfg_)
			Debug::trace("Configuration", "Configuration saveField " + inttostring(fieldCode) + " " + cfgItems_[fieldCode]->getName());

		Util::CLockCriticalSection lock( cfgSaveCS_ );

		saveField(dbAccess, cfgItems_[fieldCode]->getName(), cfgItems_[fieldCode]->getValue());
		cfgItems_[fieldCode]->setModified(false);
		cfgItems_[fieldCode]->setDefaulted(false);
	}
}


void Configuration::saveField(DBAccess *dbAccess, const std::string &fieldName, const std::string &value, bool /* batchIt */)
{
	if (!dbAccess)
		return;

	Util::CLockCriticalSection lock( cfgSaveCS_ );

	bool isTracing = Debug::isTracing("Configuration.save");

	DBConfigurationsRM *dbConfigurationsRM = dbAccess->getDBConfigurationsRM();
	if (!dbConfigurationsRM)
	{
		if (isTracing)
			Debug::trace("Configuration.save", HobString("saveField() - ") + fieldName + HobString("  could not get DBConfigurationsRM"));
		return;
	}	

	try
	{		
		DBConfiguration *cfgRec = dbConfigurationsRM->findName(fieldName);
		if (!cfgRec)
		{
			if (isTracing)
				Debug::trace("Configuration.save", HobString("saveField() creating record - ") + fieldName);
			cfgRec = dbConfigurationsRM->createRecord();
			if (cfgRec)
				cfgRec->setName(fieldName);
		}			
		if (cfgRec)
		{
			if (isTracing)
				Debug::trace("Configuration.save", HobString("saveField() - ") + fieldName + HobString(" set to value: ") + value);
			cfgRec->setValue(value);

			dbConfigurationsRM->saveRecord(cfgRec);
			delete cfgRec;
			cfgRec = (DBConfiguration *)NULL;
		}
		else if (isTracing)
			Debug::trace("Configuration.save", HobString("saveField() - ") + fieldName + HobString(" not saved due to db record not found and not created!!!"));
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Configuration::saveField() - ") + fieldName + HobString(" - ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
}


void Configuration::deleteField(DBAccess *dbAccess, const std::string &fieldName)
{
	if (!dbAccess)
		return;

	Util::CLockCriticalSection lock( cfgSaveCS_ );

	bool isTracing = Debug::isTracing("Configuration.delete");

	DBConfigurationsRM *dbConfigurationsRM = dbAccess->getDBConfigurationsRM();
	if (!dbConfigurationsRM)
	{
		if (isTracing)
			Debug::trace("Configuration.delete", HobString("deleteField() - ") + fieldName + HobString("  could not get DBConfigurationsRM"));
		return;
	}	

	try
	{		
		DBConfiguration *cfgRec = dbConfigurationsRM->findName(fieldName);
		if (cfgRec)
		{
			dbConfigurationsRM->deleteRecord(cfgRec);
			delete cfgRec;
			cfgRec = (DBConfiguration *)NULL;
		}
		else if (isTracing)
			Debug::trace("Configuration.delete", HobString("deleteField() record/field not found in database - ") + fieldName);
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Configuration::deleteField() - ") + fieldName + HobString(" - ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
}


void Configuration::writeSystemLangFile()
{
	util_filesystem::Filename langFilename;
	langFilename.setFullPath(toStringCfgI(CfgInternal::LANGFILENAME));

	std::string priLang = toStringCfg(fieldOffsetCfgPrimaryLanguageCode) + "." + toStringCfg(fieldOffsetCfgPrimaryCountryCode);
	util_string::toupper(priLang);

	std::ostream *outFile = new std::ofstream(langFilename.getFullPath().c_str(), std::ios_base::trunc);
	*outFile << priLang.c_str() << std::endl;
    outFile->flush();
    delete outFile;
}


bool Configuration::weigherConfigExistsInDB(DBAccess *dbAccess)
{
	HobString dbVal;

	//	Other fields we could check for are "scaleWeighMode", "netWeightDualRangeLimitCheck", "weigherModel"
    bool result = loadField(dbAccess, "minWeightToPrint", dbVal);

	return result;
}

void Configuration::deleteWeigherConfigFromDB(DBAccess *dbAccess)
{
	// The following line was commented out to make the scaleWeighMode available to HTe-ScaleManagement
	//deleteField(dbAccess, "scaleWeighMode");
	deleteField(dbAccess, "minWeightToPrint");
	deleteField(dbAccess, "weigherModel");
}


//METSIG?? YES, but this is still used so HTe can have access to it
void Configuration::setScaleWeighMode(WeighMode mode)
{
	setCfg(fieldOffsetCfgWeighMode, inttostring((int)mode));
	MetSig::setScaleWeighMode(mode);
}


//METSIG?? NO
UINT32 Configuration::getEffectiveWeigherScaleFactor() const
{
	Session *primaryConsoleSession = SessionManager::getInstance()->getPrimaryConsoleSession();
	HfcWeigherStatus *weigherStatus = (primaryConsoleSession) ? primaryConsoleSession->getCurrentWeight() : 0;
	UINT32 effectiveScaleFactor = (weigherStatus) ? weigherStatus->getEffectiveScaleFactor() : 0;
	return effectiveScaleFactor;
}

//METSIG?? NO
UINT32 Configuration::getWeigherScaleFactor() const
{
	Session *primaryConsoleSession = SessionManager::getInstance()->getPrimaryConsoleSession();
	ConfigurationManager *configurationManager = (primaryConsoleSession) ? primaryConsoleSession->getConfigurationManager() : 0;
	HfcWeigherConfig *weigherConfig = (configurationManager) ? configurationManager->getCurrentWeigherConfig() : 0;
	UINT32 scaleFactor = (weigherConfig) ? weigherConfig->getScaleFactor() : 0;
	return scaleFactor;
}


// SOF-789: Wrap-only
bool Configuration::isScaleModelWrapOnly() const
{
	if (FpmManager::getInstance() && FpmManager::getInstance()->readMfgScaleData())
	{
		HobString style = FpmManager::getInstance()->readMfgScaleData()->getScaleStyle();
		style.unQuote();
		style.toUpper();
		return (!style.empty() && style.substr(0, 3) == "NGS") ? true : false;
	}
	return false;
}

bool Configuration::isWrapper() const
{
	if ( FpmManager::getInstance() )
	{
		std::string s = FpmManager::getInstance()->getMFGSystemStyle();
		if ( (s == "NGW1") || (s == "NGW2") || (s == "NGW3") || (s == "AWS3") || (s == "EPCP") )
			return true;
	}
	return false;
}


std::string Configuration::toStringCfg(int fieldCode) const
{
	if (fieldCode > 0 && fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
	{
		if ( fieldOffsetCfgSNMPTrapsIp       == fieldCode || fieldOffsetCfgSNMPTrapsPort == fieldCode || 
		     fieldOffsetCfgSNMPVariablesPort == fieldCode || fieldOffsetCfgSNMPEnabled   == fieldCode )
		{
			// These four are kept in a separate Properties file.
			string keyValue("");
			Properties *scaleprops = new util_env::Properties(toStringCfgI(fieldOffsetCfgIScaleSettingsFile));
			if (scaleprops)
			{
				std::string keyName = getName(fieldCode);
				if (keyName.size())
				{
					const string *keyValuePtr = scaleprops->getProperty(keyName);
					if (keyValuePtr)
						keyValue.assign(*keyValuePtr);
				}
				delete scaleprops;
				scaleprops = 0;
			}

			return keyValue;
		}
		else if ( fieldOffsetCfgScaleModel == fieldCode )
		{
			if ( FpmManager::getInstance() && FpmManager::getInstance()->readMfgScaleData() )
			{
				HobString style = FpmManager::getInstance()->readMfgScaleData()->getScaleStyle();
				style.unQuote();
				cfgItems_[fieldCode]->setValue( style );
			}
		}

		if (traceCfg_)
			Debug::trace("Configuration", "Configuration reading " + inttostring(fieldCode) + " " + cfgItems_[fieldCode]->getName() + ": string " + cfgItems_[fieldCode]->toString());

		return cfgItems_[fieldCode]->toString();
	}
	else
		return "";
}


int Configuration::toIntCfg(int fieldCode) const
{
	if (fieldCode > 0 && fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
	{
		if (traceCfg_)
			Debug::trace("Configuration", "Configuration reading " + inttostring(fieldCode) + " " + cfgItems_[fieldCode]->getName() + ": int " + inttostring(cfgItems_[fieldCode]->toInt()));
		return cfgItems_[fieldCode]->toInt();
	}
	else
		return 0;
}


bool Configuration::toBoolCfg(int fieldCode) const
{
	if (fieldCode > 0 && fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
	{
		if (traceCfg_)
			Debug::trace("Configuration", "Configuration reading " + inttostring(fieldCode) + " " + cfgItems_[fieldCode]->getName() + ": bool " + booltostring(cfgItems_[fieldCode]->toBool()));
		return cfgItems_[fieldCode]->toBool();
	}
	else
		return false;
}


std::string Configuration::getName(int fieldCode) const
{
	if (fieldCode > 0 && fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
		return cfgItems_[fieldCode]->getName();
	else if (fieldCode >= INTERNAL_CONFIGURATION_FIRST_FIELDCODE)
		return cfgInternal.getName(fieldCode);
	else
		return "";
}


void Configuration::setCfg(int fieldCode, const string &value)
{
	if (fieldCode > 0 && fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
	{
		if (traceCfg_)
			Debug::trace("Configuration", "Configuration setting " + inttostring(fieldCode) + " " + cfgItems_[fieldCode]->getName() + ": setCfg using string: " + value);
		cfgItems_[fieldCode]->setValue(value);
	}
}


bool Configuration::isModifiedCfg(int fieldCode) const
{
	if (fieldCode > 0 && fieldCode < (int)cfgItems_.size() && cfgItems_[fieldCode])
		return cfgItems_[fieldCode]->isModified();
	else
		return false;
}


void Configuration::checkTracingCfg()
{
	traceCfg_ = Debug::isTracing("Configuration");
}

// HA-73: Coordinate changes in Tare entry precision with tareSetback & maxTare.
// HT-176: assumed three decimal digits
void Configuration::setTareSetback( std::string tare )
{
	// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating the tare.
	if (2 == getExternalWeightPrecision())
		tare.append("0");

	HobWeight newWeight = MetSig::determineImplicitDecimalWeight(tare);

	Debug::trace("Configuration.formatWeight", "setTareSetback(" + tare + "):  Set to " + Long::toString(newWeight));
	setCfg( fieldOffsetCfgTareSetback, Long::toString(newWeight) );
}

// HA-73: Coordinate changes in Tare entry precision with tareSetback & maxTare.
// HT-176: assumed three decimal digits
void Configuration::setMaxTare( std::string tare )
{
	// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating the tare.
	if (2 == getExternalWeightPrecision())
		tare.append("0");

	HobWeight newWeight = MetSig::determineImplicitDecimalWeight(tare);

	// Per Kraig Wooddell - W&M want max tare maxed at 9.995 (PT # 212).
	if (9995 < newWeight)
	{
		newWeight = 9995;
		Debug::trace("Configuration.formatWeight", "setMaxTare(" + tare + "):  Can't be greater than 9995");
	}

	Debug::trace("Configuration.formatWeight", "setTareSetback(" + tare + "):  Set to " + Long::toString(newWeight));
	setCfg( fieldOffsetCfgMaxTare, Long::toString( (HobWeight) newWeight ) );
}

void Configuration::setMinSweetheartWeight( std::string weight )
{
	// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating.
	if (2 == getExternalWeightPrecision())
		weight.append("0");

	HobWeight newWeight = MetSig::determineImplicitDecimalWeight(weight);

	Debug::trace("Configuration.formatWeight", "setMinSweetheartWeight(" + weight + "):  Set to " + Long::toString(newWeight));
	setCfg( fieldOffsetCfgMinSweetheartWeight, Long::toString(newWeight) );
}

void Configuration::validateTareEntryPrecision()
{
	// As of April 2017, we have to set the tare entry precision to the same precision as the scale's capability.
	// Thus for metric or dual-range scale, = 3.  For Avoir and single-range, = 2.
	short tarePrecision = getEffectiveWeightPrecision();
	// For the next line, to set the value in the configuration, don't call updateField(); that method doesn't allow setting of fieldOffsetCfgTareEntryPrecision.
	setCfg(fieldOffsetCfgTareEntryPrecision, inttostring(tarePrecision));

/* The code before the above standard was imposed ...
	short tarePrecision = (short) toIntCfg(fieldOffsetCfgTareEntryPrecision);
	bool validPrecision = false;

	if ( tarePrecision == 3 && getEffectiveWeightPrecision() == 3 ) // Metric or Dual-Range-Avoir configured Weighers
	{
		Debug::trace("Configuration", "tarePrecision is 3 and is valid.");
		validPrecision = true;
	}
	else if ( tarePrecision == 2 && isScaleAvoir() )
	{
		Debug::trace("Configuration", "tarePrecision is 2 and is valid.");
		validPrecision = true;
	}
	if (!validPrecision)
	{
		if (tarePrecision == 3)
		{
			Debug::trace("Configuration", "Changing tarePrecision to 2.");
			updateField(fieldOffsetCfgTareEntryPrecision, "2");
		}
		else
		{
			Debug::trace("Configuration", "Changing tarePrecision to 3.");
			updateField(fieldOffsetCfgTareEntryPrecision, "3");
		}
	}
*/
}


string Configuration::buildPrintToScreenBarcodeFilename(int filenameCounter) const
{
	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return "";
	std::string filenameString = cfg->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeDirname) +
								cfg->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeFilename) +
								inttostring(filenameCounter) +
								cfg->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeExtension);
	return filenameString;
}


bool Configuration::isIgnoreVendorAndProductNumTagsWhenUpcNumTag() const
{
	return ( toBoolCfgI(fieldOffsetCfgIIgnoreVendorAndProductNumTagsWhenUpcNumTag) || toBoolCfg(fieldOffsetCfgIgnoreVendorAndProductNumTagsWhenUpcNumTag) );
}

void Configuration::show() //const
{
	if (!Debug::isTracing("Configuration.show"))
		return;

	int end = (int)cfgItems_.size();
	for (int i = 0; i < end; i++)
	{
		if (cfgItems_[i])
			Debug::trace("Configuration.show", "(" + inttostring(cfgItems_[i]->getFieldOffsetNumber()) + ") " +
			                                   ((cfgItems_[i]->getType() == CfgItem::STRING) ? "S " : ((cfgItems_[i]->getType() == CfgItem::INT) ? "I " : ((cfgItems_[i]->getType() == CfgItem::BOOL) ? "B " : "? "))) +
											   ((cfgItems_[i]->isDefaulted()) ? "D " : "  ") +
											   ((cfgItems_[i]->isModified()) ? "M " : "  ") +
											   (((cfgItems_[i]->getCommTag()).size()) ? cfgItems_[i]->getCommTag() : "   ") +
			                                   cfgItems_[i]->getName() + ": " + cfgItems_[i]->getValue());
	}


	cfgGenericLabelText.show();
	cfgSelfService.show();
	cfgNetwork.show();
	cfgInternal.show();
	cfgEmail.show();
	cfgCommInterface.show();
	cfgCurrency.show();
	cfgMarquee.show();
}



/*****************************************************************************
 *
 * This method is for stand-alone testing of the format methods.
 *
 ****************************************************************************/
void Configuration::formatDriver()
{
	HobString h;

	{
		HobPrice p = 0;
//		formatPrice(p, h);
		h = formatHobPrice(p);
		Debug::trace("Configuration.format", HobString("price format:  0:  ") + h);
		p = 1;
//		formatPrice(p, h);
		h = formatHobPrice(p);
		Debug::trace("Configuration.format", HobString("price format:  1:  ") + h);
		p = 10;
//		formatPrice(p, h);
		h = formatHobPrice(p);
		Debug::trace("Configuration.format", HobString("price format:  10:  ") + h);
		p = 100;
//		formatPrice(p, h);
		h = formatHobPrice(p);
		Debug::trace("Configuration.format", HobString("price format:  100:  ") + h);
		p = 1000;
//		formatPrice(p, h);
		h = formatHobPrice(p);
		Debug::trace("Configuration.format", HobString("price format:  1000:  ") + h);
	}

	{
		HobWeight p = 0;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  0:  ") + h);
		p = 1;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  1:  ") + h);
		p = 10;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  10:  ") + h);
		p = 100;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  100:  ") + h);
		p = 1000;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  1000:  ") + h);
		p = 10000;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  10000:  ") + h);
	}

	{
		HobWeight p = 0;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  0:  ") + h);
		p = 1;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  1:  ") + h);
		p = 10;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  10:  ") + h);
		p = 100;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  100:  ") + h);
		p = 1000;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  1000:  ") + h);
		p = 10000;
		formatWeight(p, h);
		Debug::trace("Configuration.format", HobString("weight format:  10000:  ") + h);
	}

	{
		Debug::trace("Configuration.format", "date time test");
		HobDateTime hdt;
		hdt.show();
		formatDate(hdt, h);
		Debug::trace("Configuration.format", HobString("date format:  ") + h);

		setTimeDisplayFormat(HourFormat_AMPM);
		formatTime(hdt, h);
		Debug::trace("Configuration.format", HobString("time format 12 hour:  ") + h);
		setTimeDisplayFormat(HourFormat_MILITARY);
		formatTime(hdt, h);
		Debug::trace("Configuration.format", HobString("time format 24 hour:  ") + h);
		formatDateTime(hdt, h);

		Debug::trace("Configuration.format", HobString("datetime format:  ") + h);
	}
}

bool Configuration::isGCDCategoryPromotion() const
{
	Debug::trace("ProductManager.GCD", "Configuration::isGCDCategoryPromotion()");
	if ( !isPrepackScale() )
	{
		int productAdvertType = toIntCfg( fieldOffsetCfgGCDProductAdvertType );
		Debug::trace("ProductManager.GCD", "Configuration::isGCDCategoryPromotion():  productAdvertType = " + inttostring(productAdvertType));
		if (productAdvertType == 5)
		{
			Debug::trace("ProductManager.GCD", "Configuration::isGCDCategoryPromotion():  Returning true.");
			return true;
		}
	}
	Debug::trace("ProductManager.GCD", "Configuration::isGCDCategoryPromotion():  Returning false.");
	return false;
}


void Configuration::testCfgI()
{
	try
	{
		string as = Configuration::getInstance()->toStringCfgI(CfgInternal::MONITORMEMORYINTERVAL);
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		string as = Configuration::getInstance()->toStringCfgI("asdf");
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		int as = Configuration::getInstance()->toIntCfgI(CfgInternal::MONITORMEMORYINTERVAL);
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		int as = Configuration::getInstance()->toIntCfgI("asdf");
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		bool as = Configuration::getInstance()->toBoolCfgI(CfgInternal::MONITORMEMORYINTERVAL);
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		bool as = Configuration::getInstance()->toBoolCfgI("asdf");
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		string as = Configuration::getInstance()->toStringCfgI(42);
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		int as = Configuration::getInstance()->toIntCfgI(42);
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
	try
	{
		bool as = Configuration::getInstance()->toBoolCfgI(42);
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
		
	string as = Configuration::getInstance()->toStringCfgI(CfgInternal::SERVICELOGINID);
	int ai = Configuration::getInstance()->toIntCfgI(CfgInternal::SERVICELOGINID);
	bool ab = Configuration::getInstance()->toBoolCfgI(CfgInternal::SERVICELOGINID);

	string bas = Configuration::getInstance()->toStringCfgI(CfgInternal::SCALESERVICENAME);
	int bai = Configuration::getInstance()->toIntCfgI(CfgInternal::SCALESERVICENAME);
	bool bab = Configuration::getInstance()->toBoolCfgI(CfgInternal::SCALESERVICENAME);

	string cas = Configuration::getInstance()->toStringCfgI("scaleServiceName");
	int cai = Configuration::getInstance()->toIntCfgI("scaleServiceName");
	bool cab = Configuration::getInstance()->toBoolCfgI("scaleServiceName");

	string das = Configuration::getInstance()->toStringCfgI(CfgInternal::TIMERMANAGERENABLED);
	int dai = Configuration::getInstance()->toIntCfgI(CfgInternal::TIMERMANAGERENABLED);
	bool dab = Configuration::getInstance()->toBoolCfgI(CfgInternal::TIMERMANAGERENABLED);

	string eas = Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIDbLocation);
	int eai = Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIDbLocation);
	bool eab = Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIDbLocation);

	string fas = Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIDimBLength3_500);
	int fai = Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIDimBLength3_500);
	bool fab = Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIDimBLength3_500);

	string gas = Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIRealTimeControllerEnabled);
	int gai = Configuration::getInstance()->toIntCfgI(fieldOffsetCfgIRealTimeControllerEnabled);
	bool gab = Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIRealTimeControllerEnabled);

}
