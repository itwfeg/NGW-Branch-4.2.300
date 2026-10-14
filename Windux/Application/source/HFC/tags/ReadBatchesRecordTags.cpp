#include "ReadBatchesRecordTags.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int ReadBatchesRecordTags::_dataType_ = 0;
		std::string ReadBatchesRecordTags::_dataTypeName_("hfc::ReadBatchesRecordTags");
	#endif


	const std::string ReadBatchesRecordTags::_readBatchesBatchType_                   = "BT";
	const std::string ReadBatchesRecordTags::_readBatchesReferenceVersion_            = "RV";

	bool     ReadBatchesRecordTags::_initialized_ = false;
	TagsList ReadBatchesRecordTags::_headerTags_(endReadBatchesRecordFieldOffsets);
	TagsMap  ReadBatchesRecordTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	ReadBatchesRecordTags::ReadBatchesRecordTags(FieldOffsetsList * pFieldList)
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
			_headerTags_[fieldOffsetReadBatchesBatchType]          = _readBatchesBatchType_;
			_headerTags_[fieldOffsetReadBatchesReferenceVersion]   = _readBatchesReferenceVersion_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	ReadBatchesRecordTags::~ReadBatchesRecordTags()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


}