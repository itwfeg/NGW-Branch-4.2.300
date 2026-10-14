/* Evolution Scale Project

 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#define USE_NEW_EVOL_TAGS

#include "Product.h"
#include "OnlineOrderProduct.h"
#include "ProductDefault.h"
#include "MetSig.h"
#include "ProductTags.h"	
#include "ProductDevEvent.h"
#include "ProductTransaction.h"
#include "Class.h"
#include "Nutrifacts.h"
#include "ProductText.h"
#include "Graphic.h"
#include "HfcError.h"
#include "HfcErrorGraphic.h"
#include "RemotePrintTemplateRequest.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerTSV.h"
#include "HobTokenizerUS.h"		
#include "HobTokenTagged.h"
#include "HobList.h"

#include "Configuration.h"
#include "SessionManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Rounding.h"
#include "LabelTypeManager.h"

#include "WriteSingleField.h"

#include "FrequentShopper.h"

#include "HfcWeigherStatus.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductsRM.h"
#include "DBProduct.h"
#include "DBCoolListsRM.h"
#include "DBCoolList.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolListText.h"
#include "DBLabelsRM.h"
#include "DBROMLabelsRM.h"

#include "MultiLanguageText.h"

#include "MemShow.h"

#include "Special.h"
#include "Category.h"

#include "LabelApplier.h"

#include "COOLText.h"
#include "COOLPretext.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"
#include "math.h"
#include <string>
#include <map>

using namespace std;


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int Product::_dataType_ = 0;
	std::string Product::_dataTypeName_("Product"); 
#endif

list<Product::ProductFieldOffsets> Product::evolFieldList;           //added ats 10012002
list<Product::ProductFieldOffsets> Product::ultimaFieldList;         //added ats 10012002
list<Product::ProductFieldOffsets> Product::uiFieldList;        //added ats 10012002
list<Product::ProductFieldOffsets> Product::dbindependentFieldList;

const string Product::_PRODUCTTYPE_UNKNOWN_Name_       ("Unknown");
const string Product::_PRODUCTTYPE_RANDOMWEIGHT_Name_  ("Random Weight");
const string Product::_PRODUCTTYPE_FIXEDWEIGHT_Name_   ("Fixed Weight");
const string Product::_PRODUCTTYPE_BYCOUNT_Name_       ("By Count");
const string Product::_PRODUCTTYPE_FLUIDOUNCE_Name_    ("Fluid Ounce");

const std::string Product::_PRODUCTIONPLANNING_NO_Name_                 ("Non-Production Planning item");
const std::string Product::_PRODUCTIONPLANNING_ITEM_Name_               ("Regular Production Planning item");
const std::string Product::_PRODUCTIONPLANNING_INGREDIENT_Name_         ("Ingredient Production Planning item");
const std::string Product::_PRODUCTIONPLANNING_MIXED_Name_              ("Mixed Production Planning item");
const std::string Product::_PRODUCTIONPLANNING_RECIPES_SUGG_SELL_Name_  ("Recipes and Suggestive Selling");
const std::string Product::_PRODUCTIONPLANNING_INVALID_Name_            ("Unknown");

std::list<LabelType *> Product::labelTypesToDelete;
Util::CSmartCriticalSection Product::labelTypesToDeleteCS;


#define DUMMY_PRODUCTNUM -42


Product::Product()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	
	Debug::trace("Product.ctor", " Product()  this: " + ptrtostring(this) );

	init();

	//     SOF-375 Build a meal softkeys only look up items in Default department
	// This bug issue used the empty default constructor to create a new product but it didn't set the deptNum, leaving
	// it up to who created the new Product, and in other cases it did.  To date, there are 9 instances of this constructor
	// are created, and in every single case it was decided that it would be ok to set the DeptNum here in the constructor
	// instead of leaving it up to the memory of the person creating the call to the constructor. Catching this issue also
	// led us to be able to head off 6 other bugs that would of been bugs without catching this one.
	// This also has to occur after the init bc the init() sets deptNum to 0.
	setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));

	createProductTransaction();
	//	Set up the product transaction record
	initProductTransaction();
}

Product::Product(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	
	Debug::trace("Product.ctor", " Product(data,len,formatType)  this: " + ptrtostring(this) );
	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<ProductFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			// Only for the UI, initially set the deptNum to the currently active dept.
			// If we receive the deptNum with the data (which at the time of multi-dept development we don't), then the dept number received will override this one. 
			setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token, false, formatType);
				}
				fieldIter++;
			}

			applyRulesToReceivedFields(formatType);
			break;
		}

		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ProductTags	productTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			// SOF-3880
			ProductType PT_data( PRODUCTTYPE_UNKNOWN ); // product type
			HobString up_data; // upc number
			HobString vn_data; // vendor number
			HobString pn_data; // product number
			HobString bc_data; // by count
			HobString fb_data; // by count forced

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				HobString tokenData = "";
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? productTags.getOffset(temp) : 0;
					if (Debug::isTracing("Product.ctor"))
						Debug::trace("Product.ctor", string("Product(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						tokenData = token->getData();

						// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating the tare.
						// HT-136 Need to account for TareB
						if (fieldOffsetTareAValue == offset || fieldOffsetTareBValue == offset || fieldOffsetWeightMin == offset || fieldOffsetWeightMax == offset)
						{
							if ((EvolutionTaggedFormat == formatType) || (UltimaTaggedFormat == formatType))
								if (2 == scaleConfig->getExternalWeightPrecision())
									tokenData.append("0");
						}

						if (offset == fieldOffsetDescription)
							tokenData.stripChar('\x1D');

						updateField(offset, tokenData, false, formatType);

						// SOF-3882
						if ( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIForceNonZeroByCountForFixedWeightItems) )
						{
							if ( fieldOffsetByCount == offset )
								bc_data = tokenData;
							else if ( fieldOffsetByCountForced == offset )
								fb_data = tokenData;
						}

						// SOF-3880
						if ( Configuration::getInstance()->isIgnoreVendorAndProductNumTagsWhenUpcNumTag() )
						{
							if ( fieldOffsetProductType == offset )
								PT_data = (Product::ProductType) util_string::stringtoint( tokenData );
							else if ( fieldOffsetBarcodeUpcNum == offset )
								up_data = tokenData;
							else if ( fieldOffsetBarcodeVendorNum == offset )
								vn_data = tokenData;
							else if ( fieldOffsetBarcodeProductNum == offset )
								pn_data = tokenData;
						}
					}
				}

				// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
				// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
				if (!isReceived(fieldOffsetProductDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
				{
					setReceived(fieldOffsetProductDeptNum, true);
					setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				}

				// SOF-3882: Costco Hack
				// for fixed weight bakery items they are sending a by count of zero
				if (( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIForceNonZeroByCountForFixedWeightItems) ) &&
				    ( PRODUCTTYPE_FIXEDWEIGHT == PT_data ))
				{
					if (( isReceived( fieldOffsetByCount ) ) && ( 0 < bc_data.length() ) && ( 0 == util_string::stringtoint( bc_data ) ))
					{
						if (( !isReceived( fieldOffsetByCountForced ) ) ||
						    (( 0 < fb_data.length() ) && ( 0 == util_string::stringtoint( fb_data ) )))
						{
							Debug::trace("Product", "Repairing Non-Zero ByCount: " + bc_data + " " + fb_data );

							updateField( fieldOffsetByCount, "1", false, formatType );
						}
					}
				}

				// SOF-3880: Costco Hack
				// for fixed weight items there are instances where they are not populating 'up' but populating 'vn'. additional cases where
				// 'up' contains product number only, and they are also sending 'vn'. so lets try to handle that here...
				if (( Configuration::getInstance()->isIgnoreVendorAndProductNumTagsWhenUpcNumTag() ) &&
				    ( PRODUCTTYPE_FIXEDWEIGHT == PT_data ))
				{
					if (( isReceived( fieldOffsetBarcodeUpcNum ) ) && (( 0 == up_data.length() ) || ( 5 == up_data.length() )))
					{
						if (( isReceived( fieldOffsetBarcodeVendorNum ) ) && ( vn_data.length() ))
						{
							if (( !isReceived( fieldOffsetBarcodeProductNum ) ) || ( !pn_data.length() ))
							{
								Debug::trace("Product", "Repairing UPC Number with partial data: " + vn_data + " " + up_data );

								if ( up_data.empty() )
									up_data = "00000";

								updateField( fieldOffsetBarcodeUpcNum, vn_data + up_data, false, formatType );
							}
						}
					}
				}

				applyRulesToReceivedFields(formatType);
				delete token;
			}
			break;
		}

		default:
			break;
	}

	//	Set up the product transaction record
	initProductTransaction();
}

Product::Product(DBProduct *dbRec, DBAccess *access, bool loadLinkedData)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	Debug::trace("Product.ctor", string("Product(DBProduct, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	string msgDebug = "Product(DBProduct) - ";
	init();

	createProductTransaction();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());

		productNum = Long::valueOf(dbRec->getPLU());
		Debug::trace("Product", msgDebug + "ProductNum: " + longtostring(productNum));

		productType = (ProductType)dbRec->getType();
		description = dbRec->getDescription();

		byCountForced = dbRec->getForceByCount();
		if (byCountForced)
		{
			byCount = -1;
			byCountEntered = false;
		}
		else
			byCount = dbRec->getByCount();

		if (scaleConfig->isSelfServiceScale() && isByCount())
			byCountEntered = false;

		unitPrice = (HobPrice)dbRec->getPrice();
		unitPriceForced = dbRec->getForcePrice();
		if (unitPriceForced)
			unitPriceEntered = false;

		exceptionPrice = (HobPrice)dbRec->getExceptionPrice();

		priceModifier = dbRec->getPriceFactor();
		if (priceModifier == 0)
			priceModifier = 1;

		discountType = dbRec->getDiscountMethod();
		if ( scaleConfig->isPerScale() || ( scaleConfig->isPerPlu() && (discountType == Default_dept_type) ) )
			discountType = scaleConfig->getDefaultDiscountType();
		Debug::trace("Product.FrequentShopper", string("Product(DBProduct, DBAccess, bool loadLinkedData)  discountType: ") + inttostring(discountType));

		discountValue1 = (HobPrice)dbRec->getDiscountPrice1();
		discountValue2 = (HobPrice)dbRec->getDiscountPrice2();
		discountValue3 = (HobPrice)dbRec->getDiscountPrice3();

		tareA.setValue((HobWeight)dbRec->getTare());
		tareB.setValue((HobWeight)dbRec->getTareB());
		tareA.setForced(dbRec->getForceTare());	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
		if (tareA.isForced())
			tareEntered = false;

		proportionalTare = dbRec->getProportionalTare();
		fixedWeight = (HobWeight)dbRec->getNetWeight();
		originalFixedWeight = (HobWeight)dbRec->getNetWeight();

		shelfLifeDays = dbRec->getShelfLifeDays();
		shelfLifeHours = dbRec->getShelfLifeHours();
		shelfLifeMinutes = dbRec->getShelfLifeMinutes();

		productLifeDays = dbRec->getProductLifeDays();
		productLifeHours = dbRec->getProductLifeHours();
		productLifeMinutes = dbRec->getProductLifeMinutes();

		dontPrintPackedOn = dbRec->getDontPrintPackedOn();
		dontPrintSellBy = dbRec->getDontPrintShelfLife();
		dontPrintBestBefore = dbRec->getDontPrintProductLife();

		// fieldOffsetUPCVendorNumber, embedded within the barcode
		// fieldOffsetUPCProductNumber, embedded within the barcode

		setLabelRotation(FirstPrinter, dbRec->getLabel1Rotation());
		setLabelRotation(SecondPrinter, dbRec->getLabel2Rotation());
		setLabelRotation(ThirdPrinter, dbRec->getLabel3Rotation());

		setLabelPlacement(FirstPrinter, dbRec->getLabel1Placement());
		setLabelPlacement(SecondPrinter, dbRec->getLabel2Placement());
		setLabelPlacement(ThirdPrinter, dbRec->getLabel3Placement());

		points = dbRec->getBonusPoints();
		portions = dbRec->getPortions();

		intakeMode = static_cast<WrapperIntakeMode::IntakeMode> (dbRec->getIntakeMode());

		applySecurityLabel = dbRec->getApplySecurityLabel();

		if (!dbRec->isImageNull())
			productGraphicName = dbRec->getImage();

		if (!dbRec->isName1Null())
			productName1 = dbRec->getName1();
		if (!dbRec->isName2Null())
			productName2 = dbRec->getName2();

		if (!dbRec->isSpecialStartDateNull())
		{
			HobDateTime dt;
			HobString dts(dbRec->getSpecialStartDate());
			dt.convertFromDBFormat(&dts);
			specialStartDate = dt;
		}

		mealSideItemsGroupNum = dbRec->getMealSideItemsGroupNum();
		mealSideItemsAreFree = dbRec->getMealSideItemsAreFree();
		mealNumSidesIncluded = dbRec->getMealNumSidesIncluded();

		cutTestMeatSpecSheetGroupNum = dbRec->getCutTestMeatSpecSheetGroupNum();

		// For Pick-5 project
		weightMin = dbRec->getWeightMin();
		weightMax = dbRec->getWeightMax();
		totalPriceMin = dbRec->getTotalPriceMin();
		totalPriceMax = dbRec->getTotalPriceMax();

		defaultLabelTypeNum1   = Long::valueOf(dbRec->getLabel1());
		defaultLabelTypeNum2   = Long::valueOf(dbRec->getLabel2());
		defaultLabelTypeNum3   = Long::valueOf(dbRec->getLabel3());
		manualLabelTypeNum1    = Long::valueOf(dbRec->getManualLabelTypeNum1());
		manualLabelTypeNum2    = Long::valueOf(dbRec->getManualLabelTypeNum2());
		manualLabelTypeNum3    = Long::valueOf(dbRec->getManualLabelTypeNum3());
		prepackLabelTypeNum1   = Long::valueOf(dbRec->getPrepackLabelTypeNum1());
		prepackLabelTypeNum2   = Long::valueOf(dbRec->getPrepackLabelTypeNum2());
		prepackLabelTypeNum3   = Long::valueOf(dbRec->getPrepackLabelTypeNum3());
		prodEntryLabelTypeNum1 = Long::valueOf(dbRec->getProdEntryLabelTypeNum1());
		prodEntryLabelTypeNum2 = Long::valueOf(dbRec->getProdEntryLabelTypeNum2());
		prodEntryLabelTypeNum3 = Long::valueOf(dbRec->getProdEntryLabelTypeNum3());
		selfServLabelTypeNum1  = Long::valueOf(dbRec->getSelfServLabelTypeNum1());
		selfServLabelTypeNum2  = Long::valueOf(dbRec->getSelfServLabelTypeNum2());
		selfServLabelTypeNum3  = Long::valueOf(dbRec->getSelfServLabelTypeNum3());
		pick5LabelTypeNum1     = Long::valueOf(dbRec->getPick5LabelTypeNum1());
		pick5LabelTypeNum2     = Long::valueOf(dbRec->getPick5LabelTypeNum2());
		pick5LabelTypeNum3     = Long::valueOf(dbRec->getPick5LabelTypeNum3());
		onlineOrderingLabelTypeNum1 = Long::valueOf(dbRec->getOnlineOrderingLabelTypeNum1());
		onlineOrderingLabelTypeNum2 = Long::valueOf(dbRec->getOnlineOrderingLabelTypeNum2());
		onlineOrderingLabelTypeNum3 = Long::valueOf(dbRec->getOnlineOrderingLabelTypeNum3());

		// TODO - Larry - done - auto labels. Do we need to set the others? Numbering?
		/* For the 'label types by mode' project, continue to let the label types be set up based on the default label types.
		 * The main reason for this is when looking up an Item in Supervisor mode, the screen initialization wants to know which
		 * barcode fields need to be displayed (UPC?, Product Number?, Vendor Number?, EAN Code?), so we will base this off of
		 * the default label types, since we expect the customer to still set up default label types.
		 * Things get complicated if they have labels in some mode, or even within the same mode,
		 * that print different barcode types.  We don't handle this situation at all.
		 * The ProductManager will still set up the Label Type objects in the Product,
		 * since a Product has no way to determine what mode the Session is in;
		 * it doesn't even know what Session it is for, but the Product Manager does.
		 */
		setLabelType(FirstPrinter,  LabelType::findRecord(defaultLabelTypeNum1, deptNum, dbAccess));
		setLabelType(SecondPrinter, LabelType::findRecord(defaultLabelTypeNum2, deptNum, dbAccess));
		setLabelType(ThirdPrinter,  LabelType::findRecord(defaultLabelTypeNum3, deptNum, dbAccess));

		//	The following line can't be called until the label type objects within the Product class are set up!!
		//  Now that the labelTypes have been loaded, set up the barcode information.
		barCodeNum.getFromDatabase(dbRec, this, dbAccess);

		// FT-1265 set raw barcode DATA
		rawBarCodeData = dbRec->isRawBarCodeNumberNull() ? "" : dbRec->getRawBarCodeNumber();

		if (loadLinkedData)
		{
			HobRecordNum supportRecNum = 0;

			if (!dbRec->isCategoryNull())
			{
				supportRecNum = Long::valueOf(dbRec->getCategory());
				setCategory(Category::findRecord(supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.Category"))
					Debug::trace("Product.Category", msgDebug + "Category Num: " + longtostring(supportRecNum) + " - Category DB rec" + ((category) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isSpecialNumberNull())
			{
				supportRecNum = Long::valueOf(dbRec->getSpecialNumber());
				setSpecialProgram(Special::findRecord(supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.Special"))
					Debug::trace("Product.Special", msgDebug + "Special Num: " + longtostring(supportRecNum) + " - Special DB rec" + ((specialProgram) ? " " : " NOT ") + "found");
			}

			setGraphic(Graphic::GRAPHIC1, Graphic::findGraphic(dbRec->getGraphic1()));
			setGraphic(Graphic::GRAPHIC2, Graphic::findGraphic(dbRec->getGraphic2()));
			setGraphic(Graphic::GRAPHIC3, Graphic::findGraphic(dbRec->getGraphic3()));
			setGraphic(Graphic::GRAPHIC4, Graphic::findGraphic(dbRec->getGraphic4()));

			supportRecNum = Long::valueOf(dbRec->getReportingClass());
			setReportingClass( Class::findClassValidateLinks( supportRecNum, deptNum, dbAccess ) );
			if (Debug::isTracing("Product.Class"))
			{
				Debug::trace("Product.Class", msgDebug + "Class Num: " + longtostring(supportRecNum) + " - Class DB rec" + ((reportingClass) ? " " : " NOT ") + "found");
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Reporting class: ").append(Long::toString(getReportingClassNum()));
				msgDebug2.append("  ClassLinkStatus: ").append(reportingClass ? Long::toString(getReportingClass()->getErrorCode()) : "0");
				msgDebug2.append("  BadClass: ").append(reportingClass ? Long::toString(getReportingClass()->getBadClass()) : "0");
				msgDebug2.append("  BadReportingClass: ").append(reportingClass ? Long::toString(getReportingClass()->getBadReportingClass()) : "0");
				Debug::trace("Product.Class", msgDebug2);
			}

			if (!dbRec->isNutritionNull())
			{
				supportRecNum = Long::valueOf(dbRec->getNutrition());
				setNutrifacts(Nutrifacts::findRecord(supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.Nutrifacts"))
					Debug::trace("Product.Nutrifacts", msgDebug + "Nutrifacts Num: " + longtostring(supportRecNum) + " - Nutrifacts DB rec" + ((nutrifacts) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isTextNull())
			{
				supportRecNum = Long::valueOf(dbRec->getText());
				setProductText(psttExpandedText, ProductText::findRecord(psttExpandedText, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "Expanded Text Num: " + longtostring(supportRecNum) + " - ExpandedText DB rec" + ((productTexts_[psttExpandedText]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isSpecialMessageNull())
			{
				supportRecNum = Long::valueOf(dbRec->getSpecialMessage());
				setProductText(psttSpecialMessage, ProductText::findRecord(psttSpecialMessage, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "Special Message Num: " + longtostring(supportRecNum) + " - SpecialMessage DB rec" + ((productTexts_[psttSpecialMessage]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isNoteNull())
			{
				supportRecNum = Long::valueOf(dbRec->getNote());
				setProductText(psttProductNote, ProductText::findRecord(psttProductNote, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "Product Note Num: " + longtostring(supportRecNum) + " - ProductNote DB rec" + ((productTexts_[psttProductNote]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isMarqueeNull())
			{
				supportRecNum = Long::valueOf(dbRec->getMarquee());
				setProductText(psttMarqueeText, ProductText::findRecord(psttMarqueeText, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "Marquee Text Num: " + longtostring(supportRecNum) + " - MarqueeText DB rec" + ((productTexts_[psttMarqueeText]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isProductText5Null())
			{
				supportRecNum = Long::valueOf(dbRec->getProductText5());
				setProductText(psttProductText5, ProductText::findRecord(psttProductText5, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "ProductText5 Text Num: " + longtostring(supportRecNum) + " - ProductText5 DB rec" + ((productTexts_[psttProductText5]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isProductText6Null())
			{
				supportRecNum = Long::valueOf(dbRec->getProductText6());
				setProductText(psttProductText6, ProductText::findRecord(psttProductText6, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "ProductText6 Text Num: " + longtostring(supportRecNum) + " - ProductText6 DB rec" + ((productTexts_[psttProductText6]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isProductText7Null())
			{
				supportRecNum = Long::valueOf(dbRec->getProductText7());
				setProductText(psttProductText7, ProductText::findRecord(psttProductText7, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "ProductText7 Text Num: " + longtostring(supportRecNum) + " - ProductText7 DB rec" + ((productTexts_[psttProductText7]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isProductText8Null())
			{
				supportRecNum = Long::valueOf(dbRec->getProductText8());
				setProductText(psttProductText8, ProductText::findRecord(psttProductText8, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "ProductText8 Text Num: " + longtostring(supportRecNum) + " - ProductText8 DB rec" + ((productTexts_[psttProductText8]) ? " " : " NOT ") + "found");
			}

			if (!dbRec->isProductText9Null())
			{
				supportRecNum = Long::valueOf(dbRec->getProductText9());
				setProductText(psttProductText9, ProductText::findRecord(psttProductText9, supportRecNum, deptNum, dbAccess));
				if (Debug::isTracing("Product.ProductText"))
					Debug::trace("Product.ProductText", msgDebug + "ProductText9 Text Num: " + longtostring(supportRecNum) + " - ProductText9 DB rec" + ((productTexts_[psttProductText9]) ? " " : " NOT ") + "found");
			}

		}

		if (!dbRec->isCoolCategoryNull())
			coolCategoryNum		= Long::valueOf(dbRec->getCoolCategory());
		if (!dbRec->isCoolPreTextNull())
			coolPretextNum		= Long::valueOf(dbRec->getCoolPreText());
		if (!dbRec->isDefaultCoolTextNull())
			coolDefaultTextNum	= Long::valueOf(dbRec->getDefaultCoolText());
		if (!dbRec->isCurrentCoolTextNull())
			coolTextNum			= Long::valueOf(dbRec->getCurrentCoolText());
		if (!dbRec->isCoolTrackingTextNull())
			coolTrackingText	= dbRec->getCoolTrackingText();
		if (!dbRec->isForceCoolReviewNull())
			coolVerifyForced	= dbRec->getForceCoolReview();
		if (coolVerifyForced)
        {
            Debug::trace("Product", "Product: coolVerifyForced is TRUE");
			coolTextSelected = false;
        }
		if (!dbRec->isCoolRequiredNull())
			coolRequired		= dbRec->getCoolRequired();
        //Only check if coolText is selected if we dont have a current cool
        if ( coolRequired && dbRec->isCurrentCoolTextNull() && !coolVerifyForced )
        {
            Debug::trace("Product", "Product: coolVerifyForced=false, coolRequired=false, and setting coolRequiredText=false");
            coolRequiredText = false;
        }

		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (dbCoolListsRM)
		{
			DBCoolList *coolList = dbCoolListsRM->find(dbRec);
			if (coolList)
			{
				if (!coolList->isNumberNull())
					setCoolShortlistNum(stringtolong(coolList->getNumber()));
				if (!coolList->isFirstLinkNumberNull())
					setCoolShortlistFirstLink(stringtolong(coolList->getFirstLinkNumber()));
				delete coolList;
			}
		}
		
		productionPlanningType = (ProductionPlanningType)dbRec->getProductionPlanningType();

		errorCode = RECORD_OK;

		//	Set up the product transaction record
		initProductTransaction();
	}
}


Product::Product(Product *productTemplate, DBAccess *access, AbstractSession *session)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	Debug::trace("Product.Constructor", " Product(Product *productTemplate, DBAccess *access, AbstractSession *session) Constructor: " + ptrtostring(this) );

	init();
	createProductTransaction();

	dbAccess = access;

	//	Set all received so we can copy everything from the productTemplate.
	//	Larry says this should be OK on Sept. 27, 2002.
	productTemplate->setAllReceived(true);

	// Need to set the deptNum in the Product record first; otherwise, the update of the product number will fail
	setDeptNum(productTemplate->getDeptNum());
	updateRecord(productTemplate, session);

	setBarcodeFields(productTemplate);

	//	Set up the product transaction record
	initProductTransaction();
}


Product::~Product()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	Debug::trace("Product.dtor", "    Product: " + ptrtostring(this) + "  num " + longtostring(productNum) + "  deptNum " + longtostring(deptNum));

	if ( !printingTransactions.empty() )
	{
		// HEY!  We have outstanding product transaction records that are being printed.
		// We shouldn't be deleting this product record until the Real-Time Controller
		// notifies us that it is done with each of the transaction records.
		
		Debug::trace("Product.Delete", "SEVERE ERROR - Deleting a Product object that has outstanding Transaction Records.");

		for ( std::list<ProductTransaction*>::iterator iter( printingTransactions.begin() ); iter != printingTransactions.end(); iter++ )
		{
			ProductTransaction *trans( *iter );

			if (( trans ) && ( trans != currentProductTransaction_ ))
				delete trans;
		}

		printingTransactions.clear();
	}

	// FT-960
	if ( !completedTransactions.empty() )
	{
		for ( std::list<ProductTransaction*>::iterator iter( completedTransactions.begin() ); iter != completedTransactions.end(); iter++ )
		{
			ProductTransaction *trans( *iter );

			if (( trans ) && ( trans != currentProductTransaction_ ))
				delete trans;
		}

		completedTransactions.clear();
	}

	if (reportingClass)
	{
		delete reportingClass;
		reportingClass = 0;
	}

	if (nutrifacts)
	{
		delete nutrifacts;
		nutrifacts = 0;
	}

	if (productTexts_.size())
	{
		for (int i=0; i<(int)productTexts_.size(); i++)
			if (productTexts_[i])
			{
				delete productTexts_[i];
				productTexts_[i] = 0;
			}

		productTexts_.clear();
	}

	if (graphics.size() > 0)
	{
		Graphic *tempGraphic;
		std::vector<Graphic *>::iterator iter = graphics.begin();
		int i = 0;

		while (iter != graphics.end())
		{
			tempGraphic = (Graphic *) *iter;
			if (*iter)
			{
				Debug::trace("Product.Graphic", HobString("Delete Graphics: Product = ") + ptrtostring(this) + HobString("    *iter = ") + ptrtostring(tempGraphic) + HobString("   index = ") + HobString(Integer::toString(i)));
				delete *iter;
			}
			++iter;
			i++;
		}
		graphics.clear();
	}

	if (category)
	{
		delete category;
		category = 0;
	}

	if (specialProgram)
	{
		delete specialProgram;
		specialProgram = 0;
	}

	if (currentCOOLText)
	{
		delete currentCOOLText;
		currentCOOLText = 0;
	}

	if (currentCOOLPretext)
	{
		delete currentCOOLPretext;
		currentCOOLPretext = 0;
	}

	if (shelfLifeExpirationLabelType)
	{
		delete shelfLifeExpirationLabelType;
		shelfLifeExpirationLabelType = 0;
	}

	if (runTotalsLabelType)
	{
		delete runTotalsLabelType;
		runTotalsLabelType = 0;
	}

	if (myDbRecord)
	{
		delete myDbRecord;
		myDbRecord = 0;
	}

	if ( currentProductTransaction_ )
	{
		Debug::trace("Product.dtor", "Deleting currentProductTransaction_: " + ptrtostring(currentProductTransaction_) + " with Product number: " + longtostring(productNum));
		delete currentProductTransaction_;
		currentProductTransaction_ = 0;
	}

	if (remotePrintTemplateRequest)
	{
		delete remotePrintTemplateRequest;
		remotePrintTemplateRequest = 0;
	}

	// delete some labels
	Util::CLockCriticalSection lock( labelTypesToDeleteCS );

	for ( std::vector<LabelType*>::iterator lti = labelTypes.begin(); lti != labelTypes.end(); ++lti )
		labelTypesToDelete.push_back( *lti );

	labelTypes.clear();
	//SOF-4043
	revOrderTypes.clear();


	while ( 64 < labelTypesToDelete.size() )
	{
		if ( labelTypesToDelete.front() )
			delete labelTypesToDelete.front();

		labelTypesToDelete.pop_front();
	}
}

void Product::init()
{
	deptNum = 0;
	productNum = RECORD_NOT_FOUND;
	productType = PRODUCTTYPE_UNKNOWN;
	description = "";
	byCount = 0;
	byCountForced = false;
	byCountEntered = true;
	exceptionPrice = 0;
	unitPrice = 0;
	unitPriceForced = false;
	unitPriceEntered = true;
	unitPriceChanged = false;
	discountType = 0;
	discountValue1 = 0;
	discountValue2 = 0;
	discountValue3 = 0;
	tareEntered = true;
	proportionalTare = 0;
	fixedWeight = 0;
	originalFixedWeight = 0;
	shelfLifeDays = 0;
	shelfLifeHours = 0;
	shelfLifeMinutes = 0;
	productLifeDays = 0;
	productLifeHours = 0;
	productLifeMinutes = 0; 
	priceModifier = UNIT_PRICING;
	points = 0;
	intakeMode = WrapperIntakeMode::DefaultIntake;
	flags = 0;	//added ats 10152002
	applySecurityLabel = false;
	dontPrintPackedOn = false;
	dontPrintSellBy = false;
	dontPrintBestBefore = false;
	dontPrintSellByOverridden = false;
	dontPrintBestBeforeOverridden = false;

	productGraphicName = "";
	productName1 = "";
	productName2 = "";
	category = (Category *)NULL;
	specialProgram = (Special *)NULL;
	// don't need to initialize specialStartDate; it will default to 'now' - HobDateTime specialStartDate;

	reportingClass = (Class *)NULL;
	nutrifacts = 0;

	productTexts_.resize(psttLastProductText);
	if (productTexts_.size())
		for (int i=0; i<(int)productTexts_.size(); i++)
			productTexts_[i] = 0;
	
	scaleConfig = Configuration::getInstance();

	short maxGraphics = (short) scaleConfig->toIntCfgI(CfgInternal::MAXGRAPHICSPERPRODUCT);
	graphics.insert(graphics.begin(), maxGraphics, (Graphic *)NULL);

	labelTypes.insert(labelTypes.begin(), NewMaxPrinterTypes, (LabelType *)NULL);
	labelRotations.insert(labelRotations.begin(), MaxPrinterTypes, 0);
	labelPlacements.insert(labelPlacements.begin(), MaxPrinterTypes, LocNoneSelected);
	//SOF-4043 expiration label causes reverse ordering issue since label is preImaged after expiration label print
	revOrderTypes.insert(revOrderTypes.begin(), MaxPrinterTypes, (LabelType *)NULL);

	labelHasFrequentShopper = false;

	coolCategoryNum		= RECORD_NOT_FOUND;
	coolPretextNum		= RECORD_NOT_FOUND;
	coolDefaultTextNum	= RECORD_NOT_FOUND;
	coolTextNum			= RECORD_NOT_FOUND;
	coolTrackingText	= "";
	coolVerifyForced	= false;
	coolShortlistNum	= RECORD_NOT_FOUND;
	coolShortlistFirstLink = RECORD_NOT_FOUND;
	coolRequired		= false;
	coolTextSelected	= true;
    coolRequiredText    = true;

	currentProductTransaction_ = 0;
	
	currentCOOLText = 0;
	currentCOOLPretext = 0;

	backDateDays = 0;
	backDateDaysForced = false;
	backDateDaysEntered = true;

	keyboardNetWeight = -1;
	acceptKbdNetWeight = false;
	cancelledKbdNetWeight = false;
	
	labelBeingPrinted = PRINTING_NO_LABEL;
	shelfLifeExpirationLabelType = 0;
	runTotalsLabelType = 0;

	numOfLabelsForced = false;
	numOfLabelsEntered = false;

	productionPlanningType = PRODUCTIONPLANNING_NO;

	dbAccess = 0;
	myDbRecord = 0;

	errorCode = RECORD_NOT_FOUND;

	printingInProductionEntryMode = false;

	expirationDaysForced = false;
	expirationDays = 0;
	expirationDaysEntered = true;
	
	portions = 0;
	portionsOnLabel = false;

	mealSideItemsGroupNum = 0;
	mealSideItemsAreFree = true;
	mealNumSidesIncluded = 2;
	itemIsMealSideItem = false;

	cutTestMeatSpecSheetGroupNum = 0;

	// For Pick-5 project
	weightMin = 0;
	weightMax = 0;
	totalPriceMin = 0;
	totalPriceMax = 0;

	// Initialize which set of label types were using to be the default set
	fieldOffsetOfFirstLabelTypeNum = fieldOffsetProductUnknownField;

	defaultLabelTypeNum1   = 0;
	defaultLabelTypeNum2   = 0;
	defaultLabelTypeNum3   = 0;
	manualLabelTypeNum1    = 0;
	manualLabelTypeNum2    = 0;
	manualLabelTypeNum3    = 0;
	prepackLabelTypeNum1   = 0;
	prepackLabelTypeNum2   = 0;
	prepackLabelTypeNum3   = 0;
	prodEntryLabelTypeNum1 = 0;
	prodEntryLabelTypeNum2 = 0;
	prodEntryLabelTypeNum3 = 0;
	selfServLabelTypeNum1  = 0;
	selfServLabelTypeNum2  = 0;
	selfServLabelTypeNum3  = 0;
	pick5LabelTypeNum1     = 0;
	pick5LabelTypeNum2     = 0;
	pick5LabelTypeNum3     = 0;
	onlineOrderingLabelTypeNum1 = 0;
	onlineOrderingLabelTypeNum2 = 0;
	onlineOrderingLabelTypeNum3 = 0;

	printLabelToPrinter_ = true;

	remotePrintTemplateRequest = 0;
	printingRemoteLabel = false;
	//reversed = false;
	hookBeforePrint_ = hookAfterPrint_ = false;

	productTagsToDrop["l2"] = static_cast<int>(fieldOffsetLabelTypeNum2);
	productTagsToDrop["l3"] = static_cast<int>(fieldOffsetLabelTypeNum3);

	initFieldLists();
}

// static
void Product::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetProductNum);
		uiFieldList.push_back(fieldOffsetProductType);
		uiFieldList.push_back(fieldOffsetDescription);
		uiFieldList.push_back(fieldOffsetBarcodeType);
		uiFieldList.push_back(fieldOffsetBarcodeFields);
		uiFieldList.push_back(fieldOffsetBarcodePrefix);
		uiFieldList.push_back(fieldOffsetBarcodeProductNum);
		uiFieldList.push_back(fieldOffsetBarcodeVendorNum);
		uiFieldList.push_back(fieldOffsetBarcodeEANCode);
		uiFieldList.push_back(fieldOffsetByCount);
		uiFieldList.push_back(fieldOffsetExceptionPrice);
		uiFieldList.push_back(fieldOffsetUnitPrice);
		uiFieldList.push_back(fieldOffsetDiscountType);
		uiFieldList.push_back(fieldOffsetDiscountValue1);
		uiFieldList.push_back(fieldOffsetDiscountValue2);
		uiFieldList.push_back(fieldOffsetDiscountValue3);
		uiFieldList.push_back(fieldOffsetDiscountLabel1);
		uiFieldList.push_back(fieldOffsetDiscountLabel2);
		uiFieldList.push_back(fieldOffsetDiscountLabel3);
		uiFieldList.push_back(fieldOffsetTareAValue);
		uiFieldList.push_back(fieldOffsetProportionalTare);
		uiFieldList.push_back(fieldOffsetFixedWeight);
		uiFieldList.push_back(fieldOffsetShelfLifeDays);
		uiFieldList.push_back(fieldOffsetShelfLifeHours);				// minutes at the bottom
		uiFieldList.push_back(fieldOffsetProductLifeDays);
		uiFieldList.push_back(fieldOffsetProductLifeHours);				// minutes at the bottom
		uiFieldList.push_back(fieldOffsetPriceModifier);
		uiFieldList.push_back(fieldOffsetProductReportingClassNum);
		uiFieldList.push_back(fieldOffsetLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetLabelRotation1);
		uiFieldList.push_back(fieldOffsetLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetLabelRotation2);
		uiFieldList.push_back(fieldOffsetLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetLabelRotation3);
		uiFieldList.push_back(fieldOffsetBackDateDays);
		uiFieldList.push_back(fieldOffsetPoints);
		uiFieldList.push_back(fieldOffsetGraphicName1);
		uiFieldList.push_back(fieldOffsetGraphicName2);
		uiFieldList.push_back(fieldOffsetGraphicName3);
		uiFieldList.push_back(fieldOffsetGraphicName4);
		uiFieldList.push_back(fieldOffsetLabelTypeWidth1);
		uiFieldList.push_back(fieldOffsetLabelTypeLength1);
		uiFieldList.push_back(fieldOffsetLabelTypeWidth2);
		uiFieldList.push_back(fieldOffsetLabelTypeLength2);
		uiFieldList.push_back(fieldOffsetLabelTypeWidth3);
		uiFieldList.push_back(fieldOffsetLabelTypeLength3);
		uiFieldList.push_back(fieldOffsetByCountForced);
		uiFieldList.push_back(fieldOffsetTareAForced);
		uiFieldList.push_back(fieldOffsetFractionalPriceUnit);
		uiFieldList.push_back(fieldOffsetPoundsForAmount);
		uiFieldList.push_back(fieldOffsetUnitPriceForced);
		uiFieldList.push_back(fieldOffsetTareType);
		uiFieldList.push_back(fieldOffsetApplySecurity);
		uiFieldList.push_back(fieldOffsetDontPrintPackedOn);
		uiFieldList.push_back(fieldOffsetDontPrintSellBy);
		uiFieldList.push_back(fieldOffsetDontPrintBestBefore);
		uiFieldList.push_back(fieldOffsetLabelPlacement1);
		uiFieldList.push_back(fieldOffsetLabelPlacement2);
		uiFieldList.push_back(fieldOffsetLabelPlacement3);
		uiFieldList.push_back(fieldOffsetFractionalPrice);
		uiFieldList.push_back(fieldOffsetForcedFractionalPrice);
		uiFieldList.push_back(fieldOffsetPoundsForPrice);
		uiFieldList.push_back(fieldOffsetForcedPoundsFor);

		uiFieldList.push_back(fieldOffsetSellByDate);
		uiFieldList.push_back(fieldOffsetSellByTime);
		uiFieldList.push_back(fieldOffsetBestBeforeDate);
		uiFieldList.push_back(fieldOffsetBestBeforeTime);

		uiFieldList.push_back(fieldOffsetHasNutrifactsFlag);
		uiFieldList.push_back(fieldOffsetIntakeMode);

		uiFieldList.push_back(fieldOffsetLabelHasFreqShopper);

		uiFieldList.push_back(fieldOffsetProductGraphicName);
		uiFieldList.push_back(fieldOffsetProductName1);
		uiFieldList.push_back(fieldOffsetProductName2);
		uiFieldList.push_back(fieldOffsetProdCategoryNum);
		uiFieldList.push_back(fieldOffsetProdSpecialNum);

		uiFieldList.push_back(fieldOffsetProdCoolCategoryNum);
		uiFieldList.push_back(fieldOffsetProdCoolPretextNum);
		uiFieldList.push_back(fieldOffsetProdCoolDefaultTextNum);
		uiFieldList.push_back(fieldOffsetProdCoolTextNum);
		uiFieldList.push_back(fieldOffsetProdCoolShortlistNum);
		uiFieldList.push_back(fieldOffsetProdCoolVerifyForced);
		uiFieldList.push_back(fieldOffsetProdCOOLTrackingText);
		uiFieldList.push_back(fieldOffsetProdCoolRequired);

		// KeyboardNetWeight
		uiFieldList.push_back(fieldOffsetKeyboardNetWeight);
		uiFieldList.push_back(fieldOffsetAcceptKbdNetWeight);
		
		uiFieldList.push_back(fieldOffsetNumOfLabelsForced);
		uiFieldList.push_back(fieldOffsetNumOfLabels);

		uiFieldList.push_back(fieldOffsetProductionPlanningType);

		uiFieldList.push_back(fieldOffsetForcedBackDateDays);

		uiFieldList.push_back(fieldOffsetExpirationDaysForced);
		uiFieldList.push_back(fieldOffsetExpirationDays);

		//	As of Jan. 19, 2006, I am including the EANFormatString field as the browser
		//	will need to know the format string to determine the maximum # of characters
		//	for data entry of the EAN barcode fields.
		uiFieldList.push_back(fieldOffsetEANFormatString);
		uiFieldList.push_back(fieldOffsetEAN128CFormatString);
		
		uiFieldList.push_back(fieldOffsetGS1DatabarGTIN);
		uiFieldList.push_back(fieldOffsetGS1DatabarAI);
	
		uiFieldList.push_back(fieldOffsetBackDateDate);
		uiFieldList.push_back(fieldOffsetPortions);
		uiFieldList.push_back(fieldOffsetShowPortions);

		uiFieldList.push_back(fieldOffsetBuildAMealSideItemsGroupNum);
		uiFieldList.push_back(fieldOffsetBuildAMealSideItemsAreFree);
		uiFieldList.push_back(fieldOffsetBuildAMealNumberOfSidesIncluded);

		uiFieldList.push_back(fieldOffsetCutTestMeatSpecSheetGroupNum);

		uiFieldList.push_back(fieldOffsetTareBValue);

		uiFieldList.push_back(fieldOffsetQRCodeFormatString);
		uiFieldList.push_back(fieldOffsetDataMatrixFormatString);

		// For Pick-5 project
		uiFieldList.push_back(fieldOffsetWeightMin);
		uiFieldList.push_back(fieldOffsetWeightMax);
		uiFieldList.push_back(fieldOffsetTotalPriceMin);
		uiFieldList.push_back(fieldOffsetTotalPriceMax);

		// Automatic mode labels project
		uiFieldList.push_back(fieldOffsetDefaultLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetDefaultLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetDefaultLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetManualLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetManualLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetManualLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetPrepackLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetPrepackLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetPrepackLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetProdEntryLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetProdEntryLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetProdEntryLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetSelfServLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetSelfServLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetSelfServLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetPick5LabelTypeNum1);
		uiFieldList.push_back(fieldOffsetPick5LabelTypeNum2);
		uiFieldList.push_back(fieldOffsetPick5LabelTypeNum3);
		uiFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum1);
		uiFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum2);
		uiFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum3);
		uiFieldList.push_back(fieldOffsetShelfLifeMinutes);
		uiFieldList.push_back(fieldOffsetProductLifeMinutes);
		uiFieldList.push_back(fieldOffsetLabelStockRotation1);
		uiFieldList.push_back(fieldOffsetLabelStockRotation2);
		uiFieldList.push_back(fieldOffsetLabelStockRotation3);
	}

	if (ultimaFieldList.empty())
	{
		//initialize the field list these are the keys into the map		// tags ....
		ultimaFieldList.push_back(fieldOffsetProductNum);				// p#
		ultimaFieldList.push_back(fieldOffsetDescription);				//dt

		ultimaFieldList.push_back(fieldOffsetBarcodePrefix);			//u#
		ultimaFieldList.push_back(fieldOffsetBarcodeUpcNum);			//up

		ultimaFieldList.push_back(fieldOffsetByCount);					//bc
		ultimaFieldList.push_back(fieldOffsetExceptionPrice);			//xp
		ultimaFieldList.push_back(fieldOffsetUnitPrice);				//u$

		ultimaFieldList.push_back(fieldOffsetDiscountType);				//Dt

		ultimaFieldList.push_back(fieldOffsetTareAValue);				//ta
		ultimaFieldList.push_back(fieldOffsetProportionalTare);			//pt
		ultimaFieldList.push_back(fieldOffsetFixedWeight);				//nw  in flag for ultima

		ultimaFieldList.push_back(fieldOffsetShelfLifeDays);			//sl
		ultimaFieldList.push_back(fieldOffsetProductLifeDays);			//pl

		ultimaFieldList.push_back(fieldOffsetProductReportingClassNum);	//rc

		ultimaFieldList.push_back(fieldOffsetLabelTypeNum1);			//l1
		ultimaFieldList.push_back(fieldOffsetLabelRotation1);			//p1
		ultimaFieldList.push_back(fieldOffsetLabelTypeNum2);			//l2
		ultimaFieldList.push_back(fieldOffsetLabelRotation2);			//p2

		ultimaFieldList.push_back(fieldOffsetPoints);					//ps

		ultimaFieldList.push_back(fieldOffsetGraphicNum);				//g#
		ultimaFieldList.push_back(fieldOffsetExpandedTextNum);			//r#
		ultimaFieldList.push_back(fieldOffsetSpecialMessageNum);		//s#
		ultimaFieldList.push_back(fieldOffsetMarqueeTextNum);				//m#

		ultimaFieldList.push_back(fieldOffsetPluFlags);					//pf
		ultimaFieldList.push_back(fieldOffsetTotalsFlags);				//tf
		ultimaFieldList.push_back(fieldOffsetExceptionPriceTare);		//xt

		ultimaFieldList.push_back(fieldOffsetProdCoolCategoryNum);			//cb
		ultimaFieldList.push_back(fieldOffsetProdCoolPretextNum);			//ci
		ultimaFieldList.push_back(fieldOffsetProdCoolDefaultTextNum);		//c7
		ultimaFieldList.push_back(fieldOffsetProdCoolTextNum);				//c8
		ultimaFieldList.push_back(fieldOffsetProdCoolShortlistFirstLink);	//c9
		ultimaFieldList.push_back(fieldOffsetProdCOOLTrackingText);			//cc
		ultimaFieldList.push_back(fieldOffsetProdCoolVerifyForced);			//cf
		ultimaFieldList.push_back(fieldOffsetProdCoolRequired);				//co

		ultimaFieldList.push_back(fieldOffsetTareBValue);					//T2

		ProductTags productTags((std::list<UINT16> *)&ultimaFieldList);
	}

	if (evolFieldList.empty())
	{
		//initialize the field list these are the keys into the map		// tags ....
		evolFieldList.push_back(fieldOffsetProductDeptNum);
		evolFieldList.push_back(fieldOffsetProductNum);						// p#
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetProductType);					//PT
#endif
		evolFieldList.push_back(fieldOffsetDescription);					//dt

		evolFieldList.push_back(fieldOffsetBarcodePrefix);
		evolFieldList.push_back(fieldOffsetBarcodeUpcNum);					//up
		evolFieldList.push_back(fieldOffsetBarcodeProductNum);				//pn

#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetBarcodeVendorNum);
		evolFieldList.push_back(fieldOffsetBarcodeEANCode);
#endif

		evolFieldList.push_back(fieldOffsetByCount);						//bc
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetByCountForced);
#endif
		evolFieldList.push_back(fieldOffsetUnitPrice);						//u$
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetUnitPriceForced);
#endif
		evolFieldList.push_back(fieldOffsetPriceModifier);
		evolFieldList.push_back(fieldOffsetExceptionPrice);					//xp

		evolFieldList.push_back(fieldOffsetDiscountType);					//Dt
		evolFieldList.push_back(fieldOffsetDiscountValue1);
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetDiscountValue2);
		evolFieldList.push_back(fieldOffsetDiscountValue3);
#endif

		evolFieldList.push_back(fieldOffsetTareAValue);						//ta
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetTareAForced);
#endif
		evolFieldList.push_back(fieldOffsetProportionalTare);				//pt
		evolFieldList.push_back(fieldOffsetFixedWeight);					//nw

		evolFieldList.push_back(fieldOffsetShelfLifeDays);					//sl
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetShelfLifeHours);					//SL			// minutes at the bottom
#endif
		evolFieldList.push_back(fieldOffsetProductLifeDays);				//pl
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetProductLifeHours);				//PL			// minutes at the bottom
		evolFieldList.push_back(fieldOffsetDontPrintSellBy);
		evolFieldList.push_back(fieldOffsetDontPrintBestBefore);
		evolFieldList.push_back(fieldOffsetDontPrintPackedOn);
		evolFieldList.push_back(fieldOffsetApplySecurity);
#endif

		evolFieldList.push_back(fieldOffsetProductReportingClassNum);		//rc

		evolFieldList.push_back(fieldOffsetLabelTypeNum1);					//l1
		evolFieldList.push_back(fieldOffsetLabelRotation1);					//p1
		evolFieldList.push_back(fieldOffsetLabelTypeNum2);					//l2
		evolFieldList.push_back(fieldOffsetLabelRotation2);					//p2
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetLabelTypeNum3);					//l3
		evolFieldList.push_back(fieldOffsetLabelRotation3);					//p3
		evolFieldList.push_back(fieldOffsetLabelPlacement1);
		evolFieldList.push_back(fieldOffsetLabelPlacement2);
		evolFieldList.push_back(fieldOffsetLabelPlacement3);
#endif

		evolFieldList.push_back(fieldOffsetPoints);							//ps

#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetIntakeMode);
#endif

		evolFieldList.push_back(fieldOffsetGraphicNum);						//g#
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetGraphicName1);					//G1
		evolFieldList.push_back(fieldOffsetGraphicName2);					//G2
		evolFieldList.push_back(fieldOffsetGraphicName3);					//G3
		evolFieldList.push_back(fieldOffsetGraphicName4);					//G4
#endif

		evolFieldList.push_back(fieldOffsetExpandedTextNum);				//r#
		evolFieldList.push_back(fieldOffsetSpecialMessageNum);				//s#
#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetProdProductNoteNum);
		evolFieldList.push_back(fieldOffsetProdNutrifactNum);
#endif
		evolFieldList.push_back(fieldOffsetMarqueeTextNum);				//m#

		evolFieldList.push_back(fieldOffsetProductText5Num);
		evolFieldList.push_back(fieldOffsetProductText6Num);
		evolFieldList.push_back(fieldOffsetProductText7Num);
		evolFieldList.push_back(fieldOffsetProductText8Num);
		evolFieldList.push_back(fieldOffsetProductText9Num);

		// For backwards compatibility, add the following fields ...
		evolFieldList.push_back(fieldOffsetPluFlags);						//pf
		evolFieldList.push_back(fieldOffsetTotalsFlags);					//tf
		evolFieldList.push_back(fieldOffsetExceptionPriceTare);				//xt

#ifdef USE_NEW_EVOL_TAGS
		evolFieldList.push_back(fieldOffsetProductGraphicName);
		evolFieldList.push_back(fieldOffsetProductName1);
		evolFieldList.push_back(fieldOffsetProductName2);
		evolFieldList.push_back(fieldOffsetProdCategoryNum);
		evolFieldList.push_back(fieldOffsetProdSpecialNum);
#endif

		evolFieldList.push_back(fieldOffsetProdCoolCategoryNum);		//cb
		evolFieldList.push_back(fieldOffsetProdCoolPretextNum);			//ci
		evolFieldList.push_back(fieldOffsetProdCoolDefaultTextNum);		//c7
		evolFieldList.push_back(fieldOffsetProdCoolTextNum);			//c8
		evolFieldList.push_back(fieldOffsetProdCoolShortlistFirstLink);	//c9
		evolFieldList.push_back(fieldOffsetProdCOOLTrackingText);		//cc
		evolFieldList.push_back(fieldOffsetProdCoolVerifyForced);		//cf
		evolFieldList.push_back(fieldOffsetProdCoolRequired);			//co
		evolFieldList.push_back(fieldOffsetProdCoolShortlistNum);

		evolFieldList.push_back(fieldOffsetProductionPlanningType);
			
		evolFieldList.push_back(fieldOffsetPortions);

		evolFieldList.push_back(fieldOffsetBuildAMealSideItemsGroupNum);
		evolFieldList.push_back(fieldOffsetBuildAMealSideItemsAreFree);
		evolFieldList.push_back(fieldOffsetBuildAMealNumberOfSidesIncluded);

		evolFieldList.push_back(fieldOffsetCutTestMeatSpecSheetGroupNum);

		evolFieldList.push_back(fieldOffsetTareBValue);					//T2

		// For Pick-5 project
		evolFieldList.push_back(fieldOffsetWeightMin);
		evolFieldList.push_back(fieldOffsetWeightMax);
		evolFieldList.push_back(fieldOffsetTotalPriceMin);
		evolFieldList.push_back(fieldOffsetTotalPriceMax);

		// For automatic label types
		evolFieldList.push_back(fieldOffsetManualLabelTypeNum1);			// l4
		evolFieldList.push_back(fieldOffsetManualLabelTypeNum2);			// l5
		evolFieldList.push_back(fieldOffsetManualLabelTypeNum3);		    // l6
		evolFieldList.push_back(fieldOffsetPrepackLabelTypeNum1);			// l7
		evolFieldList.push_back(fieldOffsetPrepackLabelTypeNum2);			// l8
		evolFieldList.push_back(fieldOffsetPrepackLabelTypeNum3);			// l9
		evolFieldList.push_back(fieldOffsetProdEntryLabelTypeNum1);			// L4
		evolFieldList.push_back(fieldOffsetProdEntryLabelTypeNum2);			// L5
		evolFieldList.push_back(fieldOffsetProdEntryLabelTypeNum3);			// L6
		evolFieldList.push_back(fieldOffsetSelfServLabelTypeNum1);			// L7
		evolFieldList.push_back(fieldOffsetSelfServLabelTypeNum2);			// L8
		evolFieldList.push_back(fieldOffsetSelfServLabelTypeNum3);			// L9
		evolFieldList.push_back(fieldOffsetPick5LabelTypeNum1);				// La
		evolFieldList.push_back(fieldOffsetPick5LabelTypeNum2);				// Lb
		evolFieldList.push_back(fieldOffsetPick5LabelTypeNum3);				// Lc
		evolFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum1);	// Ld
		evolFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum2);	// Le
		evolFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum3);	// Lf
        // for shelf and product life minutes
		evolFieldList.push_back(fieldOffsetShelfLifeMinutes);				// SM
		evolFieldList.push_back(fieldOffsetProductLifeMinutes);				// PM 

		ProductTags productTags((std::list<UINT16> *)&evolFieldList);
	}

	if (dbindependentFieldList.empty())
	{
		dbindependentFieldList.push_back(fieldOffsetProductDeptNum);
		dbindependentFieldList.push_back(fieldOffsetProductNum);
		dbindependentFieldList.push_back(fieldOffsetProductType);
		dbindependentFieldList.push_back(fieldOffsetDescription);
		dbindependentFieldList.push_back(fieldOffsetBarcodePrefix);
		dbindependentFieldList.push_back(fieldOffsetBarcodeProductNum);
		dbindependentFieldList.push_back(fieldOffsetBarcodeUpcNum);
		dbindependentFieldList.push_back(fieldOffsetBarcodeVendorNum);
		dbindependentFieldList.push_back(fieldOffsetBarcodeEANCode);
		dbindependentFieldList.push_back(fieldOffsetByCount);
		dbindependentFieldList.push_back(fieldOffsetByCountForced);
		dbindependentFieldList.push_back(fieldOffsetUnitPrice);
		dbindependentFieldList.push_back(fieldOffsetUnitPriceForced);
		dbindependentFieldList.push_back(fieldOffsetPriceModifier);
		dbindependentFieldList.push_back(fieldOffsetExceptionPrice);
		dbindependentFieldList.push_back(fieldOffsetDiscountType);
		dbindependentFieldList.push_back(fieldOffsetDiscountValue1);
		dbindependentFieldList.push_back(fieldOffsetDiscountValue2);
		dbindependentFieldList.push_back(fieldOffsetDiscountValue3);
		dbindependentFieldList.push_back(fieldOffsetTareAValue);
		dbindependentFieldList.push_back(fieldOffsetTareAForced);
		dbindependentFieldList.push_back(fieldOffsetProportionalTare);
		dbindependentFieldList.push_back(fieldOffsetFixedWeight);
					// todo - we don't currently read this from the db record			dbindependentFieldList.push_back(fieldOffsetFluidOunces);
		dbindependentFieldList.push_back(fieldOffsetShelfLifeDays);
		dbindependentFieldList.push_back(fieldOffsetShelfLifeHours);					// minutes at the bottom
		dbindependentFieldList.push_back(fieldOffsetProductLifeDays);
		dbindependentFieldList.push_back(fieldOffsetProductLifeHours);					// minutes at the bottom
		dbindependentFieldList.push_back(fieldOffsetDontPrintSellBy);
		dbindependentFieldList.push_back(fieldOffsetDontPrintBestBefore);
		dbindependentFieldList.push_back(fieldOffsetDontPrintPackedOn);
		dbindependentFieldList.push_back(fieldOffsetApplySecurity);

		dbindependentFieldList.push_back(fieldOffsetProductReportingClassNum);

		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum1);
		dbindependentFieldList.push_back(fieldOffsetLabelRotation1);
		dbindependentFieldList.push_back(fieldOffsetLabelPlacement1);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum2);
		dbindependentFieldList.push_back(fieldOffsetLabelRotation2);
		dbindependentFieldList.push_back(fieldOffsetLabelPlacement2);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum3);
		dbindependentFieldList.push_back(fieldOffsetLabelRotation3);
		dbindependentFieldList.push_back(fieldOffsetLabelPlacement3);

		dbindependentFieldList.push_back(fieldOffsetPoints);
		dbindependentFieldList.push_back(fieldOffsetIntakeMode);

		dbindependentFieldList.push_back(fieldOffsetGraphicName1);
		dbindependentFieldList.push_back(fieldOffsetGraphicName2);
		dbindependentFieldList.push_back(fieldOffsetGraphicName3);
		dbindependentFieldList.push_back(fieldOffsetGraphicName4);

		dbindependentFieldList.push_back(fieldOffsetExpandedTextNum);
		dbindependentFieldList.push_back(fieldOffsetSpecialMessageNum);
		dbindependentFieldList.push_back(fieldOffsetProdProductNoteNum);
		dbindependentFieldList.push_back(fieldOffsetProdNutrifactNum);
		dbindependentFieldList.push_back(fieldOffsetMarqueeTextNum);

		dbindependentFieldList.push_back(fieldOffsetProductText5Num);
		dbindependentFieldList.push_back(fieldOffsetProductText6Num);
		dbindependentFieldList.push_back(fieldOffsetProductText7Num);
		dbindependentFieldList.push_back(fieldOffsetProductText8Num);
		dbindependentFieldList.push_back(fieldOffsetProductText9Num);

		dbindependentFieldList.push_back(fieldOffsetProductGraphicName);
		dbindependentFieldList.push_back(fieldOffsetProductName1);
		dbindependentFieldList.push_back(fieldOffsetProductName2);
		dbindependentFieldList.push_back(fieldOffsetProdCategoryNum);
		dbindependentFieldList.push_back(fieldOffsetProdSpecialNum);

		dbindependentFieldList.push_back(fieldOffsetProdCoolCategoryNum);
		dbindependentFieldList.push_back(fieldOffsetProdCoolPretextNum);
		dbindependentFieldList.push_back(fieldOffsetProdCoolDefaultTextNum);
		dbindependentFieldList.push_back(fieldOffsetProdCoolTextNum);
		dbindependentFieldList.push_back(fieldOffsetProdCoolShortlistNum);          //cs
		dbindependentFieldList.push_back(fieldOffsetProdCoolShortlistFirstLink);	//c9
		dbindependentFieldList.push_back(fieldOffsetProdCOOLTrackingText);
		dbindependentFieldList.push_back(fieldOffsetProdCoolVerifyForced);
		dbindependentFieldList.push_back(fieldOffsetProdCoolRequired);

		dbindependentFieldList.push_back(fieldOffsetProductionPlanningType);
		dbindependentFieldList.push_back(fieldOffsetPortions);					//p_

		dbindependentFieldList.push_back(fieldOffsetBuildAMealSideItemsGroupNum);
		dbindependentFieldList.push_back(fieldOffsetBuildAMealSideItemsAreFree);
		dbindependentFieldList.push_back(fieldOffsetBuildAMealNumberOfSidesIncluded);

		dbindependentFieldList.push_back(fieldOffsetCutTestMeatSpecSheetGroupNum);

		dbindependentFieldList.push_back(fieldOffsetTareBValue);

		// For Pick-5 project
		dbindependentFieldList.push_back(fieldOffsetWeightMin);
		dbindependentFieldList.push_back(fieldOffsetWeightMax);
		dbindependentFieldList.push_back(fieldOffsetTotalPriceMin);
		dbindependentFieldList.push_back(fieldOffsetTotalPriceMax);

		// For automatic label types
		dbindependentFieldList.push_back(fieldOffsetManualLabelTypeNum1);			// l4
		dbindependentFieldList.push_back(fieldOffsetManualLabelTypeNum2);			// l5
		dbindependentFieldList.push_back(fieldOffsetManualLabelTypeNum3);		    // l6
		dbindependentFieldList.push_back(fieldOffsetPrepackLabelTypeNum1);			// l7
		dbindependentFieldList.push_back(fieldOffsetPrepackLabelTypeNum2);			// l8
		dbindependentFieldList.push_back(fieldOffsetPrepackLabelTypeNum3);			// l9
		dbindependentFieldList.push_back(fieldOffsetProdEntryLabelTypeNum1);		// L4
		dbindependentFieldList.push_back(fieldOffsetProdEntryLabelTypeNum2);		// L5
		dbindependentFieldList.push_back(fieldOffsetProdEntryLabelTypeNum3);		// L6
		dbindependentFieldList.push_back(fieldOffsetSelfServLabelTypeNum1);			// L7
		dbindependentFieldList.push_back(fieldOffsetSelfServLabelTypeNum2);			// L8
		dbindependentFieldList.push_back(fieldOffsetSelfServLabelTypeNum3);			// L9
		dbindependentFieldList.push_back(fieldOffsetPick5LabelTypeNum1);			// La
		dbindependentFieldList.push_back(fieldOffsetPick5LabelTypeNum2);			// Lb
		dbindependentFieldList.push_back(fieldOffsetPick5LabelTypeNum3);			// Lc
		dbindependentFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum1);	// Ld
		dbindependentFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum2);	// Le
		dbindependentFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum3);	// Lf
        // SOF-4904 Add shelfLifeMinutes to product and transactions
		dbindependentFieldList.push_back(fieldOffsetShelfLifeMinutes);			    // SM
		dbindependentFieldList.push_back(fieldOffsetProductLifeMinutes);			// PM

		ProductTags productTags((std::list<UINT16> *)&dbindependentFieldList);
	}
}


HobRecordNum Product::getDeptNum() const
{
	return deptNum;
}
void Product::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum Product::getProductNum() const
{
	return productNum;
}
void Product::setProductNum42(HobRecordNum num)
{
	productNum = num;
}
void Product::setProductNum(HobRecordNum productNum)
{
	if ((productNum > 0) && (productNum <= MAX_PRODUCT_NUM))
	{
		Product *tempProduct = Product::findRecord(productNum, deptNum, dbAccess);
		if (tempProduct)
		{
			//	This product record already exists. Do not renumber the current product.
			delete tempProduct;
			Debug::trace("Product.renumber", HobString("Duplicate Product number ") + HobString(Long::toString(productNum)));
		}
		else
		{
			this->productNum = productNum;
		}
	}
	else
	{
		this->productNum = RECORD_NOT_FOUND;
		addError(HfcError::ERRORCODE_INVALIDRANGE, ProductTags::prodNumber);
	}
}
bool Product::isDummyProduct() const
{
	// could this check for 0 or <1 instead of -42 exactly?
	return (DUMMY_PRODUCTNUM == getProductNum());
}

HobErrorCode Product::getErrorCode() const
{
	return errorCode;
}
void Product::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}

Product::ProductType Product::getProductType() const
{
	return productType;
}
void Product::setProductType(ProductType productType)
{
	if (this->productType == productType)
		return;

	if(isValidProductType(productType))
		this->productType = productType;
	else
		this->productType = PRODUCTTYPE_UNKNOWN;
}
bool Product::isValidProductType(ProductType productType)
{
	if( (productType == PRODUCTTYPE_RANDOMWEIGHT) ||
	    (productType == PRODUCTTYPE_BYCOUNT) ||
	    (productType == PRODUCTTYPE_FIXEDWEIGHT) ||
	    (productType == PRODUCTTYPE_FLUIDOUNCE) )
		return true;
	else
		return false;
}
bool Product::isRandomWeight() const
{
	if (productType == PRODUCTTYPE_RANDOMWEIGHT)
		return true;
	else
		return false;
}
bool Product::isByCount() const
{
	if (productType == PRODUCTTYPE_BYCOUNT)
		return true;
	else
		return false;
}
bool Product::isByCountWithWeight() const
{
	if ((productType == PRODUCTTYPE_BYCOUNT) && (byCount == 1) && scaleConfig->toBoolCfg(fieldOffsetCfgPFWeightOnByCountPrinted))
		return true;
	else
		return false;
}
bool Product::isFixedWeight() const
{
	if (productType == PRODUCTTYPE_FIXEDWEIGHT)
		return true;
	else
		return false;
}
bool Product::isFluidOunce() const
{
	if (productType == PRODUCTTYPE_FLUIDOUNCE)
		return true;
	else
		return false;
}
bool Product::isFixedWtOrFluidOz() const
{
	return (isFixedWeight() || isFluidOunce());
}
void Product::typeUnknownProduct()
{
	if (productType == PRODUCTTYPE_UNKNOWN)
	{
		if (byCount > 0)
			productType = PRODUCTTYPE_BYCOUNT;
		else
			productType = PRODUCTTYPE_RANDOMWEIGHT;

		setReceived(fieldOffsetProductType, true);
	}
}

const HobString &Product::getDescription() const
{
	return description;
}
void Product::setDescription(const HobString &description)
{
	this->description = description;
}

const BarCodeNumber & Product::getBarCodeNum() const
{
	return barCodeNum;
}
BarCodeNumber & Product::getBarCodeNum()
{
	return barCodeNum;
}
void Product::setBarCodeNum(BarCodeNumber & barCodeNum)
{
	this->barCodeNum = barCodeNum;
}

int Product::getByCount() const
{
	Debug::trace("Product.ByCount", HobString("getByCount()  this->byCount = ") + HobString(Integer::toString(byCount)));
	return byCount;
}
void Product::setByCount(int byCount)
{
	if (this->byCount != byCount)
	{
		if (productType == PRODUCTTYPE_RANDOMWEIGHT)
			byCount = 0;
		else if ( byCount > 0 && byCount <= 99 )
		{
			this->byCount = byCount;
			byCountEntered = true;
		}
		else
		{
			Debug::trace("Product.HfcError", HobString("setByCount() by count entered out of range - creating HfcError - ") + HobString(Integer::toString(byCount)));
			addError(HfcError::ERRORCODE_INVALIDRANGE, ProductTags::prodByCount);
		}
	}
	else
		byCountEntered = true;
	
	Debug::trace("Product.ByCount", HobString("setByCount(") + HobString(Integer::toString(byCount)) + HobString(")    this->byCount =  ") + HobString(Integer::toString(this->byCount)));
}
int Product::getOriginalByCount() const
{
	int tempByCount = 1;

	//	Get the byCount value from the database record, if it was not a forced by count.
	if (!byCountForced && myDbRecord)
		tempByCount = myDbRecord->getByCount();

	if (Debug::isTracing("Product.ByCount"))
	{
		HobString myDebug("getOriginalByCount() = ");
		myDebug.append(Integer::toString(tempByCount));
		Debug::trace("Product.ByCount", myDebug);
	}
	return tempByCount;
}

bool Product::isByCountForced() const
{
	return byCountForced;
}
void Product::setByCountForced(bool byCountForced)
{
	this->byCountForced = byCountForced;
	if (byCountForced)
		byCount = 1;
}

bool Product::isByCountEntered() const
{
	return byCountEntered;
}
void Product::setByCountEntered(bool entered)
{
	byCountEntered = entered;
}

HobPrice Product::getExceptionPrice() const
{
	return exceptionPrice;
}
void Product::setExceptionPrice(HobPrice exceptionPrice)
{
	Debug::trace("Product.Test", HobString("setExceptionPrice() - ") + Long::toString(exceptionPrice));
	this->exceptionPrice = exceptionPrice;
}

HobPrice Product::getUnitPrice() const
{
	return unitPrice;
}
void Product::setUnitPrice(HobPrice unitPrice)
{
	//	cout << "setUnitPrice (" << unitPrice << ")" << endl;

	Debug::trace("Product", HobString("setUnitPrice() - ") + Long::toString(unitPrice));
	if ((unitPrice >= 0) && (unitPrice <= scaleConfig->getMaxUnitPrice()))
	{
	Debug::trace("Product", HobString("setUnitPrice() - setting price to ") + Long::toString(unitPrice));
		this->unitPrice = unitPrice;
//		else
//			this->unitPrice = 0;

//		currentProductTransaction_->setUnitPrice(this->unitPrice);

		// The total price, net weight, etc. will be updated when the next weigher status is received,
		// which the Product Manager will request once the update is done.
		// Just calling the following updateTotalValuesData() does not generate the message for updating the
		// browser with the new totals.
		updateTotalValuesData();

		if (!unitPriceForced || unitPrice > (HobPrice)0)
			unitPriceEntered = true;
	}
	else
	{
		Debug::trace("Product.HfcError", HobString("setUnitPrice() unit price entered out of range - creating HfcError - ") + HobString(Long::toString((long)unitPrice)));
		addError(HfcError::ERRORCODE_INVALIDRANGE, ProductTags::prodUnitPrice);
	}
}
HobPrice Product::getOriginalUnitPrice() const
{
	//	Get the unitPrice value from the database record.
	HobPrice tempUnitPrice = 0;

	if (unitPriceForced && unitPriceEntered)
		tempUnitPrice = unitPrice;
	else if (!unitPriceForced && myDbRecord)
		tempUnitPrice = (HobPrice) myDbRecord->getPrice();

	if (Debug::isTracing("Product.ByCount"))
	{
		HobString myDebug("getOriginalUnitPrice() = ");
		myDebug.append(Integer::toString(tempUnitPrice));
		Debug::trace("Product.ByCount", myDebug);
	}
	return tempUnitPrice;
}

bool Product::isUnitPriceForced() const
{
	return unitPriceForced;
}
void Product::setUnitPriceForced(bool unitPriceForced)
{
	this->unitPriceForced = unitPriceForced;
	if (unitPriceForced)
		unitPrice = (HobPrice)0;
}

bool Product::isUnitPriceEntered() const
{
	return unitPriceEntered;
}
void Product::setUnitPriceEntered(bool entered)
{
	unitPriceEntered = entered;
}

bool Product::isUnitPriceChanged() const
{
	return unitPriceChanged;
}
void Product::setUnitPriceChanged(bool changed)
{
	unitPriceChanged = changed;
}



short Product::getDiscountType() const
{
	return discountType;
}
void Product::setDiscountType(short discountType)
{
//	NOTE: COME BACK TO THIS WHEN FREQUENT SHOPPER IS SET UP.
	this->discountType = discountType;
}

HobPrice Product::getDiscountValue1() const
{
	return discountValue1;
}
void Product::setDiscountValue1(HobPrice discountValue)
{
//	NOTE: COME BACK TO THIS WHEN FREQUENT SHOPPER IS SET UP.
	this->discountValue1 = discountValue;
}

HobPrice Product::getDiscountValue2() const
{
	return discountValue2;
}
void Product::setDiscountValue2(HobPrice discountValue)
{
//	NOTE: COME BACK TO THIS WHEN FREQUENT SHOPPER IS SET UP.
	this->discountValue2 = discountValue;
}

HobPrice Product::getDiscountValue3() const
{
	return discountValue3;
}
void Product::setDiscountValue3(HobPrice discountValue)
{
//	NOTE: COME BACK TO THIS WHEN FREQUENT SHOPPER IS SET UP.
	this->discountValue3 = discountValue;
}


Tare & Product::getTareA()
{
	return tareA;
}
void Product::setTareA(HobWeight tare)
{
	if ( tare >= 0 && tare <= scaleConfig->getMaxTare() )
	{
		tareA.setValue(tare);
		tareEntered = true;
		updateTotalValuesData();
	}
}

Tare & Product::getTareB()
{
	return tareB;
}
void Product::setTareB(HobWeight tare)
{
	if ( tare >= 0 && tare <= scaleConfig->getMaxTare() )
	{
		tareB.setValue(tare);
		tareEntered = true;
		updateTotalValuesData();
	}
}

bool Product::isTareEntered() const
{
	return tareEntered;
}
void Product::setTareEntered(bool entered)
{
	tareEntered = entered;
}

const Tare & Product::getActiveTare() const
{
	return ( scaleConfig->toBoolCfg( fieldOffsetCfgUseSecondTare ) ) ? tareB : tareA;
}
Tare & Product::getActiveTare()
{
	return ( scaleConfig->toBoolCfg( fieldOffsetCfgUseSecondTare ) ) ? tareB : tareA;
}

int Product::getProportionalTare() const
{
	return proportionalTare;
}
void Product::setProportionalTare(HobString proportionalTare)
{
	if (proportionalTare.find(".") != std::string::npos)
	{
		this->proportionalTare = (int)(Double::valueOf(proportionalTare) * pow(10.0, PROP_TARE_DEC_PRECISION));
	}
	else
	{
		this->proportionalTare = Integer::valueOf(proportionalTare);
	}

}
void Product::setProportionalTare(int proportionalTare)
{
	if ((proportionalTare >= 0) && (proportionalTare < 1000))	//	We want it less than 100.0%
		this->proportionalTare = proportionalTare;
}

HobWeight Product::getFixedWeight() const
{
	return fixedWeight;
}
void Product::setFixedWeight(HobWeight fixedWeight)
{
	this->fixedWeight = fixedWeight;
}
int Product::getOriginalFixedWeight() const
{
	return originalFixedWeight;
}


int Product::getShelfLifeDays() const
{
	return shelfLifeDays;
}
void Product::setShelfLifeDays(int shelfLifeDays)
{
	if (shelfLifeDays >= 0 && shelfLifeDays <= scaleConfig->getMaxLifeDays())
		this->shelfLifeDays = shelfLifeDays;
}
void Product::adjustShelfLifeDays(int newShelfLifeDays)
{
	int deltaDays = newShelfLifeDays - shelfLifeDays;
	setShelfLifeDays(newShelfLifeDays);

	//	Do we need to update the Product Life days?
	if (deltaDays != 0)
		setProductLifeDays(productLifeDays + deltaDays);
}

short Product::getShelfLifeHours() const
{
	return shelfLifeHours;
}

void Product::setShelfLifeHours(short shelfLifeHours)
{
	if (shelfLifeHours >= 0 && shelfLifeHours <= scaleConfig->getMaxLifeHours())
		this->shelfLifeHours = shelfLifeHours;
}

void Product::adjustShelfLifeHours(int newShelfLifeHours)
{
	int deltaHours = newShelfLifeHours - shelfLifeHours;
	setShelfLifeHours(newShelfLifeHours);

	//	Do we need to update the Product Life Hours?
	if (deltaHours != 0)
		setProductLifeHours(productLifeHours + deltaHours);
}

short Product::getShelfLifeMinutes() const
{
	return shelfLifeMinutes;
}

void Product::setShelfLifeMinutes(short shelfLifeMinutes)
{
	if (shelfLifeMinutes >= 0 && shelfLifeMinutes <= scaleConfig->getMaxLifeMinutes())
		this->shelfLifeMinutes = shelfLifeMinutes;
}

void Product::adjustShelfLifeMinutes(int newShelfLifeMinutes)
{
	int deltaMinutes = newShelfLifeMinutes - shelfLifeMinutes;
	setShelfLifeMinutes(newShelfLifeMinutes);

	//	Do we need to update the Product Life Hours? Yes we do.
	// TODO jhc - need to adjust the product life minutes or hours here?  If we adjust the mins, then those mins affect the hours
	//if (deltaMinutes != 0)
	//	setProductLifeHours(productLifeHours + deltaHours);
}

HobDateTime Product::getShelfLife()
{
	setShelfLife();			//	To set the current date & time.
	return shelfLife;
}
void Product::setShelfLife() const
{
/*
 * Note: The shelfLifeDays and shelfLifeHours need to have been set before calling this method.
 */
   /* TODO ShelfLifeMinutes - updates required here? What is this method doing? UI display, etc*/ 
	shelfLife.setToCurrent();
	shelfLife.incrementDays(-backDateDays);
	shelfLife.incrementDays(shelfLifeDays);
	shelfLife.incrementHours((int)shelfLifeHours);
	shelfLife.incrementMinutes((int)shelfLifeMinutes);

	if (Debug::isTracing("Product.lifeDates"))
	{
		HobString msgDebug = "setShelfLife():  ";
		msgDebug.append("backDateDays = ").append(Integer::toString(backDateDays));
		msgDebug.append("  shelfLifeDays = ").append(Integer::toString(shelfLifeDays));
		msgDebug.append("  shelfLifeHours = ").append(Integer::toString(shelfLifeHours));
		msgDebug.append("  shelfLifeMinutes = ").append(Integer::toString(shelfLifeMinutes));
		msgDebug.append("  shelfLife = ").append(shelfLife.formatDate(FormatDate_MMDDYY));
		Debug::trace("Product.lifeDates", msgDebug);
	}
}

void Product::setShelfLife(const HobString &newDate)
{
	HobDateTime newSellByDate;
	newSellByDate.setDateParts(newDate);
	if (!newSellByDate.isValid())
		return;

	HobDateTime now;
	long nowJulian = now.getJulianDay();
	long newJulian = newSellByDate.getJulianDay();
	int deltaDays = (int) (newJulian - nowJulian);
	// The user can't set the sell by date to a date before today!!!
	if (deltaDays < 0)
		return;

	HobDateTime origSellByDate = shelfLife;
	HobString origSellByDateString = "";
	getFieldAsString(fieldOffsetSellByDate, origSellByDateString, BrowserFormat);
	Debug::trace("Product", HobString("updating sell by date - orig date:") + origSellByDateString);
	Debug::trace("Product", HobString("origSellByDate.show - "));
	if ( Debug::isTracing("Product") )
		origSellByDate.show();

	Debug::trace("Product", HobString("newSellByDate.show - "));
	if ( Debug::isTracing("Product") )
		newSellByDate.show();

	long origTime = origSellByDate.getJulianDay();
	long newTime = newSellByDate.getJulianDay();
	deltaDays = (int) (newTime - origTime);
	Debug::trace("Product", HobString("deltaDays - ") + HobString(Integer::toString(deltaDays)));

	// Nothing to do, the date is already set to the value passed in.
	if (deltaDays == 0)
		return;

	Debug::trace("Product", HobString("current shelf life days - ") + HobString(Integer::toString(shelfLifeDays)));

	//	Update the Shelf life days - it also updates the sell by date.
	int newShelfLifeDays = shelfLifeDays + deltaDays;
	setShelfLifeDays(newShelfLifeDays);
	Debug::trace("Product", HobString("new shelf life days - ") + HobString(Integer::toString(shelfLifeDays)));

	// Only update the product life days if the shelf life days were actually updated.
	if (newShelfLifeDays == shelfLifeDays)
	{
		//	Update the Product life days - it also updates the best before date.
		Debug::trace("Product", HobString("current product life days - ") + HobString(Integer::toString(productLifeDays)));
		setProductLifeDays(productLifeDays + deltaDays);
		Debug::trace("Product", HobString("new product life days - ") + HobString(Integer::toString(productLifeDays)));
	}
}

int Product::getProductLifeDays() const
{
	return productLifeDays;
}

void Product::setProductLifeDays(int productLifeDays)
{
	if (productLifeDays <= scaleConfig->getMaxLifeDays())
		this->productLifeDays = (productLifeDays <= 0) ? 0 : productLifeDays;
}

short Product::getProductLifeHours() const
{
	return productLifeHours;
}

void Product::setProductLifeHours(short productLifeHours)
{
	if (productLifeHours <= scaleConfig->getMaxLifeHours())
		this->productLifeHours = (productLifeHours <= 0) ? 0 : productLifeHours;
}

short Product::getProductLifeMinutes() const
{
	return productLifeMinutes;
}

void Product::setProductLifeMinutes(short productLifeMinutes)
{
	if (productLifeMinutes <= scaleConfig->getMaxLifeMinutes())
		this->productLifeMinutes = (productLifeMinutes <= 0) ? 0 : productLifeMinutes;
}

void Product::adjustProductLifeMinutes(int newProductLifeMinutes)
{
	// TODO jhc - This will need to be some interger division + mod thing.  
	return;
}

HobDateTime Product::getProductLife()
{
	setProductLife();			//	To set the current date & time.
	return productLife;
}
void Product::setProductLife() const
{
/*
 * Note: The productLifeDays and productLifeHours need to have been set before calling this method.
 */
 
	productLife.setToCurrent();
	productLife.incrementDays(-backDateDays);
	productLife.incrementDays(productLifeDays);
	productLife.incrementHours((int)productLifeHours);
	productLife.incrementMinutes((int)productLifeMinutes);

	if (Debug::isTracing("Product.lifeDates"))
	{
		HobString msgDebug = "setProductLife():  ";
		msgDebug.append("backDateDays = ").append(Integer::toString(backDateDays));
		msgDebug.append("  productLifeDays = ").append(Integer::toString(productLifeDays));
		msgDebug.append("  productLifeHours = ").append(Integer::toString(productLifeHours));
		msgDebug.append("  productLife = ").append(productLife.formatDate(FormatDate_MMDDYY));
		Debug::trace("Product.lifeDates", msgDebug);
	}
}
void Product::setProductLife(const HobString &newDate)
{
	HobDateTime newBestBeforeDate;
	newBestBeforeDate.setDateParts(newDate);
	if (!newBestBeforeDate.isValid())
		return;

	HobDateTime now;
	long nowJulian = now.getJulianDay();
	long newJulian = newBestBeforeDate.getJulianDay();
	int deltaDays = (int) (newJulian - nowJulian);
	// The user can't set the best before date to a date before today!!!
	if (deltaDays < 0)
		return;

	HobDateTime origBestBeforeDate = productLife;
	HobString origBestBeforeDateString = "";
	getFieldAsString(fieldOffsetBestBeforeDate, origBestBeforeDateString, BrowserFormat);
	Debug::trace("Product", HobString("updating best before date - orig date:") + origBestBeforeDateString);
	Debug::trace("Product", HobString("origBestBeforeDate.show - "));
	if ( Debug::isTracing("Product") )
		origBestBeforeDate.show();

	Debug::trace("Product", HobString("newBestBeforeDate.show - "));
	if ( Debug::isTracing("Product") )
		newBestBeforeDate.show();

	long origTime = origBestBeforeDate.getJulianDay();
	long newTime = newBestBeforeDate.getJulianDay();
	deltaDays = (int) (newTime - origTime);
	Debug::trace("Product", HobString("deltaDays - ") + HobString(Integer::toString(deltaDays)));

	// Nothing to do, the date is already set to the value passed in.
	if (deltaDays == 0)
		return;

	Debug::trace("Product", HobString("current product life days - ") + HobString(Integer::toString(productLifeDays)));

	//	Update the product life days - it also updates the best before date.
	setProductLifeDays(productLifeDays + deltaDays);
	Debug::trace("Product", HobString("new product life days - ") + HobString(Integer::toString(productLifeDays)));
}

short Product::getPoints() const
{
	return points;
}
void Product::setPoints(short points)
{
	if (points <= scaleConfig->getMaxBonusPoints())
		this->points = points;
}

short Product::getPortions() const
{
	Debug::trace("Product.Portions", "getPortions():  " + inttostring(portions));
	return portions;
}
void Product::setPortions(short value)
{
	if (portions <= 99)
		portions = value;
	Debug::trace("Product.Portions", "setPortions():  " + inttostring(portions));
}

WrapperIntakeMode::IntakeMode Product::getIntakeMode() const
{
	return intakeMode;
}
void Product::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	this->intakeMode = intakeMode;
}

short Product::getPriceModifier() const
{
	return priceModifier;
}
void Product::setPriceModifier(HobString newPriceModifier)
{
	short sPM = Integer::valueOf(newPriceModifier);

	//this function is used for both information from the browser and
	//info from the restore/comm function

	if ( sPM == scaleConfig->getQuarterPoundFor() || sPM == scaleConfig->getHalfPoundFor() || sPM == scaleConfig->getHundredGramsFor() )
		; // don't do anything, no translation needed, leave sPM as is
	else if (newPriceModifier == "1:") //metric pricing
		sPM = scaleConfig->getHundredGramsFor();
	else
	{
		if (sPM >= 1 && sPM <= 9) //data coming from browser
			; // don't do anything, no translation needed, leave sPM as is
		else if (sPM == 12) //1/2 or 50% (fractional)
			sPM = scaleConfig->getHalfPoundFor();
		else if (sPM == 14)	//1/4 or 25% (fractional)
			sPM = scaleConfig->getQuarterPoundFor();
		else if (sPM >= 20 && sPM < 100) //21,31,41,51,etc..
			sPM = sPM/10;
		else
			sPM = 1;
	}

	setPriceModifier(sPM);
}
void Product::setPriceModifier(short newPriceModifier)
{
	if (MetSig::isScaleMetric())
	{
		if (scaleConfig->isFractionalPricingEnabled() && (newPriceModifier != scaleConfig->getHundredGramsFor()))
			newPriceModifier = UNIT_PRICING;
	}
	else
	{
		if ( (scaleConfig->isFractionalPricingEnabled() && newPriceModifier != scaleConfig->getQuarterPoundFor() && newPriceModifier != scaleConfig->getHalfPoundFor())
			 || ( scaleConfig->isPoundsForPricingEnabled() && (newPriceModifier <= UNIT_PRICING || newPriceModifier > 9)) )
			newPriceModifier = UNIT_PRICING;
	}
	this->priceModifier = newPriceModifier;

	updateTotalValuesData();
}


HobRecordNum Product::getReportingClassNum() const
{
	return ((reportingClass) ? reportingClass->getClassNum() : Class::GRAND_TOTALS_CLASS);
}
void Product::setReportingClassNum(HobRecordNum reportingClassNum)
{
	if (reportingClass)
	{
		if ((reportingClassNum > 0) && (reportingClassNum < Class::GRAND_TOTALS_CLASS))
			reportingClass->setClassNum(reportingClassNum);
		else
			reportingClass->setClassNum(Class::GRAND_TOTALS_CLASS);
	}
}

Class *Product::getReportingClass()
{
	return reportingClass;
}
void Product::setReportingClass(Class *reportingClass)
{
	if (this->reportingClass != NULL)
	{
		delete this->reportingClass;
	}
	this->reportingClass = reportingClass;
}


ProductText *Product::getProductText(ProdSupportTextType type)
{
	if (type > psttUnknown && type < psttLastProductText)
		return productTexts_[type];
	else
		return 0;
}
void Product::setProductText(ProdSupportTextType type, ProductText *productText)
{
	if (type > psttUnknown && type < psttLastProductText)
	{
		if (productTexts_[type])
			delete productTexts_[type];
		productTexts_[type] = productText;

		HobString textNum = productTexts_[type] ? Long::toString(productTexts_[type]->getTextNum()) : "0";
		Debug::trace("Product.ProductText", "setProductText():  type:" + inttostring(type) + " productTextNum:" + textNum);
	}
}

void Product::setProductTextText(ProdSupportTextType type, HobString productTextText)
{
	//	This method sets the text in the Product Text record.
	if (type > psttUnknown && type < psttLastProductText)
	{
		if (productTexts_[type])
		{
			productTexts_[type]->setText(productTextText);

			Debug::trace("Product.ProductText", "setProductTextText():  type:" + inttostring(type) + "  num:" + longtostring(productTexts_[type]->getTextNum()) + "  text:" + productTextText);
		}
	}
}


bool Product::hasNutrifacts() const
{
	return (nutrifacts && nutrifacts->getErrorCode() == RECORD_OK) ? true : false;
}
Nutrifacts *Product::getNutrifacts()
{
	return nutrifacts;
}
void Product::setNutrifacts(Nutrifacts *nutrifacts)
{
	if (this->nutrifacts != NULL)
	{
		delete this->nutrifacts;
	}
	this->nutrifacts = nutrifacts;
}
void Product::setNutrifactsTextCol1(HobString nutrifactsText)
{
//	This method sets the text in the Nutrifacts record.
	Debug::trace("Product.Nutrifacts", HobString("going to setNutrifactsTextCol1(): text = ") + nutrifactsText);
	if (this->nutrifacts != NULL)
	{
		this->nutrifacts->setColumn1Data(nutrifactsText);
		Debug::trace("Product.Nutrifacts", HobString("completed setNutrifactsTextCol1(): text = ") + this->nutrifacts->getColumn1Data());
	}
}
void Product::setNutrifactsTextCol2(HobString nutrifactsText)
{
//	This method sets the text in the Nutrifacts record.
	if (this->nutrifacts != NULL)
	{
		this->nutrifacts->setColumn2Data(nutrifactsText);
	}
}
void Product::setNutrifactsTextCol3(HobString nutrifactsText)
{
//	This method sets the text in the Nutrifacts record.
	if (this->nutrifacts != NULL)
	{
		this->nutrifacts->setColumn3Data(nutrifactsText);
	}
}
void Product::setNutrifactsTextCol4(HobString nutrifactsText)
{
//	This method sets the text in the Nutrifacts record.
	if (this->nutrifacts != NULL)
	{
		this->nutrifacts->setColumn4Data(nutrifactsText);
	}
}


Graphic *Product::getGraphic(unsigned int graphicIndex) const
{
	Graphic *returnGraphic = (Graphic *)NULL;

	/* Graphics are numbered 1, 2, 3, ..., up to the maximum number of graphics configured in
	 * the system (check Configuration).
	 */

	if (graphicIndex < graphics.size())
	{
		returnGraphic = graphics[graphicIndex];
	}
	return returnGraphic;
}
void Product::setGraphic(unsigned int graphicIndex, Graphic *graphic)
{
	Graphic *currentGraphic = (Graphic *)NULL;

	if (graphicIndex < graphics.size())
	{
		currentGraphic = this->graphics[graphicIndex];
		if(currentGraphic != NULL)
		{
			delete this->graphics[graphicIndex];
		}
		this->graphics[graphicIndex] = graphic;
	}
	if (Debug::isTracing("Product.Graphic"))
	{
		HobString myDebug("setGraphic():   Product = ");
		myDebug.append(Hex::toString((long long) this));
		myDebug.append("    graphicIndex = ").append(Integer::toString(graphicIndex));
		myDebug.append("    graphic = ").append(Hex::toString((long long) graphic));
		myDebug.append("    graphics[graphicIndex] = ").append(Hex::toString((long long) graphics[graphicIndex]));
		Debug::trace("Product.Graphic", myDebug);
	}
}


HobString Product::getGraphicName(unsigned int graphicIndex) const
{
	HobString graphicName("");
	Graphic *graphic = (Graphic *)NULL;

	/* Graphics are numbered 1, 2, 3, ..., up to the maximum number of graphics configured in
	 * the system (check Configuration).
	 */

	if (graphicIndex < graphics.size())
	{
		graphic = graphics[graphicIndex];
	}
	if(graphic != NULL)
	{
		graphicName = graphic->getGraphicName();
	}
	return graphicName;
}


LabelType *Product::getLabelType(short printerNum) const
{
	LabelType *returnLabelType = (LabelType *)NULL;

	if (labelBeingPrinted == PRINTING_NEW_EXPIRATION_LABEL || labelBeingPrinted == PRINTING_EXISTING_EXPIRATION_LABEL)
	{
		if (printerNum == FirstPrinter)
			returnLabelType = shelfLifeExpirationLabelType;
	}
	else if (labelBeingPrinted == PRINTING_RUN_TOTALS_LABEL)
	{
		if (printerNum == FirstPrinter)
			returnLabelType = runTotalsLabelType;
	}
	else
	{
		//	LabelTypes are numbered 1, 2, 3, ..., up to the maximum number of printers configured in the system.
		if (printerNum >= FirstPrinter)
		{
			if (printerNum < (short) labelTypes.size()) // FT-815: fix crash
			{
				if ((printerNum == FirstPrinter) && ((short)labelTypes.size() >= 1))
					returnLabelType = labelTypes[printerNum];
				else if ((printerNum == SecondPrinter) && ((short)labelTypes.size() >= 2))
					returnLabelType = labelTypes[printerNum];
				else if ((printerNum == ThirdPrinter ) && ((short)labelTypes.size() == 3))
				returnLabelType = labelTypes[printerNum];
	}
		}
	}
	
	Debug::trace("Product.getLabelType", "getLabelType() returnLabelType: " + Hex::toString((long long)returnLabelType) );
	
	return returnLabelType;
}

void Product::setLabelType(short printerNum, LabelType *labelType)
{
	Util::CLockCriticalSection lock( labelTypesToDeleteCS );

	if ((( FirstPrinter <= printerNum ) && ( printerNum < MaxPrinterTypes )) || printerNum == ExtraPrinter_UsedForSelfServeScreenBarcodeLabel )
	{
		//Debug::trace("Product.setLabelType", HobString("setLabelType label = ") + longtostring(labelType->getLabelTypeNum()) );

		if (labelType && ((labelType->getLabelTypeNum() < 0) || (labelType->getLabelTypeNum() > scaleConfig->getMaxLabelNumber())))
			labelType->setLabelTypeNum(0);

		if (labelTypes[printerNum])
			labelTypesToDelete.push_back(labelTypes[printerNum]);

		labelTypes[printerNum] = labelType;
	}
}

void Product::reverseLabelOrder() 
{
	Debug::trace("PrinterManager.VirtualCombine", "reverseLabelOrder()");

	// SOF-3913: reverse printing order when cutter and specific modes
	bool reverseOrder = false; // TODO RLB

	if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualCombinePrint) )
	{
		Session *priSession = SessionManager::getInstance()->getPrimaryConsoleSession();

		if (( priSession ) && (
			( OpModeProductionEntry == priSession->getCurrentMode() ) ||
			( OpModeManualWeigh == priSession->getCurrentMode() ) || 
			( OpModeAutoManualApply == priSession->getCurrentMode() ) ||
			( OpModeAccumulateItems == priSession->getCurrentMode() ) ||
			( OpModeBuildAMeal == priSession->getCurrentMode() ) ||
			( OpModeSelfService == priSession->getCurrentMode() ) ) )
		{
			if (( PRINTING_NEW_EXPIRATION_LABEL != labelBeingPrinted ) &&
				( PRINTING_EXISTING_EXPIRATION_LABEL != labelBeingPrinted ) &&
				( PRINTING_RUN_TOTALS_LABEL != labelBeingPrinted ))
			{
				reverseOrder = priSession->isCutterInstalledAndEnabled();
			}
		}
	}

	//SOF-4043: Only reverse order one time for every instance
	if ( ( reverseOrder ) && ( revOrderTypes != labelTypes ) ) 
	{
		LabelType* temp = labelTypes[0];

		labelTypes[0] = labelTypes[2];
		labelTypes[2] = temp;

		if ( !labelTypes[0] )
		{
			labelTypes[0] = labelTypes[1];
			labelTypes[1] = labelTypes[2];
			labelTypes[2] = NULL;
		}

		if ( !labelTypes[0] )
		{
			labelTypes[0] = labelTypes[1];
			labelTypes[1] = NULL;
		}
		//SOF-4043 Keep a copy of the new ordering
		revOrderTypes = labelTypes;
	}

	if ( getLabelType(0) )
		currentProductTransaction_->setLabel1(getLabelType(0)->getLabelTypeNum());
	if ( getLabelType(1) )
		currentProductTransaction_->setLabel2(getLabelType(1)->getLabelTypeNum());
	if ( getLabelType(2) )
		currentProductTransaction_->setLabel3(getLabelType(2)->getLabelTypeNum());
	if ( getLabelType(3) )
		currentProductTransaction_->setLabel4(getLabelType(3)->getLabelTypeNum());
}

bool Product::isPick5Active() const
{
	Configuration *config = Configuration::getInstance();
	if (!config)
		return false;
	bool result = ( config->isPackageLicensed("SCALE_PICK5") && config->toBoolCfg(fieldOffsetCfgPick5Active) && (weightMin > 0 || weightMax > 0 || totalPriceMin > 0 || totalPriceMax > 0) );
	Debug::trace("Product.Pick5", string("isPick5Active() - result is ") + booltostring(result));
	return result;
}

/* For the 'label types by mode' project. This method will set up the Product's Label Type objects for the indicated mode.
 * If all 3 label type numbers for that mode are 0, then the default label type numbers are used.
 * Note that the first time this method is called, the Product's fieldOffsetOfFirstLabelTypeNum attribute is initialized to
 * fieldOffsetProductUnknownField, so the first time in will always set up the label types.
 */

// When Walmart Cake Label (branch-walmart-cake-label) was merged into trunk "isRemotePrintMsgReceived" no longer existed,
// so the last parameter was removed.
//bool Product::selectLabelTypesForMode(OpModeEvent mode, bool isRemotePrintMsgReceived)
bool Product::selectLabelTypesForMode(OpModeEvent mode)
{
	bool isTracing = Debug::isTracing("Product.LabelType");
	int currentActiveLabelTypeNumOffset = fieldOffsetOfFirstLabelTypeNum;
	if (isTracing)
		Debug::trace("Product.LabelType", "selectLabelTypesForMode(): currentActiveLabelTypeNumOffset=" + inttostring(currentActiveLabelTypeNumOffset));

	// Initially, use the default label types.
	fieldOffsetOfFirstLabelTypeNum = fieldOffsetDefaultLabelTypeNum1;

	if (dynamic_cast<OnlineOrderProduct *> (this))
	{
		fieldOffsetOfFirstLabelTypeNum = fieldOffsetOnlineOrderingLabelTypeNum1;
		Debug::trace("OnlineOrderProduct", "selectLabelTypesForMode(): using labelType offset for Online Ordering label - fieldOffsetOfFirstLabelTypeNum=" + inttostring(fieldOffsetOfFirstLabelTypeNum));
		// TODO-OO - Hmmm, can you order online an item that is Pick5?  If so, then which labels do you want to use
	}
	else if (isPick5Active())
		fieldOffsetOfFirstLabelTypeNum = fieldOffsetPick5LabelTypeNum1;
	else
	{
		switch (mode)
		{
			case OpModeManualWeigh:
				fieldOffsetOfFirstLabelTypeNum = fieldOffsetManualLabelTypeNum1;
				break;

			case OpModeProductionEntry:
				// case OpModePrintMultipleLabels:
				fieldOffsetOfFirstLabelTypeNum = fieldOffsetProdEntryLabelTypeNum1;
				break;

			case OpModeSelfService:
				fieldOffsetOfFirstLabelTypeNum = fieldOffsetSelfServLabelTypeNum1;
				break;

			case OpModeAutoWeigh:
			case OpModeSemiWeigh:
			case OpModeAutoManualApply:  // Chose "Prepack Operation" on Mode Dialog Counter Scale
				fieldOffsetOfFirstLabelTypeNum = fieldOffsetPrepackLabelTypeNum1;
				break;

			default:
				// Modes not considered: OpModeIdle, OpModeOperate, OpModePluEntry, OpModeSupervisor, OpModeService, OpModeServiceLogin,
				//                       OpModePowerDown, OpModeMonitor, OpModeStatus, OpModeAccumulateItems, OpModeBuildAMeal, OpModeWrapOnly
			break;
		}
	}

	if (isTracing)
		Debug::trace("Product.LabelType", string("selectLabelTypesForMode(): fieldOffsetOfFirstLabelTypeNum should be ") + inttostring(fieldOffsetOfFirstLabelTypeNum));

	if (fieldOffsetOfFirstLabelTypeNum != fieldOffsetDefaultLabelTypeNum1)
	{
		// If all of the label type nums for that mode are 0, revert back to the default label type nums
		if (getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum) == 0 && getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum + 1) == 0 && getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum + 2) == 0)
		{
			if (isTracing)
				Debug::trace("Product.LabelType", "selectLabelTypesForMode(): all label type nums are 0, using fieldOffsetDefaultLabelTypeNum1");
			fieldOffsetOfFirstLabelTypeNum = fieldOffsetDefaultLabelTypeNum1;
		}
	}

	// Return true if the set of active label types changed.
	Debug::trace("Product.LabelType", string("selectLabelTypesForMode(): returning ") + booltostring((fieldOffsetOfFirstLabelTypeNum != currentActiveLabelTypeNumOffset)));
	return (fieldOffsetOfFirstLabelTypeNum != currentActiveLabelTypeNumOffset);
}

void Product::setupLabelTypesForMode()
{
	Debug::trace("Product.LabelType", "begin setupLabelTypesForMode()");

	setLabelType(FirstPrinter, LabelType::findRecord(getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum), deptNum, dbAccess));
	setLabelType(SecondPrinter, LabelType::findRecord(getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum + 1), deptNum, dbAccess));
	setLabelType(ThirdPrinter, LabelType::findRecord(getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum + 2), deptNum, dbAccess));

	//	The following line can't be called until the label type objects within the Product class are set up!!
	//  Now that the labelTypes have been loaded, set up the barcode information.
	barCodeNum.getFromDatabase(myDbRecord, this, dbAccess);

	if (currentProductTransaction_)
	{
		if (getLabelType(FirstPrinter))
			currentProductTransaction_->setLabel1(getLabelType(FirstPrinter)->getLabelTypeNum());
		if (getLabelType(SecondPrinter))
			currentProductTransaction_->setLabel2(getLabelType(SecondPrinter)->getLabelTypeNum());
		if (getLabelType(ThirdPrinter))
			currentProductTransaction_->setLabel3(getLabelType(ThirdPrinter)->getLabelTypeNum());
	}
}


/* Return the label type num offset for the active set of label types being used, based on the mode (determined by a previous call to selectLabelTypesForMode(...)).
 */
int Product::getActiveLabelTypeNumOffsetForMode(short printerNum)
{
	return fieldOffsetOfFirstLabelTypeNum + printerNum;
}

/* Update the label type num for the active set of label types being used, based on the mode (determined by a previous call to selectLabelTypesForMode(...)).
 * The label type num of the label type being set in this method could be different if a better match was found in the family (Operate mode).
 */
void Product::updateLabelTypeNumForMode(short printerNum, HobRecordNum newLabelTypeNum)
{
	if ( getLabelTypeNum(fieldOffsetOfFirstLabelTypeNum + printerNum) != newLabelTypeNum )
		setLabelTypeNum( (fieldOffsetOfFirstLabelTypeNum + printerNum), newLabelTypeNum );
}

HobRecordNum Product::getLabelTypeNum(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetLabelTypeNum1:
		case fieldOffsetDefaultLabelTypeNum1:
			return defaultLabelTypeNum1;
			break;
		case fieldOffsetLabelTypeNum2:
		case fieldOffsetDefaultLabelTypeNum2:
			return defaultLabelTypeNum2;
			break;
		case fieldOffsetLabelTypeNum3:
		case fieldOffsetDefaultLabelTypeNum3:
			return defaultLabelTypeNum3;
			break;
		case fieldOffsetManualLabelTypeNum1:
			return manualLabelTypeNum1;
			break;
		case fieldOffsetManualLabelTypeNum2:
			return manualLabelTypeNum2;
			break;
		case fieldOffsetManualLabelTypeNum3:
			return manualLabelTypeNum3;
			break;
		case fieldOffsetPrepackLabelTypeNum1:
			return prepackLabelTypeNum1;
			break;
		case fieldOffsetPrepackLabelTypeNum2:
			return prepackLabelTypeNum2;
			break;
		case fieldOffsetPrepackLabelTypeNum3:
			return prepackLabelTypeNum3;
			break;
		case fieldOffsetProdEntryLabelTypeNum1:
			return prodEntryLabelTypeNum1;
			break;
		case fieldOffsetProdEntryLabelTypeNum2:
			return prodEntryLabelTypeNum2;
			break;
		case fieldOffsetProdEntryLabelTypeNum3:
			return prodEntryLabelTypeNum3;
			break;
		case fieldOffsetSelfServLabelTypeNum1:
			return selfServLabelTypeNum1;
			break;
		case fieldOffsetSelfServLabelTypeNum2:
			return selfServLabelTypeNum2;
			break;
		case fieldOffsetSelfServLabelTypeNum3:
			return selfServLabelTypeNum3;
			break;
		case fieldOffsetPick5LabelTypeNum1:
			return pick5LabelTypeNum1;
			break;
		case fieldOffsetPick5LabelTypeNum2:
			return pick5LabelTypeNum2;
			break;
		case fieldOffsetPick5LabelTypeNum3:
			return pick5LabelTypeNum3;
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum1:
			return onlineOrderingLabelTypeNum1;
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum2:
			return onlineOrderingLabelTypeNum2;
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum3:
			return onlineOrderingLabelTypeNum3;
			break;
		default:
			break;
	}
	return RECORD_NOT_FOUND;
}

void Product::setLabelTypeNum(int fieldOffset, HobRecordNum newLabelTypeNum)
{
	switch (fieldOffset)
	{
		case fieldOffsetLabelTypeNum1:
		case fieldOffsetDefaultLabelTypeNum1:
			defaultLabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetLabelTypeNum2:
		case fieldOffsetDefaultLabelTypeNum2:
			defaultLabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetLabelTypeNum3:
		case fieldOffsetDefaultLabelTypeNum3:
			defaultLabelTypeNum3 = newLabelTypeNum;
			break;
		case fieldOffsetManualLabelTypeNum1:
			manualLabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetManualLabelTypeNum2:
			manualLabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetManualLabelTypeNum3:
			manualLabelTypeNum3 = newLabelTypeNum;
			break;
		case fieldOffsetPrepackLabelTypeNum1:
			prepackLabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetPrepackLabelTypeNum2:
			prepackLabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetPrepackLabelTypeNum3:
			prepackLabelTypeNum3 = newLabelTypeNum;
			break;
		case fieldOffsetProdEntryLabelTypeNum1:
			prodEntryLabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetProdEntryLabelTypeNum2:
			prodEntryLabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetProdEntryLabelTypeNum3:
			prodEntryLabelTypeNum3 = newLabelTypeNum;
			break;
		case fieldOffsetSelfServLabelTypeNum1:
			selfServLabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetSelfServLabelTypeNum2:
			selfServLabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetSelfServLabelTypeNum3:
			selfServLabelTypeNum3 = newLabelTypeNum;
			break;
		case fieldOffsetPick5LabelTypeNum1:
			pick5LabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetPick5LabelTypeNum2:
			pick5LabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetPick5LabelTypeNum3:
			pick5LabelTypeNum3 = newLabelTypeNum;
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum1:
			onlineOrderingLabelTypeNum1 = newLabelTypeNum;
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum2:
			onlineOrderingLabelTypeNum2 = newLabelTypeNum;
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum3:
			onlineOrderingLabelTypeNum3 = newLabelTypeNum;
			break;
		default:
			break;
	}
}

// static
bool Product::isLabelTypeNumField(int fieldOffset)
{
	switch (fieldOffset)
	{
		case fieldOffsetLabelTypeNum1:
		case fieldOffsetLabelTypeNum2:
		case fieldOffsetLabelTypeNum3:
		case fieldOffsetDefaultLabelTypeNum1:
		case fieldOffsetDefaultLabelTypeNum2:
		case fieldOffsetDefaultLabelTypeNum3:
		case fieldOffsetManualLabelTypeNum1:
		case fieldOffsetManualLabelTypeNum2:
		case fieldOffsetManualLabelTypeNum3:
		case fieldOffsetPrepackLabelTypeNum1:
		case fieldOffsetPrepackLabelTypeNum2:
		case fieldOffsetPrepackLabelTypeNum3:
		case fieldOffsetProdEntryLabelTypeNum1:
		case fieldOffsetProdEntryLabelTypeNum2:
		case fieldOffsetProdEntryLabelTypeNum3:
		case fieldOffsetSelfServLabelTypeNum1:
		case fieldOffsetSelfServLabelTypeNum2:
		case fieldOffsetSelfServLabelTypeNum3:
		case fieldOffsetPick5LabelTypeNum1:
		case fieldOffsetPick5LabelTypeNum2:
		case fieldOffsetPick5LabelTypeNum3:
		case fieldOffsetOnlineOrderingLabelTypeNum1:
		case fieldOffsetOnlineOrderingLabelTypeNum2:
		case fieldOffsetOnlineOrderingLabelTypeNum3:
			return true;
		default:
			break;
	}
	return false;
}

short Product::getLabelRotation(short printerNum) const
{
	short returnLabelRotation = 0;
	if (printerNum >= FirstPrinter && printerNum < MaxPrinterTypes)
		returnLabelRotation = labelRotations[printerNum];
	return returnLabelRotation;
}
void Product::setLabelRotation(short printerNum, short labelRotation)
{
	if (printerNum >= FirstPrinter && printerNum < MaxPrinterTypes)
	{
		if((labelRotation != 45) && (labelRotation != 90) && (labelRotation != 135) && (labelRotation != 180) &&
		   (labelRotation != 225) && (labelRotation != 270) && (labelRotation != 315) && (labelRotation != 360))
			labelRotation = 0;

		this->labelRotations[printerNum] = labelRotation;
	}
}

short Product::getLabelPlacement(short printerNum) const
{
	short returnLabelPlacement = 8; // 8 is LocNoneSelected
	if (printerNum >= FirstPrinter && printerNum < MaxPrinterTypes)
		returnLabelPlacement = labelPlacements[printerNum];
	return returnLabelPlacement;
}
void Product::setLabelPlacement(short printerNum, short labelPlacement)
{
	if (printerNum >= FirstPrinter && printerNum < MaxPrinterTypes)
		this->labelPlacements[printerNum] = labelPlacement;
}

bool Product::isApplySecurityLabel() const
{
	return applySecurityLabel;
}
void Product::setApplySecurityLabel(bool applySecurityLabel)
{
	this->applySecurityLabel = applySecurityLabel;
}

bool Product::isDontPrintPackedOn() const
{
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("isDontPrintPackedOn() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintPackedOn = ").append(Bool::toString(dontPrintPackedOn));
		Debug::trace("Product.DontPrint", myDebug);
	}
	return dontPrintPackedOn;
}
void Product::setDontPrintPackedOn(bool dontPrintPackedOn)
{
	this->dontPrintPackedOn = dontPrintPackedOn;
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("setDontPrintPackedOn() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintPackedOn = ").append(Bool::toString(dontPrintPackedOn));
		Debug::trace("Product.DontPrint", myDebug);
	}
}

bool Product::isDontPrintSellBy() const
{
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("isDontPrintSellBy() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintSellBy = ").append(Bool::toString(dontPrintSellBy));
		Debug::trace("Product.DontPrint", myDebug);
	}
	return dontPrintSellBy;
}
void Product::setDontPrintSellBy(bool dontPrintSellBy)
{
	this->dontPrintSellBy = dontPrintSellBy;
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("setDontPrintSellBy() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintSellBy = ").append(Bool::toString(dontPrintSellBy));
		Debug::trace("Product.DontPrint", myDebug);
	}
}

bool Product::isDontPrintBestBefore() const
{
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("isDontPrintBestBefore() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintBestBefore = ").append(Bool::toString(dontPrintBestBefore));
		Debug::trace("Product.DontPrint", myDebug);
	}
	return dontPrintBestBefore;
}
void Product::setDontPrintBestBefore(bool dontPrintBestBefore)
{
	this->dontPrintBestBefore = dontPrintBestBefore;
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("setDontPrintBestBefore() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintBestBefore = ").append(Bool::toString(dontPrintBestBefore));
		Debug::trace("Product.DontPrint", myDebug);
	}
}

bool Product::isDontPrintSellByOverridden() const
{
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("isDontPrintSellByOverridden() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintSellByOverridden = ").append(Bool::toString(dontPrintSellByOverridden));
		Debug::trace("Product.DontPrint", myDebug);
	}
	return dontPrintSellByOverridden;
}
void Product::setDontPrintSellByOverridden(bool overridden)
{
	dontPrintSellByOverridden = overridden;
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("setDontPrintSellByOverridden() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintSellByOverridden = ").append(Bool::toString(dontPrintSellByOverridden));
		Debug::trace("Product.DontPrint", myDebug);
	}
}

bool Product::isDontPrintBestBeforeOverridden() const
{
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("isDontPrintBestBeforeOverridden() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintBestBeforeOverridden = ").append(Bool::toString(dontPrintBestBeforeOverridden));
		Debug::trace("Product.DontPrint", myDebug);
	}
	return dontPrintBestBeforeOverridden;
}
void Product::setDontPrintBestBeforeOverridden(bool overridden)
{
	dontPrintBestBeforeOverridden = overridden;
	if (Debug::isTracing("Product.DontPrint"))
	{
		HobString myDebug("setDontPrintBestBeforeOverridden() product: ");
		myDebug.append(Hex::toString((long long) this)).append("   dontPrintBestBeforeOverridden = ").append(Bool::toString(dontPrintBestBeforeOverridden));
		Debug::trace("Product.DontPrint", myDebug);
	}
}

const HobString &Product::getProductGraphicName() const
{
	return productGraphicName;
}
void Product::setProductGraphicName(const HobString &productGraphicName)
{
	this->productGraphicName = productGraphicName;
	if (Debug::isTracing("Product.Graphic"))
	{
		HobString myDebug("setProductGraphicName():   productGraphicName = [");
		myDebug.append(this->productGraphicName).append("]");
		Debug::trace("Product.Graphic", myDebug);
	}
}

const HobString &Product::getProductName1() const
{
	return productName1;
}
void Product::setProductName1(const HobString &name)
{
	if ((int)(name.length()) > scaleConfig->toIntCfgI(CfgInternal::PRODUCTNAMEMAXLENGTH))
	{
		productName1.assign(name, 0, scaleConfig->toIntCfgI(CfgInternal::PRODUCTNAMEMAXLENGTH));
		Debug::trace("ClipText", "productName1 clipped to [" + productName1 + "]");
	}
	else
	{
		productName1 = name;
		Debug::trace("ClipText", "productName1 is [" + productName1 + "]");
	}
}

const HobString &Product::getProductName2() const
{
	return productName2;
}
void Product::setProductName2(const HobString &name)
{
	if ((int)(name.length()) > scaleConfig->toIntCfgI(CfgInternal::PRODUCTNAMEMAXLENGTH))
	{
		productName2.assign(name, 0, scaleConfig->toIntCfgI(CfgInternal::PRODUCTNAMEMAXLENGTH));
		Debug::trace("ClipText", "productName2 clipped to [" + productName2 + "]");
	}
	else
	{
		productName2 = name;
		Debug::trace("ClipText", "productName2 is [" + productName2 + "]");
	}
}

HobRecordNum Product::getCategoryNum() const
{
	return ((category) ? category->getCategoryNum() : RECORD_NOT_FOUND);
}
void Product::setCategoryNum(HobRecordNum catNum)
{
	if (category)
		category->setCategoryNum(catNum);
	// todo - this isn't enough, the category number was updated, but the rest of the data in the Category object has not been updated
}

Category *Product::getCategory() const
{
	return category;
}
void Product::setCategory(Category *cat)
{
	if (category)
		delete category;
	category = cat;
}

HobRecordNum Product::getSpecialNum() const
{
	return ((specialProgram) ? specialProgram->getSpecialNum() : RECORD_NOT_FOUND);
}
void Product::setSpecialNum(HobRecordNum spNum)
{
	if (specialProgram)
		specialProgram->setSpecialNum(spNum);
	// todo - this isn't enough, the special number was updated, but the rest of the data in the Special object has not been updated
}

Special *Product::getSpecialProgram() const
{
	return specialProgram;
}
void Product::setSpecialProgram(Special *special)
{
	if (specialProgram)
		delete specialProgram;
	specialProgram = special;
}

const HobDateTime &Product::getSpecialStartDate() const
{
	return specialStartDate;
}
void Product::setSpecialStartDate(const HobDateTime &specialStartDate)
{
	this->specialStartDate = specialStartDate;
}
void Product::setSpecialStartDate(const HobString &newDate)
{
	specialStartDate.setDateParts(newDate);
}


/* This function came about because the xt tag has dual purpose (tare or exception price for a by count item),
* and because fields can be received in any order, so we may not know how to treat the data received at the time
* that we are processing that field in the updateField() method.
*/
void Product::applyRulesToReceivedFields(int formatType)
{
	Debug::trace("Product", HobString("applyRulesToReceivedFields() ") + HobString(Integer::toString(formatType)));

	if ((isReceived(fieldOffsetPluFlags) || isReceived(fieldOffsetTotalsFlags)) && !isReceived(fieldOffsetProductType))
		typeUnknownProduct();

	if (isRandomWeight())
		byCount = 0;

	if (isReceived(fieldOffsetBarcodeUpcNum))
	{
		// SOF-3880: when internal config set pretend field doesn't exist
		if ( (!isReceived(fieldOffsetBarcodeProductNum)) || (Configuration::getInstance()->isIgnoreVendorAndProductNumTagsWhenUpcNumTag()) )
		{
			barCodeNum.setProductNumber("");
			setReceived(fieldOffsetBarcodeProductNum, true);
		}
		// SOF-3880: when internal config set pretend field doesn't exist
		if ( (!isReceived(fieldOffsetBarcodeVendorNum)) || (Configuration::getInstance()->isIgnoreVendorAndProductNumTagsWhenUpcNumTag()) )
		{
			barCodeNum.setVendorNumber("");
			setReceived(fieldOffsetBarcodeVendorNum, true);
		}
		if (!isReceived(fieldOffsetBarcodeEANCode))
		{
			barCodeNum.setEANCode("");
			setReceived(fieldOffsetBarcodeEANCode, true);
		}
	}

	if (isReceived(fieldOffsetExceptionPriceTare))
	{
		if (isRandomWeight())
		{
			Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: product is random weight - received ta tag? ") + HobString(Bool::toString(isReceived(fieldOffsetTareAValue))));
			if (exceptionPriceTare == HobString("65535") && !isReceived(fieldOffsetTareAForced))
			{
				Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: setting to forced tare"));
				tareA.setForced(true);
				setTareA(0);
				setReceived(fieldOffsetTareAValue, true);
				setReceived(fieldOffsetTareAForced, true);
			}
			else if (!isReceived(fieldOffsetTareAValue))
			{
				Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: setting tare"));

				// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating the tare. Only if correct weigher model. -- ats
				if ((EvolutionTaggedFormat == formatType) || (UltimaTaggedFormat == formatType))
					if (2 == scaleConfig->getExternalWeightPrecision())
						exceptionPriceTare.append("0");

				// Same code as in fieldOffsetTareAValue processing below
				// set tare!
				setTareA( MetSig::determineImplicitDecimalWeight(exceptionPriceTare) );

				// mark receieved
				setReceived(fieldOffsetTareAValue, true);
				Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: set tare to ") + HobString(Long::toString(tareA.getValue())));
			}
			else
				Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: not setting anything"));
		}
		else
		{
			Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: product is NOT random weight"));
			if (!isReceived(fieldOffsetExceptionPrice))
			{
				Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: haven't received the exception price, so setting it"));
				// Same code as in fieldOffsetExceptionPrice processing above

				setExceptionPrice( MetSig::determineImplicitDecimalPrice(exceptionPriceTare) );

				setReceived(fieldOffsetExceptionPrice, true);
			}
			else
				Debug::trace("Product", HobString("applyRulesToReceivedFields() - fieldOffsetExceptionPriceTare: not setting anything2"));
		}
	}

	if (!isReceived(fieldOffsetUnitPriceForced) && isReceived(fieldOffsetUnitPrice))
	{
		setReceived(fieldOffsetUnitPriceForced, true);
		setUnitPriceForced(false);
	}

	if (!isReceived(fieldOffsetByCountForced) && isReceived(fieldOffsetByCount))
	{
		setReceived(fieldOffsetByCountForced, true);
		setByCountForced(false);
	}

	if (!isReceived(fieldOffsetTareAForced) && isReceived(fieldOffsetTareAValue))
	{
		setReceived(fieldOffsetTareAForced, true);
		tareA.setForced(false);
	}

	// If we receive the deptNum field, it's possible that we received it after we receive the number fields for support records (ie. class number, expanded text number, etc.).
	// such as when the tag for deptNum field comes after the tag for any of these support record numbers.
	// If this happens, the support record objects that were set up in the updateField() method that was called in the Product constructor 
	// may be for the wrong dept (if the deptNum received is different than the initial deptNum value), so just resetup those support record objects.
	if (isReceived(fieldOffsetProductDeptNum))
	{
		if (isReceived(fieldOffsetProductReportingClassNum) && getReportingClassNum() != RECORD_NOT_FOUND)
			updateField(fieldOffsetProductReportingClassNum, longtostring(getReportingClassNum()), false);

		// TODO - Larry - done
		/*  No longer need to reprocess the received label type numbers, since we aren't dealing with the LabelType objects anymore when receiving data.
		 *  We are only receiving the label type numbers now, then ProductManager will set up the LabelType objects for the product when it needs to.
		 *
		if (isReceived(fieldOffsetLabelTypeNum1) && getLabelType(FirstPrinter))
			updateField(fieldOffsetLabelTypeNum1, longtostring(getLabelType(FirstPrinter)->getLabelTypeNum()), false);

		if (isReceived(fieldOffsetLabelTypeNum2) && getLabelType(SecondPrinter))
			updateField(fieldOffsetLabelTypeNum2, longtostring(getLabelType(SecondPrinter)->getLabelTypeNum()), false);

		if (isReceived(fieldOffsetLabelTypeNum3) && getLabelType(ThirdPrinter))
			updateField(fieldOffsetLabelTypeNum3, longtostring(getLabelType(ThirdPrinter)->getLabelTypeNum()), false);
		*/

		if (isReceived(fieldOffsetProdCategoryNum) && getCategoryNum() != RECORD_NOT_FOUND)
			updateField(fieldOffsetProdCategoryNum, longtostring(getCategoryNum()), false);

		if (isReceived(fieldOffsetProdSpecialNum) && getSpecialNum() != RECORD_NOT_FOUND)
			updateField(fieldOffsetProdSpecialNum, longtostring(getSpecialNum()), false);

		if (isReceived(fieldOffsetProdNutrifactNum) && getNutrifacts())
			updateField(fieldOffsetProdNutrifactNum, longtostring(getNutrifacts()->getTextNum()), false);

		if (isReceived(fieldOffsetExpandedTextNum) && getProductText(psttExpandedText))
			updateField(fieldOffsetExpandedTextNum, longtostring(getProductText(psttExpandedText)->getTextNum()), false);

		if (isReceived(fieldOffsetSpecialMessageNum) && getProductText(psttSpecialMessage))
			updateField(fieldOffsetSpecialMessageNum, longtostring(getProductText(psttSpecialMessage)->getTextNum()), false);

		if (isReceived(fieldOffsetProdProductNoteNum) && getProductText(psttProductNote))
			updateField(fieldOffsetProdProductNoteNum, longtostring(getProductText(psttProductNote)->getTextNum()), false);

		if (isReceived(fieldOffsetMarqueeTextNum) && getProductText(psttMarqueeText))
			updateField(fieldOffsetMarqueeTextNum, longtostring(getProductText(psttMarqueeText)->getTextNum()), false);

		if (isReceived(fieldOffsetProductText5Num) && getProductText(psttProductText5))
			updateField(fieldOffsetProductText5Num, longtostring(getProductText(psttProductText5)->getTextNum()), false);

		if (isReceived(fieldOffsetProductText6Num) && getProductText(psttProductText6))
			updateField(fieldOffsetProductText6Num, longtostring(getProductText(psttProductText6)->getTextNum()), false);

		if (isReceived(fieldOffsetProductText7Num) && getProductText(psttProductText7))
			updateField(fieldOffsetProductText7Num, longtostring(getProductText(psttProductText7)->getTextNum()), false);

		if (isReceived(fieldOffsetProductText8Num) && getProductText(psttProductText8))
			updateField(fieldOffsetProductText8Num, longtostring(getProductText(psttProductText8)->getTextNum()), false);

		if (isReceived(fieldOffsetProductText9Num) && getProductText(psttProductText9))
			updateField(fieldOffsetProductText9Num, longtostring(getProductText(psttProductText9)->getTextNum()), false);
	}
}


// This method returns true if the tare ended up changing due to exceeding the dualrange limit.
// in which case, the tare needs to be sent to the browser.
bool Product::updateTotalValuesData()
{
	/* When update the current transaction's 'total value' fields, use the current values of the
	* product fields (unit price, by count, etc.) from the Product class, not the duplicates of these
	* fields in the ProductTransaction class; they won't be set up there until the ProductTransaction
	* is to be printed.
	*/

	// The default is that this method did not result in an adjustment to the tare as a result of changing from dualRangeMode up to down.
	bool result = false;

	bool traceOn = Debug::isTracing("Product");
	if (Debug::isTracing("Memory.update1"))
		memShow(0);

	HobWeight netWeight = 0;
	bool gotNetWeight = false;
	HfcWeigherStatus *weigherStatus = 0;

	if (currentProductTransaction_)
	{
		if (hasKbdNetWeightLabel() && isRandomWeight())
		{
			netWeight = getKeyboardNetWeight();
			gotNetWeight = true;
			Debug::trace("Product", "netWeight = getKeyboardNetWeight()");
		}
		else
		{
			weigherStatus = const_cast<HfcWeigherStatus *> (currentProductTransaction_->getWeigherStatus());
//METSIG??  Do we want to consider all of the weigher statuses and weight > minWeightToPrint to be MetSig
			if (weigherStatus && (weigherStatus->isStableWeight() || weigherStatus->isSmallMotion()) && !weigherStatus->isWeigherInError())
			{
				netWeight = weigherStatus->getWeight();
				gotNetWeight = true;
			}
		}

		if (gotNetWeight)
		{
			Debug::trace("Product", HobString("Have current transaction, updating with weight: ") + HobString(Long::toString(netWeight)));

			HobWeight totalTare = 0;

			if (!hasKbdNetWeightLabel())
			{
				if (isRandomWeight() || isByCountWithWeight())
				{
					// HT-136 Use correct Tare value
					totalTare = (scaleConfig->toBoolCfg(fieldOffsetCfgUseSecondTare)) ? getTareB().getValue() : getTareA().getValue();

					MetSig::applyTare(netWeight, totalTare, weigherStatus);

					MetSig::applyTareSetback(netWeight, totalTare);
				}
				else
				{
					totalTare = 0;
				}

				if (traceOn)
				{
					HobString hstt(Long::toString(totalTare));
					HobString hsnw(Long::toString(netWeight));
					Debug::trace("Product", HobString("(before prop tare) updating transaction with: ") +
						HobString(" totltare: ") + hstt +
						HobString("  netwght: ") + hsnw);
				}

				if (isRandomWeight() && netWeight > 0 && (proportionalTare) && !scaleConfig->isPrepackScale())	//	Proportional tare only on a Random weight.
				{
					MetSig::applyProportionalTare(netWeight, totalTare, proportionalTare, weigherStatus);
				}
			}

			if (isRandomWeight() || isByCountWithWeight() ||
				(scaleConfig->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && isFixedWeight() && getFixedWeight() > 0))
			{
				currentProductTransaction_->setTotalNetWeight(netWeight);
				currentProductTransaction_->setTotalTare(totalTare);
			}

			/* Here is where we can determine if we have to switch the dualRangeDirection from up to down because the weight goes over the dualRangeLimit,
			* when we are using the net weight as the weight for the limit check.  When we are using the gross weight, the Session has already determined
			* in which direction we are moving, up or down, since in this case, the tare doesn't play any part in determining if we are over the dual range limit.
			* We now know the net weight, and can use it to determine if we are past the dualRangeLimit if the configuration parameter that indicates
			* that we should use the net weight to make this determination is set.
			*/
			if (!hasKbdNetWeightLabel())
			{
				bool changedDualRangeDirection = MetSig::checkToSwitchDualRangeDirectionOnNetWeight(weigherStatus, netWeight);
				if (changedDualRangeDirection)
					// Now that we have changed the direction from up to down, we have to recalculate the net weight with tares that are using a
					// scale factor of 1, not 5, when in avoir weighing, or that use a scale factor of 5, not 2, when in metric weighing.
					// So to do this recursively call this updateTotalValuesData() method.
					//	Dual range is only available on the HLX and new USB weigher peripheral with dual-range load cell. (haberrc)
					updateTotalValuesData();
			}

			if (isRandomWeight())
			{
				HobValue roundedTotalPrice = MetSig::calculateRandomWeightTotalPrice(hasKbdNetWeightLabel(), unitPrice, priceModifier, netWeight);
				currentProductTransaction_->setTotalPrice(roundedTotalPrice);
			}

			updateOldTotalValuesData(netWeight, weigherStatus);

			if (traceOn)
				Debug::trace("Product", "after calling updateOldTotalValuesData");
		}
		else
		{
			if (isRandomWeight() || isByCountWithWeight() ||
				(scaleConfig->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && isFixedWeight() && getFixedWeight() > 0))
			{
				currentProductTransaction_->setTotalNetWeight(0);
				currentProductTransaction_->setTotalTare(0);
			}
			if (isRandomWeight())
				currentProductTransaction_->setTotalPrice(0);

			updateOldTotalValuesData(-1);
		}
		//	Calculate Frequent Shopper values?
		currentProductTransaction_->calculateFSDiscounts(this);
	}

	if (Debug::isTracing("Memory.update2"))
		memShow(0);

	return result;
}


void Product::updateOldTotalValuesData(HobWeight netWeight, HfcWeigherStatus * weigherStatus)
{
	bool traceOn = Debug::isTracing("Product.oldTotals");
	//	bool gotNetWeight = false;

	if (currentProductTransaction_)
	{
		if (netWeight != -1)
		{
			HobWeight totalTare = 0;

			if (traceOn)
				Debug::trace("Product.oldTotals", "Have current transaction, updating with weight: " + util_string::longtostring(netWeight));

			if (!hasKbdNetWeightLabel() && weigherStatus)
			{
				if (isRandomWeight() || isByCountWithWeight())
				{
					if (traceOn)
						Debug::trace("Product.oldTotals", "old tare is: " + util_string::longtostring(currentProductTransaction_->getOldTare()));

					totalTare = currentProductTransaction_->getOldTare();
					MetSig::applyTare(netWeight, totalTare, weigherStatus);

					if (traceOn)
						Debug::trace("Product.oldTotals", "old total tare is: " + util_string::longtostring(totalTare));

					MetSig::applyTareSetback(netWeight, totalTare);
				}

				if (isRandomWeight() && (proportionalTare) && !scaleConfig->isPrepackScale())	//	Proportional tare only on a Random weight.
				{
					MetSig::applyProportionalTare(netWeight, totalTare, proportionalTare, weigherStatus);
				}
			}


			if (isRandomWeight())
			{
				HobValue roundedTotalPrice = MetSig::calculateRandomWeightOldTotalPrice(hasKbdNetWeightLabel(), currentProductTransaction_->getOldUnitPrice(), priceModifier, netWeight);
				currentProductTransaction_->setOldFinalPrice(roundedTotalPrice);

				if (traceOn)
					Debug::trace("Product.oldTotals", "Old final price is " + util_string::longtostring(roundedTotalPrice));
			}
		}
		else
		{
			if (traceOn)
				Debug::trace("Product.oldTotals", "Did not get a net weight!");

			if (isRandomWeight())
				currentProductTransaction_->setOldFinalPrice(0);
		}
	}
}


/* This method creates a new Product Transaction record.  This record is
* intended to be used for printing a label.  So, update all the post-image
* fields (in the Product Transaction record) with the data from the Product
* record.
*/
ProductTransaction *Product::createProductTransaction()
{
	//Debug::trace("Product.currentProductTransaction_", "In Product createProductTransaction() create a new currentProductTransaction_" );

	if (!currentProductTransaction_)
		currentProductTransaction_ = new ProductTransaction(this);
	else
	{
		currentProductTransaction_ = new ProductTransaction(*currentProductTransaction_);
		currentProductTransaction_->setTransactionState(ProductTransaction::TRANSACTIONSTATE_INITIAL);
	}

	if (!currentProductTransaction_)
	{
		Debug::trace("Product", "FATAL ERROR - Could not create a new ProductTransaction Record.");
	}
	else
	{
		HobString tnum(Long::toString(currentProductTransaction_->getTransactionNum()));
		//Debug::trace("Product", HobString("Created createProductTransaction() new ProductTransaction for product: ") + HobString(Long::toString(productNum)) + HobString("  transnum:") + tnum);
	}

	return currentProductTransaction_;
}

const ProductTransaction * Product::getCurrentTransaction() const
{
	return currentProductTransaction_;
}

ProductTransaction *Product::getCurrentTransaction()
{
	return currentProductTransaction_;
}

void Product::setCurrentTransaction(ProductTransaction *trans)
{
	// If the programmer is calling this method to set the transaction, and we currently have a transaction,
	// then we must do something with it.  So, we assume the current one is no longer needed, and we delete it.
	if (currentProductTransaction_)
	{
		if (currentProductTransaction_ != trans)
		{
			//Debug::trace("Product.currentProductTransaction_", "In Product setProductTransaction() make call to delete currentProductTransaction_: " + ptrtostring(currentProductTransaction_) );

			delete currentProductTransaction_;
			currentProductTransaction_ = trans;
		}
	}
	else
		currentProductTransaction_ = trans;
}

ProductTransaction *Product::printCurrentTransaction()
{
	if ( !currentProductTransaction_ )
		return NULL;

	currentProductTransaction_->setUnitPrice(getUnitPrice());
	currentProductTransaction_->setByCount(getByCount());
	currentProductTransaction_->setPriceModifier(getPriceModifier());
	currentProductTransaction_->setTare(getActiveTare().getValue());
	if ( !( isRandomWeight() || isByCountWithWeight() ||
			( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && isFixedWeight() && getFixedWeight() > 0 ) ) )
		currentProductTransaction_->setTotalNetWeight(MetSig::BRConvertFixedWeightToTotalWeight(getFixedWeight(), isFluidOunce()));
	currentProductTransaction_->setFixedWeight(getFixedWeight());

	currentProductTransaction_->setShelfLifeDays(getShelfLifeDays());
	currentProductTransaction_->setShelfLifeHours(getShelfLifeHours());
	currentProductTransaction_->setShelfLifeMinutes(getShelfLifeMinutes());
	currentProductTransaction_->setProductLifeDays(getProductLifeDays());
	currentProductTransaction_->setProductLifeMinutes(getProductLifeMinutes());

	currentProductTransaction_->setBackDateDays(getBackDateDays());

	currentProductTransaction_->setBarCodeNum(getBarCodeNum());
	currentProductTransaction_->buildBarcodeStrings();

	//	Get the Label type. Does it contain a UPC or EAN13 barcode? Set the Barcode fields up accordingly.
	//	If no primary label type, go to the second label type.
	//	If no second label type, go to the third label type.
	//	If no third label type, assume UPCA.
	short printerNum = FirstPrinter;
	LabelType *myLabelType = (LabelType *)NULL;
	BarcodeType barcodeType = MAX_BARCODE_TYPES;

	while ((barcodeType == MAX_BARCODE_TYPES) && (printerNum < MaxPrinterTypes))
	{
		myLabelType = getLabelType(printerNum);
		if (myLabelType)
		{
			if (myLabelType->containsBarcodeType(UPCA))
				barcodeType = UPCA;
			else if (myLabelType->containsBarcodeType(EAN13))
				barcodeType = EAN13;
			else if (myLabelType->containsBarcodeType(EAN128C))
				barcodeType = EAN128C;
		}
		printerNum++;
	}

	printerNum = FirstPrinter;
	while ((barcodeType == MAX_BARCODE_TYPES) && (printerNum < MaxPrinterTypes))
	{
		myLabelType = getLabelType(printerNum);
		if (myLabelType)
		{
			short tempType = myLabelType->getBarcodeType();
			if (tempType != RECORD_NOT_FOUND)
				barcodeType = (BarcodeType)tempType;
		}
		printerNum++;
	}

	// This gives you a 12 character completely matching up tag that matches the barcode on the label - need to make this configurble I think
	//if (barcodeType != MAX_BARCODE_TYPES && scaleConfig->isEnableUPTagPrefix)
	//	currentProductTransaction_->setBarcodeNumber((currentProductTransaction_->getBarCodeNum()).getBarcodeString(barcodeType));

	if (barcodeType != MAX_BARCODE_TYPES)
	{
		// need to add configuration here to allow for prefix support
		if (scaleConfig->isEnableUPTagPrefix())
		{
			currentProductTransaction_->setBarcodeNumber((currentProductTransaction_->getBarCodeNum()).getBarcodeString(barcodeType));
		}
		else
		{
			HobString barcodeToUse = (currentProductTransaction_->getBarCodeNum()).getBarcodeString(barcodeType);
			if (barcodeType == UPCA || barcodeType == EAN13)
				barcodeToUse = barcodeToUse.getFromEnd(11);
			currentProductTransaction_->setBarcodeNumber(barcodeToUse);
		}
	}

	// todo - set any other fields tracked in the transaction record.
	currentProductTransaction_->calculateFSDiscounts(this);

	// TODO - Larry - done - what about these for auto labels? How to number?
	/* This is OK to leave this code as it was before the 'label types by mode' project, since the Label Type objects will actually be the ones that will be printing, the ones that are active for the current mode. */
	if ( getLabelType(0) )
		currentProductTransaction_->setLabel1(getLabelType(0)->getLabelTypeNum());
	if ( getLabelType(1) )
		currentProductTransaction_->setLabel2(getLabelType(1)->getLabelTypeNum());
	if ( getLabelType(2) )
		currentProductTransaction_->setLabel3(getLabelType(2)->getLabelTypeNum());
	if ( getLabelType(3) )
		currentProductTransaction_->setLabel4(getLabelType(3)->getLabelTypeNum());

	currentProductTransaction_->setGraphicName1(getGraphicName(0));
	currentProductTransaction_->setGraphicName2(getGraphicName(1));
	currentProductTransaction_->setGraphicName3(getGraphicName(2));
	currentProductTransaction_->setGraphicName4(getGraphicName(3));

	if ( currentProductTransaction_->isCOOLTransaction() )
	{
		currentProductTransaction_->setCOOLText(getCOOLTextToPrint());
		currentProductTransaction_->setCOOLTrackingText(getCoolTrackingText());
	}
	
	//	Don't need this since cutTestNum is part of Transaction now.
	//currentProductTransaction_->setCuttingTestNumber(cutTestNum);

	//	Check to see if we need to apply a Security label
	if (scaleConfig && (isApplySecurityLabel() ||
			((scaleConfig->getSecurityLblUnitPrice() > 0) && (currentProductTransaction_->getUnitPrice() >= scaleConfig->getSecurityLblUnitPrice())) ||
			((scaleConfig->getSecurityLblTtlPrice() > 0) && (currentProductTransaction_->getTotalPrice() >= scaleConfig->getSecurityLblTtlPrice()))))
	{
		Debug::trace("Product.SecurityLabel", "printCurrentTransaction() - Print a SecurityLabel");
		currentProductTransaction_->setPrintSecurityLabel(true);
	}
	else
	{
		Debug::trace("Product.SecurityLabel", "printCurrentTransaction() - Don't print a SecurityLabel");
		currentProductTransaction_->setPrintSecurityLabel(false);
	}
	
	Debug::trace("ProductTransaction.Portions", "printCurrentTransaction(): Set Portions.");
	currentProductTransaction_->setPortions(getPortions());
	currentProductTransaction_->calculatePrintableData(this);	 //	Why don't they have this? What does it do?
	currentProductTransaction_->setTransactionState(ProductTransaction::TRANSACTIONSTATE_PRINTING);

	printingTransactions.push_back(currentProductTransaction_);

	ProductTransaction *transaction( currentProductTransaction_ );

	Debug::trace("Product.PCT", HobString("printCurrentTransaction - num printing transactions : ") + Long::toString(printingTransactions.size()));
	Debug::trace("Product.PCT", HobString("printCurrentTransaction - transaction being printed : ") + ptrtostring(transaction));

	createProductTransaction();

	Debug::trace("Product.PCT", HobString("printCurrentTransaction - after creating new transaction, new trans is : ") + ptrtostring(currentProductTransaction_));
	Debug::trace("Product.PCT", "printCurrentTransaction FINISHED");

	return transaction;
}


void Product::completeTransaction(ProductTransaction *productTransaction)
{
	Debug::trace("Product.CompleteTransaction", HobString("Completing print of a ProductTransaction for product: ") + HobString(Long::toString(productNum)));

	if (!productTransaction)
		return;

	ProductTransaction *completedTransaction = findTransaction(productTransaction->getTransactionNum());
	if (completedTransaction)
	{
		// Do anything that needs to be done when this transaction is finished (printed).
		Debug::trace("Product.CompleteTransaction", HobString("Completing print of found ProductTransaction for product: ") + HobString(Long::toString(productNum)));

		printingTransactions.remove(completedTransaction);

		if (completedTransaction == currentProductTransaction_)
			currentProductTransaction_ = 0;

		completedTransactions.push_back(completedTransaction); // FT-960
		Debug::trace("Product.CompleteTransaction", HobString("Completed print - Removed ProductTransaction for product: ") + HobString(Long::toString(productNum)));
	}
	else
	{
		// Do anything that needs to be done when this transaction is finished (printed).
		HobString transnum = HobString(Long::toString(productTransaction->getTransactionNum()));
		HobString prodnum = HobString(Long::toString(productNum));
		Debug::trace("Product.CompleteTransaction", HobString("COULD NOT FIND COMPLETED TRANSACTION - productnum:") + prodnum + HobString("  transnum:") + transnum);
	}
}

ProductTransaction *Product::findTransaction(HobRecordNum transID)
{
	Debug::trace("Product", HobString("Looking for a ProductTransaction - transnum: ") + HobString(Long::toString(transID)));

	ProductTransaction *retTransaction( NULL );

	for ( std::list<ProductTransaction*>::iterator pos( printingTransactions.begin() ); pos != printingTransactions.end(); pos++ )
		if (( (*pos) ) && ( transID == (*pos)->getTransactionNum() ))
		{
			retTransaction = *pos;
			Debug::trace("Product", HobString("Found the transaction - transnum: ") + HobString(Long::toString(retTransaction->getTransactionNum())));
			break;
		}

	return retTransaction;
}


bool Product::hasOutstandingTransactions() const
{
	// Check if the printingTransactions contains any transactions.
	// This list contains any transactions passed tot he Real-Time Controller for printing.
	// Until the RTC sends back a ProductComplete Message, the transaction is said to be
	// outstanding.
	return !printingTransactions.empty();
}

ProductTransaction *Product::getFirstPrintingTransaction()
{
	Debug::trace("Product", "Looking for first printing ProductTransaction");

	if ( !printingTransactions.empty() )
	{
		std::list<ProductTransaction*>::iterator pos( printingTransactions.begin() );

		if ( printingTransactions.end() != pos )
		{
			ProductTransaction *retTransaction( *pos );

			if ( retTransaction )
			{
				Debug::trace( "Product", HobString("Found first transaction - transnum: ") + Long::toString(retTransaction->getTransactionNum()) );
				return retTransaction;
			}
		}
	}

	return NULL;
}

int Product::numOutstandingTransactions() const
{
	return printingTransactions.size();
}

void Product::clearOutstandingTransactions()
{
	Debug::trace("Product", "clearing outstanding transactions...");
	while ( !printingTransactions.empty() )
		printingTransactions.pop_front();
}

void Product::removePreviousTransaction()
{
	if ( !printingTransactions.empty() )
	{
		Debug::trace("Product", "remove previous transaction...");
		printingTransactions.pop_back();

		Debug::trace("Product", HobString("transaction count: ") + Long::toString(printingTransactions.size()));
	}
}

/*****************************************************************************************
 *
 * updateField: Update an attribute for the field indicated by the fieldOffset passed in.
 *              The data for the field is passed in as a string.
 *
 ****************************************************************************************/
bool Product::updateField(int fieldOffset, const HobString &newValue, bool linksMustExist, int formatType)
{
	HobString msgDebug = "";
	bool traceOn = Debug::isTracing("Product.updateField");
	if (traceOn)
		Debug::trace("Product.updateField", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue + "  linksMustExist " + booltostring(linksMustExist) + "  formatType " + inttostring(formatType));

	if (fieldOffset < 0)
		return false;

	int intValue = 0, savedIntValue = 0;
//	bool boolValue = true, savedBoolValue = true;
	long longValue = 0L, savedLongValue = 0L;

	switch (fieldOffset)
	{
		case fieldOffsetProductDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetProductNum:
		{
			longValue = Long::valueOf(newValue);
			savedLongValue = getProductNum();
			setProductNum((HobRecordNum)longValue);
			if (getProductNum() != longValue)
			{
				productNum = savedLongValue;
				return false;
			}
			break;
		}
		case fieldOffsetProductType:
			setProductType((ProductType)Integer::valueOf(newValue));
			break;
		case fieldOffsetDescription:
		{
			HobString temp = newValue;
			temp.convertToEvolFontSequences();
			setDescription(temp);
			break;
		}

		case fieldOffsetBarcodeType:
			// This is being commented out.  It makes no sense to change the barcodeType
			// barCodeNum.setBarcodeType((BarcodeType)Integer::valueOf(newValue));
			break;
		case fieldOffsetBarcodePrefix:
			barCodeNum.setPrefix(newValue);
			break;
		case fieldOffsetBarcodeProductNum:
			barCodeNum.setProductNumber(newValue);
			break;
		case fieldOffsetBarcodeUpcNum:
		{
			bool tracing = Debug::isTracing("Product.BarCode");

			setRawBarCodeData(newValue);
			if (tracing)
				Debug::trace("Product.BarCode", string("updateField(fieldOffsetBarcodeUpcNum) : rawBarCodeData = ") + getRawBarCodeData());
			
			break;
		}
		case fieldOffsetBarcodeVendorNum:
			barCodeNum.setVendorNumber(newValue);
			break;
		case fieldOffsetBarcodeEANCode:
			barCodeNum.setEANCode(newValue);
			break;

		case fieldOffsetByCount:
		{
			intValue = Integer::valueOf(newValue);
			savedIntValue = getByCount();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("newByCount = ").append(Integer::toString(intValue));
				Debug::trace("Product.updateField", msgDebug2);
			}
			// Ultima and quantum send -1 to indicate forced by count.  Provide backwards compatibility by supporting this.
			if (intValue == -1)
			{
				bool savedByCountForced = isByCountForced();
				setReceived(fieldOffsetByCountForced, true);
				setByCountForced(true);
				setByCount(1);
				if ( !(getByCount() == 1 && isByCountForced()) )
				{
					byCount = savedIntValue;
					byCountForced = savedByCountForced;
					return false;
				}
			}
			else
			{
				setByCount(intValue);
				if (getByCount() != intValue)
				{
					byCount = savedIntValue;
					return false;
				}
			}
			break;
		}
		case fieldOffsetByCountForced:
		{
			bool forced = Bool::valueOf(newValue);
			setByCountForced(forced);
			if (forced)
				setReceived(fieldOffsetByCount, true);
			break;
		}

		case fieldOffsetExceptionPrice:
		{
			setExceptionPrice( MetSig::determineImplicitDecimalPrice(newValue) );
			break;
		}

		case fieldOffsetExceptionPriceTare:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("fieldOffsetExceptionPriceTare: newValue: ").append(newValue);
				Debug::trace("Product.updateField", msgDebug2);
			}
			exceptionPriceTare = newValue;
			break;

		case fieldOffsetUnitPrice:
		{
			savedLongValue = getUnitPrice();
			bool savedUnitPriceForced = isUnitPriceForced();

			HobPrice resultPrice = MetSig::determineImplicitDecimalPrice(newValue);

			// Ultima and quantum send -1 to indicate forced price.  Provide backwards compatibility by supporting this.
			if (resultPrice == (HobPrice)-1)
			{
				setReceived(fieldOffsetUnitPriceForced, true);
				setUnitPriceForced(true);
				setUnitPrice(0);
				if ( !(getUnitPrice() == 0 && isUnitPriceForced()) )
				{
					unitPrice = (HobPrice)savedLongValue;
					unitPriceForced = savedUnitPriceForced;
					return false;
				}
			}
			else
			{
				setUnitPrice(resultPrice);
				if (getUnitPrice() != resultPrice)
				{
					unitPrice = (HobPrice)savedLongValue;
					return false;
				}
			}
			break;
		}

		case fieldOffsetUnitPriceForced:
		{
			bool forced = Bool::valueOf(newValue);
			setUnitPriceForced(forced);
			if (forced)
				setReceived(fieldOffsetUnitPrice, true);
			break;
		}

		case fieldOffsetDiscountType:
			setDiscountType(Integer::valueOf(newValue));

			Debug::trace("Product.FreqShopper", HobString("updateField(): discountType = ") + Integer::toString(discountType));
			break;

		//METSIG??  NO
		case fieldOffsetDiscountValue1:
		case fieldOffsetDiscountValue2:
		case fieldOffsetDiscountValue3:
		{
			bool traceOn2 = Debug::isTracing("Product.FreqShopper");
			HobString msgDebug2 = "";
			if (traceOn2)
				HobString msgDebug2 = "updateField():  ";

			short	thisDiscountType = getDiscountType();			//	If FS is perProduct, the discount type is in the PLU.
			double	thisDiscountValue = Double::valueOf(newValue);

			if (traceOn2)
			{
				HobString msgDebug3 = msgDebug2;
				msgDebug3.append("fieldOffsetDiscountValue").append(Integer::toString(fieldOffset - fieldOffsetDiscountValue1 + 1));
				msgDebug3.append(":  thisDiscountValue = ").append(util_string::doubletostring(thisDiscountValue));
				Debug::trace("Product.FreqShopper", msgDebug3);
			}
			if (newValue.find(".") != std::string::npos)
			{
				if ((thisDiscountType == Percent_off_net_value) || (thisDiscountType == Percent_added_net_value))
					thisDiscountValue *= scaleConfig->getPercentageDivisor();
				else
					thisDiscountValue *= scaleConfig->getPriceDivisor();
			}
			double newRoundDiscount = Rounding::hobround(thisDiscountValue);
			if (traceOn2)
			{
				HobString msgDebug3 = msgDebug2;
				msgDebug3.append("Now thisDiscountValue = ").append(util_string::doubletostring(newRoundDiscount));
				Debug::trace("Product.FreqShopper", msgDebug3);
			}

			switch(fieldOffset)
			{
				case fieldOffsetDiscountValue1:
				{
					setDiscountValue1((HobPrice)newRoundDiscount);
					if (traceOn2)
					{
						HobString msgDebug3 = msgDebug2;
						msgDebug3.append("discountValue1 = ").append(util_string::doubletostring(discountValue1));
						Debug::trace("Product.FreqShopper", msgDebug3);
					}
					break;
				}
				case fieldOffsetDiscountValue2:
				{
					setDiscountValue2((HobPrice)newRoundDiscount);
					if (traceOn2)
					{
						HobString msgDebug3 = msgDebug2;
						msgDebug3.append("discountValue2 = ").append(util_string::doubletostring(discountValue2));
						Debug::trace("Product.FreqShopper", msgDebug3);
					}
					break;
				}
				case fieldOffsetDiscountValue3:
				{
					setDiscountValue3((HobPrice)newRoundDiscount);
					if (traceOn2)
					{
						HobString msgDebug3 = msgDebug2;
						msgDebug3.append("discountValue3 = ").append(util_string::doubletostring(discountValue3));
						Debug::trace("Product.FreqShopper", msgDebug3);
					}
					break;
				}
				default:
					break;
			}
			break;
		}

		case fieldOffsetDiscountLabel1:
		case fieldOffsetDiscountLabel2:
		case fieldOffsetDiscountLabel3:
			// Do nothing, these aren't kept "by plu"
			break;

		case fieldOffsetKeyboardNetWeight:
		{
			if (traceOn)
				Debug::trace("Product.updateField", string("fieldOffsetKeyboardNetWeight: current keyboardNetWeight is ") + longtostring(keyboardNetWeight));

			if (newValue == "-1")
			{
				cancelledKbdNetWeight = true;
				setKeyboardNetWeight(0);
			}
			else
			{
				cancelledKbdNetWeight = false;
				setKeyboardNetWeight(MetSig::determineImplicitDecimalKeyboardNetWeight(newValue));
			}

			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("fieldOffsetKeyboardNetWeight: NEW keyboardNetWeight is ").append(Long::toString(keyboardNetWeight));
				Debug::trace("Product.updateField", msgDebug2);
			}
			break;
		}
		
		case fieldOffsetTareAValue:
		{
			Tare oldTare( tareA );

			if (traceOn)
			{
				HobString msgDebug2( msgDebug );
				msgDebug2.append("fieldOffsetTareAValue: current tare is ").append(Long::toString((long)tareA.getValue()));
				Debug::trace("Product.updateField", msgDebug2);
			}

			// compute new tare
			HobWeight newTare = MetSig::determineImplicitDecimalWeight(newValue);

			// set tare!
			setTareA( newTare );

			if (traceOn)
			{
				HobString msgDebug2( msgDebug );
				msgDebug2.append("fieldOffsetTareAValue: new tare is ").append(Long::toString(newTare));
				Debug::trace("Product.updateField", msgDebug2);
			}

			// validate tare
			if ( newTare != tareA.getValue() )
			{
				setTareA(oldTare.getValue());
				return false;
			}

			break;
		}

		case fieldOffsetTareBValue:
		{
			Tare oldTare( tareB );

			if (traceOn)
			{
				HobString msgDebug2( msgDebug );
				msgDebug2.append("fieldOffsetTareBValue: current tare is ").append(Long::toString((long)tareB.getValue()));
				Debug::trace("Product.updateField", msgDebug2);
			}

			// compute new tare
			HobWeight newTare = MetSig::determineImplicitDecimalWeight(newValue);

			// set tare!
			setTareB( newTare );

			if (traceOn)
			{
				HobString msgDebug2( msgDebug );
				msgDebug2.append("fieldOffsetTareBValue: new tare is ").append(Long::toString(newTare));
				Debug::trace("Product.updateField", msgDebug2);
			}

			// validate tare
			if ( newTare != tareB.getValue() )
			{
				setTareB(oldTare.getValue());
				return false;
			}

			break;
		}

		case fieldOffsetTareAForced:	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
		{
			bool forced = Bool::valueOf(newValue);
			tareA.setForced(forced);
			if (forced)
				setReceived(fieldOffsetTareAValue, true);
			break;
		}

		case fieldOffsetProportionalTare:
		{
			setProportionalTare(newValue);
			break;
		}

		case fieldOffsetFixedWeight:
		{
			savedLongValue = getFixedWeight();
			double newWeight = Double::valueOf(newValue);
			if (newValue.find(".") != std::string::npos)
			{
				double weightDivisor = ( scaleConfig->isFixedWeightInTenths() ) ? 10 : 1;
				newWeight = (newWeight * weightDivisor);
			}
			double newRoundWeight = Rounding::hobround(newWeight);
			setFixedWeight((HobWeight)newRoundWeight);
			if (getFixedWeight() == (HobWeight)newRoundWeight)
			{
				originalFixedWeight = (HobWeight)newRoundWeight;
			}
			else
			{
				fixedWeight = (HobWeight)savedLongValue;
				return false;
			}
			break;
		}

		case fieldOffsetShelfLifeDays:
		{
			int days = Integer::valueOf(newValue);
			if (days == 999)
			{
				if (!isReceived(fieldOffsetDontPrintSellBy))
				{
					setDontPrintSellBy(true);
					setReceived(fieldOffsetDontPrintSellBy, true);
				}
				if (!isReceived(fieldOffsetDontPrintPackedOn))
				{
					setReceived(fieldOffsetDontPrintPackedOn, true);
					setDontPrintPackedOn(false);
				}
				setShelfLifeDays(0);
			}
			if (days == 998)
			{
				if (!isReceived(fieldOffsetDontPrintSellBy))
				{
					setReceived(fieldOffsetDontPrintSellBy, true);
					setDontPrintSellBy(true);
				}
				if (!isReceived(fieldOffsetDontPrintPackedOn))
				{
					setReceived(fieldOffsetDontPrintPackedOn, true);
					setDontPrintPackedOn(true);
				}
				setShelfLifeDays(0);
			}
			else if (days >=0 && days < 998)
			{
				if (!isReceived(fieldOffsetDontPrintSellBy))
				{
					setReceived(fieldOffsetDontPrintSellBy, true);
					setDontPrintSellBy(false);
				}
				if (!isReceived(fieldOffsetDontPrintPackedOn))
				{
					setReceived(fieldOffsetDontPrintPackedOn, true);
					setDontPrintPackedOn(false);
				}
				setShelfLifeDays(days);
			}
			break;
		}
		case fieldOffsetShelfLifeHours:
			setShelfLifeHours(Integer::valueOf(newValue));
			break;
		case fieldOffsetShelfLifeMinutes:
			setShelfLifeMinutes(Integer::valueOf(newValue));
			break;

		case fieldOffsetProductLifeDays:
		{
			int days = Integer::valueOf(newValue);
			if (days == 999)
			{
				if (!isReceived(fieldOffsetDontPrintBestBefore))
				{
					setReceived(fieldOffsetDontPrintBestBefore, true);
					setDontPrintBestBefore(true);
				}
				setProductLifeDays(0);
			}
			else if (days >=0 && days < 999)
			{
				if (!isReceived(fieldOffsetDontPrintBestBefore))
				{
					setReceived(fieldOffsetDontPrintBestBefore, true);
					setDontPrintBestBefore(false);
				}
				setProductLifeDays(days);
			}
			break;
		}
		case fieldOffsetProductLifeHours:
			setProductLifeHours(Integer::valueOf(newValue));
			break;
		case fieldOffsetProductLifeMinutes:
			setProductLifeMinutes(Integer::valueOf(newValue));
			break;
		case fieldOffsetSellByDate:
			setShelfLife(newValue);
			break;
		case fieldOffsetSellByTime:
			break;
		case fieldOffsetBestBeforeDate:
			setProductLife(newValue);
			break;
		case fieldOffsetBestBeforeTime:
			break;

		case fieldOffsetPriceModifier:
			// For Fastrack, the UI is using fieldOffsetPoundsForAmount and fieldOffsetFractionalPriceUnit, but not the fieldOffsetPriceModifier anymore.
			// So if we get it back from the UI, just ignore it.
			if (formatType != BrowserFormat)
				setPriceModifier(newValue); 
			break;

		case fieldOffsetPoundsForAmount:
			if (scaleConfig->isPoundsForPricingEnabled())
				setPriceModifier(newValue); 
			break;

		case fieldOffsetLabelRotation1:		// PRIMARY LABEL ROTATION
			setLabelRotation(FirstPrinter, Integer::valueOf(newValue));
			break;

		case fieldOffsetLabelRotation2:		// SECONDARY LABEL ROTATION
			setLabelRotation(SecondPrinter, Integer::valueOf(newValue));
			break;

		case fieldOffsetLabelRotation3:		// THIRD LABEL ROTATION
			setLabelRotation(ThirdPrinter, Integer::valueOf(newValue));
			break;

		case fieldOffsetLabelPlacement1:		// PRIMARY LABEL PLACEMENT
			setLabelPlacement(FirstPrinter, Integer::valueOf(newValue));
			break;

		case fieldOffsetLabelPlacement2:		// SECONDARY LABEL PLACEMENT
			setLabelPlacement(SecondPrinter, Integer::valueOf(newValue));
			break;

		case fieldOffsetLabelPlacement3:		// THIRD LABEL PLACEMENT
			setLabelPlacement(ThirdPrinter, Integer::valueOf(newValue));
			break;

		case fieldOffsetPoints:
			setPoints(Integer::valueOf(newValue));
			break;

		case fieldOffsetIntakeMode:
			setIntakeMode(static_cast<WrapperIntakeMode::IntakeMode> (Integer::valueOf(newValue)));
			break;

		case fieldOffsetGraphicNum:	// Graphic Num - g#
		{
			// If we received G1, then we received the graphic name in the new Evolution format, so ignore the old-format tag, g#
			if (!isReceived(fieldOffsetGraphicName1))
			{
				setReceived(fieldOffsetGraphicName1, true);

				if (newValue.size() == 0 || Integer::valueOf(newValue) == 0)
				{
					setGraphic(0, (Graphic *) 0);
				}
				else
				{
					Graphic *newGraphic = Graphic::findGraphic(newValue);
					Debug::trace("Product.Graphic", HobString("updateField(): Product = ") + HobString(Long::toString(productNum)) +
																HobString("    Index = 0    graphic name = ") + HobString(newValue) +
																HobString("    Graphic = ") + ptrtostring(newGraphic));
					setGraphic(0, newGraphic);
				}
			}
			break;
		}

		case fieldOffsetGraphicName1:	// Graphic 1 NAME
		case fieldOffsetGraphicName2:	// Graphic 2 NAME
		case fieldOffsetGraphicName3:	// Graphic 3 NAME
		case fieldOffsetGraphicName4:	// Graphic 4 NAME
		{
			unsigned int graphicIndex = (fieldOffset - fieldOffsetGraphicName1);
			Graphic *newGraphic = (Graphic *) 0;

			// WEH-45
			bool isGCDProductAdvert( false );

			if (( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgGCDUseTransactionalLayout) ) &&
				( Configuration::getInstance()->toIntCfg(fieldOffsetCfgGCDProductAdvertType) == (graphicIndex+1) ))
			{
				isGCDProductAdvert = true;
			}

			if (newValue.empty() || newValue == scaleConfig->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME))
			{
				setGraphic(graphicIndex, (Graphic *) 0);
			}
			else
			{
				newGraphic = Graphic::findGraphic(newValue);
				if (linksMustExist)
				{
					if ( newGraphic )
					{
						if (( newGraphic->getErrorCode() == RECORD_OK ) || ( isGCDProductAdvert ))
							setGraphic(graphicIndex, newGraphic);
						else
						{
							delete newGraphic;
							newGraphic = 0;
						}
					}
				}
				else
				{
					if (!newGraphic)
						newGraphic = new Graphic(newValue);
					setGraphic(graphicIndex, newGraphic);
				}
			}

			//	Set up the new Graphic

			if (Debug::isTracing("Product.Graphic"))
			{
				HobString pThis = Hex::toString((long long) this);
				Debug::trace("Product.Graphic", HobString("updateField(): Product = ") + pThis +
						HobString("    Index = ") + HobString(Integer::toString(graphicIndex)) +
						HobString("    name = ") + HobString(newValue) +
						HobString("    Graphic = ") + ptrtostring(newGraphic));
			}
			break;
		}

		case fieldOffsetFractionalPriceUnit:
			if (scaleConfig->isFractionalPricingEnabled())
				setPriceModifier(newValue);
			break;

		case fieldOffsetTareType:
			break;

		case fieldOffsetApplySecurity:
			setApplySecurityLabel(Bool::valueOf(newValue));
			break;
		case fieldOffsetDontPrintPackedOn:
			setDontPrintPackedOn(Bool::valueOf(newValue));
			break;
		case fieldOffsetDontPrintSellBy:
			setDontPrintSellBy(Bool::valueOf(newValue));
			break;
		case fieldOffsetDontPrintBestBefore:
			setDontPrintBestBefore(Bool::valueOf(newValue));
			break;

		case fieldOffsetProductGraphicName:
			if (newValue == scaleConfig->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME))
			{
				HobString temp("");
				setProductGraphicName(temp);
			}
			else
				setProductGraphicName(newValue);
			break;

		case fieldOffsetProductName1:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("fieldOffsetProductName1, newValue = ").append(newValue);
				msgDebug2.append("  length = ").append(Integer::toString((int)newValue.length()));
				Debug::trace("Product.updateField", msgDebug2);
			}
			setProductName1(newValue);
			break;

		case fieldOffsetProductName2:
			Debug::trace("Product", HobString("updateField(fieldOffsetProductName2): newValue = ") + newValue);
			setProductName2(newValue);
			break;

		case fieldOffsetProductReportingClassNum:
		{
			HobRecordNum newReportingClassNum = (HobRecordNum)Long::valueOf(newValue);

			if (newReportingClassNum <= 0)
				newReportingClassNum = Class::GRAND_TOTALS_CLASS;

			Class *newReportingClass = Class::findRecord(newReportingClassNum, deptNum, dbAccess);
			if (linksMustExist)
			{
				if (newReportingClass)
				{
					newReportingClass->checkClassTree(dbAccess);
					if (newReportingClass->getErrorCode() != RECORD_OK)
						delete newReportingClass;
					else
					{
						setReportingClass(newReportingClass);
						Debug::trace("Product.Class", string("updateField() found existing Class - num:") + newValue + "  deptNum:" + longtostring(deptNum));
					}
				}
			}
			else
			{
				if (!newReportingClass)
				{
					newReportingClass = new Class();
					newReportingClass->setDeptNum(deptNum);
					newReportingClass->setClassNum(newReportingClassNum);
					newReportingClass->setErrorCode(RECORD_NOT_FOUND);

					Debug::trace("Product.Class", string("updateField() created new Class - num:") + newValue + "  deptNum:" + longtostring(deptNum));
				}
				setReportingClass(newReportingClass);
			}
			break;
		}

		// TODO - Larry - done
		// With the 'label types by mode' project, we should no longer be receiving these first three offsets from the UI.
		// We can still receive them via comm / backup / restore, in which case, we still need to deal with them.
		// We still handle them appropriately; the call to setLabelTypeNum(...) maps them to fieldOffsetDefaultLabelTypeNum1/2/3.
		case fieldOffsetLabelTypeNum1:		// PRIMARY LABEL TYPE NUMBER
		case fieldOffsetLabelTypeNum2:		// SECONDARY LABEL TYPE NUMBER
		case fieldOffsetLabelTypeNum3:		// THIRD LABEL TYPE	NUMBER

		case fieldOffsetDefaultLabelTypeNum1:
		case fieldOffsetDefaultLabelTypeNum2:
		case fieldOffsetDefaultLabelTypeNum3:
		case fieldOffsetManualLabelTypeNum1:
		case fieldOffsetManualLabelTypeNum2:
		case fieldOffsetManualLabelTypeNum3:
		case fieldOffsetPrepackLabelTypeNum1:
		case fieldOffsetPrepackLabelTypeNum2:
		case fieldOffsetPrepackLabelTypeNum3:
		case fieldOffsetProdEntryLabelTypeNum1:
		case fieldOffsetProdEntryLabelTypeNum2:
		case fieldOffsetProdEntryLabelTypeNum3:
		case fieldOffsetSelfServLabelTypeNum1:
		case fieldOffsetSelfServLabelTypeNum2:
		case fieldOffsetSelfServLabelTypeNum3:
		case fieldOffsetPick5LabelTypeNum1:
		case fieldOffsetPick5LabelTypeNum2:
		case fieldOffsetPick5LabelTypeNum3:
		case fieldOffsetOnlineOrderingLabelTypeNum1:
		case fieldOffsetOnlineOrderingLabelTypeNum2:
		case fieldOffsetOnlineOrderingLabelTypeNum3:
		{
			if (Debug::isTracing("Product.LabelType"))
				Debug::trace("Product.LabelType", string("updateField() - fieldOffset: ") + inttostring(fieldOffset) + " is being updated with value: " + newValue);
			bool labelTypeNumValid = true;
			HobRecordNum newLabelTypeNum = Long::valueOf(newValue);
			if (newLabelTypeNum <= 0)
			{
				setLabelTypeNum(fieldOffset, 0);
				Debug::trace("Product.LabelType", "updateField() - setting LabelTypeNum to 0 - fieldOffset: " + inttostring(fieldOffset));
			}
			else
			{
				if (linksMustExist)
				{
					LabelType *newLabelType = LabelType::findRecord(newLabelTypeNum, deptNum, dbAccess);
					if (newLabelType)
					{
						if (newLabelType->getErrorCode() != RECORD_OK)
							labelTypeNumValid = false;
						delete newLabelType;
						newLabelType = 0;
					}
					else
						labelTypeNumValid = false;
				}

				if (labelTypeNumValid)
				{
					setLabelTypeNum(fieldOffset, newLabelTypeNum);
					Debug::trace("Product.LabelType", "updateField() - set LabelTypeNum to " + longtostring(newLabelTypeNum) + " - label type fieldOffset: " + inttostring(fieldOffset));
				}
				else
					Debug::trace("Product.LabelType", "updateField() - labelTypeNum not valid " + longtostring(newLabelTypeNum) + " - label type fieldOffset: " + inttostring(fieldOffset));
			}
			return labelTypeNumValid;
			break;
		}

		case fieldOffsetProdNutrifactNum:
		{
			HobRecordNum newNFNum = (HobRecordNum)Long::valueOf(newValue);

			if (newNFNum <= 0)
			{
				setNutrifacts(0);
				Debug::trace("Product.Nutrifacts", "updateField() set Nutrifacts to NULL");
			}
			else
			{
				Nutrifacts *newNutriFact = Nutrifacts::findRecord(newNFNum, deptNum, dbAccess);
				if (linksMustExist)
				{
					if (newNutriFact)
					{
						if (newNutriFact->getErrorCode() != RECORD_OK)
							delete newNutriFact;
						else
						{
							setNutrifacts(newNutriFact);
							Debug::trace("Product.Nutrifacts", string("updateField() found existing Nutrifact - num:") + newValue + "  deptNum:" + longtostring(deptNum));
						}
					}
				}
				else
				{
					if (!newNutriFact)
					{
						newNutriFact = new Nutrifacts();
						newNutriFact->setDeptNum(deptNum);
						newNutriFact->setTextNum(newNFNum);
						newNutriFact->setErrorCode(RECORD_NOT_FOUND);

						Debug::trace("Product.Nutrifacts", string("updateField() created new Nutrifact - num:") + newValue + "  deptNum:" + longtostring(deptNum));
					}
					setNutrifacts(newNutriFact);
				}
			}

			break;
		}
		case fieldOffsetProdCategoryNum:
		{
			HobRecordNum newCategoryNum = (HobRecordNum)Long::valueOf(newValue);

			if (newCategoryNum <= 0)
			{
				setCategory(0);
				Debug::trace("Product.Category", "updateField() set Category to NULL");
			}
			else
			{
				Category *newCategory = Category::findRecord(newCategoryNum, deptNum, dbAccess);
				if (linksMustExist)
				{
					if (newCategory)
					{
						if (newCategory->getErrorCode() != RECORD_OK)
							delete newCategory;
						else
						{
							setCategory(newCategory);
							Debug::trace("Product.Category", string("updateField() found existing Category - num:") + newValue + "  deptNum:" + longtostring(deptNum));
						}
					}
				}
				else
				{
					if (!newCategory)
					{
						newCategory = new Category();
						newCategory->setDeptNum(deptNum);
						newCategory->setCategoryNum(newCategoryNum);
						Debug::trace("Product.Category", string("updateField() created new Category - num:") + newValue + "  deptNum:" + longtostring(deptNum));
					}
					setCategory(newCategory);
				}
			}
			break;
		}

		case fieldOffsetProdSpecialNum:
		{
			HobRecordNum newSpecialNum = (HobRecordNum)Long::valueOf(newValue);

			if (newSpecialNum <= 0)
			{
				setSpecialProgram(0);
				Debug::trace("Product.Special", "updateField() set Special to NULL");
			}
			else
			{
				Special *newSpecial = Special::findRecord(newSpecialNum, deptNum, dbAccess);
				if (linksMustExist)
				{
					if (newSpecial)
					{
						if (newSpecial->getErrorCode() != RECORD_OK)
							delete newSpecial;
						else
						{
							setSpecialProgram(newSpecial);
							Debug::trace("Product.Special", string("updateField() found existing Special - num:") + newValue + "  deptNum:" + longtostring(deptNum));
						}
					}
				}
				else
				{
					if (!newSpecial)
					{
						newSpecial = new Special();
						newSpecial->setDeptNum(deptNum);
						newSpecial->setSpecialNum(newSpecialNum);
						Debug::trace("Product.Special", string("updateField() created new Special - num:") + newValue + "  deptNum:" + longtostring(deptNum));
					}
					setSpecialProgram(newSpecial);
				}
			}
			break;
		}
		case fieldOffsetSpecialStartDate:
		{
			HobDateTime hdt;
			hdt.convertFromDBFormat(newValue);
			setSpecialStartDate(hdt);
			break;
		}
		case fieldOffsetExpandedTextNum:
			updateFieldProductText(psttExpandedText, newValue, linksMustExist);
			break;
		case fieldOffsetSpecialMessageNum:
			updateFieldProductText(psttSpecialMessage, newValue, linksMustExist);
			break;
		case fieldOffsetProdProductNoteNum:
			updateFieldProductText(psttProductNote, newValue, linksMustExist);
			break;
		case fieldOffsetMarqueeTextNum:
			updateFieldProductText(psttMarqueeText, newValue, linksMustExist);
			break;
		case fieldOffsetProductText5Num:
			updateFieldProductText(psttProductText5, newValue, linksMustExist);
			break;
		case fieldOffsetProductText6Num:
			updateFieldProductText(psttProductText6, newValue, linksMustExist);
			break;
		case fieldOffsetProductText7Num:
			updateFieldProductText(psttProductText7, newValue, linksMustExist);
			break;
		case fieldOffsetProductText8Num:
			updateFieldProductText(psttProductText8, newValue, linksMustExist);
			break;
		case fieldOffsetProductText9Num:
			updateFieldProductText(psttProductText9, newValue, linksMustExist);
			break;

		case fieldOffsetFractionalPrice:
			break;
		case fieldOffsetForcedFractionalPrice:
			break;
		case fieldOffsetPoundsForPrice:
			break;
		case fieldOffsetForcedPoundsFor:
			break;

		case fieldOffsetPluFlags:
		case fieldOffsetTotalsFlags:
		{
			flags = (UINT16)Integer::valueOf(newValue);

			/* When determining product type from the plu flags field, ignore the fieldOffsetPluFlags field
			 * if we received the fieldOffsetProductType field.
			 */
			if (!isReceived(fieldOffsetProductType))
			{
				/* Assume this is a random weight product until we determine otherwise.
				 * Note, receiving the by count field might update the product type,
				 * if we haven't received fieldOffsetProductType and if the product's type is
				 * random weight, and if the by count is > 0.
				 */
				if (flags & PRFixedWeight)
				{
					productType = PRODUCTTYPE_FIXEDWEIGHT;
					setReceived(fieldOffsetProductType, true);
				}
				else if (flags & PRFluidOunce)
				{
					productType = PRODUCTTYPE_FLUIDOUNCE;
					setReceived(fieldOffsetProductType, true);
				}
			}

			if (!isReceived(fieldOffsetApplySecurity))
			{
				setReceived(fieldOffsetApplySecurity, true);
				applySecurityLabel = (flags & PRPrintSecurityLabel) ? true : false;
			}
			break;
		}

		case fieldOffsetProdCoolCategoryNum:
			setCoolCategoryNum((HobRecordNum)Long::valueOf(newValue));
			break;
		case fieldOffsetProdCoolPretextNum:
			setCoolPretextNum((HobRecordNum)Long::valueOf(newValue));
			break;
		case fieldOffsetProdCoolDefaultTextNum:
			setCoolDefaultTextNum((HobRecordNum)Long::valueOf(newValue));
			break;
		case fieldOffsetProdCoolTextNum:
		{
			Debug::trace("Product.COOL", "updateField(fieldOffsetProdCoolTextNum)");

			// Instead of directly putting the data into the database, we should first check
			// whether there is any data and, if not, then we should be setting the field to
			// RECORD_NOT_FOUND (i.e., wiping out the existing value). Then, we should also be
			// checking for out-of-range values, and if that is the case then we should ignore
			// the field. (SJJC: 2/23/09)
			if ( newValue.size() )
			{
				HobRecordNum number = (HobRecordNum)Long::valueOf(newValue);
				if ( 0 < number && number < 999999 )
					setCoolTextNum((HobRecordNum)Long::valueOf(newValue));
				else
					setReceived(fieldOffsetProdCoolTextNum, false);
			}
			else
				setCoolTextNum((HobRecordNum)RECORD_NOT_FOUND);
			break;
		}
		case fieldOffsetProdCoolShortlistNum:
			if ( newValue.size() )
			{
				HobRecordNum number = (HobRecordNum)Long::valueOf(newValue);
				// If the value is not within range then ignore it. PLUM may send a 0 here when
				// the customer does not manage this field centrally (SJJC: 2/21/09)
				if ( 0 < number && number < 999999 )
					setCoolShortlistNum(number);
				else
					setReceived(fieldOffsetProdCoolShortlistNum, false);
			}
			else
				setCoolShortlistNum((HobRecordNum)RECORD_NOT_FOUND);
			break;
		case fieldOffsetProdCoolShortlistFirstLink:
			if ( newValue.size() )
			{
				HobRecordNum number = (HobRecordNum)Long::valueOf(newValue);
				// If the value is not within range then ignore it. PLUM may send a 0 here when
				// the customer does not manage this field centrally (SJJC: 2/21/09)
				if ( 0 < number && number < 9999999 )
					setCoolShortlistFirstLink(number);
				else
					setReceived(fieldOffsetProdCoolShortlistFirstLink, false);
			}
			else
				setCoolShortlistFirstLink((HobRecordNum)RECORD_NOT_FOUND);
			break;
		case fieldOffsetProdCOOLTrackingText:
			Debug::trace("Product.COOL", "updateField(fieldOffsetProdCOOLTrackingText)");
			setCoolTrackingText(newValue);
			// The following is here for Safeway because this is also how Ultima and Quantum scales
			// work. If one such scale receives tracking text tag ("cc") and no data, they actually
			// ignore the field instead of setting it to "no data". This was done because PLUM was
			// sending the tag without data (Safeway doesn't manage this centrally; instead, at each
			// store, operators enter the necessary value in the corresponding operator mode screen)
			// and they do not want us to wipe out the values stored at the scales. The best solution
			// to this problem should have been that PLUM should not send the tag for a field that is
			// not managed centrally. (SJJC: 2/20/09)
			if ( newValue.empty() && formatType != BrowserFormat)
			{
				setReceived(fieldOffsetProdCOOLTrackingText, false);
				Debug::trace("Product.COOL", "updateField(fieldOffsetProdCOOLTrackingText):   setReceived(fieldOffsetProdCOOLTrackingText, false)");
			}
			break;
		case fieldOffsetProdCoolVerifyForced:
			setCoolVerifyForced(Bool::valueOf(newValue));
			break;
		case fieldOffsetProdCoolRequired:
		{
			Debug::trace("Product", "updateField(fieldOffsetProdCoolRequired)");
			
			//i.e. we are modifying a product in supervisor, so force cool required if tracking number
			Session *priSession = SessionManager::getInstance()->getPrimaryConsoleSession();

			if ( (coolTrackingText.size() > 0) && priSession && priSession->isInSupervisor()) 
				setCoolRequired(true); 
			else
				setCoolRequired(Bool::valueOf(newValue));
			break;
		}
		
/*
		case fieldOffsetProductErrorCode:
		//	This is a test internal to the Product object and is only sent
		//	to the UI to report on the status of the Product.
		//	We will not set this coming from the UI.
			break;
*/

		case fieldOffsetBackDateDays:
			setBackDateDays(Integer::valueOf(newValue));
			break;

		case fieldOffsetBackDateDate:
			if (traceOn)
				Debug::trace("Product.updateField", "updateField(fieldOffsetBackDateDate) = " + newValue);
			setBackdateDateTo(newValue);
			break;

		case fieldOffsetNumOfLabelsForced:
			// We only send this to the browser; we don't care if the browser sends it back.
			break;

		case fieldOffsetNumOfLabels:
			if (currentProductTransaction_)
				currentProductTransaction_->setNumOfLabelsToPrint(Integer::valueOf(newValue));
			numOfLabelsEntered = true;
			break;

		case fieldOffsetProductionPlanningType:
			setProductionPlanningType((ProductionPlanningType)Integer::valueOf(newValue));
			break;

		case fieldOffsetExpirationDaysForced:
			// Don't need to set this coming in.
			break;

		case fieldOffsetExpirationDays:
			setExpirationDays(Integer::valueOf(newValue));
			break;

		case fieldOffsetPortions:
			if (traceOn)
				Debug::trace("Product.updateField", "updateField(fieldOffsetPortions) = " + newValue);
			Debug::trace("Product.Portions", "updateField(fieldOffsetPortions)");
			setPortions(Integer::valueOf(newValue));
			break;

		case fieldOffsetBuildAMealSideItemsGroupNum:
			setMealSideItemsGroupNum(Long::valueOf(newValue));
			break;

		case fieldOffsetBuildAMealSideItemsAreFree:
			setMealSideItemsAreFree(Bool::valueOf(newValue));
			break;

		case fieldOffsetBuildAMealNumberOfSidesIncluded:
			setMealNumSidesIncluded(Integer::valueOf(newValue));
			break;

		case fieldOffsetCutTestMeatSpecSheetGroupNum:
			setCutTestMeatSpecSheetGroupNum(Long::valueOf(newValue));
			break;

		// For Pick-5 project
		case fieldOffsetWeightMin:
		{
			setWeightMin( MetSig::determineImplicitDecimalWeight(newValue) );
			break;
		}

		case fieldOffsetWeightMax:
		{
			setWeightMax( MetSig::determineImplicitDecimalWeight(newValue) );
			break;
		}

		case fieldOffsetTotalPriceMin:
		{
			setTotalPriceMin( MetSig::determineImplicitDecimalPrice(newValue) );
			break;
		}

		case fieldOffsetTotalPriceMax:
		{
			setTotalPriceMax( MetSig::determineImplicitDecimalPrice(newValue) );
			break;
		}

		default:
			if (traceOn)
				Debug::trace("Product.updateField", "updateField(" + inttostring(fieldOffset) + ") = " + newValue + ". Do nothing.");
			return false;
			break;
	}

	return true;
}

void Product::updateFieldProductText(ProdSupportTextType type, const HobString &newValue, bool linksMustExist)
{
	HobRecordNum newTxtNum = (HobRecordNum)Long::valueOf(newValue);

	if (newTxtNum <= 0)
	{
		setProductText(type, 0);
		Debug::trace("Product.ProductText", string("updateField() set ProductText to NULL - type:") + inttostring((int)type));
	}
	else
	{
		ProductText * newProductText = ProductText::findRecord(type, newTxtNum, deptNum, dbAccess);
		if (linksMustExist)
		{
			if (newProductText)
			{
				if (newProductText->getErrorCode() != RECORD_OK)
					delete newProductText;
				else
				{
					setProductText(type, newProductText);
					Debug::trace("Product.ProductText", string("updateField() found existing ProductText - num:") + newValue + "  deptNum:" + longtostring(deptNum) + "  text type:" + inttostring((int)type));
				}
			}
		}
		else
		{
			if (!newProductText)
			{
				newProductText = new ProductText(type);
				newProductText->setDeptNum(deptNum);
				newProductText->setTextNum(newTxtNum);
				newProductText->setErrorCode(RECORD_NOT_FOUND);

				Debug::trace("Product.ProductText", HobString("updateField() created new Product Text - num:") + newValue + "  deptNum:" + longtostring(deptNum) + "  text type:" + inttostring((int)type));
			}
			setProductText(type, newProductText);
		}
	}
}


void Product::updateRecord(Product *newData, AbstractSession *session, bool linksMustExist)
{
	if (!newData)
		return;

	if (Debug::isTracing("Product.updateRecord"))
		Debug::trace("Product.updateRecord", string("updateRecord() - passed in num ") + longtostring(newData->getProductNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));

	////////////////////////////////////////////////// START DUMP TAGS ///////////////////////////////////////////////////////////////////

	if (session && session->isCommSession())
	{
		// 1. GET input string containing key-value pairs separated by vertical bar | and keys/values separated by a colon :.						
		std::string exclusionStr = HobTokenizerTSV::getExclusionString();														// RETURNS: 89:l2,l3|B9:pp,SM,bd

		// 2. Finally - get the tags we need to dump, if they exist where 89 = RT89, Products Function code
		std::vector<std::string> droplist =  HobTokenizerTSV::getRTsMappedToTags(exclusionStr, "89");							// RETURNS vector of tags to drop:  [l2],[l3]

		if (droplist.size() > 0)
		{
			for (const std::string& tag : droplist)
			{
				int tagOffset = HobTokenizerTSV::performTagDrop(productTagsToDrop, tag);
				if (tagOffset > 0)
					newData->setReceived(tagOffset, false);		// to drop the tag, just set the isReceived flag to false - just means we didnt recieve any data for this field (even tho we did)
			}
		}
	}
	
	// Need to set the deptNum first; it's needed when updating some of the other fields below (product number for one).
	// Since its fieldOffset is later in the list, it would be set too late.
	if ( newData->isReceived(fieldOffsetProductDeptNum) )
		setDeptNum(newData->getDeptNum());

	short loopCnt = 1;
	while (loopCnt < endProductFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetProductNum:
					setProductNum(newData->getProductNum());
					break;
				case fieldOffsetProductType:
					setProductType(newData->getProductType());
					break;
				case fieldOffsetDescription:
					setDescription(newData->getDescription());
					break;

				case fieldOffsetBarcodeType:
					// This is being commented out.  It makes no sense to change the barcodeType
					// barCodeNum.setBarcodeType(newData->barCodeNum.getBarcodeType());
					break;
				case fieldOffsetBarcodePrefix:
					barCodeNum.setPrefix(newData->barCodeNum.getPrefix());
					break;
				case fieldOffsetBarcodeProductNum:
					// SOF-3880
					if (( Configuration::getInstance()->isIgnoreVendorAndProductNumTagsWhenUpcNumTag() ) &&
						( newData->isReceived(fieldOffsetBarcodeUpcNum) ))
					{
						Debug::trace("Product", "ignore product num due to config setting");
					}
					else
					{
						barCodeNum.setProductNumber(newData->barCodeNum.getProductNumber());
					}
					break;
				case fieldOffsetBarcodeUpcNum:
					setRawBarCodeData(newData->getRawBarCodeData());
					break;
				case fieldOffsetBarcodeVendorNum:
					// SOF-3880
					if (( Configuration::getInstance()->isIgnoreVendorAndProductNumTagsWhenUpcNumTag() ) &&
						( newData->isReceived(fieldOffsetBarcodeUpcNum) ))
					{
						Debug::trace("Product", "ignore vendor num due to config setting");
					}
					else
					{
						barCodeNum.setVendorNumber(newData->barCodeNum.getVendorNumber());
					}
					break;
				case fieldOffsetBarcodeEANCode:
					barCodeNum.setEANCode(newData->barCodeNum.getEANCode());
					break;

				case fieldOffsetByCount:
					Debug::trace("Product.ByCount", "updateRecord()   newData->getByCount() =  " + HobString(Integer::toString(newData->getByCount())));
					setByCount(newData->getByCount());
					break;
				case fieldOffsetByCountForced:
					setByCountForced(newData->isByCountForced());
					break;

				case fieldOffsetExceptionPrice:
					setExceptionPrice(newData->getExceptionPrice());
					break;
				case fieldOffsetUnitPrice:
					setUnitPrice(newData->getUnitPrice());
					break;
				case fieldOffsetUnitPriceForced:
					setUnitPriceForced(newData->isUnitPriceForced());
					break;

				case fieldOffsetDiscountType:
					setDiscountType(newData->getDiscountType());
					Debug::trace("Product.FreqShopper", HobString("updateRecord():    discountType = ") + HobString(Long::toString(discountType)));
					break;
				case fieldOffsetDiscountValue1:
					setDiscountValue1(newData->getDiscountValue1());
					Debug::trace("Product.FreqShopper", HobString("updateRecord():    discountValue1 = ") + HobString(Long::toString(discountValue1)));
					break;
				case fieldOffsetDiscountValue2:
					setDiscountValue2(newData->getDiscountValue2());
					Debug::trace("Product.FreqShopper", HobString("updateRecord():    discountValue2 = ") + HobString(Long::toString(discountValue2)));
					break;
				case fieldOffsetDiscountValue3:
					setDiscountValue3(newData->getDiscountValue3());
					Debug::trace("Product.FreqShopper", HobString("updateRecord():    discountValue3 = ") + HobString(Long::toString(discountValue3)));
					break;

				case fieldOffsetTareAValue:
					tareA.setValue(newData->getTareA().getValue());
					break;
				case fieldOffsetTareBValue:
					tareB.setValue(newData->getTareB().getValue());
					break;
				case fieldOffsetTareAForced:	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
					tareA.setForced(newData->getTareA().isForced());
					break;
				case fieldOffsetProportionalTare:
					setProportionalTare(newData->getProportionalTare());
					break;
				case fieldOffsetFixedWeight:
					
					setFixedWeight(newData->getFixedWeight());
					
					//PTR #691 -> so when setOpModeFixedWt() is called we don't set fixed weight to 0
					// when creating product					
					originalFixedWeight = newData->getFixedWeight();

					break;
				case fieldOffsetShelfLifeDays:
					setShelfLifeDays(newData->getShelfLifeDays());
					break;
				case fieldOffsetShelfLifeHours:
					setShelfLifeHours(newData->getShelfLifeHours());
					break;
				case fieldOffsetShelfLifeMinutes:
					setShelfLifeMinutes(newData->getShelfLifeMinutes());
					break;
				case fieldOffsetProductLifeDays:
					setProductLifeDays(newData->getProductLifeDays());
					break;
				case fieldOffsetProductLifeHours:
					setProductLifeHours(newData->getProductLifeHours());
					break;
				case fieldOffsetProductLifeMinutes:
					setProductLifeMinutes(newData->getProductLifeMinutes());
					break;

				case fieldOffsetProductReportingClassNum:
				{
					HobRecordNum currentReportingClassNum = getReportingClassNum();
					HobRecordNum newReportingClassNum = newData->getReportingClassNum();

					if ((currentReportingClassNum != newReportingClassNum) && (newReportingClassNum > 0))
					{
						Class *newReportingClass = Class::findRecord(newReportingClassNum, deptNum, dbAccess);
						if (linksMustExist)
						{
							if (newReportingClass && newReportingClass->getErrorCode() != RECORD_OK)
							{
								//	How do we report an error?
								delete newReportingClass;
							}
							else if (newReportingClass)
							{
								newReportingClass->checkClassTree(dbAccess);
								setReportingClass(newReportingClass);
							}

							if (Debug::isTracing("Product.Class"))
							{
								Debug::trace("Product.Class", HobString("    Class: ") + ptrtostring(newReportingClass));
								if (newReportingClass)
									Debug::trace("Product.Class", HobString("  Class number:") + HobString(Long::toString(newReportingClass->getClassNum())) + HobString(" - updateProduct()"));
							}
						}
						else
						{
							if (!newReportingClass)
							{
								newReportingClass = new Class(); //(newReportingClassNum);
								newReportingClass->setDeptNum(deptNum);
								newReportingClass->setClassNum(newReportingClassNum);
								newReportingClass->setErrorCode(RECORD_NOT_FOUND);
								Debug::trace("Product.Class", HobString("updateRecord() creating new Class:    Class: ") + ptrtostring(newReportingClass) + HobString("  Class number:") + HobString(Long::toString(newReportingClassNum)) + HobString(" - updateProduct()"));
							}
							setReportingClass(newReportingClass);
						}
					}
				}
					break;

				case fieldOffsetGraphicName1:	// Graphic 1 NAME
				case fieldOffsetGraphicName2:	// Graphic 2 NAME
				case fieldOffsetGraphicName3:	// Graphic 3 NAME
				case fieldOffsetGraphicName4:	// Graphic 4 NAME
				{
					bool traceGraphic = Debug::isTracing("Product.Graphic");
					unsigned int graphicIndex = (loopCnt - fieldOffsetGraphicName1);
					HobString thisGraphicName = getGraphicName(graphicIndex);
					HobString newGraphicName = newData->getGraphicName(graphicIndex);
					Graphic *tempGraphic = newData->getGraphic(graphicIndex);

					// WEH-45
					bool isGCDProductAdvert( false );

					if (( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgGCDUseTransactionalLayout) ) &&
						( Configuration::getInstance()->toIntCfg(fieldOffsetCfgGCDProductAdvertType) == (graphicIndex+1) ))
					{
						isGCDProductAdvert = true;
					}

					if (traceGraphic)
					{
						HobString pThis = Hex::toString((long long) this);
						Debug::trace("Product.Graphic", HobString("updateProduct(): Product = ") + pThis +
							HobString("    Index = ") + HobString(Integer::toString(graphicIndex)) +
							HobString("    current = ") + thisGraphicName +
							HobString("    new = ") + newGraphicName +
							HobString("    tempGraphic = ") + ptrtostring(tempGraphic));
					}

					if (thisGraphicName.compare(newGraphicName) != 0)
					{
						if (!tempGraphic)
							setGraphic(graphicIndex, (Graphic *) 0);
						else if (newGraphicName == "0")
						{
							//	Do not accept "0" for a graphic file name.
							//	Other scale comm software may send a 0 for the graphic names but we will not accept it
							//	as it causes a lot of problems for the customer when they look at their PLU's in Change Product.
							if (traceGraphic)
								Debug::trace("Product.Graphic", "Will not accept a graphic file name of '0'. No new graphic.");
							setGraphic(graphicIndex, (Graphic *) 0);
						}
						// WEH-45
						// allow when graphic intended for GCD
						else if (( RECORD_OK == tempGraphic->getErrorCode() ) || ( isGCDProductAdvert ))
						{
							//	Don't use tempGraphic since newData is temporary. Get a graphic pointer we can keep.
							Graphic *newGraphic = Graphic::findGraphic(newGraphicName);
							setGraphic(graphicIndex, newGraphic);
						}
						else if (linksMustExist)
						{
							if ((session) && ((tempGraphic->getGraphicName().compare(scaleConfig->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME))) != 0))
							{
								// Send error message to browser indicating a problem with the Graphic.
								SessionMessage errMsg(Error, ErrProductDev);
								errMsg.setFormatType(BrowserFormat);

								//	Build the Graphic error, so we can specify which graphic in the PLU record.
								HfcErrorGraphic *hfcErrorGraphic = new HfcErrorGraphic(ProductErrGraphic, HfcResponse::RESPONSESTYLE_OK, tempGraphic);
								if (hfcErrorGraphic)
								{
									const char *msgdata = hfcErrorGraphic->serialize(BrowserFormat);
									int msglen = hfcErrorGraphic->getSerializedLength();

									hfcErrorGraphic->setData(msgdata);
									errMsg.setHfcObject(hfcErrorGraphic);

									Debug::trace("Product", HobString("Graphic ") + HobString(Integer::toString(graphicIndex + 1)) + HobString(" error: ") + HobString(msgdata, msglen));

									// Send the graphic not found error message to the UI (Browser)
									session->send(&errMsg, Messenger::DEST_UI);
									delete hfcErrorGraphic;
								}
							}
						}
						else
						{
							Graphic *newGraphic = new Graphic(newGraphicName);
							setGraphic(graphicIndex, newGraphic);
						}
					}
				}
					break;

				// TODO - Larry - done
				// With the 'label types by mode' project, we should no longer be receiving these first three offsets from the UI.
				// We can still receive them via comm / backup / restore, in which case, we still need to deal with them.
				// We still handle them appropriately; the calls to getLabelTypeNum(...) and setLabelTypeNum(...) map them to fieldOffsetDefaultLabelTypeNum1/2/3.
				case fieldOffsetLabelTypeNum1:		// PRIMARY LABEL TYPE NUMBER
				{
					//Debug::trace("Product.updateRecord", string("updateRecord() - fieldOffset:") + " fieldOffsetLabelTypeNum1 is being updated to new value: " + longtostring(newData->getLabelTypeNum(loopCnt)));
					setLabelTypeNum(loopCnt, newData->getLabelTypeNum(loopCnt));
					break;
				}
				case fieldOffsetLabelTypeNum2:		// SECONDARY LABEL TYPE NUMBER
				{
					//Debug::trace("Product.updateRecord", string("updateRecord() - fieldOffset:") + " fieldOffsetLabelTypeNum2 is being updated to new value: " + longtostring(newData->getLabelTypeNum(loopCnt)));
					setLabelTypeNum(loopCnt, newData->getLabelTypeNum(loopCnt));
					break;
				}
				case fieldOffsetLabelTypeNum3:		// THIRD LABEL TYPE	NUMBER
				{
					//Debug::trace("Product.updateRecord", string("updateRecord() - fieldOffset:") + " fieldOffsetLabelTypeNum3 is being updated to new value: " + longtostring(newData->getLabelTypeNum(loopCnt)));
					setLabelTypeNum(loopCnt, newData->getLabelTypeNum(loopCnt));
					break;
				}
				case fieldOffsetDefaultLabelTypeNum1:
				case fieldOffsetDefaultLabelTypeNum2:
				case fieldOffsetDefaultLabelTypeNum3:
				case fieldOffsetManualLabelTypeNum1:
				case fieldOffsetManualLabelTypeNum2:
				case fieldOffsetManualLabelTypeNum3:
				case fieldOffsetPrepackLabelTypeNum1:
				case fieldOffsetPrepackLabelTypeNum2:
				case fieldOffsetPrepackLabelTypeNum3:
				case fieldOffsetProdEntryLabelTypeNum1:
				case fieldOffsetProdEntryLabelTypeNum2:
				case fieldOffsetProdEntryLabelTypeNum3:
				case fieldOffsetSelfServLabelTypeNum1:
				case fieldOffsetSelfServLabelTypeNum2:
				case fieldOffsetSelfServLabelTypeNum3:
				case fieldOffsetPick5LabelTypeNum1:
				case fieldOffsetPick5LabelTypeNum2:
				case fieldOffsetPick5LabelTypeNum3:
				case fieldOffsetOnlineOrderingLabelTypeNum1:
				case fieldOffsetOnlineOrderingLabelTypeNum2:
				case fieldOffsetOnlineOrderingLabelTypeNum3:
				{
					if (Debug::isTracing("Product.LabelType"))
						Debug::trace("Product.LabelType", string("updateRecord() - fieldOffset:") + inttostring(loopCnt) + " is being updated to new value: " + longtostring(newData->getLabelTypeNum(loopCnt)));
					setLabelTypeNum(loopCnt, newData->getLabelTypeNum(loopCnt));
					break;
				}

				case fieldOffsetLabelRotation1:		// PRIMARY LABEL ROTATION
					setLabelRotation(FirstPrinter, newData->getLabelRotation(FirstPrinter));
					break;
				case fieldOffsetLabelRotation2:		// SECONDARY LABEL ROTATION
					setLabelRotation(SecondPrinter, newData->getLabelRotation(SecondPrinter));
					break;
				case fieldOffsetLabelRotation3:		// THIRD LABEL ROTATION
					setLabelRotation(ThirdPrinter, newData->getLabelRotation(ThirdPrinter));
					break;

				case fieldOffsetLabelPlacement1:		// PRIMARY LABEL PLACEMENT
					setLabelPlacement(FirstPrinter, newData->getLabelPlacement(FirstPrinter));
					break;
				case fieldOffsetLabelPlacement2:		// SECONDARY LABEL PLACEMENT
					setLabelPlacement(SecondPrinter, newData->getLabelPlacement(SecondPrinter));
					break;
				case fieldOffsetLabelPlacement3:		// THIRD LABEL PLACEMENT
					setLabelPlacement(ThirdPrinter, newData->getLabelPlacement(ThirdPrinter));
					break;

				case fieldOffsetPoints:
					setPoints(newData->getPoints());
					break;

				case fieldOffsetIntakeMode:
					setIntakeMode(newData->getIntakeMode());
					break;

				case fieldOffsetProductGraphicName:
					setProductGraphicName(newData->getProductGraphicName());
					break;
				case fieldOffsetProductName1:
					setProductName1(newData->getProductName1());
					break;
				case fieldOffsetProductName2:
					setProductName2(newData->getProductName2());
					break;
				case fieldOffsetProdCategoryNum:
				{
					bool traceCategory = Debug::isTracing("Product.Category");
					HobRecordNum currentCategoryNum = getCategoryNum();
					HobRecordNum newCategoryNum = newData->getCategoryNum();
					if (traceCategory)
					{
						HobString myDebug2;
						myDebug2.append("currentCategoryNum = ").append(Integer::toString(currentCategoryNum));
						myDebug2.append("   newCategoryNum = ").append(Integer::toString(newCategoryNum));
						Debug::trace("Product.Category", myDebug2);
					}
					if (currentCategoryNum != newCategoryNum)
					{
						Category *newCategory = Category::findRecord(newCategoryNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
						if (linksMustExist)
						{
							if ((newCategory && newCategory->getErrorCode() != RECORD_NOT_FOUND) ||
								(!newCategory && ( newCategoryNum == 0 || newCategoryNum == RECORD_NOT_FOUND ) ) )
							{
								setCategory(newCategory);
								if (traceCategory)
								{
									HobString myDebug2;
									myDebug2.append("setting the Category: ").append(Hex::toString((long long) newCategory));
									myDebug2.append("  Category # ").append(Long::toString(newCategoryNum));
									Debug::trace("Product.Category", myDebug2);
								}
							}
						}
						else
						{
							if (!newCategory)
							{
								newCategory = new Category();
								newCategory->setCategoryNum(newCategoryNum);
								if (traceCategory)
								{
									HobString myDebug2;
									myDebug2.append("creating new Category: ").append(Hex::toString((long long) newCategory));
									myDebug2.append("  Category # ").append(Long::toString(newCategoryNum));
									Debug::trace("Product.Category", myDebug2);
								}
							}
							setCategory(newCategory);
						}
					}
				}
					break;
				case fieldOffsetProdSpecialNum:
				{
					HobRecordNum currentSpecialNum = getSpecialNum();
					HobRecordNum newSpecialNum = newData->getSpecialNum();

					if (currentSpecialNum != newSpecialNum)
					{
						Special *newSpecial = Special::findRecord(newSpecialNum, newData->getDeptNum(), dbAccess);
						if (linksMustExist)
						{
							if ((newSpecial && newSpecial->getErrorCode() != RECORD_NOT_FOUND) ||
								(!newSpecial && ( newSpecialNum == 0 || newSpecialNum == RECORD_NOT_FOUND ) ) )
							{
								setSpecialProgram(newSpecial);
								Debug::trace("Product.Special", HobString("updateRecord() setting the Special: ") + ptrtostring(newSpecial) + HobString("  Special # ") + HobString(Long::toString(newSpecialNum)) + HobString(" - updateProduct()"));
							}
						}
						else
						{
							if (!newSpecial)
							{
								newSpecial = new Special();
								newSpecial->setSpecialNum(newSpecialNum);
								Debug::trace("Product.Special", HobString("updateRecord() creating new Special: ") + ptrtostring(newSpecial) + HobString("  Special # ") + HobString(Long::toString(newSpecialNum)) + HobString(" - updateProduct()"));
							}
							setSpecialProgram(newSpecial);
						}
					}
				}
					break;
				case fieldOffsetSpecialStartDate:
					setSpecialStartDate(newData->getSpecialStartDate());
					break;

				case fieldOffsetApplySecurity:
					setApplySecurityLabel(newData->isApplySecurityLabel());
					break;
				case fieldOffsetDontPrintPackedOn:
					setDontPrintPackedOn(newData->isDontPrintPackedOn());
					break;
				case fieldOffsetDontPrintSellBy:
					setDontPrintSellBy(newData->isDontPrintSellBy());
					break;
				case fieldOffsetDontPrintBestBefore:
					setDontPrintBestBefore(newData->isDontPrintBestBefore());
					break;

				case fieldOffsetProdNutrifactNum:
				{

					Nutrifacts *currentNutrifact = getNutrifacts();
					Nutrifacts *newNutrifact = newData->getNutrifacts();

					HobRecordNum currentNutrifactNum = ((currentNutrifact) ? currentNutrifact->getTextNum() : 0);
					HobRecordNum newNutrifactNum = ((newNutrifact) ? newNutrifact->getTextNum() : 0);

					if (currentNutrifactNum != newNutrifactNum)
					{
						Nutrifacts * newNutrifact = Nutrifacts::findRecord(newNutrifactNum, deptNum, dbAccess);

						if (linksMustExist)
						{
							if ((newNutrifact && newNutrifact->getErrorCode() != RECORD_NOT_FOUND) ||
								(!newNutrifact && ( newNutrifactNum == 0 || newNutrifactNum == RECORD_NOT_FOUND ) ) )
							{
								setNutrifacts(newNutrifact);
								Debug::trace("Product.Nutrifacts", HobString("updateRecord() setting the Nutrifact Number: ") + HobString(Long::toString(newNutrifactNum)) + HobString(" - updateProduct()"));
							}
						}
						else
						{
							if (!newNutrifact)
							{
								newNutrifact = new Nutrifacts();
								newNutrifact->setTextNum(newNutrifactNum);
								newNutrifact->setErrorCode(RECORD_NOT_FOUND);

								Debug::trace("Product.Nutrifacts", HobString("updateRecord() creating new Nutrifact:    Nutrifact Number: ") + HobString(Long::toString(newNutrifactNum)) + HobString(" - updateProduct()"));

							}
							setNutrifacts(newNutrifact);
						}
					}
					break;
				}

				case fieldOffsetExpandedTextNum:
					updateRecordProductText(psttExpandedText, newData, linksMustExist);
					break;
				case fieldOffsetSpecialMessageNum:
					updateRecordProductText(psttSpecialMessage, newData, linksMustExist);
					break;
				case fieldOffsetProdProductNoteNum:
					updateRecordProductText(psttProductNote, newData, linksMustExist);
					break;
				case fieldOffsetMarqueeTextNum:
					updateRecordProductText(psttMarqueeText, newData, linksMustExist);
					break;
				case fieldOffsetProductText5Num:
					updateRecordProductText(psttProductText5, newData, linksMustExist);
					break;
				case fieldOffsetProductText6Num:
					updateRecordProductText(psttProductText6, newData, linksMustExist);
					break;
				case fieldOffsetProductText7Num:
					updateRecordProductText(psttProductText7, newData, linksMustExist);
					break;
				case fieldOffsetProductText8Num:
					updateRecordProductText(psttProductText8, newData, linksMustExist);
					break;
				case fieldOffsetProductText9Num:
					updateRecordProductText(psttProductText9, newData, linksMustExist);
					break;

				case fieldOffsetPriceModifier:
					setPriceModifier(newData->getPriceModifier());
					break;
				case fieldOffsetProdCoolCategoryNum:
					setCoolCategoryNum(newData->getCoolCategoryNum());
					break;
				case fieldOffsetProdCoolPretextNum:
					setCoolPretextNum(newData->getCoolPretextNum());
					break;
				case fieldOffsetProdCoolDefaultTextNum:
					setCoolDefaultTextNum(newData->getCoolDefaultTextNum());
					break;
				case fieldOffsetProdCoolTextNum:
					setCoolTextNum(newData->getCoolTextNum());
					break;
				case fieldOffsetProdCoolShortlistNum:
					setCoolShortlistNum(newData->getCoolShortlistNum());
					break;
				case fieldOffsetProdCoolShortlistFirstLink:
					setCoolShortlistFirstLink(newData->getCoolShortlistFirstLink());
					break;
				case fieldOffsetProdCOOLTrackingText:
				{
					HobString tempStr = newData->getCoolTrackingText();
					tempStr.stripFontSequences();
					setCoolTrackingText(tempStr);
					break;
				}
				case fieldOffsetProdCoolVerifyForced:
					setCoolVerifyForced(newData->isCoolVerifyForced());
					break;
				case fieldOffsetBackDateDays:
					setBackDateDays(newData->getBackDateDays());
					break;
				case fieldOffsetProdCoolRequired:
				{
					Debug::trace("Product", "updateRecord(fieldOffsetProdCoolRequired)");
					setCoolRequired(newData->isCoolRequired());
					break;
				}
				case fieldOffsetProductionPlanningType:
					setProductionPlanningType(newData->getProductionPlanningType());
					break;

				case fieldOffsetPortions:
				{
					Debug::trace("Product.Portions", "updateRecord(fieldOffsetPortions)");
					setPortions(newData->getPortions());
					break;
				}

				case fieldOffsetBuildAMealSideItemsGroupNum:
					setMealSideItemsGroupNum(newData->getMealSideItemsGroupNum());
					break;

				case fieldOffsetBuildAMealSideItemsAreFree:
					setMealSideItemsAreFree(newData->getMealSideItemsAreFree());
					break;

				case fieldOffsetBuildAMealNumberOfSidesIncluded:
					setMealNumSidesIncluded(newData->getMealNumSidesIncluded());
					break;

				case fieldOffsetCutTestMeatSpecSheetGroupNum:
					setCutTestMeatSpecSheetGroupNum(newData->getCutTestMeatSpecSheetGroupNum());
					break;

				// For Pick-5 project

				case fieldOffsetWeightMin:
					setWeightMin(newData->getWeightMin());
					break;

				case fieldOffsetWeightMax:
					setWeightMax(newData->getWeightMax());
					break;

				case fieldOffsetTotalPriceMin:
					setTotalPriceMin(newData->getTotalPriceMin());
					break;

				case fieldOffsetTotalPriceMax:
					setTotalPriceMax(newData->getTotalPriceMax());
					break;

				default:
					break;
			}
		}
		loopCnt++;
	}

	typeUnknownProduct();
}

void Product::updateRecordProductText(ProdSupportTextType type, Product *newData, bool linksMustExist)
{
	ProductText *currentProductText = getProductText(type);
	ProductText *newProductText = newData->getProductText(type);

	HobRecordNum currentProductTextNum = ((currentProductText) ? currentProductText->getTextNum() : 0);
	HobRecordNum newProductTextNum = ((newProductText) ? newProductText->getTextNum() : 0);

	if (currentProductTextNum != newProductTextNum)
	{
		ProductText *newProdText = ProductText::findRecord(type, newProductTextNum, deptNum, dbAccess);

		if (linksMustExist)
		{
			if ((newProdText && newProdText->getErrorCode() != RECORD_NOT_FOUND) ||
				(!newProdText && newProductTextNum == 0))
			{
				setProductText(type, newProdText);
				Debug::trace("Product.ProductText", "updateProductProductText() setting Text type:" + inttostring(type) + "  number:" + longtostring(newProductTextNum));
			}
		}
		else
		{
			if (!newProdText)
			{
				newProdText = new ProductText(type);
				newProdText->setTextNum(newProductTextNum);
				newProdText->setErrorCode(RECORD_NOT_FOUND);

				Debug::trace("Product.ProductText", "updateProductProductText() creating new ProductText:  type:" + inttostring(type) + "  number: " + longtostring(newProductTextNum));
			}
			setProductText(type, newProdText);
		}
	}
}


bool Product::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetDescription:
		case fieldOffsetDiscountLabel1:
		case fieldOffsetDiscountLabel2:
		case fieldOffsetDiscountLabel3:
		case fieldOffsetSellByDate:
		case fieldOffsetBestBeforeDate:
		case fieldOffsetProductName1:
		case fieldOffsetProductName2:
		case fieldOffsetProdCOOLTrackingText:
		case fieldOffsetExceptionPrice:
		case fieldOffsetUnitPrice:
		case fieldOffsetDiscountValue1:
		case fieldOffsetDiscountValue2:
		case fieldOffsetDiscountValue3:
		case fieldOffsetTareAValue:
		case fieldOffsetTareBValue:
		case fieldOffsetProportionalTare:
		case fieldOffsetLabelTypeWidth1:
		case fieldOffsetLabelTypeLength1:
		case fieldOffsetLabelTypeWidth2:
		case fieldOffsetLabelTypeLength2:
		case fieldOffsetLabelTypeWidth3:
		case fieldOffsetLabelTypeLength3:
		case fieldOffsetKeyboardNetWeight:
		case fieldOffsetBackDateDate:
			return true;
			break;

		default:
			break;
	}
	return false;
}

void Product::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	bool traceOn = Debug::isTracing("Product.getFieldAsString");

	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetProductDeptNum:
			result = longtostring(deptNum);
			break;

		case fieldOffsetProductNum:
			if (productNum > 0 || format == BrowserFormatPLUOperateMode)
				result = Long::toString(productNum);
			break;

		case fieldOffsetProductType:
			result = Integer::toString(productType);
			break;

		case fieldOffsetDescription:
		{
			result = description;
			if (format == BrowserFormatPLUOperateMode)
			{
				HobString tempStr = description;
				if (traceOn)
					Debug::trace("Product.getFieldAsString", string("Description, 1) tempStr = [[") + tempStr + "]]");
				tempStr.removeTrailingNewlinesAndSpaces();
				tempStr.stripFontSequences();
				if (traceOn)
					Debug::trace("Product.getFieldAsString", string("Description, 2) tempStr = [[") + tempStr + "]]");
			}
			break;
		}

		case fieldOffsetBarcodeType:
			// SOF-4711
			//result = /*(barCodeNum.barcodeTypeIsValid()) ? Integer::toString(barCodeNum.getBarcodeType()) :*/ Integer::toString(UPCA);
			result = Integer::toString(barCodeNum.getBarcodeType());

			break;

		case fieldOffsetBarcodeFields:
			result = Integer::toString(barCodeNum.getRelevantFields());
			break;

		case fieldOffsetBarcodePrefix:
			result = barCodeNum.getPrefix();
			if (format == EvolutionTaggedFormat || format == UltimaTaggedFormat)
				result.getCharsWithPadding(2);
			break;

		case fieldOffsetBarcodeUpcNum:
			if (rawBarCodeData.empty())
			{
				if (barCodeNum.getProductNumber().empty())
					result = barCodeNum.getEANCode();
				else
				{
					//					bool checkDigit = scaleConfig->toBoolCfg(fieldOffsetCfgPFSecondCheckDigit);
					//					if ( numberSys == 2 || numberSys == 4 )

					result = barCodeNum.getVendorNumber();
					result.append(barCodeNum.getProductNumber());
				}
			}
			else
				result = rawBarCodeData;
			break;

		case fieldOffsetBarcodeProductNum:
			// SOF-3890 if we have a value from the DB, use that
			result = barCodeNum.getProductNumber();

			// SOF - 2751 - Don't write out 'pn' if that data comes from the raw barcode data. If they were doing a backup, and then used that file to load another scale, the other scale would think that the bar code's product number had been overridden, but that's not true. */
			if ( result.length() == 0 && format != UltimaTaggedFormat )
			{
				if ( format == EvolutionTaggedFormat || format == DBIndependentFormatFormat )
				{
					if ( !barCodeNum.isProductNumberFromRaw() )
						result = barCodeNum.getProductNumber();
				}
				else
					result = barCodeNum.getDerivedProductNumber();
			}
			break;

		case fieldOffsetBarcodeVendorNum:
			// SOF-3890 if we have a value from the DB, use that
			result = barCodeNum.getVendorNumber();

			// SOF - 2751 - Don't write out 'vn' if that data comes from the raw barcode data. If they were doing a backup, and then used that file to load another scale, the other scale would think that the bar code's vendor number had been overridden, but that's not true. */
			if ( result.length() == 0 && format != UltimaTaggedFormat )
			{
				if ( format == EvolutionTaggedFormat || format == DBIndependentFormatFormat )
				{
					if ( !barCodeNum.isVendorNumberFromRaw() )
						result = barCodeNum.getVendorNumber();
				}
				else
					result = barCodeNum.getDerivedVendorNumber();
			}
			break;

		case fieldOffsetBarcodeEANCode:
			// SOF-3890 if we have a value from the DB, use that
			result = barCodeNum.getEANCode();

			// SOF - 2751 - Don't write out 'Ec' if that data comes from the raw barcode data. If they were doing a backup, and then used that file to load another scale, the other scale would think that the bar code's EAN code had been overridden, but that's not true. */
			if ( result.length() == 0 && format != UltimaTaggedFormat )
			{
				if ( format == EvolutionTaggedFormat || format == DBIndependentFormatFormat )
				{
					if ( !barCodeNum.isEanCodeFromRaw() )
						result = barCodeNum.getEANCode();
				}
				else
					result = barCodeNum.getDerivedEANCode();
			}
			break;

		case fieldOffsetEANFormatString:
			result = barCodeNum.getFormatString(EAN13);
			break;

		case fieldOffsetEAN128CFormatString:					// ALV
			result = barCodeNum.getFormatString(EAN128C);
			break;

		case fieldOffsetGS1DatabarGTIN:
			result = barCodeNum.getFormatString(GS1GTIN);
			break;

		case fieldOffsetGS1DatabarAI:
			result = barCodeNum.getFormatString(GS1AI);
			break;

		case fieldOffsetQRCodeFormatString:
			result = barCodeNum.getFormatString(QR_CODE);
			break;

		case fieldOffsetDataMatrixFormatString:
			result = barCodeNum.getFormatString(DATAMATRIX);
			break;

		case fieldOffsetByCount:
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					result = (byCountForced) ? "-1" : Integer::toString((byCount > 0) ? byCount : 0);	//	-1 indicates forcedByCount
					break;

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					result = (byCountForced && !byCountEntered) ? " " : Integer::toString((byCount > 0) ? byCount : 0);	//	-1 indicates forcedByCount
					break;

				default:
					result = Integer::toString((byCount > 0) ? byCount : 0);
					break;
			}
			break;

		case fieldOffsetByCountForced:
			if (format == BrowserFormatPLUOperateMode)
				result = Bool::toString(byCountForced && !byCountEntered);
			else
				result = Bool::toString(byCountForced);
			break;

		case fieldOffsetExceptionPrice:
			switch (format)
			{
			case UltimaTaggedFormat:
			case EvolutionTaggedFormat:
				result = Long::toString(exceptionPrice);
				break;

			case BrowserFormat:
			case BrowserFormatPLUOperateMode:
				result = scaleConfig->formatHobPrice(exceptionPrice);
				break;

			default:
				result = scaleConfig->formatHobPrice(exceptionPrice, false);
			}
			break;

		case fieldOffsetUnitPrice:
		{
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					result = (unitPriceForced) ? "-1" : Long::toString(unitPrice);
					break;

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					if (unitPriceForced && !unitPriceEntered)
						result = "";
					else
						result = scaleConfig->formatHobPrice(unitPrice);
					break;

				default:
					result = scaleConfig->formatHobPrice(unitPrice, false);
			}
			if (Debug::isTracing("Product.leadingZeros"))
				Debug::trace("Product.leadingZeros", string("fieldOffsetUnitPrice:  result = ") + result);
			break;
		}

		case fieldOffsetUnitPriceForced:
			if (format == BrowserFormatPLUOperateMode)
				result = Bool::toString(unitPriceForced && !unitPriceEntered);
			else
				result = Bool::toString(unitPriceForced);
			break;

		case fieldOffsetDiscountType:
			result = Integer::toString(discountType);
			break;

		case fieldOffsetDiscountValue1:
		case fieldOffsetDiscountValue2:
		case fieldOffsetDiscountValue3:
		{
			short	 thisDiscountType = getDiscountType();			//	If FS is perProduct, the discount type is in the PLU.
			HobValue value = 0;

			Debug::trace("Product.FreqShopper", HobString("getFieldAsString():  thisDiscountType = ") + HobString(Integer::toString(thisDiscountType)));

			switch (fieldOffset)
			{
				case fieldOffsetDiscountValue1:
					value = discountValue1;
					break;
				case fieldOffsetDiscountValue2:
					value = discountValue2;
					break;
				case fieldOffsetDiscountValue3:
					value = discountValue3;
					break;
				default:
					break;
			}

			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					result = Long::toString(value);
					break;

				default:
				{
					bool useDefaultDecimalSeparator = false;
					if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
						useDefaultDecimalSeparator = true;

					if ((thisDiscountType == Percent_off_net_value) || (thisDiscountType == Percent_added_net_value))
						scaleConfig->formatValue(value, result, scaleConfig->getPercentagePrecision(), useDefaultDecimalSeparator);
					else
						result = scaleConfig->formatHobPrice(value, useDefaultDecimalSeparator);
					break;
				}
			}
			break;
		}

		case fieldOffsetDiscountLabel1:
		{
			result = scaleConfig->toStringCfgI(CfgInternal::PRODUCTDISCOUNTLEVEL1LABEL);
			break;
		}

		case fieldOffsetDiscountLabel2:
		{
			result = scaleConfig->toStringCfgI(CfgInternal::PRODUCTDISCOUNTLEVEL2LABEL);
			break;
		}

		case fieldOffsetDiscountLabel3:
		{
			result = scaleConfig->toStringCfgI(CfgInternal::PRODUCTDISCOUNTLEVEL3LABEL);
			break;
		}

		case fieldOffsetTareAValue:
			//Debug::trace("Product.formatTare", "begin getFieldAsString Tare");
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
				{
					//Debug::trace("Product.formatTare", "getFieldAsString Tare EvolutionTaggedFormat");
					if (tareA.isForced())
						result = "-1";
					else
					{
						// convert from assumed three decimal places in database
						if (2 == scaleConfig->getExternalWeightPrecision())
						{
							double dTare = MetSig::divideWeightForSingleRangeD( tareA.getValue() );
							long lTare = Rounding::hobround(dTare);
							result = Long::toString(lTare);
						}
						else
							result = Long::toString(tareA.getValue());
					}
					break;
				}

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					//Debug::trace("Product.formatTare", "getFieldAsString Tare BrowserFormatPLUOperateMode");
					if ((tareA.isForced() && !tareEntered))
						result = "";
					else
					{
						UINT32 scaleFactor = 0;
						if (format == BrowserFormat)
							scaleFactor = scaleConfig->getWeigherScaleFactor();
						else
						{
							if (currentProductTransaction_)
							{
								//Debug::trace("Product.formatTare", "getFieldAsString Tare have a current trans");
								const HfcWeigherStatus *currentWeigherStatus = currentProductTransaction_->getWeigherStatus();
								if (currentWeigherStatus)
								{
									scaleFactor = currentWeigherStatus->getEffectiveScaleFactor();
									//Debug::trace("Product.formatTare", string("getFieldAsString Tare have a currentWeigherStatus: scaleFactor:") + inttostring(scaleFactor));
								}
								//Debug::trace("Product.formatTare", string("getFieldAsString Tare have a current trans scaleFactor:") + inttostring(scaleFactor));
							}
							if (scaleFactor == 0)
							{
								//Debug::trace("Product.formatTare", "getFieldAsString Tare no scale factor yet");
								// I need the scaleFactor, and I know the Primary Console has it once it has received any Weigher Status.
								scaleFactor = (format == BrowserFormatPLUOperateMode) ? scaleConfig->getEffectiveWeigherScaleFactor() : scaleConfig->getWeigherScaleFactor();
							}
						}
						if (scaleFactor != 0)
						{
							//Debug::trace("Product.formatTare", string("getFieldAsString Tare scaleFactor is not 0 scaleFactor:") + inttostring(scaleFactor));

							HobWeight tareWeight = tareA.getValue();
							tareWeight = MetSig::applyScaleFactorToWeight(tareWeight, scaleFactor);

							if ( scaleFactor == 1 )
							{
								scaleConfig->formatWeight(tareWeight, result, true, DualRangeModeDown);
								//Debug::trace("Product.formatTare", string("BrowserFormatPLUOperateMode scalefactor 1 ") + result);
							}
							else
							{
								scaleConfig->formatWeight(tareWeight, result);
								//Debug::trace("Product.formatTare", string("BrowserFormatPLUOperateMode") + result);
							}
						}
						else
						{
							// thought I ought to do something if there is no scale factor determined.
							scaleConfig->formatWeight(tareA.getValue(), result);
						}
					}
					break;

				case SynchronizationTaggedFormat:
				default:
					scaleConfig->formatWeight(tareA.getValue(), result, false);
					break;
			}
			break;

		case fieldOffsetTareBValue:
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
				{
					//Debug::trace("Product.formatTare", "getFieldAsString Tare EvolutionTaggedFormat");
					if (tareA.isForced())	// HT-309 - The 'forced' setting for Tare A is also used for Tare B
						result = "-1";
					else
					{
						// convert from assumed three decimal places in database
						if (2 == scaleConfig->getExternalWeightPrecision())
						{
							double dTare = MetSig::divideWeightForSingleRangeD( tareB.getValue() );
							long lTare = Rounding::hobround(dTare);
							result = Long::toString(lTare);
						}
						else
							result = Long::toString(tareB.getValue());
					}
					break;
				}

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					//Debug::trace("Product.formatTare", "getFieldAsString Tare BrowserFormatPLUOperateMode");
					if ((tareA.isForced() && !tareEntered))	// HT-309 - The 'forced' setting for Tare A is also used for Tare B
						result = "";
					else
					{
						UINT32 scaleFactor = 0;
						if (format == BrowserFormat)
							scaleFactor = scaleConfig->getWeigherScaleFactor();
						else
						{
							if (currentProductTransaction_)
							{
								//Debug::trace("Product.formatTare", "getFieldAsString Tare have a current trans");
								const HfcWeigherStatus *currentWeigherStatus = currentProductTransaction_->getWeigherStatus();
								if (currentWeigherStatus)
								{
									scaleFactor = currentWeigherStatus->getEffectiveScaleFactor();
									//Debug::trace("Product.formatTare", string("getFieldAsString Tare have a currentWeigherStatus: scaleFactor:") + inttostring(scaleFactor));
								}
								//Debug::trace("Product.formatTare", string("getFieldAsString Tare have a current trans scaleFactor:") + inttostring(scaleFactor));
							}
							if (scaleFactor == 0)
							{
								//Debug::trace("Product.formatTare", "getFieldAsString Tare no scale factor yet");
								// I need the scaleFactor, and I know the Primary Console has it once it has received any Weigher Status.
								scaleFactor = (format == BrowserFormatPLUOperateMode) ? scaleConfig->getEffectiveWeigherScaleFactor() : scaleConfig->getWeigherScaleFactor();
							}
						}
						if (scaleFactor != 0)
						{
							//Debug::trace("Product.formatTare", string("getFieldAsString Tare scaleFactor is not 0 scaleFactor:") + inttostring(scaleFactor));

							HobWeight tareWeight = tareB.getValue();
							tareWeight = MetSig::applyScaleFactorToWeight(tareWeight, scaleFactor);

							if ( scaleFactor == 1 )
							{
								scaleConfig->formatWeight(tareWeight, result, true, DualRangeModeDown);
								//Debug::trace("Product.formatTare", string("BrowserFormatPLUOperateMode scalefactor 1 ") + result);
							}
							else
							{
								scaleConfig->formatWeight(tareWeight, result);
								//Debug::trace("Product.formatTare", string("BrowserFormatPLUOperateMode") + result);
							}
						}
						else
						{
							// thought I ought to do something if there is no scale factor determined.
							scaleConfig->formatWeight(tareB.getValue(), result);
						}
					}
					break;

				case SynchronizationTaggedFormat:
				default:
					scaleConfig->formatWeight(tareB.getValue(), result, false);
					break;
			}
			break;

		case fieldOffsetTareAForced:	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
			if (format == BrowserFormatPLUOperateMode)
				result = Bool::toString(tareA.isForced() && !tareEntered);
			else
				result = Bool::toString(tareA.isForced());
			break;

		case fieldOffsetProportionalTare:
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					result = Integer::toString(proportionalTare);
					break;

				default:
				{
					bool useDefaultDecimalSeparator = false;
					if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
						useDefaultDecimalSeparator = true;

					scaleConfig->formatValue(proportionalTare, result, PROP_TARE_DEC_PRECISION, useDefaultDecimalSeparator);
					break;
				}
			}
			break;

		case fieldOffsetFixedWeight:
			switch (format)
			{
				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
				case SynchronizationTaggedFormat:
				case DBIndependentFormatFormat:
				{
					bool useDefaultDecimalSeparator = false;
					if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
						useDefaultDecimalSeparator = true;

					//	This came from vxWorks Access because the browser was calculating the fixed weight to display.
					if (format == BrowserFormatPLUOperateMode)
					{
						if (isFluidOunce())
							scaleConfig->formatVolume(fixedWeight, result, OperateFixedWeightText, useDefaultDecimalSeparator);
						else
							scaleConfig->formatWeight(fixedWeight, result, useDefaultDecimalSeparator, OperateFixedWeightText);
					}
					else
					{
						if (scaleConfig->isFixedWeightInTenths())
							scaleConfig->formatValue(fixedWeight, result, 1, useDefaultDecimalSeparator);
						else
							result = Long::toString(fixedWeight);
					}
					break;
				}

				default:
					result = Long::toString(fixedWeight);
			}
			break;

		case fieldOffsetShelfLifeDays:
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					if (isDontPrintSellBy() && isDontPrintPackedOn())
						result = "998";
					else if (isDontPrintSellBy())
						result = "999";
					else
						result = Integer::toString(shelfLifeDays);
					break;
				default:
					result = Integer::toString(shelfLifeDays);
					break;
			}
			break;

		case fieldOffsetShelfLifeHours:
			result = Integer::toString(shelfLifeHours);
			break;

		case fieldOffsetShelfLifeMinutes:
			result = Integer::toString(shelfLifeMinutes);
			break;

		case fieldOffsetProductLifeDays:
			switch (format)
			{
			case UltimaTaggedFormat:
			case EvolutionTaggedFormat:
				if (isDontPrintBestBefore())
					result = "999";
				else
					result = Integer::toString(productLifeDays);
				break;
			default:
				result = Integer::toString(productLifeDays);
				break;
			}
			break;

		case fieldOffsetProductLifeHours:
			result = Integer::toString(productLifeHours);
			break;

		case fieldOffsetProductLifeMinutes:
			result = Integer::toString(productLifeMinutes);
			break;

		case fieldOffsetPriceModifier:
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
				{
					if ((priceModifier >= 1) && (priceModifier <= 9))
						result = Integer::toString(priceModifier) + HobString("1");
					else if (priceModifier == scaleConfig->getQuarterPoundFor())
						result = HobString("14");
					else if (priceModifier == scaleConfig->getHalfPoundFor())
						result = HobString("12");
					else if (priceModifier == scaleConfig->getHundredGramsFor())
						result = HobString("1:");
					else
						result = HobString("11");
					break;
				}

				default:
					result = Integer::toString(priceModifier);
					break;
			}
			break;

		case fieldOffsetProductReportingClassNum:		// REPORTING CLASS NUM
			result = Long::toString(getReportingClassNum());
			break;

		case fieldOffsetLabelTypeNum1:		// PRIMARY LABEL TYPE
		{
			LabelType *primaryLabelType = getLabelType(FirstPrinter);
			result = (primaryLabelType) ? Long::toString(primaryLabelType->getLabelTypeNum()) : "0";
			break;
		}
		case fieldOffsetLabelTypeNum2:		// SECONDARY LABEL TYPE
		{
			LabelType *secondLabelType = getLabelType(SecondPrinter);
			result = (secondLabelType) ? Long::toString(secondLabelType->getLabelTypeNum()) : "0";
			break;
		}
		case fieldOffsetLabelTypeNum3:		// THIRD LABEL TYPE
		{
			LabelType *thirdLabelType = getLabelType(ThirdPrinter);
			result = (thirdLabelType) ? Long::toString(thirdLabelType->getLabelTypeNum()) : "0";
			break;
		}

		case fieldOffsetDefaultLabelTypeNum1:
			result = Long::toString(defaultLabelTypeNum1);
			break;
		case fieldOffsetDefaultLabelTypeNum2:
			result = Long::toString(defaultLabelTypeNum2);
			break;
		case fieldOffsetDefaultLabelTypeNum3:
			result = Long::toString(defaultLabelTypeNum3);
			break;

		case fieldOffsetManualLabelTypeNum1:
			result = Long::toString(manualLabelTypeNum1);
			break;
		case fieldOffsetManualLabelTypeNum2:
			result = Long::toString(manualLabelTypeNum2);
			break;
		case fieldOffsetManualLabelTypeNum3:
			result = Long::toString(manualLabelTypeNum3);
			break;

		case fieldOffsetPrepackLabelTypeNum1:
			result = Long::toString(prepackLabelTypeNum1);
			break;
		case fieldOffsetPrepackLabelTypeNum2:
			result = Long::toString(prepackLabelTypeNum2);
			break;
		case fieldOffsetPrepackLabelTypeNum3:
			result = Long::toString(prepackLabelTypeNum3);
			break;

		case fieldOffsetProdEntryLabelTypeNum1:
			result = Long::toString(prodEntryLabelTypeNum1);
			break;
		case fieldOffsetProdEntryLabelTypeNum2:
			result = Long::toString(prodEntryLabelTypeNum2);
			break;
		case fieldOffsetProdEntryLabelTypeNum3:
			result = Long::toString(prodEntryLabelTypeNum3);
			break;

		case fieldOffsetSelfServLabelTypeNum1:
			result = Long::toString(selfServLabelTypeNum1);
			break;
		case fieldOffsetSelfServLabelTypeNum2:
			result = Long::toString(selfServLabelTypeNum2);
			break;
		case fieldOffsetSelfServLabelTypeNum3:
			result = Long::toString(selfServLabelTypeNum3);
			break;
		
		case fieldOffsetPick5LabelTypeNum1:
			result = Long::toString(pick5LabelTypeNum1);
			break;
		case fieldOffsetPick5LabelTypeNum2:
			result = Long::toString(pick5LabelTypeNum2);
			break;
		case fieldOffsetPick5LabelTypeNum3:
			result = Long::toString(pick5LabelTypeNum3);
			break;

		case fieldOffsetOnlineOrderingLabelTypeNum1:
			result = Long::toString(onlineOrderingLabelTypeNum1);
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum2:
			result = Long::toString(onlineOrderingLabelTypeNum2);
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum3:
			result = Long::toString(onlineOrderingLabelTypeNum3);
			break;

		case fieldOffsetLabelRotation1:	// PRIMARY LABEL ROTATION
			result = Integer::toString(labelRotations[FirstPrinter]);
			break;

		case fieldOffsetLabelRotation2:	// SECONDARY LABEL ROTATION
			result = Integer::toString(labelRotations[SecondPrinter]);
			break;

		case fieldOffsetLabelRotation3:	// THIRD LABEL ROTATION
			result = Integer::toString(labelRotations[ThirdPrinter]);
			break;

		case fieldOffsetLabelPlacement1:
			result = Integer::toString(labelPlacements[FirstPrinter]);
			break;

		case fieldOffsetLabelPlacement2:
			result = Integer::toString(labelPlacements[SecondPrinter]);
			break;

		case fieldOffsetLabelPlacement3:
			result = Integer::toString(labelPlacements[ThirdPrinter]);
			break;

		case fieldOffsetPoints:
			result = Integer::toString(points);
			break;

		case fieldOffsetIntakeMode:
			result = Integer::toString(intakeMode);
			break;

		case fieldOffsetGraphicNum:	// Graphic Num - g#
		{
			Graphic *gr = getGraphic(0);
			if (!gr || gr->getGraphicName().size() == 0 || gr->getGraphicName() == "0")
				result = "0";
			else if (atoi(gr->getGraphicName().c_str()) != 0)
				result = Integer::toString(atoi(gr->getGraphicName().c_str()));
			break;
		}

		case fieldOffsetGraphicName1:	// Graphic 1 NAME
		case fieldOffsetGraphicName2:	// Graphic 2 NAME
		case fieldOffsetGraphicName3:	// Graphic 3 NAME
		case fieldOffsetGraphicName4:	// Graphic 4 NAME
		{
			bool traceOn = Debug::isTracing("Product.Graphic");
			HobString msgDebug("getFieldAsString():");
			unsigned int graphicIndex = (fieldOffset - fieldOffsetGraphicName1);   // in range of 0 to 3

			Graphic *gr = getGraphic(graphicIndex);
			if (gr)
			{
				result = gr->getGraphicName();
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append(" graphicName = ").append(result);
					Debug::trace("Product.Graphic", msgDebug2);
				}
				if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
				{
					Filename file1(result);
					if (!result.empty() && file1.getFileNameExtension().empty())
						result.append(".png");
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append(" NOW graphicName = ").append(result);
						Debug::trace("Product.Graphic", msgDebug2);
					}
				}
			}
			else if (format == BrowserFormat)
				result = scaleConfig->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME);

			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append(" Exiting, graphicName = ").append(result);
				Debug::trace("Product.Graphic", msgDebug2);
			}
			break;
		}

		case fieldOffsetLabelTypeWidth1:
		{
			// TODO - Larry - done - This may not always be true because the defaults may not have a value in it, but another label type like Manual or Self Service might. 
			/* This is OK to leave this code as it was before the 'label types by mode' project, since the Label Type objects will actually be the ones that will be printing, the ones that are active for the current mode. */
			LabelType *primaryLabelType = getLabelType(FirstPrinter);
			result = (primaryLabelType && SessionManager::getInstance()->isPrinterAttached(FirstPrinter)) ? util_string::doubletostring(primaryLabelType->getWidthInUnits(MetSig::isScaleMetric()), 1, 3) : "0";
			break;
		}

		case fieldOffsetLabelTypeLength1:
		{
			// TODO - Larry - done - This may not always be true because the defaults may not have a value in it, but another label type like Manual or Self Service might.
			/* This is OK to leave this code as it was before the 'label types by mode' project, since the Label Type objects will actually be the ones that will be printing, the ones that are active for the current mode. */
			LabelType *primaryLabelType = getLabelType(FirstPrinter);
			result = (primaryLabelType && SessionManager::getInstance()->isPrinterAttached(FirstPrinter)) ? util_string::doubletostring(primaryLabelType->getLengthInUnits(MetSig::isScaleMetric()), 1, 3) : "0";
			break;
		}

		case fieldOffsetLabelTypeWidth2:
		{
			LabelType *secondLabelType = getLabelType(SecondPrinter);
			result = (secondLabelType && SessionManager::getInstance()->isPrinterAttached(SecondPrinter)) ? util_string::doubletostring(secondLabelType->getWidthInUnits(MetSig::isScaleMetric()), 1, 3) : "0";
			break;
		}

		case fieldOffsetLabelTypeLength2:
		{
			LabelType *secondLabelType = getLabelType(SecondPrinter);
			result = (secondLabelType && SessionManager::getInstance()->isPrinterAttached(SecondPrinter)) ? util_string::doubletostring(secondLabelType->getLengthInUnits(MetSig::isScaleMetric()), 1, 3) : "0";
			break;
		}

		case fieldOffsetLabelTypeWidth3:
		{
			LabelType *thirdLabelType = getLabelType(ThirdPrinter);
			result = (thirdLabelType && SessionManager::getInstance()->isPrinterAttached(ThirdPrinter)) ? util_string::doubletostring(thirdLabelType->getWidthInUnits(MetSig::isScaleMetric()), 1, 3) : "0";
			break;
		}

		case fieldOffsetLabelTypeLength3:
		{
			LabelType *thirdLabelType = getLabelType(ThirdPrinter);
			result = (thirdLabelType && SessionManager::getInstance()->isPrinterAttached(ThirdPrinter)) ? util_string::doubletostring(thirdLabelType->getLengthInUnits(MetSig::isScaleMetric()), 1, 3) : "0";
			break;
		}

		case fieldOffsetFractionalPriceUnit:
		{
			if (scaleConfig->isFractionalPricingEnabled())
			{
				if (priceModifier == scaleConfig->getQuarterPoundFor() || priceModifier == scaleConfig->getHalfPoundFor() || priceModifier == scaleConfig->getHundredGramsFor())
					result = Integer::toString(priceModifier);
				else
					result = Integer::toString(UNIT_PRICING);
			}
			else
				result = Integer::toString(UNIT_PRICING);
			break;
		}

		case fieldOffsetPoundsForAmount:
			if (priceModifier >= 1 && priceModifier <= 9)
				result = Integer::toString(priceModifier);
			else
				result = Integer::toString(UNIT_PRICING);
			break;

		case fieldOffsetTareType:
			result = "0";
			break;

		case fieldOffsetApplySecurity:
			result = Bool::toString(applySecurityLabel);
			break;

		case fieldOffsetDontPrintPackedOn:
			result = Bool::toString(dontPrintPackedOn);
			break;

		case fieldOffsetDontPrintSellBy:
			result = Bool::toString(dontPrintSellBy);
			break;

		case fieldOffsetDontPrintBestBefore:
			result = Bool::toString(dontPrintBestBefore);
			break;

		case fieldOffsetProductGraphicName:
		{
			result = productGraphicName;
			if (format == BrowserFormat || format == BrowserFormatPLUOperateMode)
			{
				if (!result.empty())
				{
					Filename file1(result);
					if (file1.getFileNameExtension().empty())
						file1.setFileNameExtension(scaleConfig->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDEFAULTEXTENSION));
					result = file1.getFullPath();
				}
			}
			Debug::trace("Product.Graphic", HobString("getFieldAsString():  productGraphicName = '") + result + HobString("'"));
			break;
		}

		case fieldOffsetProductName1:
			result = productName1;
			break;

		case fieldOffsetProductName2:
			result = productName2;
			break;

		case fieldOffsetProdCategoryNum:
			if (getCategoryNum() != RECORD_NOT_FOUND)
				result = Long::toString(getCategoryNum());
			break;

		case fieldOffsetProdSpecialNum:
			if (getSpecialNum() != RECORD_NOT_FOUND)
				result = Long::toString(getSpecialNum());
			break;

		case fieldOffsetSpecialStartDate:
			if (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat || format == SynchronizationTaggedFormat)
				result = specialStartDate.convertToDBFormat(FormatDateTime_Windux);
			else
				result = specialStartDate.formatDate(scaleConfig->getDateDisplayFormat());
			break;

		case fieldOffsetFractionalPrice:
			result = "0";
			break;

		case fieldOffsetForcedFractionalPrice:
			result = "0";
			break;

		case fieldOffsetPoundsForPrice:
			result = "0";
			break;

		case fieldOffsetForcedPoundsFor:
			result = "0";
			break;

		case fieldOffsetHasNutrifactsFlag:
			result = Bool::toString(hasNutrifacts());
			break;

		case fieldOffsetPluFlags:	//added ats 10152002
		case fieldOffsetTotalsFlags:
			result = Integer::toString(flags);
			break;

		case fieldOffsetSellByDate:
		{
			setShelfLife();
			result = shelfLife.formatDate(scaleConfig->getDateDisplayFormat());
			result.removeTrailingNewlinesAndSpaces();
			break;
		}

		case fieldOffsetSellByTime:
		{
			setShelfLife();
			result = shelfLife.formatTime(scaleConfig->getTimeDisplayFormat());
			break;
		}

		case fieldOffsetBestBeforeDate:
		{
			setProductLife();
			result = productLife.formatDate(scaleConfig->getDateDisplayFormat());
			result.removeTrailingNewlinesAndSpaces();
			break;
		}

		case fieldOffsetBestBeforeTime:
		{
			setProductLife();
			result = productLife.formatTime(scaleConfig->getTimeDisplayFormat());
			break;
		}

		case fieldOffsetProdNutrifactNum:
		{
			if (nutrifacts)
			{
				if (nutrifacts->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(nutrifacts->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isNutritionNull())
					result = myDbRecord->getNutrition();
			}
			break;
		}

		case fieldOffsetExpandedTextNum:
		{
			if (productTexts_[psttExpandedText])
			{
				if (productTexts_[psttExpandedText]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttExpandedText]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isTextNull())
					result = myDbRecord->getText();
			}
			break;
		}

		case fieldOffsetSpecialMessageNum:
		{
			if (productTexts_[psttSpecialMessage])
			{
				if (productTexts_[psttSpecialMessage]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttSpecialMessage]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isSpecialMessageNull())
					result = myDbRecord->getSpecialMessage();
			}
			break;
		}

		case fieldOffsetProdProductNoteNum:
		{
			if (productTexts_[psttProductNote])
			{
				if (productTexts_[psttProductNote]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttProductNote]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isNoteNull())
					result = myDbRecord->getNote();
			}
			break;
		}

		case fieldOffsetMarqueeTextNum:
		{
			if (productTexts_[psttMarqueeText])
			{
				if (productTexts_[psttMarqueeText]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttMarqueeText]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isMarqueeNull())
					result = myDbRecord->getMarquee();
			}
			break;
		}

		case fieldOffsetProductText5Num:
		{
			if (productTexts_[psttProductText5])
			{
				if (productTexts_[psttProductText5]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttProductText5]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isProductText5Null())
					result = myDbRecord->getProductText5();
			}
			break;
		}

		case fieldOffsetProductText6Num:
		{
			if (productTexts_[psttProductText6])
			{
				if (productTexts_[psttProductText6]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttProductText6]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isProductText6Null())
					result = myDbRecord->getProductText6();
			}
			break;
		}

		case fieldOffsetProductText7Num:
		{
			if (productTexts_[psttProductText7])
			{
				if (productTexts_[psttProductText7]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttProductText7]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isProductText7Null())
					result = myDbRecord->getProductText7();
			}
			break;
		}

		case fieldOffsetProductText8Num:
		{
			if (productTexts_[psttProductText8])
			{
				if (productTexts_[psttProductText8]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttProductText8]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isProductText8Null())
					result = myDbRecord->getProductText8();
			}
			break;
		}

		case fieldOffsetProductText9Num:
		{
			if (productTexts_[psttProductText9])
			{
				if (productTexts_[psttProductText9]->getTextNum() != RECORD_NOT_FOUND)
					result = Long::toString(productTexts_[psttProductText9]->getTextNum());
			}
			else if ((format == EvolutionTaggedFormat) || (format == UltimaTaggedFormat))
			{
				if (myDbRecord && !myDbRecord->isProductText9Null())
					result = myDbRecord->getProductText9();
			}
			break;
		}

		/*
		case fieldOffsetProductErrorCode:
		result = Long::toString((long)errorCode);
		break;
		*/

		case fieldOffsetLabelHasFreqShopper:
			if (format == BrowserFormatPLUOperateMode)   // probably not BrowserFormat too
			{
				if ((discountValue1 == 0 && discountValue2 == 0 && discountValue3 == 0) &&
					/* HT-155 */
					((!scaleConfig->toBoolCfg(fieldOffsetCfgEnableGlobalPercentOff))))
					result = Bool::toString(false);
				else
					result = Bool::toString(labelHasFrequentShopper);
			}
			break;

		case fieldOffsetExceptionPriceTare:
			if (isFixedWtOrFluidOz())
				;
			else if (isRandomWeight())
			{
				if (tareA.isForced())
					result = "65535";
				else
				{
					switch (format)
					{
						case UltimaTaggedFormat:
						case EvolutionTaggedFormat:
							if (2 == scaleConfig->getExternalWeightPrecision())
							{
								double dTare = MetSig::divideWeightForSingleRangeD(tareA.getValue());
								long lTare = Rounding::hobround(dTare);
								result = Long::toString(lTare);
							}
							else
								result = Long::toString(tareA.getValue());
							break;

						case BrowserFormat:
						case BrowserFormatPLUOperateMode:
							scaleConfig->formatWeight(tareA.getValue(), result);
							break;

						case SynchronizationTaggedFormat:
						default:
							scaleConfig->formatWeight(tareA.getValue(), result, false);
					}
				}
			}
			else // is by count - use the exception price
			{
				switch (format)
				{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					result = Long::toString(exceptionPrice);
					break;

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					result = scaleConfig->formatHobPrice(exceptionPrice);
					break;

				default:
					result = scaleConfig->formatHobPrice(exceptionPrice, false);
					break;
				}
			}
			break;

		case fieldOffsetProdCoolCategoryNum:
		{
			if (coolCategoryNum >= 0)
				result = Long::toString(coolCategoryNum);
			break;
		}

		case fieldOffsetProdCoolPretextNum:
		{
			if (coolPretextNum > 0)
				result = Long::toString(coolPretextNum);
			break;
		}

		case fieldOffsetProdCoolDefaultTextNum:
		{
			if (coolDefaultTextNum > 0)
				result = Long::toString(coolDefaultTextNum);
			break;
		}

		case fieldOffsetProdCoolTextNum:
		{
			if (coolTextNum > 0)
				result = Long::toString(coolTextNum);
			break;
		}

		case fieldOffsetProdCoolShortlistNum:
		{
			if (coolShortlistNum > 0)
				result = Long::toString(coolShortlistNum);
			break;
		}

		case fieldOffsetProdCoolShortlistFirstLink:
		{
			if (coolShortlistFirstLink > 0)
				result = Long::toString(coolShortlistFirstLink);
			break;
		}

		case fieldOffsetProdCOOLTrackingText:
			result = coolTrackingText;
			break;

		case fieldOffsetProdCoolVerifyForced:
			result = Bool::toString(coolVerifyForced);
			break;

		case fieldOffsetBackDateDays:
			if (backDateDaysForced && !backDateDaysEntered)
				result = " ";
			else
				result = Integer::toString(backDateDays);
			break;

		case fieldOffsetForcedBackDateDays:
			if (format == BrowserFormatPLUOperateMode)
				result = Bool::toString(backDateDaysForced && !backDateDaysEntered);
			else
				result = Bool::toString(backDateDaysForced);
			break;

		case fieldOffsetBackDateDate:
		{
			setBackdateDate();
			result = backdateDate.formatDate(scaleConfig->getDateDisplayFormat());
			result.removeTrailingNewlinesAndSpaces();
			break;
		}

		case fieldOffsetProdCoolRequired:
			result = Bool::toString(coolRequired);
			break;

		case fieldOffsetNumOfLabelsForced:
			result = Bool::toString(numOfLabelsForced && !numOfLabelsEntered);
			break;

		case fieldOffsetNumOfLabels:
			if (currentProductTransaction_)
			{
				if (format == BrowserFormatPLUOperateMode)
				{
					if (numOfLabelsForced && !numOfLabelsEntered)
						result = "";
					else
						result = Integer::toString(currentProductTransaction_->getNumOfLabelsToPrint());
				}
				else
					result = Integer::toString(currentProductTransaction_->getNumOfLabelsToPrint());
			}
			break;

		case fieldOffsetProductionPlanningType:
			result = Integer::toString(productionPlanningType);
			break;

		case fieldOffsetKeyboardNetWeight:
			if (hasKbdNetWeightLabel() && (keyboardNetWeight > -1))
			{
				if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
					scaleConfig->formatWeight(keyboardNetWeight, result, true, KeyboardNetWeight);
				else
					scaleConfig->formatWeight(keyboardNetWeight, result, false, KeyboardNetWeight);
			}
			break;

		case fieldOffsetAcceptKbdNetWeight:
			result = Bool::toString(hasKbdNetWeightLabel());
			break;

		case fieldOffsetExpirationDaysForced:
			if (format == BrowserFormatPLUOperateMode)
				result = Bool::toString(expirationDaysForced && !expirationDaysEntered);
			break;

		case fieldOffsetExpirationDays:
			if (format == BrowserFormatPLUOperateMode)
				result = (expirationDaysForced && !expirationDaysEntered) ? " " : Integer::toString(expirationDays);
			break;

		case fieldOffsetPortions:
			result = Integer::toString(portions);
			break;

		case fieldOffsetShowPortions:
			result = Bool::toString(portionsOnLabel);
			break;

		case fieldOffsetBuildAMealSideItemsGroupNum:
			result = Long::toString(mealSideItemsGroupNum);
			break;

		case fieldOffsetBuildAMealSideItemsAreFree:
			result = Bool::toString(mealSideItemsAreFree);
			break;

		case fieldOffsetBuildAMealNumberOfSidesIncluded:
			result = Integer::toString(mealNumSidesIncluded);
			break;

		case fieldOffsetCutTestMeatSpecSheetGroupNum:
			result = Long::toString(cutTestMeatSpecSheetGroupNum);
			break;

		// For Pick-5 project

		case fieldOffsetWeightMin:
			//Debug::trace("Product", "begin getFieldAsString WeightMin");
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
				{
					// convert from assumed three decimal places in database
					if (2 == scaleConfig->getExternalWeightPrecision())
					{
						double dMin = MetSig::divideWeightForSingleRangeD(weightMin);
						long lMin = Rounding::hobround(dMin);
						result = Long::toString(lMin);
					}
					else
						result = Long::toString(weightMin);
					break;
				}

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					scaleConfig->formatWeight(weightMin, result);
					break;

				case SynchronizationTaggedFormat:
				default:
					result = Long::toString(weightMin);
					break;
			}
			break;

		case fieldOffsetWeightMax:
			result = Long::toString(weightMax);
			//Debug::trace("Product", "begin getFieldAsString WeightMax");
			switch (format)
			{
			case UltimaTaggedFormat:
			case EvolutionTaggedFormat:
			{
				// convert from assumed three decimal places in database
				if (2 == scaleConfig->getExternalWeightPrecision())
				{
					double dMax = MetSig::divideWeightForSingleRangeD(weightMax);
					long lMax = Rounding::hobround(dMax);
					result = Long::toString(lMax);
				}
				else
					result = Long::toString(weightMax);
				break;
			}

			case BrowserFormat:
			case BrowserFormatPLUOperateMode:
				scaleConfig->formatWeight(weightMax, result);
				break;

			case SynchronizationTaggedFormat:
			default:
				result = Long::toString(weightMax);
				break;
			}
			break;

		case fieldOffsetTotalPriceMin:
		{
			switch (format)
			{
				case UltimaTaggedFormat:
				case EvolutionTaggedFormat:
					result = Long::toString(totalPriceMin);
					break;

				case BrowserFormat:
				case BrowserFormatPLUOperateMode:
					result = scaleConfig->formatHobPrice(totalPriceMin);
					break;

				default:
					result = scaleConfig->formatHobPrice(totalPriceMin, false);
			}
			break;
		}

		case fieldOffsetTotalPriceMax:
		{
			switch (format)
			{
			case UltimaTaggedFormat:
			case EvolutionTaggedFormat:
				result = Long::toString(totalPriceMax);
				break;

			case BrowserFormat:
			case BrowserFormatPLUOperateMode:
				result = scaleConfig->formatHobPrice(totalPriceMax);
				break;

			default:
				result = scaleConfig->formatHobPrice(totalPriceMax, false);
			}
			break;
		}

		case fieldOffsetLabelStockRotation1:
		{
			LabelType *lt = getLabelType(FirstPrinter);
			result = inttostring((lt) ? lt->getRotationOnLabelStock() : 0);
			break;
		}
		case fieldOffsetLabelStockRotation2:
		{
			LabelType *lt = getLabelType(SecondPrinter);
			result = inttostring((lt) ? lt->getRotationOnLabelStock() : 0);
			break;
		}
		case fieldOffsetLabelStockRotation3:
		{
			LabelType *lt = getLabelType(ThirdPrinter);
			result = inttostring((lt) ? lt->getRotationOnLabelStock() : 0);
			break;
		}

		default:
			break;
	}

	if (Debug::isTracing("Product.getFieldAsString"))
		Debug::trace("Product.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *Product::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<ProductFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		case BrowserFormatPLUOperateMode:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = uiFieldList.begin();
			while (fieldIter != uiFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, format);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case SynchronizationTaggedFormat:
		{
			ProductTags productTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = productTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		case UltimaTaggedFormat:
		{
			ProductTags productTags;
			HobString tag = "";
			HobString fieldData = "";

			createUltimaFlag();

			fieldIter = ultimaFieldList.begin();
			while (fieldIter != ultimaFieldList.end())
			{
				tag = productTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, format);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		case DBIndependentFormatFormat:
		{
			ProductTags productTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = dbindependentFieldList.begin();
			while (fieldIter != dbindependentFieldList.end())
			{
				tag = productTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, format);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = string("Product::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBProduct *Product::getMyDbRecord() const
{
	return myDbRecord;
}

void Product::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool Product::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Product::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)   //No need to delete if there is no record
	{
		DBProductsRM *dbRM = dbAccess->getDBProductsRM();
		if (dbRM)
		{
			Debug::trace("Product", string("  Checking text records used only by this Product (# ") + longtostring(productNum));

			if (productTexts_.size() >= psttLastProductText)
				for (int i = psttUnknown + 1; i < psttLastProductText; i++)
				{
					if (productTexts_[i])
					{
						productTexts_[i]->testIfUsedByMany();
						if (productTexts_[i]->isUsedByMany() == false)
						{
							productTexts_[i]->deleteRecord();
							Debug::trace("Product", "  Deleting Product text type:" + inttostring(i) + "  num:" + longtostring(productTexts_[i]->getTextNum()));
						}
					}
				}

			Debug::trace("Product", string("deleteRecord() - num ") + longtostring(productNum) + "  deptNum " + longtostring(deptNum));
			Debug::trace("Product", HobString("Deleting # ") + HobString(Long::toString(productNum)));
			result = dbRM->deleteRecord(myDbRecord);

			//	todo - IMPORTANT: We need to delete totals for this product, but totals are not available for the database yet. (CCD 9/25/02)
		}
	}
	return result;
}

void Product::saveRecord()
{
	HobString msgDebug = "saveRecord() - ";
	bool isTracing = Debug::isTracing("Product.save");

	if (productNum > 0 && dbAccess)
	{
		DBProductsRM *dbRM = dbAccess->getDBProductsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			if (isTracing)
				Debug::trace("Product.save", msgDebug + string("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbProductRecord
			myDbRecord->setDeptNumber(Long::toString(deptNum));
			myDbRecord->setPLU(Long::toString(productNum));

			//add typeUnknownProduct() here
			typeUnknownProduct();

			myDbRecord->setType(productType);
			
			Debug::trace("Product.text", "description before removeSpacesBeforeNewlines(): [" + description + "]");
			description.removeSpacesBeforeNewlines();
			Debug::trace("Product.text", "description before removeTrailingNewlinesAndSpaces(): [" + description + "]");
			description.removeTrailingNewlinesAndSpaces();
			Debug::trace("Product.text", "Saving description as: [" + description + "]");

			myDbRecord->setDescription(description);

			HobRecordNum reportingClassNum = Class::GRAND_TOTALS_CLASS;
			if (reportingClass != NULL)
				reportingClassNum = reportingClass->getClassNum();
			myDbRecord->setReportingClass(Long::toString(reportingClassNum));

			if (barCodeNum.getPrefix().empty())
				myDbRecord->setBarCodePrefixNull();
			else
				myDbRecord->setBarCodePrefix(Integer::valueOf(barCodeNum.getPrefix()));

			if (barCodeNum.isProductNumberFromRaw())
			{
				myDbRecord->setBarCodeProductNumberNull();
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "Set barcode ProductNumber in db to Null.");
			}
			else
			{
				myDbRecord->setBarCodeProductNumber(Integer::valueOf(barCodeNum.getProductNumber()));
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "Set ProductNumber in db to " + barCodeNum.getProductNumber());
			}

			if (barCodeNum.isVendorNumberFromRaw())
			{
				myDbRecord->setBarCodeVendorNumberNull();
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "Set VendorNumber in db to Null.");
			}
			else
			{
				myDbRecord->setBarCodeVendorNumber(Integer::valueOf(barCodeNum.getVendorNumber()));
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "Set VendorNumber in db to " + barCodeNum.getVendorNumber());
			}

			if (barCodeNum.isEanCodeFromRaw())
			{
				myDbRecord->setBarCodeEANCodeNull();
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "Set EANCode in db to Null.");
			}
			else
			{
				// Allow 10 digits for EAN code. For HEB Mexico demo.
				myDbRecord->setBarCodeEANCode(barCodeNum.getEANCode());
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "Set EANCode in db to " + barCodeNum.getEANCode());
			}

			if (rawBarCodeData.empty())
			{
				myDbRecord->setRawBarCodeNumberNull();
				if (isTracing)
					Debug::trace("Product.BarCode", msgDebug + "Set rawBarCode in db to Null.");
			}
			else
			{
				myDbRecord->setRawBarCodeNumber(rawBarCodeData);
				if (isTracing)
					Debug::trace("Product.BarCode", msgDebug + "Set rawBarCode in db to: " + rawBarCodeData + " and ProductNumber, VendorNumber & EANCode to Null.");
			}

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "byCount =  " + inttostring(byCount));
			myDbRecord->setByCount((byCountForced) ? 1 : byCount);
			myDbRecord->setForceByCount(byCountForced);

			myDbRecord->setExceptionPrice(exceptionPrice);
			myDbRecord->setPrice((unitPriceForced) ? (HobPrice)0 : unitPrice);
			myDbRecord->setForcePrice(unitPriceForced);

			myDbRecord->setDiscountMethod(discountType);
			myDbRecord->setDiscountPrice1(discountValue1);
			myDbRecord->setDiscountPrice2(discountValue2);
			myDbRecord->setDiscountPrice3(discountValue3);

			//	fieldOffsetDiscountLabel1
			//	fieldOffsetDiscountLabel2
			//	fieldOffsetDiscountLabel3

			myDbRecord->setTare((tareA.isForced()) ? 0 : tareA.getValue());
			myDbRecord->setTareB((tareA.isForced()) ? 0 : tareB.getValue());	// HT-309 - The 'forced' setting for Tare A is also used for Tare B

			myDbRecord->setForceTare(tareA.isForced());		// HT-136 There is only one Forced Tare field, shared by TareA and TareB
			myDbRecord->setProportionalTare(proportionalTare);
			myDbRecord->setNetWeight(fixedWeight);

			myDbRecord->setShelfLifeDays(shelfLifeDays);
			myDbRecord->setShelfLifeHours(shelfLifeHours);
			myDbRecord->setShelfLifeMinutes(shelfLifeMinutes);

			myDbRecord->setProductLifeDays(productLifeDays);
			myDbRecord->setProductLifeHours(productLifeHours);
			myDbRecord->setProductLifeMinutes(productLifeMinutes);

			myDbRecord->setPriceFactor(priceModifier);

			Graphic *graphic;
			if((graphic = getGraphic(Graphic::GRAPHIC1)) != NULL)
			{
				if ( graphic->getGraphicName().length() > 30 )
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 1 name: ") + graphic->getGraphicName().substr(0, 30));
					myDbRecord->setGraphic1(graphic->getGraphicName().substr(0, 30));
				}
				else
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 1 name: ") + graphic->getGraphicName());
					myDbRecord->setGraphic1(graphic->getGraphicName());
				}
			}
			else
				myDbRecord->setGraphic1Null();

			if((graphic = getGraphic(Graphic::GRAPHIC2)) != NULL)
			{
				if ( graphic->getGraphicName().length() > 30 )
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 2 name: ") + graphic->getGraphicName().substr(0, 30));
					myDbRecord->setGraphic2(graphic->getGraphicName().substr(0, 30));
				}
				else
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 2 name: ") + graphic->getGraphicName());
					myDbRecord->setGraphic2(graphic->getGraphicName());
				}
			}
			else
				myDbRecord->setGraphic2Null();

			if((graphic = getGraphic(Graphic::GRAPHIC3)) != NULL)
			{
				if ( graphic->getGraphicName().length() > 30 )
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 3 name: ") + graphic->getGraphicName().substr(0, 30));
					myDbRecord->setGraphic3(graphic->getGraphicName().substr(0, 30));
				}
				else
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 3 name: ") + graphic->getGraphicName());
					myDbRecord->setGraphic3(graphic->getGraphicName());
				}
			}
			else
				myDbRecord->setGraphic3Null();

			if((graphic = getGraphic(Graphic::GRAPHIC4)) != NULL)
			{
				if ( graphic->getGraphicName().length() > 30 )
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 4 name: ") + graphic->getGraphicName().substr(0, 30));
					myDbRecord->setGraphic4(graphic->getGraphicName().substr(0, 30));
				}
				else
				{
					Debug::trace("Product.Graphic", msgDebug + HobString("saving graphic 4 name: ") + graphic->getGraphicName());
					myDbRecord->setGraphic4(graphic->getGraphicName());
				}
			}
			else
				myDbRecord->setGraphic4Null();

			myDbRecord->setLabel1(Long::toString(defaultLabelTypeNum1));
			myDbRecord->setLabel2(Long::toString(defaultLabelTypeNum2));
			myDbRecord->setLabel3(Long::toString(defaultLabelTypeNum3));
			myDbRecord->setManualLabelTypeNum1(Long::toString(manualLabelTypeNum1));
			myDbRecord->setManualLabelTypeNum2(Long::toString(manualLabelTypeNum2));
			myDbRecord->setManualLabelTypeNum3(Long::toString(manualLabelTypeNum3));
			myDbRecord->setPrepackLabelTypeNum1(Long::toString(prepackLabelTypeNum1));
			myDbRecord->setPrepackLabelTypeNum2(Long::toString(prepackLabelTypeNum2));
			myDbRecord->setPrepackLabelTypeNum3(Long::toString(prepackLabelTypeNum3));
			myDbRecord->setProdEntryLabelTypeNum1(Long::toString(prodEntryLabelTypeNum1));
			myDbRecord->setProdEntryLabelTypeNum2(Long::toString(prodEntryLabelTypeNum2));
			myDbRecord->setProdEntryLabelTypeNum3(Long::toString(prodEntryLabelTypeNum3));
			myDbRecord->setSelfServLabelTypeNum1(Long::toString(selfServLabelTypeNum1));
			myDbRecord->setSelfServLabelTypeNum2(Long::toString(selfServLabelTypeNum2));
			myDbRecord->setSelfServLabelTypeNum3(Long::toString(selfServLabelTypeNum3));
			myDbRecord->setPick5LabelTypeNum1(Long::toString(pick5LabelTypeNum1));
			myDbRecord->setPick5LabelTypeNum2(Long::toString(pick5LabelTypeNum2));
			myDbRecord->setPick5LabelTypeNum3(Long::toString(pick5LabelTypeNum3));
			myDbRecord->setOnlineOrderingLabelTypeNum1(Long::toString(onlineOrderingLabelTypeNum1));
			myDbRecord->setOnlineOrderingLabelTypeNum2(Long::toString(onlineOrderingLabelTypeNum2));
			myDbRecord->setOnlineOrderingLabelTypeNum3(Long::toString(onlineOrderingLabelTypeNum3));

			myDbRecord->setLabel1Rotation(getLabelRotation(FirstPrinter));
			myDbRecord->setLabel2Rotation(getLabelRotation(SecondPrinter));
			myDbRecord->setLabel3Rotation(getLabelRotation(ThirdPrinter));

			myDbRecord->setLabel1Placement(getLabelPlacement(FirstPrinter));
			myDbRecord->setLabel2Placement(getLabelPlacement(SecondPrinter));
			myDbRecord->setLabel3Placement(getLabelPlacement(ThirdPrinter));

			myDbRecord->setBonusPoints(points);
			myDbRecord->setPortions(portions);
			myDbRecord->setIntakeMode(intakeMode);

			// fieldOffsetUPCVendorNumber, embedded within the barcode
			// fieldOffsetUPCProductNumber, embedded within the barcode


			// fieldOffsetFractionalPriceUnit,
			// fieldOffsetPoundsForAmount,

			// fieldOffsetTareType,

			myDbRecord->setApplySecurityLabel(applySecurityLabel);

			myDbRecord->setDontPrintPackedOn(isDontPrintPackedOn());
			myDbRecord->setDontPrintShelfLife(isDontPrintSellBy());
			myDbRecord->setDontPrintProductLife(isDontPrintBestBefore());

			if (productGraphicName.empty())
				myDbRecord->setImageNull();
			else
				myDbRecord->setImage(productGraphicName);

			if (productName1.empty())
				myDbRecord->setName1Null();
			else
				myDbRecord->setName1(productName1);

			if (productName2.empty())
				myDbRecord->setName2Null();
			else
				myDbRecord->setName2(productName2);

			if (isTracing)
				Debug::trace("Product.save", msgDebug + HobString("category = ") + HobString(Hex::toString((long long)category)));
			if (category)
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + HobString("category is NOT null - categoryNum=") + HobString(Long::toString(category->getCategoryNum())));

				if (category->getCategoryNum() == RECORD_NOT_FOUND)
					myDbRecord->setCategoryNull();			//	No Category number
				else
				{
					myDbRecord->setCategory(Long::toString((long)category->getCategoryNum()));	//	Set the Category number

					//	Save the Category record to the Database.
					// This is now done by the ProductManager   category->saveRecord();
				}
			}
			else
				myDbRecord->setCategoryNull();			//	No Category number

			if (isTracing)
				Debug::trace("Product.save", msgDebug + HobString("specialProgram = ") + HobString(Hex::toString((long long)specialProgram)));
			if (specialProgram)
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + HobString("specialProgram is NOT null - specialNum=") + HobString(Long::toString(specialProgram->getSpecialNum())));

				if (specialProgram->getSpecialNum() == RECORD_NOT_FOUND)
					myDbRecord->setSpecialNumberNull();			//	No Special number
				else
				{
					myDbRecord->setSpecialNumber(Long::toString((long)specialProgram->getSpecialNum()));	//	Set the Special number

					//	Save the Specials record to the Database.
					// This is now done by the ProductManager   specialProgram->saveRecord();
				}
			}
			else
				myDbRecord->setSpecialNumberNull();			//	No Special number

			myDbRecord->setSpecialStartDate(specialStartDate.convertToDBFormat());

			// fieldOffsetFractionalPrice,
			// fieldOffsetForcedFractionalPrice,
			// fieldOffsetPoundsForPrice,
			// fieldOffsetForcedPoundsFor,

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "nutrifacts = " + Hex::toString((long long)nutrifacts));
			if (nutrifacts)
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "nutrifacts is NOT null - nutrifacts=" + longtostring(nutrifacts->getTextNum()));

				if (nutrifacts->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setNutritionNull();			//	No Nutrifacts number
				else
				{
					myDbRecord->setNutrition(Integer::toString(nutrifacts->getTextNum()));	//	Set the Nutrifacts number

					//	Save the Nutrifacts record to the Database.
					/*
					if (nutrifacts->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productNote to the database.");
						// This is now done by the ProductManager   nutrifacts->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setNutritionNull();			//	No Nutrifacts


			if (isTracing)
				Debug::trace("Product.save", msgDebug + "expandedText = " + ptrtostring(productTexts_[psttExpandedText]));
			if (productTexts_[psttExpandedText])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "expandedText is NOT null - expandedTextNum=" + longtostring(productTexts_[psttExpandedText]->getTextNum()));

				if (productTexts_[psttExpandedText]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setTextNull();			//	No Expanded text number
				else
				{
					myDbRecord->setText(Integer::toString(productTexts_[psttExpandedText]->getTextNum()));	//	Set the Expanded text number

					//	Save the Expanded text record to the Database.
					/*
					if (productTexts_[psttExpandedText]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the expandedText to the database.");
						// This is now done by the ProductManager   productTexts_[psttExpandedText]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setTextNull();			//	No Expanded text

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "specialMessage = " + ptrtostring(productTexts_[psttSpecialMessage]));
			if (productTexts_[psttSpecialMessage])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "specialMessage is NOT null - specialMessageNum=" + longtostring(productTexts_[psttSpecialMessage]->getTextNum()));

				if (productTexts_[psttSpecialMessage]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setSpecialMessageNull();			//	No Special message number
				else
				{
					myDbRecord->setSpecialMessage(Integer::toString(productTexts_[psttSpecialMessage]->getTextNum()));		//	Set the Special message number

					//	Save the Special message record to the Database.
					/*
					if (productTexts_[psttSpecialMessage]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the specialMessage to the database.");
						// This is now done by the ProductManager   productTexts_[psttSpecialMessage]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setSpecialMessageNull();			//	No Special message number

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "productNote = " + ptrtostring(productTexts_[psttProductNote]));
			if (productTexts_[psttProductNote])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "productNote is NOT null - productNote=" + longtostring(productTexts_[psttProductNote]->getTextNum()));

				if (productTexts_[psttProductNote]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setNoteNull();			//	No Product note number
				else
				{
					myDbRecord->setNote(Integer::toString(productTexts_[psttProductNote]->getTextNum()));	//	Set the Product note number

					//	Save the Product note record to the Database.
					/*
					if (productTexts_[psttProductNote]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productNote to the database.");
						// This is now done by the ProductManager   productTexts_[psttProductNote]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setNoteNull();			//	No Product note


			if (isTracing)
				Debug::trace("Product.save", msgDebug + "marqueeText = " + ptrtostring(productTexts_[psttMarqueeText]));
			if (productTexts_[psttMarqueeText])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "marqueeText is NOT null - marqueeText=" + longtostring(productTexts_[psttMarqueeText]->getTextNum()));

				if (productTexts_[psttMarqueeText]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setMarqueeNull();			//	No Marquee text number
				else
				{
					myDbRecord->setMarquee(Integer::toString(productTexts_[psttMarqueeText]->getTextNum()));	//	Set the Marquee text number

					//	Save the Marquee text record to the Database.
					/*
					if (productTexts_[psttMarqueeText]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the marqueeText to the database.");
						// This is now done by the ProductManager   productTexts_[psttMarqueeText]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setMarqueeNull();			//	No Marquee text

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "productText5 = " + ptrtostring(productTexts_[psttProductText5]));
			if (productTexts_[psttProductText5])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "productText5 is NOT null - productText5Num=" + longtostring(productTexts_[psttProductText5]->getTextNum()));

				if (productTexts_[psttProductText5]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setProductText5Null();			//	No productText5 number
				else
				{
					myDbRecord->setProductText5(Integer::toString(productTexts_[psttProductText5]->getTextNum()));	//	Set the productText5 number

					//	Save the productText5 record to the Database.
					/*
					if (productTexts_[psttProductText5]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productText5 to the database.");
						// This is now done by the ProductManager   productTexts_[psttProductText5]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setProductText5Null();			//	No productText5

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "productText6 = " + ptrtostring(productTexts_[psttProductText6]));
			if (productTexts_[psttProductText6])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "productText6 is NOT null - productText6Num=" + longtostring(productTexts_[psttProductText6]->getTextNum()));

				if (productTexts_[psttProductText6]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setProductText6Null();			//	No productText6 number
				else
				{
					myDbRecord->setProductText6(Integer::toString(productTexts_[psttProductText6]->getTextNum()));	//	Set the productText6 number

					//	Save the productText6 record to the Database.
					/*
					if (productTexts_[psttProductText6]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productText6 to the database.");
						// This is now done by the ProductManager   productTexts_[psttProductText6]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setProductText6Null();			//	No productText6

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "productText7 = " + ptrtostring(productTexts_[psttProductText7]));
			if (productTexts_[psttProductText7])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "productText7 is NOT null - productText7Num=" + longtostring(productTexts_[psttProductText6]->getTextNum()));

				if (productTexts_[psttProductText7]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setProductText7Null();			//	No productText7 number
				else
				{
					myDbRecord->setProductText7(Integer::toString(productTexts_[psttProductText7]->getTextNum()));	//	Set the productText7 number

					//	Save the productText7 record to the Database.
					/*
					if (productTexts_[psttProductText7]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productText7 to the database.");
						// This is now done by the ProductManager   productTexts_[psttProductText7]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setProductText7Null();			//	No productText7

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "productText8 = " + ptrtostring(productTexts_[psttProductText8]));
			if (productTexts_[psttProductText8])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "productText8 is NOT null - productText8Num=" + longtostring(productTexts_[psttProductText8]->getTextNum()));

				if (productTexts_[psttProductText8]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setProductText8Null();			//	No productText8 number
				else
				{
					myDbRecord->setProductText8(Integer::toString(productTexts_[psttProductText8]->getTextNum()));	//	Set the productText8 number

					//	Save the productText8 record to the Database.
					/*
					if (productTexts_[psttProductText8]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productText8 to the database.");
						// This is now done by the ProductManager   productTexts_[psttProductText8]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setProductText8Null();			//	No productText8

			if (isTracing)
				Debug::trace("Product.save", msgDebug + "productText9 = " + ptrtostring(productTexts_[psttProductText9]));
			if (productTexts_[psttProductText9])
			{
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "productText9 is NOT null - productText9Num=" + longtostring(productTexts_[psttProductText9]->getTextNum()));

				if (productTexts_[psttProductText9]->getTextNum() == RECORD_NOT_FOUND)
					myDbRecord->setProductText9Null();			//	No productText9 number
				else
				{
					myDbRecord->setProductText9(Integer::toString(productTexts_[psttProductText9]->getTextNum()));	//	Set the productText9 number

					//	Save the productText9 record to the Database.
					/*
					if (productTexts_[psttProductText9]->getErrorCode() == RECORD_OK)
					{
						Debug::trace("Product.save", msgDebug + "Saving the productText9 to the database.");
						// This is now done by the ProductManager   productTexts_[psttProductText9]->saveRecord();
					}
					*/
				}
			}
			else
				myDbRecord->setProductText9Null();			//	No productText9


			if (coolCategoryNum >= 0)
			{
				myDbRecord->setCoolCategory(Long::toString(coolCategoryNum));
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "setCoolCategory(" + myDbRecord->getCoolCategory() + ")");
			}
			else
			{
				myDbRecord->setCoolCategoryNull();			//	No COOL Category number
				if (isTracing)
					Debug::trace("Product.save", msgDebug + "setCoolCategoryNull()");
			}

			if (coolPretextNum == RECORD_NOT_FOUND)
				myDbRecord->setCoolPreTextNull();			//	No COOL Pretext number
			else
				myDbRecord->setCoolPreText(Long::toString(coolPretextNum));

			if (coolDefaultTextNum == RECORD_NOT_FOUND)
				myDbRecord->setDefaultCoolTextNull();		//	No Default COOL Text number
			else
				myDbRecord->setDefaultCoolText(Long::toString(coolDefaultTextNum));

			if (coolTextNum == RECORD_NOT_FOUND)
				myDbRecord->setCurrentCoolTextNull();		//	No COOL Text number
			else
				myDbRecord->setCurrentCoolText(Long::toString(coolTextNum));

			// First set the cool list to null then, if there is data for it, set it to the correct value
			myDbRecord->setCoolListNull();
			if (coolShortlistNum != RECORD_NOT_FOUND)
			{
				DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();

				if(dbCoolListsRM)
				{
					HobString shortlistNumStr = Long::toString(coolShortlistNum); 
					DBCoolList *coolList = dbCoolListsRM->findNumber(shortlistNumStr, longtostring(deptNum));
					
					if(coolList)
					{
						myDbRecord->setCoolList(coolList);
						delete coolList;
					}
					else
					{
						coolList = dbCoolListsRM->createRecord();

						if(coolList)
						{
							coolList->setNumber(shortlistNumStr);
							coolList->setDeptNumber(longtostring(deptNum));
							dbCoolListsRM->saveRecord(coolList);
							myDbRecord->setCoolList(coolList);

							delete coolList;
						}
 					}
				}
			}
			else if(coolShortlistFirstLink != RECORD_NOT_FOUND)
			{
				DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
				if (dbCoolListsRM)
				{
					DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
					if (dbCoolListTextsRM)
					{
						HobString prodNumStr = Long::toString(productNum);
						HobString shortlistFirstLinkStr = Long::toString(coolShortlistFirstLink);
						DBCoolList *dbCoolList = dbCoolListsRM->findFirstLink(shortlistFirstLinkStr);
						if (dbCoolList)
						{
							if (dbCoolList->isNumberNull())
							{
								dbCoolList->setDeptNumber(longtostring(deptNum));
								dbCoolList->setNumber(prodNumStr);
								dbCoolListsRM->saveRecord(dbCoolList);
							}
							myDbRecord->setCoolList(dbCoolList);
						}
						else
						{
							dbCoolList = dbCoolListsRM->createRecord();
							if (dbCoolList)
							{
								dbCoolList->setFirstLinkNumber(shortlistFirstLinkStr);
								dbCoolList->setDeptNumber(longtostring(deptNum));
								dbCoolList->setNumber(prodNumStr);
								dbCoolListsRM->saveRecord(dbCoolList);

								myDbRecord->setCoolList(dbCoolList);
							}

							//	If this list exists, it does not start at the beginning of the list. We need to duplicate the entries from here down.
							int order = 0;
							int nextLink = coolShortlistNum;
							DBQuery<DBCoolListText> *linkTextsList = 0;

							HobString shortlistStr = Long::toString(nextLink);
							linkTextsList = dbCoolListTextsRM->retrieveLink(shortlistStr);

							if(linkTextsList)
							{
								DBQuery<DBCoolListText>::const_iterator listTextsIter = linkTextsList->begin();

								if (isTracing)
									Debug::trace("Product.save", "nextLink = " + longtostring(nextLink));

								if (listTextsIter != linkTextsList->end())
								{
									if (isTracing)
										Debug::trace("Product.save", "Past if statement.");
																										
									while(!(listTextsIter->isNextLinkNumberNull()))
									{
										if (isTracing)
											Debug::trace("Product.save", "Indside the while loop.");
										DBCoolListText *dbCoolListText = dbCoolListTextsRM->createRecord(shortlistStr, dbCoolList, deptNum);
										if (dbCoolListText)
										{
											dbCoolListText->setOrder(order++);
											dbCoolListText->setLinkNumber(listTextsIter->getLinkNumber());
											dbCoolListText->setNextLinkNumber(listTextsIter->getNextLinkNumber());
											dbCoolListText->setCoolTextNumber(listTextsIter->getCoolTextNumber());
											dbCoolListTextsRM->saveRecord(dbCoolListText);
											delete dbCoolListText;
											dbCoolListText = 0;
										}

										++listTextsIter;
									}

									delete linkTextsList;
									linkTextsList = 0;
								}
							}
						}
						if (dbCoolList)
							delete dbCoolList;
					}
				}
			}

			if ( coolTrackingText.empty() )
				myDbRecord->setCoolTrackingTextNull();
			else
				myDbRecord->setCoolTrackingText(coolTrackingText);

			myDbRecord->setForceCoolReview(coolVerifyForced);
			myDbRecord->setCoolRequired(coolRequired);
			if (Debug::isTracing("Clare"))
			{
				HobString myDebug = msgDebug + HobString("coolVerifyForced = ");
				myDebug.append(Bool::toString(coolVerifyForced));
				myDebug.append("  coolRequired = ").append(Bool::toString(coolRequired));
				Debug::trace("Clare", myDebug);
			}

			myDbRecord->setProductionPlanningType(productionPlanningType);

			myDbRecord->setMealSideItemsGroupNum(mealSideItemsGroupNum);
			myDbRecord->setMealSideItemsAreFree(mealSideItemsAreFree);
			myDbRecord->setMealNumSidesIncluded(mealNumSidesIncluded);

			myDbRecord->setCutTestMeatSpecSheetGroupNum(cutTestMeatSpecSheetGroupNum);

			// For Pick-5 project
			myDbRecord->setWeightMin(weightMin);
			myDbRecord->setWeightMax(weightMax);
			myDbRecord->setTotalPriceMin(totalPriceMin);
			myDbRecord->setTotalPriceMax(totalPriceMax);

			try
			{
				dbRM->saveRecord(myDbRecord);
				if (isTracing)
					Debug::trace("Product.save", msgDebug + string("saveRecord() - saved num ") + longtostring(productNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Product::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
Product *Product::findRecord(HobRecordNum productNum, HobRecordNum deptNum, DBAccess *dbAccess, bool inOperateMode, bool isOnlineOrderingItem)
{
	Product *rec = (Product *)NULL;

	if (productNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBProductsRM *dbRM = dbAccess->getDBProductsRM();
	if (!dbRM)
		return rec;

	if (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgOnlineOrderingEnabled))
		isOnlineOrderingItem = false;

	try
	{
		DBProduct *dbRec = dbRM->findPLU(longtostring(productNum), longtostring(deptNum), inOperateMode);
		if (dbRec)
		{
			rec = (isOnlineOrderingItem) ? (new OnlineOrderProduct(dbRec, dbAccess)) : (new Product(dbRec, dbAccess));
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("Product"))
					Debug::trace("Product", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getProductNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
				if (isOnlineOrderingItem)
					Debug::trace("OnlineOrderProduct", "findRecord() - is OnlineOrderProduct - rec ptr: " + ptrtostring(rec));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "Product::findRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}



void Product::createUltimaFlag() const
{
					//create ultima type of plu flag
					if(applySecurityLabel)
					{
						flags |= PRPrintSecurityLabel;
					}
					if(productType == PRODUCTTYPE_FIXEDWEIGHT)
					{
						flags |= PRFixedWeight;
					}
					if(productType == PRODUCTTYPE_FLUIDOUNCE)
					{
						flags |= PRFluidOunce;
					}
					if(myDbRecord == NULL)
					{
						flags |= PRBadRecord;
					}


/* currently the setting of these flag bit might be done (be a part of) in configuration
	ats 10152002
					if()
					{
						flags |= PRPriceChange;
					}
					if()
					{
						flags |= PRChangeRecord;
					}
					if()
					{
						flags |= PRAllowBackDating;
					}
					if()
					{
						flags |= PRProductTotals;
					}
					if()
					{
						flags |= PRManualChangeDigit;
					}
					if()
					{
						flags |= PRAutoChangeDigit;
					}
*/
}


void Product::initProductTransaction()
{
	ProductTransaction *newTransaction = getCurrentTransaction();

	if(newTransaction)
	{
		HobString newDescription(description);
		newDescription.stripFontSequences();

		newTransaction->setDeptNum(deptNum);
		newTransaction->setProductNumber(productNum);
		newTransaction->setReportingClass(getReportingClassNum());
		newTransaction->setProductType(productType);
		newTransaction->setProductionPlanningType(productionPlanningType);
		newTransaction->setProductDescription(newDescription);

		newTransaction->setUnitPrice(unitPrice);
		newTransaction->setExceptionPrice(exceptionPrice);
		newTransaction->setPriceModifier(priceModifier);
		newTransaction->setByCount(byCount);
		newTransaction->setTare(tareA.getValue());
		newTransaction->setProportionalTare(proportionalTare);
		if ( !( isRandomWeight() || isByCountWithWeight() ||
		        ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgRequireFixedWeightVerification) && isFixedWeight() && getFixedWeight() > 0 ) ) )
			newTransaction->setTotalNetWeight(MetSig::BRConvertFixedWeightToTotalWeight(getFixedWeight(), isFluidOunce()));
		newTransaction->setFixedWeight(fixedWeight);

		newTransaction->setShelfLifeDays(shelfLifeDays);
		newTransaction->setShelfLifeHours(shelfLifeHours);
		newTransaction->setShelfLifeMinutes(shelfLifeMinutes);
		newTransaction->setProductLifeDays(productLifeDays);
		newTransaction->setProductLifeHours(productLifeHours);
		newTransaction->setProductLifeMinutes(productLifeMinutes);

		newTransaction->setBarCodeNum(barCodeNum);

		// TODO - Larry - done - what about these for auto labels? How to number?
		/* TODO - Larry - Still need to check/test this one.
		 * With the 'label types by mode' project, I'm commenting these out because they are also set up by the printCurrentTransaction() method,
		 * which gets called right before printing, so initially setting these here is meaningless.  Also can't set these up here because initProductTransaction()
		 * is called by the Product constructors, but the Label Types aren't being set up in the Product constructor (with the db record as a parameter) any more.
		 */
		// Not meaningless... needed for transaction sycronization
		if ( getLabelType(0) )
			newTransaction->setLabel1(getLabelType(0)->getLabelTypeNum());
		if ( getLabelType(1) )
			newTransaction->setLabel2(getLabelType(1)->getLabelTypeNum());
		if ( getLabelType(2) )
			newTransaction->setLabel3(getLabelType(2)->getLabelTypeNum());
		if ( getLabelType(3) )
			newTransaction->setLabel4(getLabelType(3)->getLabelTypeNum());

		newTransaction->setGraphicName1(getGraphicName(0));
		newTransaction->setGraphicName2(getGraphicName(1));
		newTransaction->setGraphicName3(getGraphicName(2));
		newTransaction->setGraphicName4(getGraphicName(3));

		if ( isCoolRequired() )
			newTransaction->setCOOLTransaction();
		// We will set this values at the end, when the transaction is to be printed, and
		// only if the product requires COOL.
//		newTransaction->setCOOLText(getCOOLTextToPrint());
//		newTransaction->setCOOLTrackingText(coolTrackingText);


		newTransaction->setOldBarcodeDigit(util_string::stringtoint(getBarCodeNum().getMostSignificantUPCDigit()));
		newTransaction->setOldUnitPrice(getUnitPrice());
//		newTransaction->setOldFinalPrice();
		newTransaction->setOldTare(getTareA().getValue());
		newTransaction->setOldTareB(getTareB().getValue());
		newTransaction->setOldByCount(getByCount());
		newTransaction->setOldFixedWeight(getFixedWeight());
		newTransaction->setOldShelfLifeDays(getShelfLifeDays());
		newTransaction->setOldShelfLifeHours(getShelfLifeHours());
		newTransaction->setOldShelfLifeMinutes(getShelfLifeMinutes());
		newTransaction->setOldProductLifeDays(getProductLifeDays());
		newTransaction->setOldProductLifeHours(getProductLifeHours());
		newTransaction->setOldProductLifeMinutes(getProductLifeMinutes());
		newTransaction->setOldLabel1(getLabelType(FirstPrinter) ? getLabelType(FirstPrinter)->getLabelTypeNum() : 0);
		newTransaction->setOldLabel2(getLabelType(SecondPrinter) ? getLabelType(SecondPrinter)->getLabelTypeNum() : 0);
		newTransaction->setOldLabel3(getLabelType(ThirdPrinter) ? getLabelType(ThirdPrinter)->getLabelTypeNum() : 0);
//		newTransaction->setOldLabel4(getLabelType(ForthPrinter) ? getLabelType(ForthPrinter)->getLabelTypeNum() : 0);
		newTransaction->setOldGraphicName1(getGraphic(Graphic::GRAPHIC1) ? getGraphic(Graphic::GRAPHIC1)->getGraphicName() : "");
		newTransaction->setOldGraphicName2(getGraphic(Graphic::GRAPHIC2) ? getGraphic(Graphic::GRAPHIC2)->getGraphicName() : "");
		newTransaction->setOldGraphicName3(getGraphic(Graphic::GRAPHIC3) ? getGraphic(Graphic::GRAPHIC3)->getGraphicName() : "");
		newTransaction->setOldGraphicName4(getGraphic(Graphic::GRAPHIC4) ? getGraphic(Graphic::GRAPHIC4)->getGraphicName() : "");

		if (isByCount() || isFixedWtOrFluidOz())
		{
			if (!isByCountForced() || (isByCountForced() && (getByCount() > 0)))
				setOpModeByCount();

			if (isFixedWtOrFluidOz() && !isByCountForced())
				setOpModeFixedWt();
		}
	}
}


bool Product::setOpModeByCount()
{
	ProductTransaction *currentTrans = getCurrentTransaction();
	if (!currentTrans)
		return false;

	bool updatePrice = true;

	if (byCount == 0)
		byCount = getOriginalByCount();
	
	if (!byCountEntered)
	{
		currentTrans->setTotalPrice(0);
		currentTrans->setOldFinalPrice(0);
	}
	else
	{
		// If the operator ever changes the price from what it was in the database (and if it's forced price) we won't recalculate the total price using the by count value,
		// whether it was changed or not.  The by count value for the unit price string will be the current by count value, which is in the product object.
		//if ( !isUnitPriceChanged() ) // SOF-3717 - Removed this check that was put in for SOF-2744 ...
		{
			HobPrice totalPrice = MetSig::byCountCalcTotalPrice(getOriginalByCount(), byCount, unitPrice, exceptionPrice);
			currentTrans->setTotalPrice(totalPrice);
		}

		// Also update the old total price
		int oldByCount = currentTrans->getOldByCount();
		HobPrice oldUnitPrice = currentTrans->getOldUnitPrice();
		if ( oldByCount && oldUnitPrice )
			currentTrans->setOldFinalPrice(MetSig::byCountCalcTotalPrice(oldByCount, oldByCount, oldUnitPrice, exceptionPrice));
	}
	return updatePrice;
}


void Product::setOpModeFixedWt()
{
	int originalByCount = getOriginalByCount();
	double newFixedWeight = (((double)originalFixedWeight) *(double) abs(byCount)) / (double) originalByCount;
	setFixedWeight((HobWeight) newFixedWeight);
}


bool Product::isFieldPreImage(short field)
{
	bool fieldIsPreImage;

	switch(field)
	{
//		case fieldOffsetLabelTypeNum1:		//	These fields need other validations.
//		case fieldOffsetLabelTypeNum2:
//		case fieldOffsetLabelTypeNum3:
//      cases and all of the new label types now too
		case fieldOffsetGraphicName1:
		case fieldOffsetGraphicName2:
		case fieldOffsetGraphicName3:
		case fieldOffsetGraphicName4:
		case fieldOffsetProdCOOLTrackingText:
		case fieldOffsetProdCoolTextNum:
		case fieldOffsetProdCoolRequired:	//	Not an actual field on the label, but will require the label to be pre-imaged.
			fieldIsPreImage = true;
			break;
		default:
			fieldIsPreImage = false;
			break;
	}
	return fieldIsPreImage;
}


void Product::getFieldNameAsString(int fieldOffset, HobString &fieldName)
{
	switch (fieldOffset)
	{
		case fieldOffsetProductNum:
			fieldName = "ProductNum:";
			break;
		case fieldOffsetProductType:
			fieldName = "ProductType:";
			break;
		case fieldOffsetDescription:
			fieldName = "Description:";
			break;

		case fieldOffsetBarcodeType:
			fieldName = "BarcodeType:";
			break;
		case fieldOffsetBarcodeFields:
			fieldName = "BarcodeFields:";
			break;
		case fieldOffsetBarcodePrefix:
			fieldName = "BarcodePrefix:";
			break;
		case fieldOffsetBarcodeProductNum:
			fieldName = "BarcodeProductNum:";
			break;
		case fieldOffsetBarcodeUpcNum:
			fieldName = "BarcodeUpcNum:";
			break;
		case fieldOffsetBarcodeVendorNum:
			fieldName = "BarcodeVendorNum:";
			break;
		case fieldOffsetBarcodeEANCode:
			fieldName = "BarcodeEANCode:";
			break;

		case fieldOffsetByCount:
			fieldName = "ByCount:";
			break;
		case fieldOffsetExceptionPrice:
			fieldName = "ExceptionPrice:";
			break;
		case fieldOffsetUnitPrice:
			fieldName = "UnitPrice:";
			break;

		case fieldOffsetDiscountType:
			fieldName = "DiscountType:";
			break;
		case fieldOffsetDiscountValue1:
			fieldName = "DiscountValue1:";
			break;
		case fieldOffsetDiscountValue2:
			fieldName = "DiscountValue2:";
			break;
		case fieldOffsetDiscountValue3:
			fieldName = "DiscountValue3:";
			break;

		case fieldOffsetDiscountLabel1:
			fieldName = "DiscountLabel1:";
			break;
		case fieldOffsetDiscountLabel2:
			fieldName = "DiscountLabel2:";
			break;
		case fieldOffsetDiscountLabel3:
			fieldName = "DiscountLabel3:";
			break;

		case fieldOffsetTareAValue:
			fieldName = "TareAValue:";
			break;
		case fieldOffsetTareBValue:
			fieldName = "TareBValue:";
			break;
		case fieldOffsetProportionalTare:
			fieldName = "ProportionalTare:";
			break;
		case fieldOffsetFixedWeight:
			fieldName = "FixedWeight:";
			break;

		case fieldOffsetShelfLifeDays:
			fieldName = "ShelfLifeDays:";
			break;
		case fieldOffsetShelfLifeHours:
			fieldName = "ShelfLifeHours:";
			break;
		case fieldOffsetShelfLifeMinutes:
			fieldName = "ShelfLifeMinutes:";
			break;
		case fieldOffsetProductLifeDays:
			fieldName = "ProductLifeDays:";
			break;
		case fieldOffsetProductLifeHours:
			fieldName = "ProductLifeHours:";
			break;
		case fieldOffsetProductLifeMinutes:
			fieldName = "ProductLifeMinutes:";
			break;

		case fieldOffsetPriceModifier:
			fieldName = "PriceModifier:";
			break;

		case fieldOffsetProductReportingClassNum:
			fieldName = "ReportingClassNum:";
			break;

		case fieldOffsetLabelTypeNum1:
		case fieldOffsetDefaultLabelTypeNum1:
			fieldName = "LabelTypeNum1:";
			break;
		case fieldOffsetLabelTypeNum2:
		case fieldOffsetDefaultLabelTypeNum2:
			fieldName = "LabelTypeNum2:";
			break;
		case fieldOffsetLabelTypeNum3:
		case fieldOffsetDefaultLabelTypeNum3:
			fieldName = "LabelTypeNum3:";
			break;

		case fieldOffsetManualLabelTypeNum1:
			fieldName = "ManualLabelTypeNum1:";
			break;
		case fieldOffsetManualLabelTypeNum2:
			fieldName = "ManualLabelTypeNum2:";
			break;
		case fieldOffsetManualLabelTypeNum3:
			fieldName = "ManualLabelTypeNum3:";
			break;

		case fieldOffsetPrepackLabelTypeNum1:
			fieldName = "PrepackLabelTypeNum1:";
			break;
		case fieldOffsetPrepackLabelTypeNum2:
			fieldName = "PrepackLabelTypeNum2:";
			break;
		case fieldOffsetPrepackLabelTypeNum3:
			fieldName = "PrepackLabelTypeNum3:";
			break;

		case fieldOffsetProdEntryLabelTypeNum1:
			fieldName = "ProdEntryLabelTypeNum1:";
			break;
		case fieldOffsetProdEntryLabelTypeNum2:
			fieldName = "ProdEntryLabelTypeNum2:";
			break;
		case fieldOffsetProdEntryLabelTypeNum3:
			fieldName = "ProdEntryLabelTypeNum3:";
			break;

		case fieldOffsetSelfServLabelTypeNum1:
			fieldName = "SelfServLabelTypeNum1:";
			break;
		case fieldOffsetSelfServLabelTypeNum2:
			fieldName = "SelfServLabelTypeNum2:";
			break;
		case fieldOffsetSelfServLabelTypeNum3:
			fieldName = "SelfServLabelTypeNum3:";
			break;

		case fieldOffsetPick5LabelTypeNum1:
			fieldName = "Pick5LabelTypeNum1:";
			break;
		case fieldOffsetPick5LabelTypeNum2:
			fieldName = "Pick5LabelTypeNum2:";
			break;
		case fieldOffsetPick5LabelTypeNum3:
			fieldName = "Pick5LabelTypeNum3:";
			break;

		case fieldOffsetOnlineOrderingLabelTypeNum1:
			fieldName = "OnlineOrderingLabelTypeNum1:";
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum2:
			fieldName = "OnlineOrderingLabelTypeNum2:";
			break;
		case fieldOffsetOnlineOrderingLabelTypeNum3:
			fieldName = "OnlineOrderingLabelTypeNum3:";
			break;

		case fieldOffsetLabelRotation1:
			fieldName = "LabelRotation1:";
			break;
		case fieldOffsetLabelRotation2:
			fieldName = "LabelRotation2:";
			break;
		case fieldOffsetLabelRotation3:
			fieldName = "LabelRotation3:";
			break;

		case fieldOffsetBackDateDays:
			fieldName = "BackdateDays:";
			break;
		case fieldOffsetBackDateDate:
			fieldName = "BackdateDate:";
			break;
		case fieldOffsetPoints:
			fieldName = "Points:";
			break;
		case fieldOffsetIntakeMode:
			fieldName = "IntakeMode:";
			break;

		case fieldOffsetGraphicName1:
			fieldName = "GraphicName1:";
			break;
		case fieldOffsetGraphicName2:
			fieldName = "GraphicName2:";
			break;
		case fieldOffsetGraphicName3:
			fieldName = "GraphicName3:";
			break;
		case fieldOffsetGraphicName4:
			fieldName = "GraphicName4:";
			break;

		//	Label stock dimensions - may need to go with printer messages.
		case fieldOffsetLabelTypeWidth1:
			fieldName = "LabelTypeWidth1:";
			break;
		case fieldOffsetLabelTypeLength1:
			fieldName = "LabelTypeLength1:";
			break;
		case fieldOffsetLabelTypeWidth2:
			fieldName = "LabelTypeWidth2:";
			break;
		case fieldOffsetLabelTypeLength2:
			fieldName = "LabelTypeLength2:";
			break;
		case fieldOffsetLabelTypeWidth3:
			fieldName = "LabelTypeWidth3:";
			break;
		case fieldOffsetLabelTypeLength3:
			fieldName = "LabelTypeLength3:";
			break;

		case fieldOffsetByCountForced:
			fieldName = "ByCountForced:";
			break;
		case fieldOffsetTareAForced:	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
			fieldName = "TareAForced:";
			break;
		case fieldOffsetFractionalPriceUnit:
			fieldName = "FractionalPriceUnit:";
			break;
		case fieldOffsetPoundsForAmount:
			fieldName = "PoundsForAmount:";
			break;
		case fieldOffsetUnitPriceForced:
			fieldName = "ForcedUnitPrice:";
			break;
		case fieldOffsetTareType:
			fieldName = "TareType:";
			break;
		case fieldOffsetApplySecurity:
			fieldName = "ApplySecurity:";
			break;
		case fieldOffsetDontPrintPackedOn:
			fieldName = "DontPrintPackedOn:";
			break;
		case fieldOffsetDontPrintSellBy:
			fieldName = "DontPrintSellBy:";
			break;
		case fieldOffsetDontPrintBestBefore:
			fieldName = "DontPrintBestBefore:";
			break;
		case fieldOffsetLabelPlacement1:
			fieldName = "PrimaryLabelPlacement:";
			break;
		case fieldOffsetLabelPlacement2:
			fieldName = "SecondLabelPlacement:";
			break;
		case fieldOffsetLabelPlacement3:
			fieldName = "ThirdLabelPlacement:";
			break;
		case fieldOffsetFractionalPrice:
			fieldName = "FractionalPrice:";
			break;
		case fieldOffsetForcedFractionalPrice:
			fieldName = "ForcedFractionalPrice:";
			break;
		case fieldOffsetPoundsForPrice:
			fieldName = "PoundsForPrice:";
			break;
		case fieldOffsetForcedPoundsFor:
			fieldName = "ForcedPoundsFor:";
			break;

		case fieldOffsetSellByDate:
			fieldName = "SellByDate:";
			break;
		case fieldOffsetSellByTime:
			fieldName = "SellByTime:";
			break;
		case fieldOffsetBestBeforeDate:
			fieldName = "BestBeforeDate:";
			break;
		case fieldOffsetBestBeforeTime:
			fieldName = "BestBeforeTime:";
			break;

		case fieldOffsetHasNutrifactsFlag:
			fieldName = "HasNutrifactsFlag:";
			break;

		case fieldOffsetEANFormatString:
			fieldName = "EANFormatString:";
			break;
			
		case fieldOffsetEAN128CFormatString:
			fieldName = "EAN128CFormatString:";
			break;

		case fieldOffsetGS1DatabarGTIN:
			fieldName = "GS1DatabarGTIN:";
			break;
			
		case fieldOffsetGS1DatabarAI:
			fieldName = "GS1DatabarAI:";
			break;

		case fieldOffsetQRCodeFormatString:
			fieldName = "QrCodeFormatString:";
			break;

		case fieldOffsetDataMatrixFormatString:
			fieldName = "DataMatrixFormatString:";
			break;

		case fieldOffsetEANprefix:
			fieldName = "EANprefix:";
			break;

		case fieldOffsetProductGraphicName:
			fieldName = "ProductGraphicName:";
			break;
		case fieldOffsetProdSpecialNum:
			fieldName = "SpecialNum:";
			break;
		case fieldOffsetSpecialStartDate:
			fieldName = "SpecialStartDate:";
			break;

		case fieldOffsetProductErrorCode:
			fieldName = "ErrorCode:";
			break;

		case fieldOffsetPluFlags:
			fieldName = "PluFlags:";
			break;
		case fieldOffsetTotalsFlags:
			fieldName = "TotalsFlags:";
			break;
		case fieldOffsetExceptionPriceTare:
			fieldName = "ExceptionPriceTare:";
			break;

		case fieldOffsetGraphicNum:
			fieldName = "GraphicNum:";
			break;
		case fieldOffsetProdNutrifactNum:
			fieldName = "NutrifactsNum:";
			break;
		case fieldOffsetExpandedTextNum:
			fieldName = "ExpandedTextNum:";
			break;
		case fieldOffsetSpecialMessageNum:
			fieldName = "SpecialMessageNum:";
			break;
		case fieldOffsetProdProductNoteNum:
			fieldName = "ProductNotesNum:";
			break;
		case fieldOffsetMarqueeTextNum:
			fieldName = "MarqueeTextNum:";
			break;
		case fieldOffsetProductText5Num:
			fieldName = "ProductText5Num:";
			break;
		case fieldOffsetProductText6Num:
			fieldName = "ProductText6Num:";
			break;
		case fieldOffsetProductText7Num:
			fieldName = "ProductText7Num:";
			break;
		case fieldOffsetProductText8Num:
			fieldName = "ProductText8Num:";
			break;
		case fieldOffsetProductText9Num:
			fieldName = "ProductText9Num:";
			break;

		case fieldOffsetProdCoolCategoryNum:
			fieldName = "CoolCategoryNum:";
			break;
		case fieldOffsetProdCoolPretextNum:
			fieldName = "CoolPretextNum:";
			break;
		case fieldOffsetProdCoolDefaultTextNum:
			fieldName = "CoolDefaultTextNum:";
			break;
		case fieldOffsetProdCoolTextNum:
			fieldName = "CoolTextNum:";
			break;
		case fieldOffsetProdCoolShortlistNum:
			fieldName = "CoolShortlistNum:";
			break;
		case fieldOffsetProdCoolShortlistFirstLink:
			fieldName = "CoolShortlistFirstLink:";
			break;
		case fieldOffsetProdCOOLTrackingText:
			fieldName = "COOLTrackingText:";
			break;
		case fieldOffsetProdCoolVerifyForced:
			fieldName = "ForcedCOOLVerify:";
			break;
		case fieldOffsetProductionPlanningType:
			fieldName = "Production Planning Type:";
			break;

		case fieldOffsetBuildAMealSideItemsGroupNum:
			fieldName = "Meal Side Items Group Num";
			break;
		case fieldOffsetBuildAMealSideItemsAreFree:
			fieldName = "Meal Side Items Are Free";
			break;
		case fieldOffsetBuildAMealNumberOfSidesIncluded:
			fieldName = "Meal Num Sides Included";
			break;
		case fieldOffsetCutTestMeatSpecSheetGroupNum:
			fieldName = "Cut Test Meat Spec Sheet Group Num";
			break;
			// For Pick-5 project
		case fieldOffsetWeightMin:
			fieldName = "Weight Min to Print";
			break;
		case fieldOffsetWeightMax:
			fieldName = "Weight Max to Print";
			break;
		case fieldOffsetTotalPriceMin:
			fieldName = "Total Price Min to Print";
			break;
		case fieldOffsetTotalPriceMax:
			fieldName = "Total Price Max to Print";
			break;

		default:
			fieldName = HobString("Unknown field #") + HobString(Integer::toString(fieldOffset));
			break;
	}
}

void Product::show()
{
	if (Debug::isTracing("Product.show"))
	{
		HobString fieldData;
		HobString fieldName;
		for (int i = fieldOffsetProductNum; i < endProductFieldOffsets; i++)
		{
			fieldData = "";
			fieldName = "";
			getFieldNameAsString(i, fieldName);
			getFieldAsString(i, fieldData, BrowserFormat);
			Debug::trace("Product.show", fieldName + fieldData);
		}
	}
}


void Product::setBarcodeFields( Product* defProduct )
{
	//	Get the Label type. Does it contain a UPC or EAN13 barcode? Set the Barcode fields up accordingly.
	//	If no primary label type, go to the second label type.
	//	If no second label type, go to the third label type.
	//	If no third label type, assume UPCA.
	short printerNum = FirstPrinter;
	LabelType *myLabelType = (LabelType *)NULL;

	while (!myLabelType && (printerNum < MaxPrinterTypes))
	{
		myLabelType = getLabelType(printerNum);
		printerNum++;
	}
	short barcodeType = ((myLabelType) ? myLabelType->getBarcodeType() : (short)UPCA);
	barCodeNum.setBarcodeType(barcodeType);

	ProductDefault *pd = dynamic_cast<ProductDefault *> (defProduct);
	if (pd)
	{
		barCodeNum.setFormatStrings(pd);
		if (barcodeType == EAN13 || barcodeType == EAN128C)
			barCodeNum.setPrefix(pd->getEAN13PrefixString());
	}
}

bool Product::needsNoWeigherStatus() const
{
	if (isFixedWtOrFluidOz() ||
		(isByCount() && !isByCountWithWeight()) ||
		(isByCountWithWeight() && !SessionManager::getInstance()->isWeigherAttached()) ||
		(isByCountWithWeight() && scaleConfig->isSelfServiceScale()) ||
		(isRandomWeight() && hasKbdNetWeightLabel()))
		return true;
	else
		return false;
}

HobString Product::getFormattedFixedWeightString(int UNUSED(format)) const
{	//	NOTE: Eventually, get rid of this method.
	HobString result = "";
	return result;
}


bool Product::areForcedFieldsEntered() const
{
	bool tracing = Debug::isTracing("Product");

	if (unitPriceForced && !unitPriceEntered)
	{
		if (tracing)
			Debug::trace("Product", "areForcedFieldsEntered - user hasn't entered a unit price when one is required - returning false");
		return false;
	}

	if (tareA.isForced() && !tareEntered && (isRandomWeight() || isByCountWithWeight()))	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
	{
		if (tracing)
			Debug::trace("Product", "areForcedFieldsEntered - user hasn't entered a tare when one is required - returning false");
		return false;
	}

	if (needsNoWeigherStatus())
	{
		if (tracing)
			Debug::trace("Product", HobString("areForcedFieldsEntered - needsNoWeigherStatus - by count entered:") + HobString(Bool::toString(isByCountEntered())));
		if ((isByCountForced() || (scaleConfig->isSelfServiceScale() && isByCount())) && !isByCountEntered())
		{
			if (tracing)
				Debug::trace("Product", "areForcedFieldsEntered - user hasn't entered a by count when one is required - returning false");
			return false;
		}
	}

	if (numOfLabelsForced && !numOfLabelsEntered)
	{
		if (tracing)
			Debug::trace("Product", "areForcedFieldsEntered - user hasn't entered the number of labels to print in Production Entry mode - returning false");
		return false;
	}

	if (backDateDaysForced && !backDateDaysEntered)
	{
		if (tracing)
			Debug::trace("Product", "areForcedFieldsEntered - user hasn't entered the backDate days when it is required - returning false");
		return false;
	}

    if (coolRequired && !coolRequiredText)
    {
        if (tracing)
            Debug::trace("Product", "areForcedFieldsEntered - user hasn't selected a COOL text required - returning false");
        return false;
    }

	return true;
}

bool Product::isTotalPriceOutOfRange() const
{
	if (currentProductTransaction_)
	{
		HobValue currentTotalPrice = currentProductTransaction_->getTotalPrice();
		if (currentTotalPrice > scaleConfig->getMaxTotalPrice() || (currentTotalPrice < 0 && currentTotalPrice < -(scaleConfig->getMaxTotalPrice())))
		{
			Debug::trace("Product", HobString("isTotalPriceOutOfRange() - total price is out of range: ") + HobString(Long::toString(currentTotalPrice)));
			return true;
		}
	}
	return false;
}

bool Product::isReadyToPrint() const
{
	if (!currentProductTransaction_)
	{
		Debug::trace("Product", "isReadyToPrint - currentProductTransaction_ is NULL! - returning false");
		return false;
	}

	if (!areForcedFieldsEntered())
    {
        Debug::trace("Product", "isReadyToPrint - user hasn't entered a forced field when one is required - returning false");
		return false;
    }

	if (tareA.isForced() && !tareEntered && (isRandomWeight() || isByCountWithWeight()))	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
	{
		Debug::trace("Product", "isReadyToPrint - user hasn't entered a tare when one is required - returning false");
		return false;
	}

	if (isTotalPriceOutOfRange())
	{
		Debug::trace("Product", "isReadyToPrint - total price is invalid - returning false");
		return false;
	}

	if (coolVerifyForced && !coolTextSelected)
	{
		Debug::trace("Product", "isReadyToPrint - user hasn't selected or verified the COOL text when one is required - returning false");
		return false;
	}

	if (needsNoWeigherStatus())
	{
		Debug::trace("Product", HobString("isReadyToPrint - needsNoWeigherStatus - by count entered:") + HobString(Bool::toString(isByCountEntered())));
		if ((isByCountForced() || (scaleConfig->isSelfServiceScale() && isByCount())) && !isByCountEntered())
		{
			Debug::trace("Product", "isReadyToPrint - user hasn't entered a by count when one is required - returning false");
			return false;
		}
		else
		{
			Debug::trace("Product", "isReadyToPrint - currentProduct does not need a weigher status - returning true");
			return true;
		}
	}

	HfcWeigherStatus *currentWeight = const_cast<HfcWeigherStatus *> (currentProductTransaction_->getWeigherStatus());
	if (!currentWeight)
	{
		Debug::trace("Product", "isReadyToPrint - currentProductTransaction_ does not have a weigher status - returning true");
		return false;
	}

	if (isRandomWeight() || isByCountWithWeight())
	{
//METSIG??  Do we want to consider all of the weigher statuses and weight > minWeightToPrint to be MetSig
		Debug::trace("Product", "isReadyToPrint - is random weight or is by count with weight");
		if (!currentWeight->isWeigherInError() && currentWeight->isStableWeight() &&
			 currentProductTransaction_->getTotalNetWeight() > (HobWeight)0 &&
			 currentProductTransaction_->getTotalNetWeight() >= scaleConfig->getMinWeightToPrint())
		{
			Debug::trace("Product", "isReadyToPrint - is stable weight, with weight > 0 and > min weight to print - returning true");
			return true;
		}
		else
		{
			if (currentWeight->isWeigherInError())
				Debug::trace("Product", "isReadyToPrint - current weight is in error - returning false");
			if (!currentWeight->isStableWeight())
				Debug::trace("Product", "isReadyToPrint - current weight is not a stable weight - returning false");
			if (currentProductTransaction_->getTotalNetWeight() <= (HobWeight)0)
			{
				if (isByCountWithWeight())	//	We don't need a positive weight for this case.
					return true;
				else
				Debug::trace("Product", "isReadyToPrint - current weight is <= 0 - returning false");
			}
			if (currentProductTransaction_->getTotalNetWeight() < scaleConfig->getMinWeightToPrint())
				Debug::trace("Product", "isReadyToPrint - current weight is < min weight to print - returning false");
		}
	}
	else
	{
		Debug::trace("Product", "isReadyToPrint - not random weight and not by count with weight - returning true");
		return true;
	}

	Debug::trace("Product", "isReadyToPrint - fell through to end of method - returning false");
	return false;
}

ProductDevError Product::getNotReadyToPrintErrorCode()
{
	ProductDevError errCode = ProductErrCannotPrintLabel;

	if (isUnitPriceForced() && !isUnitPriceEntered())
		errCode = ProductErrPriceEntryRequired;
	else if ((isByCountForced() || (scaleConfig->isSelfServiceScale() && isByCount())) && !isByCountEntered())
		errCode = ProductErrByCountEntryRequired;
	else if (getTareA().isForced() && !isTareEntered())	// HT-136 There is only one Forced Tare field, shared by TareA and TareB
		errCode = ProductErrTareEntryRequired;
	else if (isTotalPriceOutOfRange())
		errCode = ProductErrTotalPriceOutOfRange;

	if (getProductType() == PRODUCTTYPE_RANDOMWEIGHT)
	{
		if (currentProductTransaction_)
		{
			const HfcWeigherStatus *currentWeigherStatus = currentProductTransaction_->getWeigherStatus();
			if (!currentWeigherStatus)
				errCode = ProductErrPrintNoWeight;
			else if (currentWeigherStatus->isWeigherInError())
				// something about kbnetweight made me change this - jhc (in Evolution->ProductManager version 248
				errCode = ProductErrInvalidWeightToPrint;
			else if (currentProductTransaction_->getTotalNetWeight() <= (HobWeight)0)
				// something about kbnetweight made me change this - jhc (in Evolution->ProductManager version 248
				errCode = ProductErrPrintNoWeight;
			else if (currentProductTransaction_->getTotalNetWeight() < scaleConfig->getMinWeightToPrint())
				errCode = ProductErrPrintBelowMinWeight;
		}
	}
/*			else if (hasOutstandingTransactions())
*/
	return errCode;
}

bool Product::hasLabelTypeError() const
{
	bool result = false;
	short printerNum;
	LabelType *myLabelType = NULL;
	Product *prod = const_cast<Product*> (this);
	for (printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
	{
		myLabelType = prod->getLabelType(printerNum);
		if ((myLabelType && ((myLabelType->getErrorCode() != RECORD_OK) || (myLabelType->isLabelMediaError() == true))) ||
			(!myLabelType && (printerNum == FirstPrinter)))
		{
			result = true;
			break;
		}
	}
	return result;
}


bool Product::hasFrequentShopperLabel() const
{
	return labelHasFrequentShopper;
}

void Product::testForFrequentShopperLabel()
{
	labelHasFrequentShopper = true;

	LabelType *label = getLabelType(FirstPrinter);
	if (!label || !label->labelHasFrequentShopper())
	{
		label = getLabelType(SecondPrinter);
		if (!label || !label->labelHasFrequentShopper())
		{
			label = getLabelType(ThirdPrinter);
			if (!label || !label->labelHasFrequentShopper())
			{
				labelHasFrequentShopper = false;
			}
		}
	}
}


void Product::getProductTextSize(int textType, int& width, int& height, std::string& fontName, int& pointSize)
{
	FieldType fieldType = UNKNOWN_LABEL_FIELD;
	switch (textType)
	{
		case psttExpandedText:   // EXPANDED_TEXT
			fieldType = EXPANDED_TEXT_1;  break;
		case psttSpecialMessage: // SPECIAL_MESSAGE
			fieldType = SPECIAL_MESSAGE;  break;
		case psttProductNote:
			break;
		case psttMarqueeText:
			break;
		case psttProductText5:
			fieldType = PRODUCT_TEXT_5;  break;
		case psttProductText6:
			fieldType = PRODUCT_TEXT_6;  break;
		case psttProductText7:
			fieldType = PRODUCT_TEXT_7;  break;
		case psttProductText8:
			fieldType = PRODUCT_TEXT_8;  break;
		case psttProductText9:
			fieldType = PRODUCT_TEXT_9;  break;
		default:
			break;
	}

	if ( getLabelFormSizeAndFont(fieldType, width, height, fontName, pointSize) )
		return;

	switch (textType)
	{
		case psttExpandedText:
			width     = scaleConfig->toIntCfgI(CfgInternal::EXPANDEDTEXTWIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::EXPANDEDTEXTHEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::EXPANDEDTEXTFONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::EXPANDEDTEXTPOINTSIZE);
			break;
		case psttSpecialMessage:
			width     = scaleConfig->toIntCfgI(CfgInternal::SPECIALMESSAGEWIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::SPECIALMESSAGEHEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::SPECIALMESSAGEFONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::SPECIALMESSAGEPOINTSIZE);
			break;
		case psttProductNote:
			width     = scaleConfig->toIntCfgI(CfgInternal::PRODUCTNOTEWIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::PRODUCTNOTEHEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODUCTNOTEFONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODUCTNOTEPOINTSIZE);
			break;
		case psttMarqueeText:
			width     = scaleConfig->toIntCfgI(CfgInternal::MARQUEETEXTWIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::MARQUEETEXTHEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::MARQUEETEXTFONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::MARQUEETEXTPOINTSIZE);
			break;
		case psttProductText5:
			width     = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT5WIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT5HEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODUCTTEXT5FONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT5POINTSIZE);
			break;
		case psttProductText6:
			width     = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT6WIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT6HEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODUCTTEXT6FONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT6POINTSIZE);
			break;
		case psttProductText7:
			width     = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT7WIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT7HEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODUCTTEXT7FONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT7POINTSIZE);
			break;
		case psttProductText8:
			width     = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT8WIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT8HEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODUCTTEXT8FONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT8POINTSIZE);
			break;
		case psttProductText9:
			width     = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT9WIDTH);
			height    = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT9HEIGHT);
			fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODUCTTEXT9FONTNAME);
			pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODUCTTEXT9POINTSIZE);
			break;
		default:
			width     = 1;
			height    = 1;
			fontName  = "";
			pointSize = 1;
			break;
	}
}


void Product::getDescriptionSize(int& width, int& height, std::string& fontName, int& pointSize)
{
	if ( getLabelFormSizeAndFont(DESCRIPTION, width, height, fontName, pointSize) )
		return;

	width     = scaleConfig->toIntCfgI(CfgInternal::PRODDESCRIPTIONWIDTH);
	height    = scaleConfig->toIntCfgI(CfgInternal::PRODDESCRIPTIONHEIGHT);
	fontName  = scaleConfig->toStringCfgI(CfgInternal::PRODDESCRIPTIONFONTNAME);
	pointSize = scaleConfig->toIntCfgI(CfgInternal::PRODDESCRIPTIONPOINTSIZE);
}


void Product::getCoolTrackingTextSize(int& width, int& height, std::string& fontName, int& pointSize)
{
	if ( getLabelFormSizeAndFont(TRACKING_TEXT, width, height, fontName, pointSize) )
		return;

	width     = scaleConfig->toIntCfgI(CfgInternal::COOLTRACKINGTEXTWIDTH);
	height    = scaleConfig->toIntCfgI(CfgInternal::COOLTRACKINGTEXTHEIGHT);
	fontName  = scaleConfig->toStringCfgI(CfgInternal::COOLTRACKINGTEXTFONTNAME);
	pointSize = scaleConfig->toIntCfgI(CfgInternal::COOLTRACKINGTEXTPOINTSIZE);
}


bool Product::getLabelFormSizeAndFont(FieldType fieldType, int& width, int& height, std::string& fontName, int& pointSize)
{
	bool foundLabelForm = true;
	LabelType* label = getLabelType(FirstPrinter);

	if ( !label || !label->getLabelFormSizeAndFont(fieldType, width, height, fontName, pointSize) )
	{
		label = getLabelType(SecondPrinter);
		if ( !label || !label->getLabelFormSizeAndFont(fieldType, width, height, fontName, pointSize) )
		{
			label = getLabelType(ThirdPrinter);
			if ( !label || !label->getLabelFormSizeAndFont(fieldType, width, height, fontName, pointSize) )
				foundLabelForm = false;
		}
	}

	if ( !foundLabelForm )
	{
		width = 0;
		height = 0;
		fontName.clear();
		pointSize = 0;

		return false;
	}

	return true;
}


// virtual, override from basic functionality in HfcObject
HobString Product::getMyKeyValue() const
{
	return HobString(Long::toString((long) productNum));
}

// virtual, override from basic functionality in HfcObject
EvolDeviceType Product::getMyEvolDevice() const
{
	return ProductDev;
}


HobRecordNum Product::getCoolCategoryNum() const
{
	return coolCategoryNum;
}
void Product::setCoolCategoryNum(HobRecordNum recordNum)
{
	if (recordNum >= 0)
		coolCategoryNum = recordNum;
	Debug::trace("Clare", "setCoolCategoryNum: " + HobString(Integer::toString((int)coolCategoryNum)));
}


HobRecordNum Product::getCoolPretextNum() const
{
	return coolPretextNum;
}
void Product::setCoolPretextNum(HobRecordNum recordNum)
{
	coolPretextNum = recordNum;
}


HobRecordNum Product::getCoolDefaultTextNum() const
{
	return coolDefaultTextNum;
}
void Product::setCoolDefaultTextNum(HobRecordNum recordNum)
{
	coolDefaultTextNum = recordNum;
}


HobRecordNum Product::getCoolTextNum() const
{
	return coolTextNum;
}
void Product::setCoolTextNum(HobRecordNum recordNum)
{
	coolTextNum = recordNum;
	coolTextSelected = true;
    coolRequiredText = true;
	Debug::trace("Product.COOL", "setCoolTextNum():  coolTextNum = " + longtostring(coolTextNum) + ",  coolTextSelected = true");
}

bool Product::isCoolTextSelected() const
{
	return coolTextSelected;
}

void Product::setCoolTextSelected(bool selected)
{
	coolTextSelected = selected;
}


HobRecordNum Product::getCoolShortlistNum() const
{
	return coolShortlistNum;
}
void Product::setCoolShortlistNum(HobRecordNum recordNum)
{
	coolShortlistNum = recordNum;
}

HobRecordNum Product::getCoolShortlistFirstLink() const
{
	return coolShortlistFirstLink;
}
void Product::setCoolShortlistFirstLink(HobRecordNum recordNum)
{
	coolShortlistFirstLink = recordNum;
}

void Product::saveCoolList(DBCoolList *dbCoolList)
{
	bool traceOn = Debug::isTracing("Product.COOL");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "saveCoolList():";

	if (!dbCoolList)
	{
		if (traceOn)
			Debug::trace("Product.COOL", msgDebug + "  dbCoolList is null!");
		return;
	}
	if (!myDbRecord)
	{
		if (traceOn)
			Debug::trace("Product.COOL", msgDebug + "  myDbRecord is null!");
		return;
	}
	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
	{
		if (traceOn)
			Debug::trace("Product.COOL", msgDebug + "  dbProductsRM is null!");
		return;
	}

	if (!dbCoolList->isNumberNull())
		setCoolShortlistNum(stringtoint(dbCoolList->getNumber()));
	if (!dbCoolList->isFirstLinkNumberNull())
		setCoolShortlistFirstLink(stringtoint(dbCoolList->getFirstLinkNumber()));


/*
	myDbRecord->setCoolList(dbCoolList);
	try
	{
		dbProductsRM->saveRecord(myDbRecord);
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "Product::saveCoolList() ********** database exception **********");
		DBException::Type t = dbe.getType();
		HobString myDebug("     DBException: type: ");
		myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
		Debug::trace("DBAccess.exception", myDebug);
	}
*/
}


const HobString &Product::getCoolTrackingText() const
{
	if (Debug::isTracing("Product.COOL"))
	{
		HobString myDebug("getCoolTrackingText():  coolTrackingText = ");
		myDebug.append(coolTrackingText);
		Debug::trace("Product.COOL", myDebug);
	}
	
	return coolTrackingText;
}
void Product::setCoolTrackingText(const HobString &theText)
{
	coolTrackingText = theText;

	if (strlen(coolTrackingText.c_str()) > 0)
		setCoolRequired(true);
	
	if (Debug::isTracing("Product.COOL"))
	{
		HobString myDebug("setCoolTrackingText(");
		myDebug.append(theText).append("):  coolTrackingText = ").append(coolTrackingText);
		Debug::trace("Product.COOL", myDebug);
	}
}

bool Product::isCoolVerifyForced() const
{
	if (Debug::isTracing("Product.COOL"))
	{
		HobString myDebug("isCoolVerifyForced():  coolVerifyForced = ");
		myDebug.append(Bool::toString(coolVerifyForced));
		Debug::trace("Product.COOL", myDebug);
	}
	return coolVerifyForced;
}
void Product::setCoolVerifyForced(bool forced)
{
	coolVerifyForced = forced;
	if (Debug::isTracing("Product.COOL"))
	{
		HobString myDebug("setCoolVerifyForced():  coolVerifyForced = ");
		myDebug.append(Bool::toString(coolVerifyForced));
		Debug::trace("Product.COOL", myDebug);
	}
}

std::string Product::getCOOLTextToPrint() const
{
	bool tracing = Debug::isTracing("Product.COOLText");
	std::string result;
	HobString msgDebug;
	if (tracing)
		msgDebug = "getCOOLTextToPrint():   ";

	if ( isCoolRequired() )
	{
		if (coolTextNum > 0 && dbAccess)
		{
			if (tracing)
				Debug::trace("Product.COOLText", msgDebug + "coolTextNum is " + HobString(Long::toString(coolTextNum)));
			if (currentCOOLText && currentCOOLText->getRecordNum() != coolTextNum)
			{
				if (tracing)
					Debug::trace("Product.COOLText", msgDebug + "coolTextNum is different than the current num, deleting current record - " + HobString(Long::toString(currentCOOLText->getRecordNum())));
				delete currentCOOLText;
				currentCOOLText = 0;
			}
			if (!currentCOOLText)
			{
				if (tracing)
                {
					Debug::trace("Product.COOLText", msgDebug + "Finding cool text record in the database");
                }
				currentCOOLText = COOLText::findRecord(coolTextNum, deptNum, dbAccess);
				if (tracing)
                {
					if (currentCOOLText)
                    {
						Debug::trace("Product.COOLText", msgDebug + "Found cool text record in the database" + HobString(Long::toString(currentCOOLText->getRecordNum())));
                    }
					else
                    {
						Debug::trace("Product.COOLText", msgDebug + "Don't have a database record for this cool text number");
                    }
                }
			}
		}

		if (!currentCOOLText)
		{
			if (tracing)
				Debug::trace("Product.COOLText", msgDebug + "Don't have a cool text record, creating a blank one ");
			currentCOOLText = new COOLText();
			currentCOOLText->setDeptNum(deptNum);
		}

		if (coolPretextNum > 0 && dbAccess)
		{
			if (tracing)
				Debug::trace("Product.COOLText", msgDebug + "coolPretextNum is " + HobString(Long::toString(coolPretextNum)));
			if (currentCOOLPretext && currentCOOLPretext->getRecordNum() != coolPretextNum)
			{
				if (tracing)
					Debug::trace("Product.COOLText", msgDebug + "coolPretextNum is different than the current num, deleting current record - " + HobString(Long::toString(currentCOOLPretext->getRecordNum())));
				delete currentCOOLPretext;
				currentCOOLPretext = 0;
			}
			if (!currentCOOLPretext)
			{
				if (tracing)
					Debug::trace("Product.COOLText", msgDebug + "Finding cool pretext record in the database");
				currentCOOLPretext = COOLPretext::findCOOLPretext(coolPretextNum, deptNum, dbAccess);
				if (tracing)
                {
					if (currentCOOLPretext)
                    {
						Debug::trace("Product.COOLText", msgDebug + "Found cool pretext record in the database" + HobString(Long::toString(currentCOOLPretext->getRecordNum())));
                    }
					else
                    {
						Debug::trace("Product.COOLText", msgDebug + "Don't have a database record for this cool pretext number");
                    }
                }
			}
		}

		if (!currentCOOLPretext)
		{
			if (tracing)
				Debug::trace("Product.COOLText", msgDebug + "Don't have a cool pretext record, creating a blank one ");
			currentCOOLPretext = new COOLPretext();
			currentCOOLPretext->setDeptNum(deptNum);
		}

		if (currentCOOLPretext && !currentCOOLPretext->getName1().empty())
			result.append(currentCOOLPretext->getName1()).append(" ");

		if (currentCOOLText && !currentCOOLText->getText().empty())
			result.append(currentCOOLText->getText());
	}
	else
	{
		result.assign("No COOL Required");
	}

	if (tracing)
		Debug::trace("Product.COOLText", msgDebug + "The text is *" + result + "*");

	return result;
}
/*
const HobString &Product::getCurrentCOOLText()
{
	bool tracing = Debug::isTracing("Product");

	if (coolTextNum > 0 && dbAccess)
	{
		if (tracing)
			Debug::trace("Product", HobString("getCurrentCOOLText():  coolTextNum is ") + HobString(Long::toString(coolTextNum)));
		if (currentCOOLText && currentCOOLText->getRecordNum() != coolTextNum)
		{
			if (tracing)
				Debug::trace("Product", HobString("getCurrentCOOLText():  coolTextNum is different than the current record num, deleting current record - ") + HobString(Long::toString(currentCOOLText->getRecordNum())));
			delete currentCOOLText;
			currentCOOLText = NULL;
		}
		if (!currentCOOLText)
		{
			if (tracing)
				Debug::trace("Product", HobString("getCurrentCOOLText():  finding cool text record in the database"));
			currentCOOLText = COOLText::findCOOLText(coolTextNum, dbAccess);
			if (tracing)
				if (currentCOOLText)
					Debug::trace("Product", HobString("getCurrentCOOLText():  found cool text record in the database") + HobString(Long::toString(currentCOOLText->getRecordNum())));
				else
					Debug::trace("Product", HobString("getCurrentCOOLText():  don't have a database record for this cool text number"));
		}
	}

	if (!currentCOOLText)
	{
		if (tracing)
			Debug::trace("Product", HobString("getCurrentCOOLText():  don't have a cool text record, creating a blank one "));
		currentCOOLText = new COOLText();
		currentCOOLText->setDeptNum(deptNum);
	}

	if (tracing)
		Debug::trace("Product", HobString("getCurrentCOOLText():  returning text *") + currentCOOLText->getText() + HobString("*"));
	return currentCOOLText->getText();
}
*/

void Product::generateEmbeddedLabels()
{
	for ( int i = 0; i < MaxPrinterTypes; ++i )
	{
		if ( labelTypes[i] )
			labelTypes[i]->generateEmbeddedLabels();
	}
}

int Product::getBackDateDays() const
{
	return backDateDays;
}
void Product::setBackDateDays(int days)
{
	backDateDays = days;
	backDateDaysEntered = true;
}

bool Product::isBackDateDaysForced() const
{
	return backDateDaysForced;
}
void Product::setBackDateDaysForced(bool forced)
{
	backDateDaysForced = forced;
	if (backDateDaysForced)
	{
		backDateDays = 0;
		backDateDaysEntered = false;
	}
}

HobDateTime Product::getBackdateDate()
{
	setBackdateDate();
	Debug::trace("Product.lifeDates", "getBackdateDate() = [" + HobString(backdateDate.formatDate()) + "]");
	return backdateDate;
}

void Product::setBackdateDate() const
{
	backdateDate.setToCurrent();
	backdateDate.incrementDays(-backDateDays);
	if (Debug::isTracing("Product.lifeDates"))
	{
		HobString msgDebug = "setBackdateDate():  ";
		msgDebug.append("backDateDays = ").append(Integer::toString(backDateDays));
		Debug::trace("Product.lifeDates", msgDebug);
	}
}

void Product::setBackdateDateTo(const HobString &newDate)
{
	bool traceOn = Debug::isTracing("Product.lifeDates");
	HobString msgDebug;
	HobDateTime newBackdateDate;
	newBackdateDate.setDateParts(newDate);
	if (traceOn)
	{
		msgDebug = "setBackdateDateTo(" + HobString(newBackdateDate.formatDate()) + ")";
		Debug::trace("Product.lifeDates", msgDebug);
		msgDebug.append(":  ");
	}
	if (!newBackdateDate.isValid())
	{
		if (traceOn)
			Debug::trace("Product.lifeDates", msgDebug + "Date is invalid! Exiting.");
		return;
	}

	HobDateTime now;
	long nowJulian = now.getJulianDay();
	long newJulian = newBackdateDate.getJulianDay();
	int deltaDays = (int) (nowJulian - newJulian);
	// The user can't set the backdated packed on date to a date after today!!!
	if (deltaDays < 0)
	{
		if (traceOn)
			Debug::trace("Product.lifeDates", msgDebug + "Date is in the future! Will not use. Exiting.");
		return;
	}
	setBackDateDays(deltaDays);
}

bool Product::isBackDateDaysEntered() const
{
	return backDateDaysEntered;
}
void Product::setBackDateDaysEntered(bool entered)
{
	backDateDaysEntered = entered;
}


bool Product::isCoolRequired() const
{
	return coolRequired;
}

void Product::setCoolRequired(bool value)
{
	coolRequired = value;
}


short Product::getLabelBeingPrinted() const
{
	return labelBeingPrinted;
}
void Product::setLabelBeingPrinted(short whichLabel)
{
	labelBeingPrinted = whichLabel;
}

// The Product object takes ownership of the LabelType pointer being passed in.
void Product::setExpirationLabel( LabelType *value )
{
	Util::CLockCriticalSection lock( labelTypesToDeleteCS );

	if ( shelfLifeExpirationLabelType ) {
		labelTypesToDelete.push_back( shelfLifeExpirationLabelType );
		//delete shelfLifeExpirationLabelType;
	}

	shelfLifeExpirationLabelType = value;
}

// The Product object takes ownership of the LabelType pointer being passed in.
void Product::setRunTotalsLabel( LabelType *value )
{
	Util::CLockCriticalSection lock( labelTypesToDeleteCS );
	if ( runTotalsLabelType ) {
		labelTypesToDelete.push_back( runTotalsLabelType );
		//delete runTotalsLabelType;
	}
		
	runTotalsLabelType = value;
}


const HobString &Product::getRawBarCodeData() const
{
	return rawBarCodeData;
}
void Product::setRawBarCodeData(const HobString &rbcd)
{
	rawBarCodeData = rbcd;
	if (Debug::isTracing("Product.BarCode"))
	{
		HobString myDebug("setRawBarCodeData():   rawBarCodeData = [");
		myDebug.append(rawBarCodeData).append("]");
		Debug::trace("Product.BarCode", myDebug);
	}
}


bool Product::isNumOfLabelsForced() const
{
	return numOfLabelsForced;
}
void Product::setNumOfLabelsForced(bool forced)
{
	numOfLabelsForced = forced;
	numOfLabelsEntered = (numOfLabelsForced) ? false : true;
}


bool Product::isNumOfLabelsEntered() const
{
	return numOfLabelsEntered;
}
void Product::setNumOfLabelsEntered(bool entered)
{
	numOfLabelsEntered = entered;
}


Product::ProductionPlanningType Product::getProductionPlanningType() const
{
	return productionPlanningType;
}
void Product::setProductionPlanningType(ProductionPlanningType type)
{
	if ( PRODUCTIONPLANNING_NO <= type && type < PRODUCTIONPLANNING_INVALID )
		productionPlanningType = type;
}



HobWeight Product::getKeyboardNetWeight() const
{
	return keyboardNetWeight;
}
void Product::setKeyboardNetWeight(HobWeight weight)
{
	keyboardNetWeight = weight;
}


bool Product::hasKbdNetWeightLabel() const
{
	return acceptKbdNetWeight & !cancelledKbdNetWeight;
}

void Product::testForKbdNetWeightLabel()
{
	acceptKbdNetWeight = true;

	LabelType *label = getLabelType(FirstPrinter);
	if (!label || !label->labelHasKbdNetWeight())
	{
		label = getLabelType(SecondPrinter);
		if (!label || !label->labelHasKbdNetWeight())
		{
			label = getLabelType(ThirdPrinter);
			if (!label || !label->labelHasKbdNetWeight())
			{
				acceptKbdNetWeight = false;
			}
		}
	}
}

void Product::testForPortionsOnLabel()
{
	portionsOnLabel = true;

	LabelType *label = getLabelType(FirstPrinter);
	if (!label || !label->labelHasPortions())
	{
		label = getLabelType(SecondPrinter);
		if (!label || !label->labelHasPortions())
		{
			label = getLabelType(ThirdPrinter);
			if (!label || !label->labelHasPortions())
			{
				portionsOnLabel = false;
			}
		}
	}
	Debug::trace("Portions", "Product::testForPortionsOnLabel() = " + HobString(Bool::toString(portionsOnLabel)));
}

void Product::checkSpecialFieldsOnLabel()
{
	Debug::trace("Portions", "Product::checkSpecialFieldsOnLabel()");
	//	This method will check to see if certain fields are on the label.
	testForKbdNetWeightLabel();		//	FORCED_KB_WEIGHT
	testForPortionsOnLabel();		//	PORTION_COUNT
}

int Product::findOffsetInUIFieldList(int fieldOffset)
{
	int returnVal = -1;
	int loop = 0;

	std::list<ProductFieldOffsets>::iterator fieldIter = uiFieldList.begin();
	while ((returnVal == -1) && (fieldIter != uiFieldList.end()))
	{
		if (fieldOffset == *fieldIter)
			returnVal = loop;
		else
			++loop;

		fieldIter++;
	}
	return returnVal;
}

bool Product::isPrintingInProductionEntryMode() const
{
	return printingInProductionEntryMode;
}
void Product::setPrintingInProductionEntryMode(bool printing)
{
	Debug::trace("Product", HobString( "set printing in prod entry mode = " ) + Bool::toString(printing) + HobString(" was ") + Bool::toString(printingInProductionEntryMode));
	printingInProductionEntryMode = printing;
}


bool Product::isExpirationDaysForced() const
{
	return expirationDaysForced;
}
void Product::setExpirationDaysForced(bool forced)
{
	expirationDaysForced = forced;
	if (expirationDaysForced)
		expirationDaysEntered = false;
}

int Product::getExpirationDays() const
{
	return expirationDays;
}
void Product::setExpirationDays(int days)
{
	expirationDays = days;
	expirationDaysEntered = true;
}


HobRecordNum Product::getMealSideItemsGroupNum() const
{
	return mealSideItemsGroupNum;
}
void Product::setMealSideItemsGroupNum(HobRecordNum groupNum)
{
	mealSideItemsGroupNum = groupNum;
}

bool Product::getMealSideItemsAreFree() const
{
	return mealSideItemsAreFree;
}
void Product::setMealSideItemsAreFree(bool free)
{
	mealSideItemsAreFree = free;
}

int Product::getMealNumSidesIncluded() const
{
	return mealNumSidesIncluded;
}
void Product::setMealNumSidesIncluded(int num)
{
	mealNumSidesIncluded = num;
}

bool Product::getItemIsMealSideItem() const
{
	return itemIsMealSideItem;
}
void Product::setItemIsMealSideItem(bool isSideItem)
{
	itemIsMealSideItem = isSideItem;
}

HobRecordNum Product::getCutTestMeatSpecSheetGroupNum() const
{
	return cutTestMeatSpecSheetGroupNum;
}
void Product::setCutTestMeatSpecSheetGroupNum(HobRecordNum groupNum)
{
	cutTestMeatSpecSheetGroupNum = groupNum;
}

// For Pick-5 project
HobWeight Product::getWeightMin() const
{
	return weightMin;
}
void Product::setWeightMin(HobWeight w)
{
	weightMin = w;
}

HobWeight Product::getWeightMax() const
{
	return weightMax;
}
void Product::setWeightMax(HobWeight w)
{
	weightMax = w;
}

HobPrice Product::getTotalPriceMin() const
{
	return totalPriceMin;
}
void Product::setTotalPriceMin(HobPrice p)
{
	totalPriceMin = p;
}

HobPrice Product::getTotalPriceMax() const
{
	return totalPriceMax;
}
void Product::setTotalPriceMax(HobPrice p)
{
	totalPriceMax = p;
}


const std::string& Product::getProductTypeAsString(ProductType type)
{
	switch ( type )
	{
		case PRODUCTTYPE_RANDOMWEIGHT:     return _PRODUCTTYPE_RANDOMWEIGHT_Name_;
		case PRODUCTTYPE_FIXEDWEIGHT:      return _PRODUCTTYPE_FIXEDWEIGHT_Name_;
		case PRODUCTTYPE_BYCOUNT:          return _PRODUCTTYPE_BYCOUNT_Name_;
		case PRODUCTTYPE_FLUIDOUNCE:       return _PRODUCTTYPE_FLUIDOUNCE_Name_;
		default:                           return _PRODUCTTYPE_UNKNOWN_Name_;
	}
}


const std::string& Product::getProductionPlanningTypeAsString(ProductionPlanningType type)
{
	switch ( type )
	{
		case PRODUCTIONPLANNING_NO:						return _PRODUCTIONPLANNING_NO_Name_;
		case PRODUCTIONPLANNING_ITEM:					return _PRODUCTIONPLANNING_ITEM_Name_;
		case PRODUCTIONPLANNING_INGREDIENT:				return _PRODUCTIONPLANNING_INGREDIENT_Name_;
		case PRODUCTIONPLANNING_MIXED:					return _PRODUCTIONPLANNING_MIXED_Name_;
		case PRODUCTIONPLANNING_RECIPES_SUGG_SELL:      return _PRODUCTIONPLANNING_RECIPES_SUGG_SELL_Name_;
		default:										return _PRODUCTIONPLANNING_INVALID_Name_;
	}
}


Configuration *Product::getScaleConfig() const
{
	return scaleConfig;
}
void Product::setScaleConfig(Configuration *config)
{
	scaleConfig = config;
}
