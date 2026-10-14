#include "OperatorTotalTags.h"


#include "OperatorTotal.h"

#include "HobOperatorTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int OperatorTotalTags::dataType = 0;
	std::string OperatorTotalTags::dataTypeName("hfc::OperatorTotalTags"); 
#endif


	bool     OperatorTotalTags::_initialized_ = false;
	TagsList OperatorTotalTags::_operatorTotalTags_(OperatorTotal::endOperatorTotalFieldOffsets);
	TagsMap  OperatorTotalTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	OperatorTotalTags::OperatorTotalTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( !_initialized_ )
		{
			_operatorTotalTags_[0]                                                = emptyTag;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalWeight]            = _totalWeight_;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalPrice]             = _totalPrice_;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalPackages]          = _totalPackages_;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalRuns]              = _totalRuns_;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalItems]             = _totalItems_;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalFlags]             = _totalFlags_;
			_operatorTotalTags_[OperatorTotal::fieldOffsetTotalOperatorNum]       = HobOperatorTags::opNumber;

			_initialized_ = true;
		}

		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	OperatorTotalTags::~OperatorTotalTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
