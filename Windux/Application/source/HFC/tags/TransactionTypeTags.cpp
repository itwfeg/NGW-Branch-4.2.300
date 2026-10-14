#include "TransactionTypeTags.h"


//#include "TransactionType.h"
#include "Debug.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif


namespace hfc
{


#ifdef USE_ACCOUNTANT
	int TransactionTypeTags::dataType = 0;
	std::string TransactionTypeTags::dataTypeName("hfc::TransactionTypeTags"); 
#endif


	const std::string TransactionTypeTags::transactionTypeNumber = "tk";
	const std::string TransactionTypeTags::transactionTypeName1  = "tn";
	const std::string TransactionTypeTags::transactionTypeName2  = "tN";
	const std::string TransactionTypeTags::grossWeight           = "tw";     // New (for this data type) for Evolution.
	const std::string TransactionTypeTags::upcChangeDigit        = "uc";
	const std::string TransactionTypeTags::reportsToPrepack      = "rp";     // 16 bit flag value
	const std::string TransactionTypeTags::manualChangeDigit     = "mc";     // New for Evolution.
	const std::string TransactionTypeTags::autoChangeDigit       = "ac";     // New for Evolution.
	const std::string TransactionTypeTags::allowProductBackDate  = "bd";     // New for Evolution.
	const std::string TransactionTypeTags::useBackgroundColor    = "TB";
	const std::string TransactionTypeTags::backgroundColor       = "Tb";
	const std::string TransactionTypeTags::textForLabel          = "tl";
	const std::string TransactionTypeTags::transactionTypeFlag   = "nf";


	bool     TransactionTypeTags::_initialized_ = false;
	TagsList TransactionTypeTags::_transactionTypeTags_(endTransactionTypeFieldOffsets);
	TagsMap  TransactionTypeTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	TransactionTypeTags::TransactionTypeTags(FieldOffsetsList *pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( !_initialized_ )
		{
			//initialize the the table of tags for the TransactionType class
			_transactionTypeTags_[0]                                                   = emptyTag;
			_transactionTypeTags_[fieldOffsetTransactionTypeNum]                       = transactionTypeNumber; 
			_transactionTypeTags_[fieldOffsetTransactionTypeErrorCode]                 = emptyTag;
			_transactionTypeTags_[fieldOffsetTransactionTypeName1]                     = transactionTypeName1;
			_transactionTypeTags_[fieldOffsetTransactionTypeName2]                     = transactionTypeName2;
			_transactionTypeTags_[fieldOffsetTransactionTypeGrossWeight]               = grossWeight;
			_transactionTypeTags_[fieldOffsetTransactionTypeUpcChangeDigit]            = upcChangeDigit;
			_transactionTypeTags_[fieldOffsetTransactionTypeAffectsProduction]         = reportsToPrepack;
			_transactionTypeTags_[fieldOffsetTransactionTypeManualChangeUPCDigit]      = manualChangeDigit;
			_transactionTypeTags_[fieldOffsetTransactionTypeAutomaticChangeUPCDigit]   = autoChangeDigit;
			_transactionTypeTags_[fieldOffsetTransactionTypeAllowProductBackDating]    = allowProductBackDate;
			_transactionTypeTags_[fieldOffsetTransactionTypeUseBackgroundColor]        = useBackgroundColor;
			_transactionTypeTags_[fieldOffsetTransactionTypeBackgroundColor]           = backgroundColor;
			_transactionTypeTags_[fieldOffsetTransactionTypeTextForLabel]              = textForLabel;
			_transactionTypeTags_[fieldOffsetTransactionTypeFlags]                     = transactionTypeFlag;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	TransactionTypeTags::~TransactionTypeTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
