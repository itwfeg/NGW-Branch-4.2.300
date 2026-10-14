#include "SynchronizationRecordTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int SynchronizationRecordTags::_dataType_ = 0;
		std::string SynchronizationRecordTags::_dataTypeName_("hfc::SynchronizationRecordTags");
	#endif



	const std::string SynchronizationRecordTags::_synchronizationCommand_           = "SC";

	bool     SynchronizationRecordTags::_initialized_ = false;
	TagsList SynchronizationRecordTags::_headerTags_(endSynchronizationRecordFieldOffsets);
	TagsMap  SynchronizationRecordTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	SynchronizationRecordTags::SynchronizationRecordTags(FieldOffsetsList * pFieldList)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the HfcCommSynchronization class
			_headerTags_[0]                                        = emptyTag;
			_headerTags_[fieldOffsetSynchronizationCommand]        = _synchronizationCommand_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	SynchronizationRecordTags::~SynchronizationRecordTags()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


}