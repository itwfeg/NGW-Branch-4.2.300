#include "ClassTotalTags.h"


#include "ClassTotal.h"

#include "ClassTags.h"
#include "TransactionTypeTags.h"
#include "DeptTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int ClassTotalTags::dataType = 0;
	std::string ClassTotalTags::dataTypeName("hfc::ClassTotalTags"); 
#endif


	bool     ClassTotalTags::_initialized_ = false;
	TagsList ClassTotalTags::_classTotalTags_(ClassTotal::endClassTotalFieldOffsets);
	TagsMap  ClassTotalTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	ClassTotalTags::ClassTotalTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( !_initialized_ )
		{
			_classTotalTags_[0]                                              = emptyTag;
			_classTotalTags_[ClassTotal::fieldOffsetTotalWeight]             = _totalWeight_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalPrice]              = _totalPrice_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalPackages]           = _totalPackages_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalRuns]               = _totalRuns_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalItems]              = _totalItems_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalFlags]              = _totalFlags_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalClassNum]           = ClassTags::classNumber;
			_classTotalTags_[ClassTotal::fieldOffsetTotalTransactionTypeNum] = _transactionTypeNumber_;
			_classTotalTags_[ClassTotal::fieldOffsetTotalReportingClassNum]  = ClassTags::reportingClassNumber;
			_classTotalTags_[ClassTotal::fieldOffsetTotalDeptNum]            = DeptTags::deptNum;

			_initialized_ = true;
		}

		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	ClassTotalTags::~ClassTotalTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
