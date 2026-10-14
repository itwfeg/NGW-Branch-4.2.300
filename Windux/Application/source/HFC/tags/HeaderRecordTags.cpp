#include "HeaderRecordTags.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int HeaderRecordTags::_dataType_ = 0;
		std::string HeaderRecordTags::_dataTypeName_("hfc::HeaderRecordTags");
	#endif


	const std::string HeaderRecordTags::_headerDataType_                   = "DT";
	const std::string HeaderRecordTags::_headerScaleRole_                  = "SR";
	const std::string HeaderRecordTags::_headerHostname_                   = "HN";
	const std::string HeaderRecordTags::_headerIPAddress_                  = "IP";
	const std::string HeaderRecordTags::_headerCurrentTime_                = "CT";

	bool     HeaderRecordTags::_initialized_ = false;
	TagsList HeaderRecordTags::_headerTags_(endHeaderRecordFieldOffsets);
	TagsMap  HeaderRecordTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	HeaderRecordTags::HeaderRecordTags(FieldOffsetsList * pFieldList)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the HeaderRecord class
			_headerTags_[0]                                        = emptyTag;
			_headerTags_[fieldOffsetHeaderDataType]                = _headerDataType_;
			_headerTags_[fieldOffsetHeaderScaleRole]               = _headerScaleRole_;
			_headerTags_[fieldOffsetHeaderHostname]                = _headerHostname_;
			_headerTags_[fieldOffsetHeaderIPAddress]               = _headerIPAddress_;
			_headerTags_[fieldOffsetHeaderCurrentTime]             = _headerCurrentTime_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	HeaderRecordTags::~HeaderRecordTags()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


}