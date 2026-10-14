/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FREQUENTSHOPPER_H
#define FREQUENTSHOPPER_H

#include "evolution.h"
#include "HfcObject.h"
#include "Rounding.h"

class Configuration;
class Product;

typedef enum FrequentShopperFieldOffsets
{
	fieldOffsetFSRoundingMethod = 1,
	fieldOffsetFSPerScale,
	fieldOffsetFSDiscountType,
	fieldOffsetFSDiscountFormula,

	endFrequentShopperFieldOffsets
}   FrequentShopperFieldOffsets_t;

typedef enum DiscountMethod
{
    RII_DISCOUNT_METHOD,			//  [ 0]	TDP = (NW * (UP - CO))
    IBM_DISCOUNT_METHOD,			//  [ 1]	TS = (RW * (UP - DP))
    IBM_EM2_DISCOUNT_METHOD,		//  [ 2]	TDP = (RW * DP)
    PERCENT_DISCOUNT_METHOD,		//  [ 3]	TDP = TP - (TP * P)
    NCR_DISCOUNT_METHOD,			//  [ 4]	TS = (RW2 * (UP - DP))
    SPECIAL_SAVINGS,				//  [ 5]	NOT A True FREQUENT SHOPPER METHOD
    PERCENT_ADDED_METHOD,			//  [ 6]	TDP = TP + (TP * P)
    ICL_SASI_DISCOUNT_METHOD,		//  [ 7]	TDP = ((TP * DP) / UP)
    IBM_CENTS_OFF_DISCOUNT_METHOD,	//  [ 8]	TS = (RW * CO)
    FLEX_DISCOUNT_METHOD_PER_SCALE,	//  [ 9]	SET FLEXIBLE FS FOR ALL PLU's
    FLEX_DISCOUNT_METHOD_PER_PLU,	//  [10]	SET FLEXIBLE FS ON A PLU BY PLU BASIS

	//	ADD NEW METHODS BEFORE THE FOLLOWING
	INVALID_DISCOUNT_METHOD
}   DiscountMethod_t;

typedef enum DiscountFormula
{
    RII_FS_FORMULA,				//  [ 0]	TDP = (NW * (DV))
    IBM_FS_FORMULA,				//  [ 1]	TS = (RW * (DV))
    IBM_EM2_FS_FORMULA,			//  [ 2]	TDP = (RW * DV)
    PERCENT_OFF_FS_FORMULA,		//  [ 3]	TDP = TP - (TP * P)
    NCR_FS_FORMULA,				//  [ 4]	TS = (RW2 * (DV))
    SPECIAL_SAVINGS_FORMULA,	//  [ 5]	NOT A True FREQUENT SHOPPER METHOD
    PERCENT_ADDED_FS_FORMULA,	//  [ 6]	TDP = TP + (TP * P)
    ICL_SASI_FS_FORMULA,		//  [ 7]	TDP = ((TP * DV) / UP)
    PACKAGE_DISCOUNT_FS_FORMULA,//  [ 8]	TDP = TP - DV
    IBM_ACE_DISCOUNT_FS_FORMULA,	//  [ 9]	Use IBM_EM2_FS_FORMULA for DiscountPrice, IBM_FS_FORMULA for CentsOff

	//	ADD NEW FORMULAS BEFORE THE FOLLOWING
	INVALID_DISCOUNT_FORMULA
}   DiscountFormula_t;

typedef enum DiscountType
{
    Default_dept_type,
    Cents_off_unit_price,
    Discount_price_by_weight,
    Percent_off_net_value,
    Special_price,
    Percent_added_net_value,
    Discount_per_package,

	//	ADD NEW DISCOUNT TYPES BEFORE THE FOLLOWING
	INVALID_DISCOUNT_TYPE
}   DiscountType_t;


class FrequentShopper //: public HfcObject
{
	public:
		FrequentShopper(const FrequentShopper&);
		FrequentShopper();
//		FrequentShopper(const char *data, int len, int formatType);

		~FrequentShopper();

//		const char *serialize(int format) const;

		HobPrice getTotalPrice1() const;
		void setTotalPrice1(HobPrice totalPrice);
		HobPrice getTotalPrice2() const;
		void setTotalPrice2(HobPrice totalPrice);
		HobPrice getTotalPrice3() const;
		void setTotalPrice3(HobPrice totalPrice);

		HobPrice getTotalSavings1() const;
		void setTotalSavings1(HobPrice totalSavings);
		HobPrice getTotalSavings2() const;
		void setTotalSavings2(HobPrice totalSavings);
		HobPrice getTotalSavings3() const;
		void setTotalSavings3(HobPrice totalSavings);

		HobPrice getUnitPrice1() const;
		void setUnitPrice1(HobPrice totalPrice);
		HobPrice getUnitPrice2() const;
		void setUnitPrice2(HobPrice totalPrice);
		HobPrice getUnitPrice3() const;
		void setUnitPrice3(HobPrice totalPrice);

		void recalculateWeight(int byCount);
		void calculatePackageDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		void calculateICLSASIDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		void calculateRIIDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		void calculateIBMDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		void calculateIBMEM2Discount(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		void calculateAdvertisedPrice(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		void calculatePercentDiscount(HobPrice *fsTotalPrice, HobPrice *fsSavings);
		static void validateDefaultDiscountValues();
		void setupFrequentShopperData(short pluDiscountType);
		void calculateDiscounts(const Product *product);

		HobString getDiscountTypeString() const;

		HobPrice calculateDiscountUnitPrice(const Product *product, HobPrice fsTotalPrice);
		HobPrice calculateCentsOff(HobPrice unitPrice, HobPrice discountValue);
		long divideAndRound(long dividend, int divisor, short rounding_method);
		int GetRoundingAddend(short rounding_method);

		void show();

		short getRoundingWeightPrecision(HobWeight netWeight);		//	For the HLX which uses 3 digit precision for weight.


	private:

		long		junkParameter;	//	So I can see all of the private parameters.

		Configuration *scaleConfig;
		HobPrice totalPrice1;
		HobPrice totalPrice2;
		HobPrice totalPrice3;
		HobPrice totalSavings1;
		HobPrice totalSavings2;
		HobPrice totalSavings3;
		HobPrice unitPrice1;
		HobPrice unitPrice2;
		HobPrice unitPrice3;

//	Temp variables for the Frequent shopper calculations
		HobPrice unitPrice;
		HobPrice totalPrice;
		HobWeight netWeight;
		HobWeight recalculatedWeight;
		HobPrice discountValue;
	    short discountFormula;
	    short discountType;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //FREQUENTSHOPPER_H
