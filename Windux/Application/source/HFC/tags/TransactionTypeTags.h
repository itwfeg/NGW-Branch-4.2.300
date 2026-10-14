#ifndef TRANSACTIONTYPETAGS_H
#define TRANSACTIONTYPETAGS_H



#include "Tags.h"



namespace hfc
{


	class TransactionTypeTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetTransactionTypeNum = 1,
				fieldOffsetTransactionTypeErrorCode,
				fieldOffsetTransactionTypeName1,
				fieldOffsetTransactionTypeName2,
				fieldOffsetTransactionTypeGrossWeight,
				fieldOffsetTransactionTypeUpcChangeDigit,
				fieldOffsetTransactionTypeAffectsProduction,
				fieldOffsetTransactionTypeManualChangeUPCDigit,
				fieldOffsetTransactionTypeAutomaticChangeUPCDigit,
				fieldOffsetTransactionTypeAllowProductBackDating,
				fieldOffsetTransactionTypeFlags,
				fieldOffsetTransactionTypeUseBackgroundColor,
				fieldOffsetTransactionTypeBackgroundColor,
				fieldOffsetTransactionTypeTextForLabel,

				endTransactionTypeFieldOffsets
			} TransactionTypeFieldOffsets;


			/* --- Constructors and destructor --- */
			TransactionTypeTags(FieldOffsetsList * pFieldList = 0);
			~TransactionTypeTags();


			/* --- Instance function members --- */


			/* --- Class data members --- */
			static const std::string transactionTypeNumber;
			static const std::string transactionTypeName1;
			static const std::string transactionTypeName2;
			static const std::string grossWeight;
			static const std::string upcChangeDigit;
			static const std::string reportsToPrepack;
			static const std::string manualChangeDigit;
			static const std::string autoChangeDigit;
			static const std::string allowProductBackDate;
			static const std::string transactionTypeFlag;
			static const std::string useBackgroundColor;
			static const std::string backgroundColor;
			static const std::string textForLabel;


		protected:
			const TagsList & getMyTagsList() const { return _transactionTypeTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _transactionTypeTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif	//TRANSACTIONTYPETAGS_H
