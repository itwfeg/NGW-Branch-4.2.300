/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTAGS_H
#define PRODUCTTAGS_H

#include "Tags.h"

class ProductTags : public Tags
{
	public:
		ProductTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductTags();

		//ultima and Evolution defined tags
		static const std::string prodNumber;
		static const std::string prodType;
		static const std::string prodDescription;

		//static const std::string prodBarType;
		static const std::string prodBarPrefix;
		static const std::string prodUpcNum;					 
		static const std::string prodProductNumber;
		static const std::string prodVendorNumber;
		static const std::string prodBarEanCode;

		static const std::string prodByCount;
		static const std::string prodByCountForced;
		static const std::string prodExceptPrice;
		static const std::string prodExceptPriceTare;
		static const std::string prodUnitPrice;
		static const std::string prodUnitPriceForced;

		static const std::string prodDiscountType;
		static const std::string prodDiscount1;
		static const std::string prodDiscount2;
		static const std::string prodDiscount3;

		static const std::string prodTareA;
		static const std::string prodTareB;
		static const std::string prodTareAForced;
		static const std::string prodProportionalTare;
		static const std::string prodFixedWeight;

		static const std::string prodShelfLife;
		static const std::string prodShelfLifeHours;
		static const std::string prodShelfLifeMinutes;

		static const std::string prodDontPrintSellBy;

		static const std::string prodProdLife;
		static const std::string prodProdLifeHours;
		static const std::string prodProdLifeMinutes;
		
		static const std::string prodDontPrintBestBefore;
		static const std::string prodDontPrintPackedOn;

		static const std::string prodApplySecurityLabel;

		static const std::string prodPriceModifier;

		static const std::string prodLabelType1;
		static const std::string prodLabelRotate1;
		static const std::string prodLabelPlacement1;
		static const std::string prodLabelType2;
		static const std::string prodLabelRotate2;
		static const std::string prodLabelPlacement2;
		static const std::string prodLabelType3;
		static const std::string prodLabelRotate3;
		static const std::string prodLabelPlacement3;

		//static const std::string prodPortions;
		static const std::string prodPoints;
	
		static const std::string prodIntakeMode;

		static const std::string graphicNumber;
		static const std::string prodGraphicName1;
		static const std::string prodGraphicName2;
		static const std::string prodGraphicName3;
		static const std::string prodGraphicName4;

		static const std::string pluFlags;
		static const std::string totalsFlags;

#ifndef NOTSELFSERVICE
		static const std::string productGraphicName;
		static const std::string prodName1;
		static const std::string prodName2;
		static const std::string specialStartDate;
#endif

		static const std::string eANFormatString;		// Product Defaults only
		static const std::string eAN128CFormatString;		// Product Defaults only

		static const std::string gS1GTINFormatString;		// Product Defaults only
		static const std::string gS1AIFormatString;			// Product Defaults only

		static const std::string qrCodeFormatString;		// Product Defaults only
		static const std::string dataMatrixFormatString;    // Product Defaults only

		static const std::string eANprefix;				// Product Defaults only

		// COOL related tags, others come from the COOLTags class
		static const std::string defaultCOOLTextNum;
		static const std::string currentCOOLTextNum;
		static const std::string verifyCOOLText;
		static const std::string coolInfoRequired;
		static const std::string coolTrackingText;

		static const std::string productionPlanningType;

		/* The following tags are defined in their corresponding tags definition class.
		prodReportClass - defined in ClassTags.h
		expandedTextNumber - defined in ExpandedTextTags.h
		specialTextNumber - defined in SpecialMessageTags.h
		productNoteNumber - defined in ProductNotesTags.h
		nutrifactNumber - defined in NutrifactsDataTags.h
		categoryNumber - defined in CategoryTags.h
		specialNumber - defined in SpecialsTags.h
		*/

		static const std::string buildAMealSideItemsGroupNum;
		static const std::string buildAMealSideItemsAreFree;
		static const std::string buildAMealNumberOfSidesIncluded;

		static const std::string cutTestMeatSpecSheetGroupNum;

		// For Pick-5 project
		static const std::string weightMin;
		static const std::string weightMax;
		static const std::string totalPriceMin;
		static const std::string totalPriceMax;

		// Automatic labels based on mode
		static const std::string manualLabelTypeNum1;
		static const std::string manualLabelTypeNum2;
		static const std::string manualLabelTypeNum3;
		static const std::string prepackLabelTypeNum1;
		static const std::string prepackLabelTypeNum2;
		static const std::string prepackLabelTypeNum3;
		static const std::string prodEntryLabelTypeNum1;
		static const std::string prodEntryLabelTypeNum2;
		static const std::string prodEntryLabelTypeNum3;
		static const std::string selfServLabelTypeNum1;
		static const std::string selfServLabelTypeNum2;
		static const std::string selfServLabelTypeNum3;
		static const std::string pick5LabelTypeNum1;
		static const std::string pick5LabelTypeNum2;
		static const std::string pick5LabelTypeNum3;
		static const std::string onlineOrderingLabelTypeNum1;
		static const std::string onlineOrderingLabelTypeNum2;
		static const std::string onlineOrderingLabelTypeNum3;

	protected:
		const TagsList &getMyTagsList() const {return productTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList productTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTTAGS_H
