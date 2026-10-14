#include "OperatorChangeTags.h"


#include "OperatorChange.h"
#include "HobOperatorTags.h"
#include "ProductTags.h"
#include "TotalsTags.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int OperatorChangeTags::dataType = 0;
	std::string OperatorChangeTags::dataTypeName("hfc::OperatorChangeTags"); 
#endif


	const std::string OperatorChangeTags::_changedField_        = "tc";
	const std::string OperatorChangeTags::_newValue_            = "nv";
	const std::string OperatorChangeTags::_oldValue_            = "ov";
	const std::string OperatorChangeTags::_changeDate_          = "d_";
	const std::string OperatorChangeTags::_changeTime_          = "t_";
	const std::string OperatorChangeTags::_changeDateTime_      = "oD";

	TagsList OperatorChangeTags::_totalTags_(OperatorChange::endFieldOffsets);
	TagsMap  OperatorChangeTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	OperatorChangeTags::OperatorChangeTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( pFieldList )
		{
			//initialize the table of tags for any Totals class
			_totalTags_[0]                                            = emptyTag;
			_totalTags_[OperatorChange::fieldOffsetOperatorId]        = HobOperatorTags::opNumber;
			_totalTags_[OperatorChange::fieldOffsetOperatorRFID]      = HobOperatorTags::opRFID;
			_totalTags_[OperatorChange::fieldOffsetProductNumber]     = ProductTags::prodNumber;
			_totalTags_[OperatorChange::fieldOffsetChangedField]      = _changedField_;
			_totalTags_[OperatorChange::fieldOffsetNewValue]          = _newValue_;
			_totalTags_[OperatorChange::fieldOffsetOldValue]          = _oldValue_;
			_totalTags_[OperatorChange::fieldOffsetDate]              = _changeDate_;
			_totalTags_[OperatorChange::fieldOffsetTime]              = _changeTime_;
			_totalTags_[OperatorChange::fieldOffsetDateTime]          = _changeDateTime_;
			_totalTags_[OperatorChange::fieldOffsetTransactionType]   = TotalsTags::_transactionTypeNumber_;
			_totalTags_[OperatorChange::fieldOffsetTotalWeight]       = TotalsTags::_totalWeight_;
			_totalTags_[OperatorChange::fieldOffsetTotalPrice]        = TotalsTags::_totalPrice_;

			putTagsInMap(pFieldList);
		}
	}


	OperatorChangeTags::~OperatorChangeTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
