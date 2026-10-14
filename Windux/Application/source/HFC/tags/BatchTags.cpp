#include "BatchTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace hfc
{


#ifdef USE_ACCOUNTANT
	int BatchTags::dataType = 0;
	std::string BatchTags::dataTypeName("hfc::BatchTags"); 
#endif



	const std::string BatchTags::_batchNumber_                     = "B#";
	const std::string BatchTags::_batchScaleId_                    = "Bl";
	const std::string BatchTags::_batchVersion_                    = "BV";
	const std::string BatchTags::_batchRemoteVersion_              = "BR";
	const std::string BatchTags::_batchOrigin_                     = "Bo";
	const std::string BatchTags::_batchType_                       = "BT";
	const std::string BatchTags::_batchCreatedDateTime_            = "BC";
	const std::string BatchTags::_batchCompletedDateTime_          = "Bc";
	const std::string BatchTags::_batchSignature_                  = "BS";
	const std::string BatchTags::_batchPendingApplication_         = "BA";
	const std::string BatchTags::_batchAppliedDateTime_            = "Ba";
	const std::string BatchTags::_batchPendingSynchronization_     = "BY";
	const std::string BatchTags::_batchSynchronizedDateTime_       = "By";
	const std::string BatchTags::_batchPendingDeletion_            = "BD";
	const std::string BatchTags::_batchDeletedDateTime_            = "Bd";

	bool     BatchTags::_initialized_ = false;
	TagsList BatchTags::_batchTags_(endBatchFieldOffsets);
	TagsMap  BatchTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum



	BatchTags::BatchTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the Batch class
			_batchTags_[0]                                      = emptyTag;
			_batchTags_[fieldOffsetBatchNumber]                 = _batchNumber_;
			_batchTags_[fieldOffsetBatchScaleId]                = _batchScaleId_;
			_batchTags_[fieldOffsetBatchVersion]                = _batchVersion_;
			_batchTags_[fieldOffsetBatchRemoteVersion]          = _batchRemoteVersion_;
			_batchTags_[fieldOffsetBatchOrigin]                 = _batchOrigin_;
			_batchTags_[fieldOffsetBatchType]                   = _batchType_;
			_batchTags_[fieldOffsetBatchCreatedDateTime]        = _batchCreatedDateTime_;
			_batchTags_[fieldOffsetBatchCompletedDateTime]      = _batchCompletedDateTime_;
			_batchTags_[fieldOffsetBatchSignature]              = _batchSignature_;
			_batchTags_[fieldOffsetBatchPendingApplication]     = _batchPendingApplication_;
			_batchTags_[fieldOffsetBatchAppliedDateTime]        = _batchAppliedDateTime_;
			_batchTags_[fieldOffsetBatchPendingSynchronization] = _batchPendingSynchronization_;
			_batchTags_[fieldOffsetBatchSynchronizedDateTime]   = _batchSynchronizedDateTime_;
			_batchTags_[fieldOffsetBatchPendingDeletion]        = _batchPendingDeletion_;
			_batchTags_[fieldOffsetBatchDeletedDateTime]        = _batchDeletedDateTime_;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	BatchTags::~BatchTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


}