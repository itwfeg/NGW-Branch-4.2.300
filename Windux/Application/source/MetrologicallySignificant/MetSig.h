/* HT Scale Project
 * COPYRIGHT (c) 2020.  All Rights Reserved.
 * ITWFEG, Troy, Ohio  USA
 */

#ifndef METSIG_H
#define METSIG_H

/***************************************************************************************************************************
****************************************************************************************************************************
*****
*****   BEGIN
*****
*****   THE FOLLOWING CODE IS CONSIDERED TO BE SEALABLE BY WEIGHTS & MEASURES; IT DETERMINES FIRST AND FINAL WEIGHT/PRICE.
*****   IF YOU CHANGE IT, YOU MUST CHANGE THE VERSION NUMBER DEFINED BELOW!!
*****
****************************************************************************************************************************
***************************************************************************************************************************/

// OIML R76 European regulation
// SOF-5333 - Changed to W&M rounding, instead of Standard Conventional rounding that we had been using, initiating new version 203
#define METROLOGICALLY_SIGNIFICANT_APPLICATION_VERSION 203


#include "evolution.h"
#include "CCriticalSection.h"
#include "Weigher.h"

class WeigherStatus;
class HfcWeigherStatus;

#define PROP_TARE_DEC_PRECISION	1


class MetSig
{
	public:

		static std::string getApplicationVersion();

		/*****************************************************************************************************************
		 *  search source code for //METSIG??
		 *  searching code for 'price', 'total', 'totalprice', 'total_price', 'calculate', 'calc', 
		 *****************************************************************************************************************/

		// Price calculation routines

		static HobPrice calculateUnitPriceRounded(const HobPrice unitPriceToUse, const int byCountToUse);									// Originally from Configuration.cpp
		static bool calculateUnitPriceNoFractionalCents(HobPrice *result, const HobPrice unitPriceToUse, const int byCountToUse);		// Originally from Configuration.cpp
		static HobPrice calcFixedWeightPricePerUnit(const HobPrice thePrice, const HobWeight theFixedWeight);								// originally from ProductTransaction.cpp
		static HobValue divideAndRoundPortions(const long dividend, const int divisor, const short precision);								// originally from ProductTransaction.cpp

		static long calcByCountCost(const long qty_purch, const long deal_qty, const long deal_price, const long single_price, const short  method);		// originally from ByCountCalcs.cpp
		static long calcByCountCost2(const long qty_purch, const long deal_qty, const long deal_price, const long single_price);							// originally from ByCountCalcs.cpp
		static long calcGCD(const long num1, const long num2);																								// originally from ByCountCalcs.cpp
		static long calcSinglePrice(const long deal_qty, const long deal_price);																			// originally from ByCountCalcs.cpp
		static HobPrice byCountCalcTotalPrice(const int originalByCount, const int byCount, const HobPrice unitPrice, const HobPrice exceptionPrice);		// originally from ByCountCalcs.cpp

		static HobPrice determineImplicitDecimalPrice(const std::string priceStr);


		// Weigher-related and weight calculation routines

		static WeighMode getScaleWeighMode();
		static void setScaleWeighMode(const WeighMode mode);
		static bool isScaleAvoir();
		static bool isScaleMetric();

		static bool isWeigherDualRangeCapable(const WeigherModel model = endWeigherModel);
		static bool isWeigherConfiguredForSingleRange();
		static bool isWeigherConfiguredForDualRange();

		static WgDualRangeDirection getDualRangeDirection();
		static void setDualRangeDirection(const WgDualRangeDirection direction);
		static bool checkToSwitchDualRangeDirection(HfcWeigherStatus *newWeight, bool hasCurrentProductInOperateMode);
		static bool checkToSwitchDualRangeDirectionOnNetWeight(HfcWeigherStatus *weigherStatus, const HobWeight netWeight);

		static HobWeight applyScaleFactorToWeight(HobWeight weight, UINT32 scaleFactor);

		static HobWeight weightCountsToFixedWeight(const HobWeight weight);
		static HobWeight fixedWeightToWeightCounts(const HobWeight fixedWeight);
		static HobWeight fixedWeightOuncesToDecimalWeight(const HobWeight fixedWeight);
		static HobWeight ouncesToPoundCounts(HobWeight ounces, bool isInTenthsOfAnOunce);

		static HobWeight ouncesToGrams(const HobWeight ounces);
		static HobWeight gramsToOunces(const HobWeight grams);
		static HobWeight fluidOuncesToLiters(const HobWeight totalOunces);
		static long fluidOuncesToGallons(const HobWeight fluidOunces);
		static long fluidOuncesToQuarts(const HobWeight fluidOunces);
		static long fluidOuncesToPints(const HobWeight fluidOunces);

		static HobWeight BRConvertFixedWeightToTotalWeight(const HobWeight fixedWeight, const bool fluidOz);

		static HobWeight divideWeightForSingleRange(const HobWeight weight);
		static double divideWeightForSingleRangeD(const HobWeight weight);

		static HobWeight determineImplicitDecimalWeight(const std::string weightStr);
		static HobWeight determineImplicitDecimalKeyboardNetWeight(const std::string weightStr);

		static double roundWeightTo2DecimalsThenBackTo3(const double weight);

		static void applyTare(HobWeight &netWeight, HobWeight &totalTare, const HfcWeigherStatus *weigherStatus);
		static void applyTareSetback(HobWeight &netWeight, const HobWeight totalTare);
		static void applyProportionalTare(HobWeight &netWeight, HobWeight &totalTare, const int proportionalTare, const HfcWeigherStatus *weigherStatus);
		static HobPrice calculateRandomWeightTotalPrice(const bool hasKbdNetWeightLabel, const HobPrice unitPrice, const long priceModifier, const HobWeight netWeight);
		static HobPrice calculateRandomWeightOldTotalPrice(const bool hasKbdNetWeightLabel, const HobPrice unitPrice, const long priceModifier, const HobWeight netWeight);


		// Test routines

		static void test_allMetSig();
		
		// Price calculation routines

		static void test_calculateUnitPriceRounded();
		static void test_calculateUnitPriceNoFractionalCents();
		static void test_calcFixedWeightPricePerUnit();
		static void test_divideAndRoundPortions();

		static void test_byCountCalcTotalPrice();

		static void test_determineImplicitDecimalPrice();


		// Weigher-related and weight calculation routines

		static void test_isWeigherDualRangeCapable();

		static void test_checkToSwitchDualRangeDirection();
		static void test_checkToSwitchDualRangeDirectionOnNetWeight();

		static void test_applyScaleFactorToWeight();

		static void test_weightCountsToFixedWeight();
		static void test_fixedWeightToWeightCounts();
		static void test_fixedWeightOuncesToDecimalWeight();
		static void test_ouncesToPoundCounts();

		static void test_ouncesToGrams();
		static void test_gramsToOunces();
		static void test_fluidOuncesToLiters();
		static void test_fluidOuncesToGallons();
		static void test_fluidOuncesToQuarts();
		static void test_fluidOuncesToPints();

		static void test_BRConvertFixedWeightToTotalWeight();

		static void test_determineImplicitDecimalWeight();
		static void test_determineImplicitDecimalKeyboardNetWeight();

		static void test_roundWeightTo2DecimalsThenBackTo3();

		static void test_applyTare();
		static void test_applyTareSetback();
		static void test_applyProportionalTare();
		static void test_calculateRandomWeightTotalPrice();
		static void test_calculateRandomWeightOldTotalPrice();



	private:
		MetSig();

		static WeighMode currentWeighMode;
		static WgDualRangeDirection currentDualRangeDirection;

		static Util::CSmartCriticalSection  metSigCS_calculateUnitPriceRounded;
		static Util::CSmartCriticalSection  metSigCS_calculateUnitPriceNoFractionalCents;
		static Util::CSmartCriticalSection  metSigCS_calcFixedWeightPricePerUnit;
		static Util::CSmartCriticalSection  metSigCS_divideAndRoundPortions;
		static Util::CSmartCriticalSection  metSigCS_byCountCalcTotalPrice;
		static Util::CSmartCriticalSection  metSigCS_determineImplicitDecimalPrice;

		static Util::CSmartCriticalSection  metSigCS_setScaleWeighMode;
		static Util::CSmartCriticalSection  metSigCS_setDualRangeDirection;
		static Util::CSmartCriticalSection  metSigCS_checkToSwitchDualRangeDirection;
		static Util::CSmartCriticalSection  metSigCS_checkToSwitchDualRangeDirectionOnNetWeight;
		static Util::CSmartCriticalSection  metSigCS_isWeigherDualRangeCapable;
		static Util::CSmartCriticalSection  metSigCS_applyScaleFactorToWeight;
		static Util::CSmartCriticalSection  metSigCS_weightCountsToFixedWeight;
		static Util::CSmartCriticalSection  metSigCS_ouncesToGrams;
		static Util::CSmartCriticalSection  metSigCS_gramsToOunces;
		static Util::CSmartCriticalSection  metSigCS_ouncesToLiters;
		static Util::CSmartCriticalSection  metSigCS_BRConvertFixedWeightToTotalWeight;
		static Util::CSmartCriticalSection  metSigCS_determineImplicitDecimalWeight;
		static Util::CSmartCriticalSection  metSigCS_determineImplicitDecimalKeyboardNetWeight;
		static Util::CSmartCriticalSection  metSigCS_roundWeightTo2DecimalsThenBackTo3;
		static Util::CSmartCriticalSection  metSigCS_applyTare;
		static Util::CSmartCriticalSection  metSigCS_applyTareSetback;
		static Util::CSmartCriticalSection  metSigCS_applyProportionalTare;
		static Util::CSmartCriticalSection  metSigCS_calculateRandomWeightTotalPrice;
		static Util::CSmartCriticalSection  metSigCS_calculateRandomWeightOldTotalPrice;

		static bool testingMetSig;

		//static Util::CSmartCriticalSection  metSigCS;
};


/***************************************************************************************************************************
****************************************************************************************************************************
*****
*****   END
*****
*****   THE ABOVE CODE IS CONSIDERED TO BE SEALABLE BY WEIGHTS & MEASURES; IT DETERMINES FIRST AND FINAL WEIGHT/PRICE.
*****   IF YOU CHANGE IT, YOU MUST CHANGE THE VERSION NUMBER DEFINED ABOVE!!
*****
****************************************************************************************************************************
***************************************************************************************************************************/

#endif //METSIG_H
