#ifndef TRANSACTIONTAGS_H
#define TRANSACTIONTAGS_H



#include "Tags.h"



namespace hfc
{


	class TransactionTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			TransactionTags(FieldOffsetsList * pFieldList = 0);
			~TransactionTags();


			/* --- Instance function members --- */


			/* --- Class data members --- */
			static const std::string _scaleId_;
			static const std::string _transactionId_;
			static const std::string _transactionGroup_;
			static const std::string _transactionType_;
			static const std::string _transactionCommittedDateTime_;
			static const std::string _transactionCommittedDate_;
			static const std::string _transactionCommittedTime_;
			static const std::string _transactionCommittedDayWeek_;
			static const std::string _transactionVoid_;
			static const std::string _transactionAffectsProduction_;
			static const std::string _transactionReadAndClear_;
			static const std::string _transactionPackagesNumber_;
			static const std::string _transactionPrintedSecurityLabel_;
			static const std::string _transactionCOOLFlag_;
			static const std::string _transactionTotalValue_;
			static const std::string _transactionOldTotalValue_;
			static const std::string _productFinalPrice_;
			static const std::string _productFinalTare_;
			static const std::string _productFinalNetWeight_;
			static const std::string _productFixedWeight_;
			//static const std::string _productShelfLifeMinutes_;
			//static const std::string _productLifeMinutes_;
			static const std::string _productLabel4_;
			static const std::string _productOldBarcodeDigit_;
			static const std::string _productOldUnitPrice_;
			static const std::string _productOldFinalPrice_;
			static const std::string _productOldTare_;
			static const std::string _productOldByCount_;
			static const std::string _productOldFixedWeight_;
			static const std::string _productOldShelfLifeDays_;
			static const std::string _productOldShelfLifeHours_;
			static const std::string _productOldShelfLifeMinutes_;
			static const std::string _productOldProductLifeDays_;
			static const std::string _productOldProductLifeHours_;
			static const std::string _productOldProductLifeMinutes_;
			static const std::string _productOldLabel1_;
			static const std::string _productOldLabel2_;
			static const std::string _productOldLabel3_;
			static const std::string _productOldLabel4_;
			static const std::string _productOldGraphicName1_;
			static const std::string _productOldGraphicName2_;
			static const std::string _productOldGraphicName3_;
			static const std::string _productOldGraphicName4_;
			static const std::string _operatorId_;
			static const std::string _operatorName_;
			static const std::string _operatorChangeType_;
			static const std::string _cutTestPercentOfPrimalWeight_;
			static const std::string _cutTestPercentOfPrimalCost_;
            static const std::string _modeChange_;
			static const std::string _sweetheartWeight_;


		protected:
			const TagsList & getMyTagsList() const { return _transactionTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _transactionTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};

}



#endif /* TRANSACTIONTAGS_H */
