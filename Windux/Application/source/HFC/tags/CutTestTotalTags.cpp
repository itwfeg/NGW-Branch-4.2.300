#include "CutTestTotalTags.h"


#include "CutTestTotal.h"

#include "CuttingTestTags.h"
#include "ClassTags.h"
#include "ProductTags.h"
#include "TransactionTypeTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
int CutTestTotalTags::dataType = 0;
std::string CutTestTotalTags::dataTypeName("hfc::CutTestTotalTags"); 
#endif


bool     CutTestTotalTags::_initialized_ = false;
TagsList CutTestTotalTags::_totalTags_(CutTestTotal::endCutTestTotalFieldOffsets);
TagsMap  CutTestTotalTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


CutTestTotalTags::CutTestTotalTags(FieldOffsetsList * pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_initialized_ )
	{
		_totalTags_[0]                                                = emptyTag;
		_totalTags_[CutTestTotal::fieldOffsetTotalWeight]             = _totalWeight_;
		_totalTags_[CutTestTotal::fieldOffsetTotalPrice]              = _totalPrice_;
		_totalTags_[CutTestTotal::fieldOffsetTotalPackages]           = _totalPackages_;
		_totalTags_[CutTestTotal::fieldOffsetTotalRuns]               = _totalRuns_;
		_totalTags_[CutTestTotal::fieldOffsetTotalItems]              = _totalItems_;
		_totalTags_[CutTestTotal::fieldOffsetTotalFlags]              = _totalFlags_;
		_totalTags_[CutTestTotal::fieldOffsetTotalCutTestNum]         = CuttingTestTags::cuttingTestNum;
		_totalTags_[CutTestTotal::fieldOffsetTotalProductNum]         = ProductTags::prodNumber;
		_totalTags_[CutTestTotal::fieldOffsetTotalTransactionTypeNum] = _transactionTypeNumber_;

		_initialized_ = true;
	}

	if ( pFieldList )
		putTagsInMap(pFieldList);
}


CutTestTotalTags::~CutTestTotalTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

}
