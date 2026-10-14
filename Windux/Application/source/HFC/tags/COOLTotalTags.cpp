#include "COOLTotalTags.h"


#include "CoolTotal.h"

#include "ProductTags.h"
#include "TransactionTags.h"
#include "COOLTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int COOLTotalTags::dataType = 0;
	std::string COOLTotalTags::dataTypeName("hfc::COOLTotalTags"); 
#endif


	const std::string COOLTotalTags::_firstLogDate_        = "ck";
	const std::string COOLTotalTags::_firstLogTime_        = "cp";
	const std::string COOLTotalTags::_lastLogDate_         = "cl";
	const std::string COOLTotalTags::_lastLogtime_         = "cn";


	bool     COOLTotalTags::_initialized_ = false;
	TagsList COOLTotalTags::_totalTags_(COOLTotal::endCOOLTotalFieldOffsets);
	TagsMap  COOLTotalTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	COOLTotalTags::COOLTotalTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( !_initialized_ )
		{
			_totalTags_[0]                                               = emptyTag;
			_totalTags_[COOLTotal::fieldOffsetTotalProductNum]           = ProductTags::prodNumber;
			_totalTags_[COOLTotal::fieldOffsetTotalTransactionTypeNum]   = _transactionTypeNumber_;
			_totalTags_[COOLTotal::fieldOffsetTotalCOOLFirstLogDate]     = _firstLogDate_;
			_totalTags_[COOLTotal::fieldOffsetTotalCOOLFirstLogTime]     = _firstLogTime_;
			_totalTags_[COOLTotal::fieldOffsetTotalCOOLLastLogDate]      = _lastLogDate_;
			_totalTags_[COOLTotal::fieldOffsetTotalCOOLLastLogTime]      = _lastLogtime_;
			_totalTags_[COOLTotal::fieldOffsetTotalCOOLText]             = COOLTags::coolTextText;
			_totalTags_[COOLTotal::fieldOffsetTotalCOOLTrackingText]     = ProductTags::coolTrackingText;
			_totalTags_[COOLTotal::fieldOffsetTotalWeight]               = _totalWeight_;
			_totalTags_[COOLTotal::fieldOffsetTotalPrice]                = _totalPrice_;
			_totalTags_[COOLTotal::fieldOffsetTotalPackages]             = _totalPackages_;
			_totalTags_[COOLTotal::fieldOffsetTotalRuns]                 = _totalRuns_;
			_totalTags_[COOLTotal::fieldOffsetTotalItems]                = _totalItems_;

			_initialized_ = true;
		}

		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	COOLTotalTags::~COOLTotalTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
