#include "edb.h"
#include DBPRODUCT_INC
#include DBCOOLLIST_INC
//#include DEBUG_INC

#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProduct::DBProduct(edb_adapter::DBProductI* d)
		: decorated_(d)
{
}


DBProduct::DBProduct(const DBProduct& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBProduct::~DBProduct()
{
	if ( decorated_ )
	{
		delete decorated_;
		decorated_ = NULL;
	}
}


void DBProduct::setCoolList(const DBCoolList* list)
{
	if ( decorated_ && list )
		decorated_->setCoolList(list->decorated_);
}


std::ostream& operator<<(std::ostream& os, const DBProduct& rec)
{
	os << "DBProduct PLU# " << rec.getPLU() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isTypeNull() )
		os << ", Type: " << rec.getType();
	if ( !rec.isReportingClassNull() )
		os << ", ReportingClass: " << rec.getReportingClass();
	if ( !rec.isByCountNull() )
		os << ", ByCount: " << rec.getByCount();
	if ( !rec.isForceByCountNull() )
		os << ", ForceByCount: " << rec.getForceByCount();
	if ( !rec.isPriceNull() )
		os << ", Price: " << rec.getPrice();
	if ( !rec.isPriceFactorNull() )
		os << ", PriceFactor: " << rec.getPriceFactor();
	if ( !rec.isForcePriceNull() )
		os << ", ForcePrice: " << rec.getForcePrice();
	if ( !rec.isDiscountPrice1Null() )
		os << ", DiscountPrice1: " << rec.getDiscountPrice1();
	if ( !rec.isDiscountPrice2Null() )
		os << ", DiscountPrice2: " << rec.getDiscountPrice2();
	if ( !rec.isDiscountPrice3Null() )
		os << ", DiscountPrice3: " << rec.getDiscountPrice3();
	if ( !rec.isDiscountMethodNull() )
		os << ", DiscountMethod: " << rec.getDiscountMethod();
	if ( !rec.isExceptionPriceNull() )
		os << ", ExceptionPrice: " << rec.getExceptionPrice();
	if ( !rec.isDescriptionNull() )
		os << ", Description: " << rec.getDescription();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isBarCodePrefixNull() )
		os << ", BarCode Prefix: " << rec.getBarCodePrefix();
	if ( !rec.isBarCodeProductNumberNull() )
		os << ", BarCode ProductNumber: " << rec.getBarCodeProductNumber();
	if ( !rec.isBarCodeVendorNumberNull() )
		os << ", BarCode VendorNumber: " << rec.getBarCodeVendorNumber();
	if ( !rec.isBarCodeEANCodeNull() )
		os << ", BarCode EANCode: " << rec.getBarCodeEANCode();
	if ( !rec.isRawBarCodeNumberNull() )
		os << ", Raw BarCode Number: " << rec.getRawBarCodeNumber();
	if ( !rec.isNetWeightNull() )
		os << ", NetWeight: " << rec.getNetWeight();
	if ( !rec.isShelfLifeDaysNull() )
		os << ", ShelfLifeDays: " << rec.getShelfLifeDays();
	if ( !rec.isShelfLifeHoursNull() )
		os << ", ShelfLifeHours: " << rec.getShelfLifeHours();
	if (!rec.isShelfLifeMinutesNull())
		os << ", ShelfLifeMinutes: " << rec.getShelfLifeMinutes();
	if ( !rec.isDontPrintShelfLifeNull() )
		os << ", DontPrintShelfLife: " << rec.getDontPrintShelfLife();
	if ( !rec.isProductLifeDaysNull() )
		os << ", ProductLifeDays: " << rec.getProductLifeDays();
	if ( !rec.isProductLifeHoursNull() )
		os << ", ProductLifeHours: " << rec.getProductLifeHours();
	if (!rec.isProductLifeMinutesNull())
		os << ", ProductLifeMinutes: " << rec.getProductLifeMinutes();
	if ( !rec.isDontPrintProductLifeNull() )
		os << ", DontPrintProductLife: " << rec.getDontPrintProductLife();
	if ( !rec.isDontPrintPackedOnNull() )
		os << ", DontPrintPackedOn: " << rec.getDontPrintPackedOn();
	if ( !rec.isSpecialMessageNull() )
		os << ", SpecialMessage: " << rec.getSpecialMessage();
	if ( !rec.isTextNull() )
		os << ", Text: " << rec.getText();
	if ( !rec.isNutritionNull() )
		os << ", Nutrition: " << rec.getNutrition();
	if ( !rec.isMarqueeNull() )
		os << ", Marquee: " << rec.getMarquee();
	if ( !rec.isNoteNull() )
		os << ", Note: " << rec.getNote();
	if ( !rec.isCategoryNull() )
		os << ", Category: " << rec.getCategory();
	if ( !rec.isGraphic1Null() )
		os << ", Graphic1: " << rec.getGraphic1();
	if ( !rec.isGraphic2Null() )
		os << ", Graphic2: " << rec.getGraphic2();
	if ( !rec.isGraphic3Null() )
		os << ", Graphic3: " << rec.getGraphic3();
	if ( !rec.isGraphic4Null() )
		os << ", Graphic4: " << rec.getGraphic4();
	if ( !rec.isImageNull() )
		os << ", Image: " << rec.getImage();
	if ( !rec.isLabel1Null() )
		os << ", Label1: " << rec.getLabel1();
	if ( !rec.isLabel1RotationNull() )
		os << ", Label1Rotation: " << rec.getLabel1Rotation();
	if ( !rec.isLabel1PlacementNull() )
		os << ", Label1Placement: " << rec.getLabel1Placement();
	if ( !rec.isLabel2Null() )
		os << ", Label2: " << rec.getLabel2();
	if ( !rec.isLabel2RotationNull() )
		os << ", Label2Rotation: " << rec.getLabel2Rotation();
	if ( !rec.isLabel2PlacementNull() )
		os << ", Label2Placement: " << rec.getLabel2Placement();
	if ( !rec.isLabel3Null() )
		os << ", Label3: " << rec.getLabel3();
	if ( !rec.isLabel3RotationNull() )
		os << ", Label3Rotation: " << rec.getLabel3Rotation();
	if ( !rec.isLabel3PlacementNull() )
		os << ", Label3Placement: " << rec.getLabel3Placement();
	if ( !rec.isProportionalTareNull() )
		os << ", ProportionalTare: " << rec.getProportionalTare();
	if ( !rec.isTareNull() )
		os << ", Tare: " << rec.getTare();
	if ( !rec.isForceTareNull() )
		os << ", ForceTare: " << rec.getForceTare();
	if ( !rec.isBonusPointsNull() )
		os << ", BonusPoints: " << rec.getBonusPoints();
	if ( !rec.isIntakeModeNull() )
		os << " [IntakeMode: " << rec.getIntakeMode();
	if ( !rec.isApplySecurityLabelNull() )
		os << ", ApplySecurityLabel: " << (rec.getApplySecurityLabel()?'Y':'N');
	if ( !rec.isSpecialNumberNull() )
		os << " [SpecialNumber: " << rec.getSpecialNumber();
	if ( !rec.isSpecialStartDateNull() )
		os << " [SpecialStartDate: " << rec.getSpecialStartDate();
	if ( !rec.isCurrentCoolTextNull() )
		os << ", CurrentCoolText: " << rec.getCurrentCoolText();
	if ( !rec.isDefaultCoolTextNull() )
		os << ", DefaultCoolText: " << rec.getDefaultCoolText();
	if ( !rec.isCoolCategoryNull() )
		os << ", CoolCategory: " << rec.getCoolCategory();
	if ( !rec.isCoolPreTextNull() )
		os << ", CoolPreText: " << rec.getCoolPreText();
	if ( !rec.isCoolTrackingTextNull() )
		os << ", CoolTrackingText: " << rec.getCoolTrackingText();
	if ( !rec.isForceCoolReviewNull() )
		os << ", ForceCoolReview: " << rec.getForceCoolReview();
	if ( !rec.isCoolRequiredNull() )
		os << ", CoolRequired: " << rec.getCoolRequired();
	if ( !rec.isProductionPlanningTypeNull() )
		os << "ProductionPlanningType: " << rec.getProductionPlanningType();
	if ( !rec.isPortionsNull() )
		os << ", Portions: " << rec.getPortions();
	if ( !rec.isMealSideItemsGroupNumNull() )
		os << ", MealSideItemsGroupNum: " << rec.getMealSideItemsGroupNum();
	if ( !rec.isMealSideItemsAreFreeNull() )
		os << ", MealSideItemsAreFree: " << rec.getMealSideItemsAreFree();
	if ( !rec.isMealNumSidesIncludedNull() )
		os << ", MealNumSidesIncluded: " << rec.getMealNumSidesIncluded();
	if ( !rec.isCutTestMeatSpecSheetGroupNumNull() )
		os << ", CutTestMeatSpecSheetGroupNum: " << rec.getCutTestMeatSpecSheetGroupNum();
	if (!rec.isTareBNull())
		os << ", TareB: " << rec.getTareB();
	// For Pick-5 project
	if ( !rec.isWeightMinNull() )
		os << ", WeightMin: " << rec.getWeightMin();
	if (!rec.isWeightMaxNull())
		os << ", WeightMax: " << rec.getWeightMax();
	if (!rec.isTotalPriceMinNull())
		os << ", TotalPriceMin: " << rec.getTotalPriceMin();
	if (!rec.isTotalPriceMaxNull())
		os << ", TotalPriceMax: " << rec.getTotalPriceMax();
	// For automatic mode labels
	if (!rec.isManualLabelTypeNum1Null())
		os << ", ManualLabelTypeNum1: " << rec.getManualLabelTypeNum1();
	if (!rec.isManualLabelTypeNum2Null())
		os << ", ManualLabelTypeNum2: " << rec.getManualLabelTypeNum2();
	if (!rec.isManualLabelTypeNum3Null())
		os << ", ManualLabelTypeNum3: " << rec.getManualLabelTypeNum3();
	if (!rec.isPrepackLabelTypeNum1Null())
		os << ", PrepackLabelTypeNum1: " << rec.getPrepackLabelTypeNum1();
	if (!rec.isPrepackLabelTypeNum2Null())
		os << ", PrepackLabelTypeNum2: " << rec.getPrepackLabelTypeNum2();
	if (!rec.isPrepackLabelTypeNum3Null())
		os << ", PrepackLabelTypeNum3: " << rec.getPrepackLabelTypeNum3();
	if (!rec.isProdEntryLabelTypeNum1Null())
		os << ", ProdEntryLabelTypeNum1: " << rec.getProdEntryLabelTypeNum1();
	if (!rec.isProdEntryLabelTypeNum2Null())
		os << ", ProdEntryLabelTypeNum2: " << rec.getProdEntryLabelTypeNum2();
	if (!rec.isProdEntryLabelTypeNum3Null())
		os << ", ProdEntryLabelTypeNum3: " << rec.getProdEntryLabelTypeNum3();
	if (!rec.isSelfServLabelTypeNum1Null())
		os << ", SelfServLabelTypeNum1: " << rec.getSelfServLabelTypeNum1();
	if (!rec.isSelfServLabelTypeNum2Null())
		os << ", SelfServLabelTypeNum2: " << rec.getSelfServLabelTypeNum2();
	if (!rec.isSelfServLabelTypeNum3Null())
		os << ", SelfServLabelTypeNum3: " << rec.getSelfServLabelTypeNum3();
	if (!rec.isPick5LabelTypeNum1Null())
		os << ", Pick5LabelTypeNum1: " << rec.getPick5LabelTypeNum1();
	if (!rec.isPick5LabelTypeNum2Null())
		os << ", Pick5LabelTypeNum2: " << rec.getPick5LabelTypeNum2();
	if (!rec.isPick5LabelTypeNum3Null())
		os << ", Pick5LabelTypeNum3: " << rec.getPick5LabelTypeNum3();
	if (!rec.isOnlineOrderingLabelTypeNum1Null())
		os << ", OnlineOrderingLabelTypeNum1: " << rec.getOnlineOrderingLabelTypeNum1();
	if (!rec.isOnlineOrderingLabelTypeNum2Null())
		os << ", OnlineOrderingLabelTypeNum2: " << rec.getOnlineOrderingLabelTypeNum2();
	if (!rec.isOnlineOrderingLabelTypeNum3Null())
		os << ", OnlineOrderingLabelTypeNum3: " << rec.getOnlineOrderingLabelTypeNum3();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
