#include "HourlyTotalTags.h"


#include "HourlyTotal.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int HourlyTotalTags::dataType = 0;
	std::string HourlyTotalTags::dataTypeName("hfc::HourlyTotalTags"); 
#endif


	const std::string HourlyTotalTags::_hourNum_        = "xh";


	bool     HourlyTotalTags::_initialized_ = false;
	TagsList HourlyTotalTags::_hourlyTotalTags_(HourlyTotal::endHourlyTotalFieldOffsets);
	TagsMap  HourlyTotalTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	HourlyTotalTags::HourlyTotalTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		if ( !_initialized_ )
		{
			_hourlyTotalTags_[0]                                              = emptyTag;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetTotalWeight]            = _totalWeight_;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetTotalPrice]             = _totalPrice_;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetTotalPackages]          = _totalPackages_;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetTotalRuns]              = _totalRuns_;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetTotalItems]             = _totalItems_;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetTotalFlags]             = _totalFlags_;
			_hourlyTotalTags_[HourlyTotal::fieldOffsetHourlyTotalNum]         = _hourNum_;

			_initialized_ = true;
		}

		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	HourlyTotalTags::~HourlyTotalTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}
