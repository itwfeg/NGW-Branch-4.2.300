#include "HfcCommHeaderTags.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace hfc
{


#ifdef USE_ACCOUNTANT
	int HfcCommHeaderTags::dataType = 0;
	std::string HfcCommHeaderTags::dataTypeName("HfcCommHeaderTags"); 
#endif



	const std::string HfcCommHeaderTags::_headerDataType_                   = "";
	const std::string HfcCommHeaderTags::_headerHostname_                   = "";
	const std::string HfcCommHeaderTags::_headerIPAddress_                  = "";
	const std::string HfcCommHeaderTags::_headerCurrentTime_                = "";

	bool     HfcCommHeaderTags::_initialized_ = false;
	TagsList HfcCommHeaderTags::_headerTags_(endHfcCommHeaderFieldOffsets);
	TagsMap  HfcCommHeaderTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	HfcCommHeaderTags::HfcCommHeaderTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the HfcCommHeader class
			_headerTags_[0]                                        = emptyTag;
			_headerTags_[fieldOffsetHeaderDataType]                = _headerDataType_;
			_headerTags_[fieldOffsetHeaderHostname]                = _headerHostname_;
			_headerTags_[fieldOffsetHeaderIPAddress]               = _headerIPAddress_;
			_headerTags_[fieldOffsetHeaderCurrentTime]             = _headerCurrentTime_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	HfcCommHeaderTags::~HfcCommHeaderTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}