#ifndef DBPRODUCTI_H
#define DBPRODUCTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListI;


	class DBProductI
	{
		public:

			virtual ~DBProductI() {}


			virtual DBProductI* clone() const = 0;
			virtual const DBProductI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getPLU() const = 0;
			virtual void setPLU(const std::string& plu) = 0;
			virtual bool isPLUNull() const { return false; }
			virtual bool isPLUWritable() const { return true; }

			virtual std::string getDeptNumber() const = 0;
			virtual void setDeptNumber(const std::string& number) = 0;
			virtual bool isDeptNumberNull() const { return false; }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual int getType() const = 0;
			virtual void setType(int type) = 0;
			virtual bool isTypeNull() const { return false; }
			virtual void setTypeNull() = 0;
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getReportingClass() const = 0;
			virtual void setReportingClass(const std::string& rc) = 0;
			virtual bool isReportingClassNull() const { return false; }
			virtual void setReportingClassNull() = 0;
			virtual bool isReportingClassWritable() const { return true; }

			virtual int getByCount() const = 0;
			virtual void setByCount(int bc) = 0;
			virtual bool isByCountNull() const { return false; }
			virtual void setByCountNull() = 0;
			virtual bool isByCountWritable() const { return true; }

			virtual bool getForceByCount() const = 0;
			virtual void setForceByCount(bool fbc) = 0;
			virtual bool isForceByCountNull() const { return false; }
			virtual void setForceByCountNull() = 0;
			virtual bool isForceByCountWritable() const { return true; }

			virtual int getPrice() const = 0;
			virtual void setPrice(int price) = 0;
			virtual bool isPriceNull() const { return false; }
			virtual void setPriceNull() = 0;
			virtual bool isPriceWritable() const { return true; }

			virtual int getPriceFactor() const = 0;
			virtual void setPriceFactor(int pf) = 0;
			virtual bool isPriceFactorNull() const { return false; }
			virtual void setPriceFactorNull() = 0;
			virtual bool isPriceFactorWritable() const { return true; }

			virtual bool getForcePrice() const = 0;
			virtual void setForcePrice(bool fp) = 0;
			virtual bool isForcePriceNull() const { return false; }
			virtual void setForcePriceNull() = 0;
			virtual bool isForcePriceWritable() const { return true; }

			virtual int getDiscountPrice1() const = 0;
			virtual void setDiscountPrice1(int dp) = 0;
			virtual bool isDiscountPrice1Null() const { return false; }
			virtual void setDiscountPrice1Null() = 0;
			virtual bool isDiscountPrice1Writable() const { return true; }

			virtual int getDiscountPrice2() const = 0;
			virtual void setDiscountPrice2(int dp) = 0;
			virtual bool isDiscountPrice2Null() const { return false; }
			virtual void setDiscountPrice2Null() = 0;
			virtual bool isDiscountPrice2Writable() const { return true; }

			virtual int getDiscountPrice3() const = 0;
			virtual void setDiscountPrice3(int dp) = 0;
			virtual bool isDiscountPrice3Null() const { return false; }
			virtual void setDiscountPrice3Null() = 0;
			virtual bool isDiscountPrice3Writable() const { return true; }

			virtual int getDiscountMethod() const = 0;
			virtual void setDiscountMethod(int dm) = 0;
			virtual bool isDiscountMethodNull() const { return false; }
			virtual void setDiscountMethodNull() = 0;
			virtual bool isDiscountMethodWritable() const { return true; }

			virtual int getExceptionPrice() const = 0;
			virtual void setExceptionPrice(int xp) = 0;
			virtual bool isExceptionPriceNull() const { return false; }
			virtual void setExceptionPriceNull() = 0;
			virtual bool isExceptionPriceWritable() const { return true; }

			virtual std::string getDescription() const = 0;
			virtual void setDescription(const std::string& desc) = 0;
			virtual bool isDescriptionNull() const { return false; }
			virtual void setDescriptionNull() = 0;
			virtual bool isDescriptionWritable() const { return true; }

			virtual std::string getName1() const = 0;
			virtual void setName1(const std::string& name) = 0;
			virtual bool isName1Null() const { return false; }
			virtual void setName1Null() = 0;
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const = 0;
			virtual void setName2(const std::string& name) = 0;
			virtual bool isName2Null() const { return false; }
			virtual void setName2Null() = 0;
			virtual bool isName2Writable() const { return true; }

			virtual int getBarCodePrefix() const = 0;
			virtual void setBarCodePrefix(int bcpr) = 0;
			virtual bool isBarCodePrefixNull() const { return false; }
			virtual void setBarCodePrefixNull() = 0;
			virtual bool isBarCodePrefixWritable() const { return true; }

			virtual int getBarCodeProductNumber() const = 0;
			virtual void setBarCodeProductNumber(int bcpn) = 0;
			virtual bool isBarCodeProductNumberNull() const { return false; }
			virtual void setBarCodeProductNumberNull() = 0;
			virtual bool isBarCodeProductNumberWritable() const { return true; }

			virtual int getBarCodeVendorNumber() const = 0;
			virtual void setBarCodeVendorNumber(int bcvn) = 0;
			virtual bool isBarCodeVendorNumberNull() const { return false; }
			virtual void setBarCodeVendorNumberNull() = 0;
			virtual bool isBarCodeVendorNumberWritable() const { return true; }

			// Allow 10 digits for EAN code. For HEB Mexico demo.
			virtual std::string getBarCodeEANCode() const = 0;
			virtual void setBarCodeEANCode(std::string bcec) = 0;
			virtual bool isBarCodeEANCodeNull() const { return false; }
			virtual void setBarCodeEANCodeNull() = 0;
			virtual bool isBarCodeEANCodeWritable() const { return true; }

			virtual std::string getRawBarCodeNumber() const = 0;
			virtual void setRawBarCodeNumber(const std::string& barcode) = 0;
			virtual bool isRawBarCodeNumberNull() const { return false; }
			virtual void setRawBarCodeNumberNull() = 0;
			virtual bool isRawBarCodeNumberWritable() const { return true; }

			virtual int getNetWeight() const = 0;
			virtual void setNetWeight(int nw) = 0;
			virtual bool isNetWeightNull() const { return false; }
			virtual void setNetWeightNull() = 0;
			virtual bool isNetWeightWritable() const { return true; }

			virtual int getShelfLifeDays() const = 0;
			virtual void setShelfLifeDays(int sld) = 0;
			virtual bool isShelfLifeDaysNull() const { return false; }
			virtual void setShelfLifeDaysNull() = 0;
			virtual bool isShelfLifeDaysWritable() const { return true; }

			virtual int getShelfLifeHours() const = 0;
			virtual void setShelfLifeHours(int slh) = 0;
			virtual bool isShelfLifeHoursNull() const { return false; }
			virtual void setShelfLifeHoursNull() = 0;
			virtual bool isShelfLifeHoursWritable() const { return true; }

			virtual int getShelfLifeMinutes() const = 0;
			virtual void setShelfLifeMinutes(int slm) = 0;
			virtual bool isShelfLifeMinutesNull() const { return false; }
			virtual void setShelfLifeMinutesNull() = 0;
			virtual bool isShelfLifeMinutesWritable() const { return true; }

			virtual bool getDontPrintShelfLife() const = 0;
			virtual void setDontPrintShelfLife(bool dpsl) = 0;
			virtual bool isDontPrintShelfLifeNull() const { return false; }
			virtual void setDontPrintShelfLifeNull() = 0;
			virtual bool isDontPrintShelfLifeWritable() const { return true; }

			virtual int getProductLifeDays() const = 0;
			virtual void setProductLifeDays(int pld) = 0;
			virtual bool isProductLifeDaysNull() const { return false; }
			virtual void setProductLifeDaysNull() = 0;
			virtual bool isProductLifeDaysWritable() const { return true; }

			virtual int getProductLifeHours() const = 0;
			virtual void setProductLifeHours(int plh) = 0;
			virtual bool isProductLifeHoursNull() const { return false; }
			virtual void setProductLifeHoursNull() = 0;
			virtual bool isProductLifeHoursWritable() const { return true; }

			virtual int getProductLifeMinutes() const = 0;
			virtual void setProductLifeMinutes(int plm) = 0;
			virtual bool isProductLifeMinutesNull() const { return false; }
			virtual void setProductLifeMinutesNull() = 0;
			virtual bool isProductLifeMinutesWritable() const { return true; }

			virtual bool getDontPrintProductLife() const = 0;
			virtual void setDontPrintProductLife(bool dppl) = 0;
			virtual bool isDontPrintProductLifeNull() const { return false; }
			virtual void setDontPrintProductLifeNull() = 0;
			virtual bool isDontPrintProductLifeWritable() const { return true; }

			virtual bool getDontPrintPackedOn() const = 0;
			virtual void setDontPrintPackedOn(bool dppo) = 0;
			virtual bool isDontPrintPackedOnNull() const { return false; }
			virtual void setDontPrintPackedOnNull() = 0;
			virtual bool isDontPrintPackedOnWritable() const { return true; }

			virtual std::string getNutrition() const = 0;
			virtual void setNutrition(const std::string& n) = 0;
			virtual bool isNutritionNull() const { return false; }
			virtual void setNutritionNull() = 0;
			virtual bool isNutritionWritable() const { return true; }

			virtual std::string getText() const = 0;
			virtual void setText(const std::string& t) = 0;
			virtual bool isTextNull() const { return false; }
			virtual void setTextNull() = 0;
			virtual bool isTextWritable() const { return true; }

			virtual std::string getSpecialMessage() const = 0;
			virtual void setSpecialMessage(const std::string& sm) = 0;
			virtual bool isSpecialMessageNull() const { return false; }
			virtual void setSpecialMessageNull() = 0;
			virtual bool isSpecialMessageWritable() const { return true; }

			virtual std::string getNote() const = 0;
			virtual void setNote(const std::string& n) = 0;
			virtual bool isNoteNull() const { return false; }
			virtual void setNoteNull() = 0;
			virtual bool isNoteWritable() const { return true; }

			virtual std::string getMarquee() const = 0;
			virtual void setMarquee(const std::string& t) = 0;
			virtual bool isMarqueeNull() const { return false; }
			virtual void setMarqueeNull() = 0;
			virtual bool isMarqueeWritable() const { return true; }

			virtual std::string getProductText5() const = 0;
			virtual void setProductText5(const std::string& t) = 0;
			virtual bool isProductText5Null() const { return false; }
			virtual void setProductText5Null() = 0;
			virtual bool isProductText5Writable() const { return true; }

			virtual std::string getProductText6() const = 0;
			virtual void setProductText6(const std::string& t) = 0;
			virtual bool isProductText6Null() const { return false; }
			virtual void setProductText6Null() = 0;
			virtual bool isProductText6Writable() const { return true; }

			virtual std::string getProductText7() const = 0;
			virtual void setProductText7(const std::string& t) = 0;
			virtual bool isProductText7Null() const { return false; }
			virtual void setProductText7Null() = 0;
			virtual bool isProductText7Writable() const { return true; }

			virtual std::string getProductText8() const = 0;
			virtual void setProductText8(const std::string& t) = 0;
			virtual bool isProductText8Null() const { return false; }
			virtual void setProductText8Null() = 0;
			virtual bool isProductText8Writable() const { return true; }

			virtual std::string getProductText9() const = 0;
			virtual void setProductText9(const std::string& t) = 0;
			virtual bool isProductText9Null() const { return false; }
			virtual void setProductText9Null() = 0;
			virtual bool isProductText9Writable() const { return true; }

			virtual std::string getCategory() const = 0;
			virtual void setCategory(const std::string& cat) = 0;
			virtual bool isCategoryNull() const { return false; }
			virtual void setCategoryNull() = 0;
			virtual bool isCategoryWritable() const { return true; }

			virtual std::string getGraphic1() const = 0;
			virtual void setGraphic1(const std::string& g1) = 0;
			virtual bool isGraphic1Null() const { return false; }
			virtual void setGraphic1Null() = 0;
			virtual bool isGraphic1Writable() const { return true; }

			virtual std::string getGraphic2() const = 0;
			virtual void setGraphic2(const std::string& g2) = 0;
			virtual bool isGraphic2Null() const { return false; }
			virtual void setGraphic2Null() = 0;
			virtual bool isGraphic2Writable() const { return true; }

			virtual std::string getGraphic3() const = 0;
			virtual void setGraphic3(const std::string& g3) = 0;
			virtual bool isGraphic3Null() const { return false; }
			virtual void setGraphic3Null() = 0;
			virtual bool isGraphic3Writable() const { return true; }

			virtual std::string getGraphic4() const = 0;
			virtual void setGraphic4(const std::string& g4) = 0;
			virtual bool isGraphic4Null() const { return false; }
			virtual void setGraphic4Null() = 0;
			virtual bool isGraphic4Writable() const { return true; }

			virtual std::string getImage() const = 0;
			virtual void setImage(const std::string& img) = 0;
			virtual bool isImageNull() const { return false; }
			virtual void setImageNull() = 0;
			virtual bool isImageWritable() const { return true; }

			virtual std::string getLabel1() const = 0;
			virtual void setLabel1(const std::string& l1) = 0;
			virtual bool isLabel1Null() const { return false; }
			virtual void setLabel1Null() = 0;
			virtual bool isLabel1Writable() const { return true; }

			virtual int getLabel1Rotation() const = 0;
			virtual void setLabel1Rotation(int l1r) = 0;
			virtual bool isLabel1RotationNull() const { return false; }
			virtual void setLabel1RotationNull() = 0;
			virtual bool isLabel1RotationWritable() const { return true; }

			virtual int getLabel1Placement() const = 0;
			virtual void setLabel1Placement(int l1p) = 0;
			virtual bool isLabel1PlacementNull() const { return false; }
			virtual void setLabel1PlacementNull() = 0;
			virtual bool isLabel1PlacementWritable() const { return true; }

			virtual std::string getLabel2() const = 0;
			virtual void setLabel2(const std::string& l2) = 0;
			virtual bool isLabel2Null() const { return false; }
			virtual void setLabel2Null() = 0;
			virtual bool isLabel2Writable() const { return true; }

			virtual int getLabel2Rotation() const = 0;
			virtual void setLabel2Rotation(int l2r) = 0;
			virtual bool isLabel2RotationNull() const { return false; }
			virtual void setLabel2RotationNull() = 0;
			virtual bool isLabel2RotationWritable() const { return true; }

			virtual int getLabel2Placement() const = 0;
			virtual void setLabel2Placement(int l2p) = 0;
			virtual bool isLabel2PlacementNull() const { return false; }
			virtual void setLabel2PlacementNull() = 0;
			virtual bool isLabel2PlacementWritable() const { return true; }

			virtual std::string getLabel3() const = 0;
			virtual void setLabel3(const std::string& l3) = 0;
			virtual bool isLabel3Null() const { return false; }
			virtual void setLabel3Null() = 0;
			virtual bool isLabel3Writable() const { return true; }

			virtual int getLabel3Rotation() const = 0;
			virtual void setLabel3Rotation(int l3r) = 0;
			virtual bool isLabel3RotationNull() const { return false; }
			virtual void setLabel3RotationNull() = 0;
			virtual bool isLabel3RotationWritable() const { return true; }

			virtual int getLabel3Placement() const = 0;
			virtual void setLabel3Placement(int l3p) = 0;
			virtual bool isLabel3PlacementNull() const { return false; }
			virtual void setLabel3PlacementNull() = 0;
			virtual bool isLabel3PlacementWritable() const { return true; }

			virtual int getProportionalTare() const = 0;
			virtual void setProportionalTare(int pt) = 0;
			virtual bool isProportionalTareNull() const { return false; }
			virtual void setProportionalTareNull() = 0;
			virtual bool isProportionalTareWritable() const { return true; }

			virtual int getTare() const = 0;
			virtual void setTare(int t) = 0;
			virtual bool isTareNull() const { return false; }
			virtual void setTareNull() = 0;
			virtual bool isTareWritable() const { return true; }

			virtual bool getForceTare() const = 0;
			virtual void setForceTare(bool ft) = 0;
			virtual bool isForceTareNull() const { return false; }
			virtual void setForceTareNull() = 0;
			virtual bool isForceTareWritable() const { return true; }

			virtual int getBonusPoints() const = 0;
			virtual void setBonusPoints(int bp) = 0;
			virtual bool isBonusPointsNull() const { return false; }
			virtual void setBonusPointsNull() = 0;
			virtual bool isBonusPointsWritable() const { return true; }

			virtual int getIntakeMode() const = 0;
			virtual void setIntakeMode(int mode) = 0;
			virtual bool isIntakeModeNull() const { return false; }
			virtual void setIntakeModeNull() = 0;
			virtual bool isIntakeModeWritable() const { return true; }

			virtual bool getApplySecurityLabel() const = 0;
			virtual void setApplySecurityLabel(bool asl) = 0;
			virtual bool isApplySecurityLabelNull() const { return false; }
			virtual void setApplySecurityLabelNull() = 0;
			virtual bool isApplySecurityLabelWritable() const { return true; }

			virtual std::string getSpecialNumber() const = 0;
			virtual void setSpecialNumber(const std::string& special) = 0;
			virtual bool isSpecialNumberNull() const { return false; }
			virtual void setSpecialNumberNull() = 0;
			virtual bool isSpecialNumberWritable() const { return true; }

			virtual std::string getSpecialStartDate() const = 0;
			virtual void setSpecialStartDate(const std::string& ssd) = 0;
			virtual bool isSpecialStartDateNull() const { return false; }
			virtual void setSpecialStartDateNull() = 0;
			virtual bool isSpecialStartDateWritable() const { return true; }

			virtual std::string getCurrentCoolText() const = 0;
			virtual void setCurrentCoolText(const std::string& t) = 0;
			virtual bool isCurrentCoolTextNull() const { return false; }
			virtual void setCurrentCoolTextNull() = 0;
			virtual bool isCurrentCoolTextWritable() const { return true; }

			virtual std::string getDefaultCoolText() const = 0;
			virtual void setDefaultCoolText(const std::string& t) = 0;
			virtual bool isDefaultCoolTextNull() const { return false; }
			virtual void setDefaultCoolTextNull() = 0;
			virtual bool isDefaultCoolTextWritable() const { return true; }

			virtual std::string getCoolCategory() const = 0;
			virtual void setCoolCategory(const std::string& cat) = 0;
			virtual bool isCoolCategoryNull() const { return false; }
			virtual void setCoolCategoryNull() = 0;
			virtual bool isCoolCategoryWritable() const { return true; }

			virtual std::string getCoolPreText() const = 0;
			virtual void setCoolPreText(const std::string& t) = 0;
			virtual bool isCoolPreTextNull() const { return false; }
			virtual void setCoolPreTextNull() = 0;
			virtual bool isCoolPreTextWritable() const { return true; }

			virtual void setCoolList(const DBCoolListI* list) = 0;
			virtual bool isCoolListNull() const { return false; }
			virtual void setCoolListNull() = 0;
			virtual bool isCoolListWritable() const { return true; }

			virtual std::string getCoolTrackingText() const = 0;
			virtual void setCoolTrackingText(const std::string& text) = 0;
			virtual bool isCoolTrackingTextNull() const { return false; }
			virtual void setCoolTrackingTextNull() = 0;
			virtual bool isCoolTrackingTextWritable() const { return true; }

			virtual bool getForceCoolReview() const = 0;
			virtual void setForceCoolReview(bool fcr) = 0;
			virtual bool isForceCoolReviewNull() const { return false; }
			virtual void setForceCoolReviewNull() = 0;
			virtual bool isForceCoolReviewWritable() const { return true; }

			virtual bool getCoolRequired() const = 0;
			virtual void setCoolRequired(bool cr) = 0;
			virtual bool isCoolRequiredNull() const { return false; }
			virtual void setCoolRequiredNull() = 0;
			virtual bool isCoolRequiredWritable() const { return true; }

			virtual int getProductionPlanningType() const = 0;
			virtual void setProductionPlanningType(int type) = 0;
			virtual bool isProductionPlanningTypeNull() const { return false; }
			virtual void setProductionPlanningTypeNull() = 0;
			virtual bool isProductionPlanningTypeWritable() const { return true; }

			virtual int getPortions() const = 0;
			virtual void setPortions(int p) = 0;
			virtual bool isPortionsNull() const { return false; }
			virtual void setPortionsNull() = 0;
			virtual bool isPortionsWritable() const { return true; }

			virtual int getMealSideItemsGroupNum() const = 0;
			virtual void setMealSideItemsGroupNum(int p) = 0;
			virtual bool isMealSideItemsGroupNumNull() const { return false; }
			virtual void setMealSideItemsGroupNumNull() = 0;
			virtual bool isMealSideItemsGroupNumWritable() const { return true; }

			virtual bool getMealSideItemsAreFree() const = 0;
			virtual void setMealSideItemsAreFree(bool f) = 0;
			virtual bool isMealSideItemsAreFreeNull() const { return false; }
			virtual void setMealSideItemsAreFreeNull() = 0;
			virtual bool isMealSideItemsAreFreeWritable() const { return true; }

			virtual int getMealNumSidesIncluded() const = 0;
			virtual void setMealNumSidesIncluded(int p) = 0;
			virtual bool isMealNumSidesIncludedNull() const { return false; }
			virtual void setMealNumSidesIncludedNull() = 0;
			virtual bool isMealNumSidesIncludedWritable() const { return true; }

			virtual int getCutTestMeatSpecSheetGroupNum() const = 0;
			virtual void setCutTestMeatSpecSheetGroupNum(int p) = 0;
			virtual bool isCutTestMeatSpecSheetGroupNumNull() const { return false; }
			virtual void setCutTestMeatSpecSheetGroupNumNull() = 0;
			virtual bool isCutTestMeatSpecSheetGroupNumWritable() const { return true; }

			virtual int getTareB() const = 0;
			virtual void setTareB(int t) = 0;
			virtual bool isTareBNull() const { return false; }
			virtual void setTareBNull() = 0;
			virtual bool isTareBWritable() const { return true; }

			// For Pick-5 project

			virtual int getWeightMin() const = 0;
			virtual void setWeightMin(int w) = 0;
			virtual bool isWeightMinNull() const { return false; }
			virtual void setWeightMinNull() = 0;
			virtual bool isWeightMinWritable() const { return true; }

			virtual int getWeightMax() const = 0;
			virtual void setWeightMax(int w) = 0;
			virtual bool isWeightMaxNull() const { return false; }
			virtual void setWeightMaxNull() = 0;
			virtual bool isWeightMaxWritable() const { return true; }

			virtual int getTotalPriceMin() const = 0;
			virtual void setTotalPriceMin(int p) = 0;
			virtual bool isTotalPriceMinNull() const { return false; }
			virtual void setTotalPriceMinNull() = 0;
			virtual bool isTotalPriceMinWritable() const { return true; }

			virtual int getTotalPriceMax() const = 0;
			virtual void setTotalPriceMax(int p) = 0;
			virtual bool isTotalPriceMaxNull() const { return false; }
			virtual void setTotalPriceMaxNull() = 0;
			virtual bool isTotalPriceMaxWritable() const { return true; }
			// Automatic mode labels project
			virtual std::string getManualLabelTypeNum1() const = 0;
			virtual void setManualLabelTypeNum1(const std::string& l1) = 0;
			virtual bool isManualLabelTypeNum1Null() const { return false; }
			virtual void setManualLabelTypeNum1Null() = 0;
			virtual bool isManualLabelTypeNum1Writable() const { return true; }
			virtual std::string getManualLabelTypeNum2() const = 0;
			virtual void setManualLabelTypeNum2(const std::string& l2) = 0;
			virtual bool isManualLabelTypeNum2Null() const { return false; }
			virtual void setManualLabelTypeNum2Null() = 0;
			virtual bool isManualLabelTypeNum2Writable() const { return true; }
			virtual std::string getManualLabelTypeNum3() const = 0;
			virtual void setManualLabelTypeNum3(const std::string& l3) = 0;
			virtual bool isManualLabelTypeNum3Null() const { return false; }
			virtual void setManualLabelTypeNum3Null() = 0;
			virtual bool isManualLabelTypeNum3Writable() const { return true; }
			// prepack
			virtual std::string getPrepackLabelTypeNum1() const = 0;
			virtual void setPrepackLabelTypeNum1(const std::string& l1) = 0;
			virtual bool isPrepackLabelTypeNum1Null() const { return false; }
			virtual void setPrepackLabelTypeNum1Null() = 0;
			virtual bool isPrepackLabelTypeNum1Writable() const { return true; }
			virtual std::string getPrepackLabelTypeNum2() const = 0;
			virtual void setPrepackLabelTypeNum2(const std::string& l2) = 0;
			virtual bool isPrepackLabelTypeNum2Null() const { return false; }
			virtual void setPrepackLabelTypeNum2Null() = 0;
			virtual bool isPrepackLabelTypeNum2Writable() const { return true; }
			virtual std::string getPrepackLabelTypeNum3() const = 0;
			virtual void setPrepackLabelTypeNum3(const std::string& l3) = 0;
			virtual bool isPrepackLabelTypeNum3Null() const { return false; }
			virtual void setPrepackLabelTypeNum3Null() = 0;
			virtual bool isPrepackLabelTypeNum3Writable() const { return true; }
			// prodentry
			virtual std::string getProdEntryLabelTypeNum1() const = 0;
			virtual void setProdEntryLabelTypeNum1(const std::string& l1) = 0;
			virtual bool isProdEntryLabelTypeNum1Null() const { return false; }
			virtual void setProdEntryLabelTypeNum1Null() = 0;
			virtual bool isProdEntryLabelTypeNum1Writable() const { return true; }
			virtual std::string getProdEntryLabelTypeNum2() const = 0;
			virtual void setProdEntryLabelTypeNum2(const std::string& l2) = 0;
			virtual bool isProdEntryLabelTypeNum2Null() const { return false; }
			virtual void setProdEntryLabelTypeNum2Null() = 0;
			virtual bool isProdEntryLabelTypeNum2Writable() const { return true; }
			virtual std::string getProdEntryLabelTypeNum3() const = 0;
			virtual void setProdEntryLabelTypeNum3(const std::string& l3) = 0;
			virtual bool isProdEntryLabelTypeNum3Null() const { return false; }
			virtual void setProdEntryLabelTypeNum3Null() = 0;
			virtual bool isProdEntryLabelTypeNum3Writable() const { return true; }
			// selfserv
			virtual std::string getSelfServLabelTypeNum1() const = 0;
			virtual void setSelfServLabelTypeNum1(const std::string& l1) = 0;
			virtual bool isSelfServLabelTypeNum1Null() const { return false; }
			virtual void setSelfServLabelTypeNum1Null() = 0;
			virtual bool isSelfServLabelTypeNum1Writable() const { return true; }
			virtual std::string getSelfServLabelTypeNum2() const = 0;
			virtual void setSelfServLabelTypeNum2(const std::string& l2) = 0;
			virtual bool isSelfServLabelTypeNum2Null() const { return false; }
			virtual void setSelfServLabelTypeNum2Null() = 0;
			virtual bool isSelfServLabelTypeNum2Writable() const { return true; }
			virtual std::string getSelfServLabelTypeNum3() const = 0;
			virtual void setSelfServLabelTypeNum3(const std::string& l3) = 0;
			virtual bool isSelfServLabelTypeNum3Null() const { return false; }
			virtual void setSelfServLabelTypeNum3Null() = 0;
			virtual bool isSelfServLabelTypeNum3Writable() const { return true; }
			// pick5
			virtual std::string getPick5LabelTypeNum1() const = 0;
			virtual void setPick5LabelTypeNum1(const std::string& l1) = 0;
			virtual bool isPick5LabelTypeNum1Null() const { return false; }
			virtual void setPick5LabelTypeNum1Null() = 0;
			virtual bool isPick5LabelTypeNum1Writable() const { return true; }
			virtual std::string getPick5LabelTypeNum2() const = 0;
			virtual void setPick5LabelTypeNum2(const std::string& l2) = 0;
			virtual bool isPick5LabelTypeNum2Null() const { return false; }
			virtual void setPick5LabelTypeNum2Null() = 0;
			virtual bool isPick5LabelTypeNum2Writable() const { return true; }
			virtual std::string getPick5LabelTypeNum3() const = 0;
			virtual void setPick5LabelTypeNum3(const std::string& l3) = 0;
			virtual bool isPick5LabelTypeNum3Null() const { return false; }
			virtual void setPick5LabelTypeNum3Null() = 0;
			virtual bool isPick5LabelTypeNum3Writable() const { return true; }

			// OnlineOrdering
			virtual std::string getOnlineOrderingLabelTypeNum1() const = 0;
			virtual void setOnlineOrderingLabelTypeNum1(const std::string& l1) = 0;
			virtual bool isOnlineOrderingLabelTypeNum1Null() const { return false; }
			virtual void setOnlineOrderingLabelTypeNum1Null() = 0;
			virtual bool isOnlineOrderingLabelTypeNum1Writable() const { return true; }
			virtual std::string getOnlineOrderingLabelTypeNum2() const = 0;
			virtual void setOnlineOrderingLabelTypeNum2(const std::string& l2) = 0;
			virtual bool isOnlineOrderingLabelTypeNum2Null() const { return false; }
			virtual void setOnlineOrderingLabelTypeNum2Null() = 0;
			virtual bool isOnlineOrderingLabelTypeNum2Writable() const { return true; }
			virtual std::string getOnlineOrderingLabelTypeNum3() const = 0;
			virtual void setOnlineOrderingLabelTypeNum3(const std::string& l3) = 0;
			virtual bool isOnlineOrderingLabelTypeNum3Null() const { return false; }
			virtual void setOnlineOrderingLabelTypeNum3Null() = 0;
			virtual bool isOnlineOrderingLabelTypeNum3Writable() const { return true; }

			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTI_H
