/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCT_H
#define PRODUCT_H


#include <vector>
#include <list>

#include "evolution.h"
#include "HfcObject.h"
#include "ProductDevEvent.h"
#include "ProductText.h"
#include "HobDateTime.h"
#include "BarCodeNumber.h"
#include "Tare.h"
#include "WrapperIntakeMode.h"
#include "Label.h"
#include "Weigher.h"
#include "OpModeEvent.h"
#include "CCriticalSection.h"


class Dept;
class Class;
class Nutrifacts;
class ProductText;
class LabelType;
class LabelInfo;
class Graphic;
class Report;
class AbstractSession;
class Configuration;
class ProductTransaction;

class Category;
class Special;
namespace edb
{
class DBProductStatistic;
}

class COOLText;
class COOLPretext;

class DBAccess;
namespace edb
{
class DBProduct;
class DBCoolList;
}
class HfcPrinterStockMetrics;
class HfcWeigherStatus;

class RemotePrintTemplateRequest;


typedef enum ProdSupportFunction
{
	psfNoFunction,
	psfNew,
	psfCopyAndEdit,
	psfUse,
	psfDelete,
	psfDeleteForAll,
	endProdSupportFunction
}   ProdSupportFunction_t;

typedef enum SelfServicePrintOption
{
	SelfServOption_PrintToLabel,
	SelfServOption_PrintToScreen,
	SelfServOption_PrintToScreenAndLabel,
	MaxSelfServPrintOptions
} SelfServicePrintOption_t;


//added ats 10152002
const UINT16 PRPriceChange 			= 0x0001;
const UINT16 PRBadRecord 			= 0x0002;
const UINT16 PRFixedWeight			= 0x0004;
const UINT16 PRChangeRecord			= 0x0008;
const UINT16 PRFluidOunce			= 0x0010; 	//not sure if some of these need to be here!
const UINT16 PRPrintSecurityLabel 	= 0x0040;
const UINT16 PRAllowBackDating 		= 0x1000;
const UINT16 PRProductTotals		= 0x2000;
const UINT16 PRManualChangeDigit	= 0x4000;
const UINT16 PRAutoChangeDigit		= 0x8000;



class Product : public HfcObject
{
	public:

		typedef enum
		{
			PRODUCTTYPE_UNKNOWN,

			PRODUCTTYPE_RANDOMWEIGHT,
			PRODUCTTYPE_FIXEDWEIGHT,
			PRODUCTTYPE_BYCOUNT,
			PRODUCTTYPE_FLUIDOUNCE,

			PRODUCTTYPE_INVALID
		} ProductType;

		typedef enum
		{
			PRODUCTIONPLANNING_NO,

			PRODUCTIONPLANNING_ITEM,
			PRODUCTIONPLANNING_INGREDIENT,
			PRODUCTIONPLANNING_MIXED,
			PRODUCTIONPLANNING_RECIPES_SUGG_SELL,		// recipes and suggestive selling

			PRODUCTIONPLANNING_INVALID
		} ProductionPlanningType;

		typedef enum ProductFieldOffsets
		{
			fieldOffsetProductUnknownField = 0,
			fieldOffsetProductNum,
			fieldOffsetProductType,
			fieldOffsetDescription,

			fieldOffsetBarcodeType,
			fieldOffsetBarcodeFields,					// 5
			fieldOffsetBarcodePrefix,
			fieldOffsetBarcodeProductNum,
			fieldOffsetBarcodeVendorNum,
			fieldOffsetBarcodeEANCode,

			fieldOffsetByCount,							// 10
			fieldOffsetExceptionPrice,
			fieldOffsetUnitPrice,

			fieldOffsetDiscountType,
			fieldOffsetDiscountValue1,
			fieldOffsetDiscountValue2,					// 15
			fieldOffsetDiscountValue3,

			// todo - These shouldn't go here; they are not 'by plu'.
			// They should be in some 'general' Product configuration screen.
			fieldOffsetDiscountLabel1,
			fieldOffsetDiscountLabel2,
			fieldOffsetDiscountLabel3,

			fieldOffsetTareAValue,						// 20
			fieldOffsetProportionalTare,
			fieldOffsetFixedWeight,

			fieldOffsetShelfLifeDays,
			fieldOffsetShelfLifeHours,
			fieldOffsetProductLifeDays,					// 25
			fieldOffsetProductLifeHours,

			fieldOffsetPriceModifier,

			fieldOffsetProductReportingClassNum,		// REPORTING CLASS NUMBER

			fieldOffsetLabelTypeNum1,
			fieldOffsetLabelRotation1,					// 30
			fieldOffsetLabelTypeNum2,
			fieldOffsetLabelRotation2,
			fieldOffsetLabelTypeNum3,
			fieldOffsetLabelRotation3,

			fieldOffsetBackDateDays,					// 35
			fieldOffsetPoints,

			fieldOffsetGraphicName1,
			fieldOffsetGraphicName2,
			fieldOffsetGraphicName3,
			fieldOffsetGraphicName4,					// 40

			//	Label stock dimensions - may need to go with printer messages.
			fieldOffsetLabelTypeWidth1,					//	The width in inches or mm
			fieldOffsetLabelTypeLength1,				//	The length in inches or mm
			fieldOffsetLabelTypeWidth2,					//	The width in inches or mm
			fieldOffsetLabelTypeLength2,				//	The length in inches or mm
			fieldOffsetLabelTypeWidth3,					// 45	//	The width in inches or mm
			fieldOffsetLabelTypeLength3,				//	The length in inches or mm

			fieldOffsetByCountForced,
			fieldOffsetTareAForced,
			fieldOffsetFractionalPriceUnit,
			fieldOffsetPoundsForAmount,					// 50
			fieldOffsetUnitPriceForced,
			fieldOffsetTareType,
			fieldOffsetApplySecurity,
			fieldOffsetDontPrintPackedOn,
			fieldOffsetDontPrintSellBy,					// 55
			fieldOffsetDontPrintBestBefore,
			fieldOffsetLabelPlacement1,
			fieldOffsetLabelPlacement2,
			fieldOffsetLabelPlacement3,
			fieldOffsetFractionalPrice,					// 60
			fieldOffsetForcedFractionalPrice,
			fieldOffsetPoundsForPrice,
			fieldOffsetForcedPoundsFor,

			fieldOffsetSellByDate,
			fieldOffsetSellByTime,						// 65
			fieldOffsetBestBeforeDate,
			fieldOffsetBestBeforeTime,

			fieldOffsetHasNutrifactsFlag,

			fieldOffsetIntakeMode,

			fieldOffsetLabelHasFreqShopper,				// 70		//	One of the labels for this product has Frequent shopper fields.

			fieldOffsetProductGraphicName,
			fieldOffsetProductName1,
			fieldOffsetProductName2,
			fieldOffsetProdCategoryNum,
			fieldOffsetProdSpecialNum,					// 75

			fieldOffsetProdCoolCategoryNum,
			fieldOffsetProdCoolPretextNum,
			fieldOffsetProdCoolDefaultTextNum,
			fieldOffsetProdCoolTextNum,
			fieldOffsetProdCoolShortlistNum,			// 80
			fieldOffsetProdCoolVerifyForced,
			fieldOffsetProdCOOLTrackingText,
			fieldOffsetProdCoolRequired,

			fieldOffsetKeyboardNetWeight,
			fieldOffsetAcceptKbdNetWeight,				// 85		//	One of the labels for this product has the FORCED_KB_WEIGHT field.
	
			// These are used for Production Entry mode.  They are not actual fields in the database.
			fieldOffsetNumOfLabelsForced,
			fieldOffsetNumOfLabels,

			fieldOffsetProductionPlanningType,

			fieldOffsetForcedBackDateDays,
		
			fieldOffsetExpirationDaysForced,			// 90
			fieldOffsetExpirationDays,

			fieldOffsetEANFormatString,				
			fieldOffsetEAN128CFormatString,							// Product Defaults only

			fieldOffsetGS1DatabarGTIN,
			fieldOffsetGS1DatabarAI,					// 95

			fieldOffsetBackDateDate,
			fieldOffsetPortions,
			fieldOffsetShowPortions,

			fieldOffsetBuildAMealSideItemsGroupNum,
			fieldOffsetBuildAMealSideItemsAreFree,		// 100
			fieldOffsetBuildAMealNumberOfSidesIncluded,
			fieldOffsetItemIsBuildAMealSideItem,        // internal flag only, no external access

			fieldOffsetCutTestMeatSpecSheetGroupNum,

			fieldOffsetTareBValue,                 

			fieldOffsetQRCodeFormatString,				// 105
			fieldOffsetDataMatrixFormatString,

			// For Pick-5 project
			fieldOffsetWeightMin,
			fieldOffsetWeightMax,
			fieldOffsetTotalPriceMin,
			fieldOffsetTotalPriceMax,					// 110

			// automatic labels based on mode
			fieldOffsetDefaultLabelTypeNum1,
			fieldOffsetDefaultLabelTypeNum2,
			fieldOffsetDefaultLabelTypeNum3,

			fieldOffsetManualLabelTypeNum1,
			fieldOffsetManualLabelTypeNum2,				// 115
			fieldOffsetManualLabelTypeNum3,

			fieldOffsetPrepackLabelTypeNum1,
			fieldOffsetPrepackLabelTypeNum2,
			fieldOffsetPrepackLabelTypeNum3,

			fieldOffsetProdEntryLabelTypeNum1,			// 120
			fieldOffsetProdEntryLabelTypeNum2,
			fieldOffsetProdEntryLabelTypeNum3,

			fieldOffsetSelfServLabelTypeNum1,
			fieldOffsetSelfServLabelTypeNum2,
			fieldOffsetSelfServLabelTypeNum3,			// 125

			fieldOffsetPick5LabelTypeNum1,
			fieldOffsetPick5LabelTypeNum2,
			fieldOffsetPick5LabelTypeNum3,

			fieldOffsetOnlineOrderingLabelTypeNum1,
			fieldOffsetOnlineOrderingLabelTypeNum2,    // 130
			fieldOffsetOnlineOrderingLabelTypeNum3,
			
			fieldOffsetShelfLifeMinutes,
			fieldOffsetProductLifeMinutes,			
            
            fieldOffsetLabelStockRotation1,
			fieldOffsetLabelStockRotation2,            // 135
			fieldOffsetLabelStockRotation3,
			
				//	IMPORTANT!  To add any params to the Product DOM, insert them before this comment (fieldOffsetEANprefix) [CCD 01/31/11].

			fieldOffsetEANprefix,										// Product Defaults only
			fieldOffsetSpecialStartDate,				
			fieldOffsetProductErrorCode,
			fieldOffsetPluFlags,						// 140				//added for ultima compatibility ats 09302002
			fieldOffsetTotalsFlags,								    	//added for ultima compatibility
			fieldOffsetExceptionPriceTare,								//added for ultima compatibility
			fieldOffsetGraphicNum,						
			fieldOffsetExpandedTextNum,									//r#
			fieldOffsetSpecialMessageNum,				// 145				//s#
			fieldOffsetProdProductNoteNum,							    //n#
			fieldOffsetProdNutrifactNum,								//N#
			fieldOffsetProdCoolShortlistFirstLink,		
			fieldOffsetMarqueeTextNum,									//m#
			fieldOffsetBarcodeUpcNum,					// 150				//up

			fieldOffsetProductText5Num,				
			fieldOffsetProductText6Num,             
			fieldOffsetProductText7Num,					
			fieldOffsetProductText8Num,
			fieldOffsetProductText9Num,                 // 155

			fieldOffsetProductDeptNum,	

			fieldOffsetOrderNum,
			fieldOffsetOrderProductId,					// Product ID per the online order db, not the PLU number
			fieldOffsetOrderCustomerName,
			fieldOffsetOrderPickupTime,					// 160

			endProductFieldOffsets
		}   ProductFieldOffsets_t;


		static const short PRINTING_NO_LABEL = 0;
		static const short PRINTING_PRODUCTION_LABEL = 1;
		static const short PRINTING_NEW_EXPIRATION_LABEL = 2;
		static const short PRINTING_EXISTING_EXPIRATION_LABEL = 3;
		static const short PRINTING_RUN_TOTALS_LABEL = 4;

		static const int MAX_PRODUCT_NUM = 999999;
		static const int MAX_VALID_PLU = 999992;

		Product();
		Product(const char *data, int len, int formatType);
		Product(edb::DBProduct *dbProductRec, DBAccess *access = (DBAccess *)NULL, bool loadLinkedData = true);
		Product(Product *productTemplate, DBAccess *access = 0, AbstractSession *session = 0);

		~Product();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobRecordNum getProductNum() const;
		void setProductNum(HobRecordNum productNum);
		void setProductNum42(HobRecordNum num);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		ProductType getProductType() const;
		void setProductType(ProductType productType);
		static bool isValidProductType(ProductType productType);
		bool isRandomWeight() const;
		bool isByCount() const;
		bool isByCountWithWeight() const;
		bool isFixedWeight() const;
		bool isFluidOunce() const;
		bool isFixedWtOrFluidOz() const;
		bool isDummyProduct() const;

		const HobString &getDescription() const;
		void setDescription(const HobString &description);

		const BarCodeNumber & getBarCodeNum() const;
		BarCodeNumber & getBarCodeNum();
		void setBarCodeNum(BarCodeNumber & barCodeNum);


		int getByCount() const;
		void setByCount(int byCount);
		int getOriginalByCount() const;

		bool isByCountForced() const;
		void setByCountForced(bool byCountForced);

		bool isByCountEntered() const;
		void setByCountEntered(bool entered);


		HobPrice getExceptionPrice() const;
		void setExceptionPrice(HobPrice exceptionPrice);

		HobPrice getUnitPrice() const;
		void setUnitPrice(HobPrice unitPrice);
#if 0
		void setUnitPrice(Double unitPrice);
#endif
		HobPrice getOriginalUnitPrice() const;

		bool isUnitPriceForced() const;
		void setUnitPriceForced(bool unitPriceForced);

		bool isUnitPriceEntered() const;
		void setUnitPriceEntered(bool entered);

		bool isUnitPriceChanged() const;
		void setUnitPriceChanged(bool entered);


		short getDiscountType() const;
		void setDiscountType(short discountType);

		HobPrice getDiscountValue1() const;
		void setDiscountValue1(HobPrice discountValue);

		HobPrice getDiscountValue2() const;
		void setDiscountValue2(HobPrice discountValue);

		HobPrice getDiscountValue3() const;
		void setDiscountValue3(HobPrice discountValue);


		Tare & getTareA();
		void setTareA(HobWeight tare);

		Tare & getTareB();
		void setTareB(HobWeight tare);

		const Tare & getActiveTare() const;
		Tare & getActiveTare();

		bool isTareEntered() const;
		void setTareEntered(bool entered);

		int getProportionalTare() const;
		void setProportionalTare(HobString proportionalTare);
		void setProportionalTare(int proportionalTare);

		HobWeight getFixedWeight() const;
		void setFixedWeight(HobWeight fixedWeight);
		int getOriginalFixedWeight() const;

		int getShelfLifeDays() const;
		void setShelfLifeDays(int shelfLifeDays);
		void adjustShelfLifeDays(int newShelfLifeDays);

		short getShelfLifeHours() const;
		void setShelfLifeHours(short shelfLifeHours);
		void adjustShelfLifeHours(int newShelfLifeHours);

		short getShelfLifeMinutes() const;
		void setShelfLifeMinutes(short shelfLifeMinutes);
		void adjustShelfLifeMinutes(int shelfLifeMinutes);

		HobDateTime getShelfLife();
		void setShelfLife() const;
		void setShelfLife(const HobString &newDate);

		int getProductLifeDays() const;
		void setProductLifeDays(int productLifeDays);

		short getProductLifeHours() const;
		void setProductLifeHours(short productLifeHours);

		short getProductLifeMinutes() const;
		void setProductLifeMinutes(short productLifeMinutes);
		void adjustProductLifeMinutes(int newShelfLifeMinutes);

		HobDateTime getProductLife();
		void setProductLife() const;
		void setProductLife(const HobString &newDate);

		short getPoints() const;
		void setPoints(short points);

		short getPortions() const;
		void setPortions(short value);

		WrapperIntakeMode::IntakeMode getIntakeMode() const;
		void setIntakeMode(WrapperIntakeMode::IntakeMode mode);

		short getPriceModifier() const;
		void setPriceModifier(short priceModifier);

		HobRecordNum getReportingClassNum() const;
		void setReportingClassNum(HobRecordNum reportingClassNum);

		Class *getReportingClass();
		void setReportingClass(Class *reportingClass);

		bool hasNutrifacts() const;
		Nutrifacts *getNutrifacts();
		void setNutrifacts(Nutrifacts *nutrifacts);
		void setNutrifactsTextCol1(HobString nutrifactsText);
		void setNutrifactsTextCol2(HobString nutrifactsText);
		void setNutrifactsTextCol3(HobString nutrifactsText);
		void setNutrifactsTextCol4(HobString nutrifactsText);


		ProductText *getProductText(ProdSupportTextType type);
		void setProductText(ProdSupportTextType type, ProductText *productText);
		void setProductTextText(ProdSupportTextType type, HobString productTextText);


		Graphic *getGraphic(unsigned int graphicIndex) const;
		void setGraphic(unsigned int graphicIndex, Graphic *graphic);

		HobString getGraphicName(unsigned int graphicIndex) const;

		LabelType *getLabelType(short printerNum) const;
		void setLabelType(short printerNum, LabelType *labelType);

		void reverseLabelOrder();
		// When Walmart Cake Label (branch-walmart-cake-label) was merged into trunk "isRemotePrintMsgReceived" no longer existed,
		// so the last parameter was removed.
		//bool selectLabelTypesForMode(OpModeEvent mode, bool isRemotePrintMsgReceived);
		bool selectLabelTypesForMode(OpModeEvent mode);

		void setupLabelTypesForMode();
		int getActiveLabelTypeNumOffsetForMode(short printerNum = 0 /*FirstPrinter*/);
		void updateLabelTypeNumForMode(short printerNum, HobRecordNum newLabelTypeNum);
		HobRecordNum getLabelTypeNum(int fieldOffset) const;
		void setLabelTypeNum(int fieldOffset, HobRecordNum newLabelTypeNum);
		static bool isLabelTypeNumField(int fieldOffset);

		short getLabelRotation(short printerNum) const;
		void setLabelRotation(short printerNum, short rotation);

		short getLabelPlacement(short printerNum) const;
		void setLabelPlacement(short printerNum, short placement);


		bool isApplySecurityLabel() const;
		void setApplySecurityLabel(bool applySecurityLabel);

		bool isDontPrintPackedOn() const;
		void setDontPrintPackedOn(bool dontPrintPackedOn);

		bool isDontPrintSellBy() const;
		void setDontPrintSellBy(bool dontPrintSellBy);

		bool isDontPrintSellByOverridden() const;
		void setDontPrintSellByOverridden(bool override);

		bool isDontPrintBestBefore() const;
		void setDontPrintBestBefore(bool dontPrintBestBefore);

		bool isDontPrintBestBeforeOverridden() const;
		void setDontPrintBestBeforeOverridden(bool override);


		const HobString &getProductGraphicName() const;
		void setProductGraphicName(const HobString &productGraphicName);

		const HobString &getProductName1() const;
		void setProductName1(const HobString &name);

		const HobString &getProductName2() const;
		void setProductName2(const HobString &name);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum spNum);
		Category *getCategory() const;
		void setCategory(Category *cat);

		HobRecordNum getSpecialNum() const;
		void setSpecialNum(HobRecordNum spNum);
		Special *getSpecialProgram() const;
		void setSpecialProgram(Special *special);

		const HobDateTime &getSpecialStartDate() const;
		void setSpecialStartDate(const HobDateTime &specialStartDate);
		void setSpecialStartDate(const HobString &newDate);

		bool updateTotalValuesData();
		void updateOldTotalValuesData(HobWeight netWeight, HfcWeigherStatus * weigherStatus = 0);


		ProductTransaction *getCurrentTransaction();
		const ProductTransaction *getCurrentTransaction() const;
		void setCurrentTransaction(ProductTransaction *trans);
		ProductTransaction *printCurrentTransaction();
		void completeTransaction(ProductTransaction *productTransaction);
		ProductTransaction *findTransaction(HobRecordNum transID);
		bool hasOutstandingTransactions() const;
		ProductTransaction *getFirstPrintingTransaction();
		int numOutstandingTransactions() const;
		void clearOutstandingTransactions();
		void removePreviousTransaction();

		Configuration *getScaleConfig() const;
		void setScaleConfig(Configuration *config);


		bool updateField(int fieldOffset, const HobString &newValue, bool linksMustExist = false, int formatType = BrowserFormat);
		void updateFieldProductText(ProdSupportTextType type, const HobString &newValue, bool linksMustExist);

		void updateRecord(Product *newData, AbstractSession *session = 0, bool linksMustExist = false);
		void updateRecordProductText(ProdSupportTextType type, Product *newData, bool linksMustExist);

		bool isFieldQuoted(int fieldOffset) const;
		virtual void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBProduct *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static Product *findRecord(HobRecordNum productNum, HobRecordNum deptNum, DBAccess *access = (DBAccess *)NULL, bool inOperateMode = false, bool isOnlineOrderingItem = false);

		static void initFieldLists();			//initializes a list of field offset in either ultima, Evolution or Browser format


		// overrides from HfcObject class
		virtual HobString getMyKeyValue() const;
		virtual EvolDeviceType getMyEvolDevice() const;


		void initProductTransaction();

		bool setOpModeByCount();
		void setOpModeFixedWt();

		static bool isFieldPreImage(short field);
		void setBarcodeFields(Product* p);
		bool needsNoWeigherStatus() const;
		HobString getFormattedFixedWeightString(int format) const;

		void getFieldNameAsString(int fieldOffset, HobString &fieldName);
		void show();

		bool areForcedFieldsEntered() const;
		bool isTotalPriceOutOfRange() const;
		bool isReadyToPrint() const;
		ProductDevError getNotReadyToPrintErrorCode();
		bool hasLabelTypeError() const;

		bool hasFrequentShopperLabel() const;
		void testForFrequentShopperLabel();

		// The following methods return the width and height of a specific label
		// form. If the label form is not found it sets both parameters to defaults.
		void getProductTextSize(int textType, int& width, int& height, std::string& fontName, int& pointSize);
		void getDescriptionSize(int& width, int& height, std::string& fontName, int& pointSize);
		void getCoolTrackingTextSize(int& width, int& height, std::string& fontName, int& pointSize);
		bool getLabelFormSizeAndFont(FieldType fieldType, int& width, int& height, std::string& fontName, int& pointSize);

		HobRecordNum getCoolCategoryNum() const;
		void setCoolCategoryNum(HobRecordNum recordNum);
		HobRecordNum getCoolPretextNum() const;
		void setCoolPretextNum(HobRecordNum recordNum);
		HobRecordNum getCoolDefaultTextNum() const;
		void setCoolDefaultTextNum(HobRecordNum recordNum);
		HobRecordNum getCoolTextNum() const;
		void setCoolTextNum(HobRecordNum recordNum);
		bool isCoolTextSelected() const;
		void setCoolTextSelected(bool selected);
		HobRecordNum getCoolShortlistNum() const;
		void setCoolShortlistNum(HobRecordNum recordNum);
		HobRecordNum getCoolShortlistFirstLink() const;
		void setCoolShortlistFirstLink(HobRecordNum recordNum);
		const HobString &getCoolTrackingText() const;
		void setCoolTrackingText(const HobString &theText);
		void saveCoolList(edb::DBCoolList *dbCoolList);

		bool isCoolVerifyForced() const;
		void setCoolVerifyForced(bool forced);

		std::string getCOOLTextToPrint() const;

		void generateEmbeddedLabels();

		int getBackDateDays() const;
		void setBackDateDays(int days);

		bool isBackDateDaysForced() const;
		void setBackDateDaysForced(bool forced);

		bool isBackDateDaysEntered() const;
		void setBackDateDaysEntered(bool entered);

		HobDateTime getBackdateDate();
		void setBackdateDate() const;
		void setBackdateDateTo(const HobString &newDate);

		bool isCoolRequired() const;
		void setCoolRequired(bool value);

		const HobString &getRawBarCodeData() const;
		void setRawBarCodeData(const HobString &rbcd);

		short getLabelBeingPrinted() const;
		void setLabelBeingPrinted(short whichLabel);

		void setExpirationLabel(LabelType *value);
		void setRunTotalsLabel(LabelType *value);

		// I dont think for the most part these are even used - 9-22-17
		// Used for Production Entry mode.
		bool isNumOfLabelsForced() const;
		void setNumOfLabelsForced(bool forced);

		bool isNumOfLabelsEntered() const;
		void setNumOfLabelsEntered(bool entered);

		// Used for Production Planning (FPP)
		ProductionPlanningType getProductionPlanningType() const;
		void setProductionPlanningType(ProductionPlanningType type);

		HobWeight getKeyboardNetWeight() const;
		void setKeyboardNetWeight(HobWeight weight);

		bool hasKbdNetWeightLabel() const;
		void testForKbdNetWeightLabel();

		void testForPortionsOnLabel();
		void checkSpecialFieldsOnLabel();

		int findOffsetInUIFieldList(int fieldOffset);

		bool isPrintingInProductionEntryMode() const;
		void setPrintingInProductionEntryMode(bool printing);

		bool isExpirationDaysForced() const;
		void setExpirationDaysForced(bool forced);

		int getExpirationDays() const;
		void setExpirationDays(int days);


		HobRecordNum getMealSideItemsGroupNum() const;
		void setMealSideItemsGroupNum(HobRecordNum groupNum);

		bool getMealSideItemsAreFree() const;
		void setMealSideItemsAreFree(bool free);

		int getMealNumSidesIncluded() const;
		void setMealNumSidesIncluded(int num);

		bool getItemIsMealSideItem() const;
		void setItemIsMealSideItem(bool isSideItem);

		bool isMealBuilderItem() const {
			return (( 0 != getMealSideItemsGroupNum() ) && ( 0 < getMealNumSidesIncluded() ) &&
				(( PRODUCTTYPE_BYCOUNT == getProductType() ) || ( PRODUCTTYPE_FIXEDWEIGHT == getProductType() )));}


		HobRecordNum getCutTestMeatSpecSheetGroupNum() const;
		void setCutTestMeatSpecSheetGroupNum(HobRecordNum groupNum);

		bool isCutTestMeatSpecSheetItem() const { return ( 0 != getCutTestMeatSpecSheetGroupNum() ); }

		bool isPrintLabelToPrinter() const { return printLabelToPrinter_; }
		void setPrintLabelToPrinter(bool toPrinter) { printLabelToPrinter_ = toPrinter; }


		static const std::string& getProductTypeAsString(ProductType type);
		static const std::string& getProductionPlanningTypeAsString(ProductionPlanningType type);

		ProductTransaction *createProductTransaction();

		// For the Walmart remote Print Template project (cake label)
		RemotePrintTemplateRequest *getRemotePrintTemplateRequest() const { return remotePrintTemplateRequest; }
		void setRemotePrintTemplateRequest(RemotePrintTemplateRequest *rptr)  { remotePrintTemplateRequest = rptr; }

		bool isPrintingRemoteLabel() const { return printingRemoteLabel; }
		void setPrintingRemoteLabel(bool printing) { printingRemoteLabel = printing; }

		bool hookBeforePrint() const {return hookBeforePrint_;}
		void setHookBeforePrint( bool value ) {hookBeforePrint_ = value;}

		bool hookAfterPrint() const {return hookAfterPrint_;}
		void setHookAfterPrint( bool value ) {hookAfterPrint_ = value;}


		// For Pick-5 project
		HobWeight getWeightMin() const;
		void setWeightMin(HobWeight w);

		HobWeight getWeightMax() const;
		void setWeightMax(HobWeight w);

		HobPrice getTotalPriceMin() const;
		void setTotalPriceMin(HobPrice p);

		HobPrice getTotalPriceMax() const;
		void setTotalPriceMax(HobPrice p);

		// For the 'label types by mode' project
		// default - these map to the original LbNum1, LbNum2, LbNum3 database fields.
		HobRecordNum getDefaultLabelTypeNum1() const { return defaultLabelTypeNum1; }
		void setDefaultLabelTypeNum1(HobRecordNum label) { defaultLabelTypeNum1 = label; }

		HobRecordNum getDefaultLabelTypeNum2() const { return defaultLabelTypeNum2; }
		void setDefaultLabelTypeNum2(HobRecordNum label) { defaultLabelTypeNum2 = label; }

		HobRecordNum getDefaultLabelTypeNum3() const { return defaultLabelTypeNum3; }
		void setDefaultLabelTypeNum3(HobRecordNum label) { defaultLabelTypeNum3 = label; }
		// manual
		HobRecordNum getManualLabelTypeNum1() const { return manualLabelTypeNum1; }
		void setManualLabelTypeNum1(HobRecordNum label) { manualLabelTypeNum1 = label; }

		HobRecordNum getManualLabelTypeNum2() const { return manualLabelTypeNum2; }
		void setManualLabelTypeNum2(HobRecordNum label) { manualLabelTypeNum2 = label; }

		HobRecordNum getManualLabelTypeNum3() const { return manualLabelTypeNum3; }
		void setManualLabelTypeNum3(HobRecordNum label) { manualLabelTypeNum3 = label; }
		// prepack
		HobRecordNum getPrepackLabelTypeNum1() const { return prepackLabelTypeNum1; }
		void setPrepackLabelTypeNum1(HobRecordNum label) { prepackLabelTypeNum1 = label; }

		HobRecordNum getPrepackLabelTypeNum2() const { return prepackLabelTypeNum2; }
		void setPrepackLabelTypeNum2(HobRecordNum label) { prepackLabelTypeNum2 = label; }

		HobRecordNum getPrepackLabelTypeNum3() const { return prepackLabelTypeNum3; }
		void setPrepackLabelTypeNum3(HobRecordNum label) { prepackLabelTypeNum3 = label; }
		// prodEntry
		HobRecordNum getProdEntryLabelTypeNum1() const { return prodEntryLabelTypeNum1; }
		void setProdEntryLabelTypeNum1(HobRecordNum label) { prodEntryLabelTypeNum1 = label; }

		HobRecordNum getProdEntryLabelTypeNum2() const { return prodEntryLabelTypeNum2; }
		void setProdEntryLabelTypeNum2(HobRecordNum label) { prodEntryLabelTypeNum2 = label; }

		HobRecordNum getProdEntryLabelTypeNum3() const { return prodEntryLabelTypeNum3; }
		void setProdEntryLabelTypeNum3(HobRecordNum label) { prodEntryLabelTypeNum3 = label; }
		// selfServ
		HobRecordNum getSelfServLabelTypeNum1() const { return selfServLabelTypeNum1; }
		void setSelfServLabelTypeNum1(HobRecordNum label) { selfServLabelTypeNum1 = label; }

		HobRecordNum getSelfServLabelTypeNum2() const { return selfServLabelTypeNum2; }
		void setSelfServLabelTypeNum2(HobRecordNum label) { selfServLabelTypeNum2 = label; }

		HobRecordNum getSelfServLabelTypeNum3() const { return selfServLabelTypeNum3; }
		void setSelfServLabelTypeNum3(HobRecordNum label) { selfServLabelTypeNum3 = label; }
		// pick5
		HobRecordNum getPick5LabelTypeNum1() const { return pick5LabelTypeNum1; }
		void setPick5LabelTypeNum1(HobRecordNum label) { pick5LabelTypeNum1 = label; }

		HobRecordNum getPick5LabelTypeNum2() const { return pick5LabelTypeNum2; }
		void setPick5LabelTypeNum2(HobRecordNum label) { pick5LabelTypeNum2 = label; }

		HobRecordNum getPick5LabelTypeNum3() const { return pick5LabelTypeNum3; }
		void setPick5LabelTypeNum3(HobRecordNum label) { pick5LabelTypeNum3 = label; }

		bool isPick5Active() const;

		// OnlineOrdering
		HobRecordNum getOnlineOrderingLabelTypeNum1() const { return onlineOrderingLabelTypeNum1; }
		void setOnlineOrderingLabelTypeNum1(HobRecordNum label) { onlineOrderingLabelTypeNum1 = label; }

		HobRecordNum getOnlineOrderingLabelTypeNum2() const { return onlineOrderingLabelTypeNum2; }
		void setOnlineOrderingLabelTypeNum2(HobRecordNum label) { onlineOrderingLabelTypeNum2 = label; }

		HobRecordNum getOnlineOrderingLabelTypeNum3() const { return onlineOrderingLabelTypeNum3; }
		void setOnlineOrderingLabelTypeNum3(HobRecordNum label) { onlineOrderingLabelTypeNum3 = label; }

		bool isCancelledKbdNetWeight() const { return cancelledKbdNetWeight; }


	protected:

		HobRecordNum deptNum;

		HobRecordNum productNum;

		HobErrorCode errorCode;

		/** Random weight, fixed weight descriptions See USM pg 5-12 */
		ProductType productType;

		HobString description;

		BarCodeNumber barCodeNum;

		int byCount;

		bool byCountForced;

		bool byCountEntered;

		HobPrice exceptionPrice;

		HobString exceptionPriceTare;  // this could either be a HobPrice or a HobWeight, so I'm just setting up as a string to hold the value received

		HobPrice unitPrice;

		bool unitPriceForced;

		bool unitPriceEntered;

		bool unitPriceChanged;

		short discountType;

		/**
		 * This is used for regular price, discount price, cents off, percent off, percent added, package discount.
		 * The unitPrice should not be greater than the regularPrice.
		 * The discount price and cents off should not be greater than the unitPrice.
		 */
		HobPrice discountValue1;
		HobPrice discountValue2;
		HobPrice discountValue3;

		Tare tareA;
		Tare tareB;

		bool tareEntered;

		int proportionalTare;

		HobWeight fixedWeight;

		int originalFixedWeight;

		int shelfLifeDays;

		/**
		 * DPS-5.1.1
		 * Valid values are 0 to 23. (24 hours should be entered as: shelfLifeDays = 1,  and shelfLifeHours = 0)
		 * The initial value should be 0.
		 * Separating the days and hours fields allows easier backwards compatibility.
		 */
		short shelfLifeHours;

		// valid values are 0-59
		short shelfLifeMinutes;

		mutable HobDateTime shelfLife;

		int productLifeDays;

		/**
		 * DPS-5.1.1
		 * Valid values are 0 to 23. (24 hours should be entered as: productLifeDays = 1,  and productLifeHours = 0)
		 * The initial value should be 0.
		 * Separating the days and hours fields allows easier backwards compatibility.
		 */
		short productLifeHours;

		// valid values are 0-59
		short productLifeMinutes;

		mutable HobDateTime productLife;

		short points;

		WrapperIntakeMode::IntakeMode intakeMode;

		short priceModifier;

		/** @clientCardinality 0..*
		 * @supplierCardinality 1*/
		Class *reportingClass;

		/**
		 * @clientCardinality 1
		 * @directed
		 */
		Nutrifacts *nutrifacts;


		std::vector<ProductText *> productTexts_;


		std::vector<Graphic *> graphics;

		std::vector<LabelType *> labelTypes;
		//SOF-4043 
		std::vector<LabelType *> revOrderTypes;


		/** Could be between 0 and 359 inclusive;
		 *  although, only 0, 90, 180, and 270 are currently supported.
		 */
		std::vector<short> labelRotations;

		/** Could be ;
		 *  although, only 0, 90, 180, and 270 are currently supported.
		 */
		std::vector<short> labelPlacements;

		/**
		 * This flag indicates whether a security label should be applied to a package.
		 *
		 * This functionality does not necessarily need to be kept in its own flag field,
		 * if the code to determine whether a security label should be applied is fast enough
		 * to execute.
		 *
		 * A security label needs to be applied if the item's unit price > minUnitPriceToApplySecurityLabel,
		 * maintained in the
		 *  Configuration class, or if the current product's total cost >
		 * minTotalPriceToApplySecurityLabel, maintained in the
		 *  Configuration class.
		 *
		 * See plu_lookup.c, TestPLUForSecurityLabel() function, for the business rule code for
		 * determining if a security label should be applied.
		 */
		bool applySecurityLabel;

		bool dontPrintPackedOn;
		bool dontPrintSellBy;
		bool dontPrintBestBefore;

		bool dontPrintSellByOverridden;
		bool dontPrintBestBeforeOverridden;

		HobString productGraphicName;

		HobString productName1;
		HobString productName2;

		Category *category;

		Special *specialProgram;

		HobDateTime specialStartDate;

		bool labelHasFrequentShopper;

		HobRecordNum coolCategoryNum;
		HobRecordNum coolPretextNum;
		HobRecordNum coolDefaultTextNum;
		HobRecordNum coolTextNum;
		HobString	 coolTrackingText;
		bool		 coolVerifyForced;
		HobRecordNum coolShortlistNum;		
		HobRecordNum coolShortlistFirstLink;    //	This will be a temporary holding place when loading an Ultima/Quantum database.
		bool		 coolTextSelected;
        bool		 coolRequiredText;

		int backDateDays;				//	Number of days to back date a product. Used in Operate mode only.

		// This field is set by the ProductManager, since its value is based on the TotalType.allowProductBackDating attribute of the currently active Total Type.
		// There is no way for a product to know the currently active TotalType, but the ProductManager can get it from its Session.
		bool backDateDaysForced;
		bool backDateDaysEntered;
		
		mutable HobDateTime backdateDate;		//	The actual packed on date to use for backdating.

		// the flag we will use to indicate that the RTC is to print the Shelf Life Expiration Label instead of the primary label type.
		short labelBeingPrinted;

		LabelType *shelfLifeExpirationLabelType;
		LabelType *runTotalsLabelType;


		HobRecordNum mealSideItemsGroupNum;

		bool mealSideItemsAreFree;

		int mealNumSidesIncluded;

		bool itemIsMealSideItem;


		HobRecordNum cutTestMeatSpecSheetGroupNum;

		// For Pick-5 project
		HobWeight weightMin;
		HobWeight weightMax;
		HobPrice totalPriceMin;
		HobPrice totalPriceMax;

		// For the 'label types by mode' project
		int fieldOffsetOfFirstLabelTypeNum;

		HobRecordNum defaultLabelTypeNum1;
		HobRecordNum defaultLabelTypeNum2;
		HobRecordNum defaultLabelTypeNum3;
		HobRecordNum manualLabelTypeNum1;
		HobRecordNum manualLabelTypeNum2;
		HobRecordNum manualLabelTypeNum3;
		HobRecordNum prepackLabelTypeNum1;
		HobRecordNum prepackLabelTypeNum2;
		HobRecordNum prepackLabelTypeNum3;
		HobRecordNum prodEntryLabelTypeNum1;
		HobRecordNum prodEntryLabelTypeNum2;
		HobRecordNum prodEntryLabelTypeNum3;
		HobRecordNum selfServLabelTypeNum1;
		HobRecordNum selfServLabelTypeNum2;
		HobRecordNum selfServLabelTypeNum3;
		HobRecordNum pick5LabelTypeNum1;
		HobRecordNum pick5LabelTypeNum2;
		HobRecordNum pick5LabelTypeNum3;
		HobRecordNum onlineOrderingLabelTypeNum1;
		HobRecordNum onlineOrderingLabelTypeNum2;
		HobRecordNum onlineOrderingLabelTypeNum3;

		/**
		 * @supplierCardinality 0..*
		 */
		ProductTransaction *currentProductTransaction_;

		std::list<ProductTransaction *> printingTransactions;
		std::list<ProductTransaction *> completedTransactions; // FT-960

		DBAccess *dbAccess;

		edb::DBProduct *myDbRecord;

		Configuration *scaleConfig;

		void init();

		//added ats 10152002
		mutable UINT16 flags;		//used for the ultima format only
		void createUltimaFlag() const;
		void setPriceModifier(HobString newPriceModifier);

		void typeUnknownProduct();
		void applyRulesToReceivedFields(int formatType);

		mutable COOLText *currentCOOLText;
		mutable COOLPretext *currentCOOLPretext;

		bool coolRequired;
		HobString rawBarCodeData;

		// Used for Production Entry mode.
		bool numOfLabelsForced;
		bool numOfLabelsEntered;

		// Used for Production Planning (FPP)
		ProductionPlanningType productionPlanningType;
		
		// Used for Keyboard Net weight (Turkey Weigher)
		HobWeight keyboardNetWeight;
		bool acceptKbdNetWeight;
		bool cancelledKbdNetWeight;

		bool printingInProductionEntryMode;

		bool expirationDaysForced;
		int expirationDays;
		bool expirationDaysEntered;

		short portions;
		bool portionsOnLabel;		//	Show this field if it is on the label.

		// Used to indicate whether any labels for this product will be printed to a printer, or to a file that will be displayed to the screen.
		// If both, then set this attribute to true.
		bool printLabelToPrinter_;

		// For the Walmart remote Print Template project (cake label)
		RemotePrintTemplateRequest *remotePrintTemplateRequest;

		bool printingRemoteLabel;
		//bool reversed;

		bool hookBeforePrint_;
		bool hookAfterPrint_;

		//added ats 09272002
		static std::list<ProductFieldOffsets> evolFieldList, ultimaFieldList, uiFieldList, dbindependentFieldList;

		static const std::string _PRODUCTTYPE_UNKNOWN_Name_;
		static const std::string _PRODUCTTYPE_RANDOMWEIGHT_Name_;
		static const std::string _PRODUCTTYPE_FIXEDWEIGHT_Name_;
		static const std::string _PRODUCTTYPE_BYCOUNT_Name_;
		static const std::string _PRODUCTTYPE_FLUIDOUNCE_Name_;

		static const std::string _PRODUCTIONPLANNING_NO_Name_;
		static const std::string _PRODUCTIONPLANNING_ITEM_Name_;
		static const std::string _PRODUCTIONPLANNING_INGREDIENT_Name_;
		static const std::string _PRODUCTIONPLANNING_MIXED_Name_;
		static const std::string _PRODUCTIONPLANNING_RECIPES_SUGG_SELL_Name_;
		static const std::string _PRODUCTIONPLANNING_INVALID_Name_;

		static std::list<LabelType *> labelTypesToDelete;
		static Util::CSmartCriticalSection labelTypesToDeleteCS;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //PRODUCT_H




