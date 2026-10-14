/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductUpdateValuesMsg.h"
#include "HobTokenizerCSV.h"
#include "Configuration.h"
#include "HfcWeigherStatus.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductUpdateValuesMsg::dataType = 0;
std::string HfcProductUpdateValuesMsg::dataTypeName("HfcProductUpdateValuesMsg"); 
#endif


const std::string HfcProductUpdateValuesMsg::_invalidTotalPriceDisplay_ = "-----";

HfcProductUpdateValuesMsg::HfcProductUpdateValuesMsg(const ProductTransaction *pProductTransaction)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	myProductTransaction = pProductTransaction;
}

HfcProductUpdateValuesMsg::HfcProductUpdateValuesMsg(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	/* This message is used to send updates of the total price, total tare, and
	 * total net weight to the Browser. It can also receive a changed Total price from
	 * the Browser when the TP is changed on a ByCount ,FixedWt or FluidOz PLU.
	 * Since we could get many stable weights
	 * before the label is printed, in Manual Weigh Mode, we wanted to have a
	 * smaller message to send to the Browser (instead of including this data
	 * with Product data.
	 */
	switch (formatType)
	{
		case BrowserFormat:
//			if (myProductTransaction)
			{
				HobString temp(data, len);
				HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
				HobString token;
				short loopCnt = 1;
				while (tok.next(&token) != -1)
				{
					//setReceived(loopCnt, (token.length() == 0) ? false : true);
					setReceived(loopCnt, true);
						
					switch (loopCnt)
					{
						case 4:
						{
							token.unQuoteForCSV();
							if (!token.empty())
							{
								std::string::size_type pos = token.find(Configuration::getInstance()->getMonetarySymbol());
								if (pos == 0)
									token = token.substr(Configuration::getInstance()->getMonetarySymbol().size());
							}
							totalPrice = MetSig::determineImplicitDecimalPrice(token);
							Debug::trace("ProductManager", HobString("HfcProductUpdateValuesMsg constructor  totalPrice is ") + token + HobString("  ") + util_string::doubletostring(totalPrice));
						}
							break;
						default:	//	We don't want fields 1-3 - only the Total price field.
							break;
					}
					loopCnt++;
				}
			}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductUpdateValuesMsg::~HfcProductUpdateValuesMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void HfcProductUpdateValuesMsg::init()
{
	myProductTransaction = (ProductTransaction *)NULL;
	totalPrice = (HobPrice)0;

	totalTareDisplay = "";
	netWeightDisplay = "";
	unitPriceDisplay = "";
	totalPriceDisplay = "";
	fsUnitPriceDisplay = "";
	fsTotalPriceDisplay = "";
	fsTotalSavingsDisplay = "";
	dualRangeIndDisplay = "";
	poundsForDisplay = "";
	fractionalPricingForDisplay = "";

	tareWeight = 0;

	zeroIndicator = false;
}


const ProductTransaction *HfcProductUpdateValuesMsg::getProductTransaction() const
{
	return myProductTransaction;
}


HobPrice HfcProductUpdateValuesMsg::getTotalPrice() const
{
	return totalPrice;
}


const HobString &HfcProductUpdateValuesMsg::gettotalTareDisplay() const
{
	return totalTareDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getNetWeightDisplay() const
{
	return netWeightDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getUnitPriceDisplay() const
{
	return unitPriceDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getTotalPriceDisplay() const
{
	return totalPriceDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getFsUnitPriceDisplay() const
{
	return fsUnitPriceDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getFsTotalPriceDisplay() const
{
	return fsTotalPriceDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getFsTotalSavingsDisplay() const
{
	return fsTotalSavingsDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getPoundsForDisplay() const
{
	Debug::trace("Stupid", HobString("HPUVMsg::getPoundsForDisplay():  poundsForDisplay = ") + poundsForDisplay);
	return poundsForDisplay;
}
const HobString &HfcProductUpdateValuesMsg::getFractionalPricingForDisplay() const
{
	Debug::trace("Stupid", HobString("HPUVMsg::getFractionalPricingForDisplay():  fractionalPricingForDisplay = ") + fractionalPricingForDisplay);
	return fractionalPricingForDisplay;
}

bool HfcProductUpdateValuesMsg::isZeroIndicator() const
{
	return zeroIndicator;
}


void HfcProductUpdateValuesMsg::determineDisplayValues(int format) const
{
	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return;

	bool useDefaultDecimalSeparator = (format == BrowserFormat) ? true : false;

	totalTareDisplay = "";
	netWeightDisplay = "";

	unitPriceDisplay = "";
	totalPriceDisplay = "";

	fsUnitPriceDisplay = "";
	fsTotalPriceDisplay = "";
	fsTotalSavingsDisplay = "";

	dualRangeIndDisplay = "";

	poundsForDisplay = "";
	fractionalPricingForDisplay = "";

	if (myProductTransaction)
	{
		const HfcWeigherStatus *ptWeigherStatus = myProductTransaction->getWeigherStatus();
		bool weightIsValid = false;
		if (myProductTransaction->needsNoWeigherStatus())
		{
			
			const Product *myProduct = myProductTransaction->getProduct();
			if (myProduct && myProduct->isRandomWeight() && myProduct->hasKbdNetWeightLabel())
			{
				//	Random weight product with a keyboard net weight.
				if  (myProduct->getKeyboardNetWeight() > 0)
					cfg->formatWeight(myProduct->getKeyboardNetWeight(), netWeightDisplay, useDefaultDecimalSeparator, KeyboardNetWeight);

				// FT-980 display unit price
				if ( !myProduct->isByCountWithWeight() )
					unitPriceDisplay = cfg->formatHobPrice( myProductTransaction->getUnitPrice(), useDefaultDecimalSeparator );
			}
			else
			{
				// is a by count (but not 'weight on a bycount'), fixed weight, or fluid ounce

				// display the by count in the netWeight display field. (used for the customer display, has no affect for the main browser window
				netWeightDisplay = Integer::toString(myProductTransaction->getByCount());
			}

			if (myProduct->isByCount() && cfg->toIntCfg(fieldOffsetCfgUnitPricePrintMethod) > (int)UnitPricePrintMethods::ForText)
			{
				UnitPricePrintMethods methodUsed = cfg->formatUnitPriceForPrinting(myProduct, myProductTransaction, unitPriceDisplay);
				// We don't want to display the traditional method, 'For', since we never have in the past, so blank it out if that method was used.
				if (ForText == methodUsed)
					unitPriceDisplay = "";
				Debug::trace("HfcProductUpdateValuesMsg", string("HPUVMsg::determineDisplayValues():  unitPriceDisplay = ") + unitPriceDisplay);
			}

			if (myProduct && myProduct->isTotalPriceOutOfRange())
				totalPriceDisplay = _invalidTotalPriceDisplay_;
			else if (myProductTransaction->getTotalPrice() > 0)
				totalPriceDisplay = cfg->formatHobPrice(myProductTransaction->getTotalPrice(), useDefaultDecimalSeparator);
//				cfg->formatPrice(myProductTransaction->getTotalPrice(), totalPriceDisplay);

			// totalTareDisplay and unitPriceDisplay remain blank
			weightIsValid = true;
		}
		else
		{
			if (!ptWeigherStatus)
			{
				weightIsValid = true;
				cfg->formatWeight(myProductTransaction->getTotalNetWeight(), netWeightDisplay, useDefaultDecimalSeparator);
			}
			else
			{
				if (ptWeigherStatus->isWeigherInError())
					netWeightDisplay = "----";
//METSIG??  Do we want to consider all of the weigher statuses and weight > minWeightToPrint to be MetSig
				else if (ptWeigherStatus->isPeripheralComm() && !ptWeigherStatus->isManagerDeleted() && (ptWeigherStatus->isStableWeight() || ptWeigherStatus->isSmallMotion()))
				{
					weightIsValid = true;
					if ( MetSig::isWeigherConfiguredForSingleRange() || ( MetSig::isWeigherConfiguredForDualRange() && ptWeigherStatus->getDualRangeDirection() == DualRangeDown ) )
						cfg->formatWeight(myProductTransaction->getTotalNetWeight(), netWeightDisplay, useDefaultDecimalSeparator, DualRangeModeDown);
					else
						cfg->formatWeight(myProductTransaction->getTotalNetWeight(), netWeightDisplay, useDefaultDecimalSeparator);

					if (MetSig::isScaleMetric())
					{
						dualRangeIndDisplay = (ptWeigherStatus->getScaleFactor() == 5) ? "5g" : "2g";
						if ( ptWeigherStatus->getDualRangeDirection() == DualRangeDown )
							dualRangeIndDisplay = "5g";
					}
				}
			}

			if (weightIsValid)
			{
				if ( ptWeigherStatus && ( MetSig::isWeigherConfiguredForSingleRange() || ( MetSig::isWeigherConfiguredForDualRange() && ptWeigherStatus->getDualRangeDirection() == DualRangeDown ) ) )
				{	
					cfg->formatWeight(myProductTransaction->getTotalTare(), totalTareDisplay, useDefaultDecimalSeparator, DualRangeModeDown);
					tareWeight = myProductTransaction->getTotalTare();
				}
				else
				{
					cfg->formatWeight(myProductTransaction->getTotalTare(), totalTareDisplay, useDefaultDecimalSeparator);
					tareWeight = myProductTransaction->getTotalTare();
				}
			}

			const Product *myProduct = myProductTransaction->getProduct();
			if (!myProduct || !myProduct->isByCountWithWeight())
				unitPriceDisplay = cfg->formatHobPrice(myProductTransaction->getUnitPrice(), useDefaultDecimalSeparator);
//				cfg->formatPrice(myProductTransaction->getUnitPrice(), unitPriceDisplay);
			
			if (weightIsValid)
			{
				if (myProduct && myProduct->isTotalPriceOutOfRange())
					totalPriceDisplay = _invalidTotalPriceDisplay_;
				else if (myProductTransaction->getTotalPrice() > 0)
					totalPriceDisplay = cfg->formatHobPrice(myProductTransaction->getTotalPrice(), useDefaultDecimalSeparator);
//				cfg->formatPrice(myProductTransaction->getTotalPrice(), totalPriceDisplay);
			}
		}

		if ( weightIsValid && ptWeigherStatus )
		{
			//zeroIndicator = (weightIsValid && ptWeigherStatus && ptWeigherStatus->getWeight() == (HobWeight)0);
			zeroIndicator = ( ptWeigherStatus->isZeroStableWeight() );

			// For Canadian W&M, they require the zero indicator to be turned off when the net weight is < 0 due to item tare,
			// even when there is nothing on the platter.  Don't need to check tare > 0, just net weight < 0
			// (negative gross weight taken care of since weightIsValid would be false in that case).
			// The only way to determine if in Canada is if scale is in metric mode.
			// NOTE:  Any country using metric will now fall into this category.
			if (MetSig::isScaleMetric() && myProductTransaction->getTotalNetWeight() < 0 )
				zeroIndicator = false;
		}
		
		const FrequentShopper *myFrequentShopperData = myProductTransaction->getFrequentShopperData();
		if (myFrequentShopperData)
		{
			if (weightIsValid && myFrequentShopperData->getTotalPrice1() > 0.0)
				fsTotalPriceDisplay = cfg->formatHobPrice(myFrequentShopperData->getTotalPrice1(), useDefaultDecimalSeparator);
//				cfg)->formatPrice(myFrequentShopperData->getTotalPrice1(), fsTotalPriceDisplay);
		
			if (weightIsValid && myFrequentShopperData->getTotalSavings1() > 0.0)
				fsTotalSavingsDisplay = cfg->formatHobPrice(myFrequentShopperData->getTotalSavings1(), useDefaultDecimalSeparator);
//				cfg->formatPrice(myFrequentShopperData->getTotalSavings1(), fsTotalSavingsDisplay);

			if (weightIsValid && myFrequentShopperData->getUnitPrice1() > 0.0)
				fsUnitPriceDisplay = cfg->formatHobPrice(myFrequentShopperData->getUnitPrice1(), useDefaultDecimalSeparator);
//				cfg->formatPrice(myFrequentShopperData->getUnitPrice1(), fsUnitPriceDisplay);
		}

		if (cfg->isPoundsForPricingEnabled())
		{
			const Product *myProduct = myProductTransaction->getProduct();

			if ( myProduct )
			{
				int poundsFor = myProduct->getPriceModifier();
				if (poundsFor > 1 && poundsFor < 10)
					poundsForDisplay = Integer::toString(poundsFor);
				Debug::trace("Stupid", HobString("HPUVMsg::determineDisplayValues():  poundsForDisplay = ") + poundsForDisplay);
			}
		}
		else if (cfg->isFractionalPricingEnabled())
		{
			const Product *myProduct = myProductTransaction->getProduct();

			if ( myProduct )
			{
				int fractional = myProduct->getPriceModifier();
				if (fractional > 1)
				{
					if (( MetSig::isScaleAvoir() ) && ( 25 == fractional ))
						fractionalPricingForDisplay = "1/4lb";
					else if (( MetSig::isScaleAvoir() ) && ( 50 == fractional ))
						fractionalPricingForDisplay = "1/2lb";
					else if (( MetSig::isScaleMetric() ) && ( 100 == fractional ))
						fractionalPricingForDisplay = "100g";
				}
				Debug::trace("Stupid", HobString("HPUVMsg::determineDisplayValues():  fractionalPricingForDisplay = ") + fractionalPricingForDisplay);
			}
		}
	}
}

const char *HfcProductUpdateValuesMsg::serialize(int format) const
{
	HfcObject::serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
		{
			determineDisplayValues(format);
			HobString stringForQuoting;

			if (myProductTransaction)
				HfcObject::serializedData.append(Long::toString(myProductTransaction->getProductNumber()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			stringForQuoting = totalTareDisplay;
			stringForQuoting.quoteForCSV();
			HfcObject::serializedData.append(stringForQuoting);
			HfcObject::serializedData.append(BrowserFormatSeparator);

			stringForQuoting = netWeightDisplay;
			stringForQuoting.quoteForCSV();
			HfcObject::serializedData.append(stringForQuoting);
			HfcObject::serializedData.append(BrowserFormatSeparator);

			stringForQuoting = totalPriceDisplay;
			stringForQuoting.quoteForCSV();
			HfcObject::serializedData.append(stringForQuoting);
			HfcObject::serializedData.append(BrowserFormatSeparator);

			stringForQuoting = unitPriceDisplay;
			stringForQuoting.quoteForCSV();
			HfcObject::serializedData.append(stringForQuoting);
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(Bool::toString(zeroIndicator));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(dualRangeIndDisplay);

			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(poundsForDisplay);

			if (myProductTransaction)
			{
				const FrequentShopper *myFrequentShopperData = myProductTransaction->getFrequentShopperData();
				if (myFrequentShopperData)
				{
					HfcObject::serializedData.append(BrowserFormatSeparator);
					stringForQuoting = fsTotalPriceDisplay;
					stringForQuoting.quoteForCSV();
					HfcObject::serializedData.append(stringForQuoting);

					HfcObject::serializedData.append(BrowserFormatSeparator);
					stringForQuoting = fsTotalSavingsDisplay;
					stringForQuoting.quoteForCSV();
					HfcObject::serializedData.append(stringForQuoting);

					HfcObject::serializedData.append(BrowserFormatSeparator);
					stringForQuoting = fsUnitPriceDisplay;
					stringForQuoting.quoteForCSV();
					HfcObject::serializedData.append(stringForQuoting);
				}
			}
			break;
		}

		default:
			HfcObject::serializedData = HobString("HfcProductUpdateValuesMsg::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

