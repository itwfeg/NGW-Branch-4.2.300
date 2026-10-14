#include "ProductTotalTags.h"


#include "ProductTotal.h"

#include "ProductTags.h"
#include "ClassTags.h"
#include "DeptTags.h"
#include "TransactionTypeTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int ProductTotalTags::dataType = 0;
	std::string ProductTotalTags::dataTypeName("hfc::ProductTotalTags"); 
#endif


	bool     ProductTotalTags::_initialized_ = false;
	TagsList ProductTotalTags::_totalTags_(ProductTotal::endProductTotalFieldOffsets);
	TagsMap  ProductTotalTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	ProductTotalTags::ProductTotalTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( !_initialized_ )
		{
			_totalTags_[0]                                                = emptyTag;
			_totalTags_[ProductTotal::fieldOffsetTotalWeight]             = _totalWeight_;
			_totalTags_[ProductTotal::fieldOffsetTotalPrice]              = _totalPrice_;
			_totalTags_[ProductTotal::fieldOffsetTotalPackages]           = _totalPackages_;
			_totalTags_[ProductTotal::fieldOffsetTotalRuns]               = _totalRuns_;
			_totalTags_[ProductTotal::fieldOffsetTotalItems]              = _totalItems_;
			_totalTags_[ProductTotal::fieldOffsetTotalFlags]              = _totalFlags_;
			_totalTags_[ProductTotal::fieldOffsetTotalProductNum]         = ProductTags::prodNumber;
			_totalTags_[ProductTotal::fieldOffsetTotalTransactionTypeNum] = _transactionTypeNumber_;
			_totalTags_[ProductTotal::fieldOffsetTotalReportingClassNum]  = ClassTags::reportingClassNumber;
			_totalTags_[ProductTotal::fieldOffsetTotalDeptNum]            = DeptTags::deptNum;

			_initialized_ = true;
		}

		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	ProductTotalTags::~ProductTotalTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
