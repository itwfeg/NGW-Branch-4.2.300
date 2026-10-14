#include "SynchronizationChangeTags.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace hfc
{


#ifdef USE_ACCOUNTANT
	int SynchronizationChangeTags::dataType = 0;
	std::string SynchronizationChangeTags::dataTypeName("SynchronizationChangeTags"); 
#endif



//	const std::string SynchronizationChangeTags::_synchronizationChangeIdentifier_                 = "CI";
	const std::string SynchronizationChangeTags::_synchronizationChangeBatchNumber_                = "B#";
	const std::string SynchronizationChangeTags::_synchronizationChangeDevice_                     = "YD";
	const std::string SynchronizationChangeTags::_synchronizationChangeEvent_                      = "YE";
	const std::string SynchronizationChangeTags::_synchronizationChangeFormat_                     = "YF";
	const std::string SynchronizationChangeTags::_synchronizationChangeChange_                     = "YC";

	bool     SynchronizationChangeTags::_initialized_ = false;
	TagsList SynchronizationChangeTags::_synchronizationChangeTags_(endSynchronizationChangeFieldOffsets);
	TagsMap  SynchronizationChangeTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	SynchronizationChangeTags::SynchronizationChangeTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the SynchronizationChange class
			_synchronizationChangeTags_[0]                                                      = emptyTag;
//			_synchronizationChangeTags_[fieldOffsetSynchronizationChangeIdentifier]             = _synchronizationChangeIdentifier_;
			_synchronizationChangeTags_[fieldOffsetSynchronizationChangeBatchNumber]            = _synchronizationChangeBatchNumber_;
			_synchronizationChangeTags_[fieldOffsetSynchronizationChangeDevice]                 = _synchronizationChangeDevice_;
			_synchronizationChangeTags_[fieldOffsetSynchronizationChangeEvent]                  = _synchronizationChangeEvent_;
			_synchronizationChangeTags_[fieldOffsetSynchronizationChangeFormat]                 = _synchronizationChangeFormat_;
			_synchronizationChangeTags_[fieldOffsetSynchronizationChangeChange]                 = _synchronizationChangeChange_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	SynchronizationChangeTags::~SynchronizationChangeTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}