#ifndef BATCHTAGS_H
#define BATCHTAGS_H



#include "Tags.h"



namespace hfc
{


	class BatchTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum BatchFieldOffsets
			{
				fieldOffsetBatchNumber = 1,
				fieldOffsetBatchScaleId,
				fieldOffsetBatchVersion,
				fieldOffsetBatchRemoteVersion,
				fieldOffsetBatchOrigin,
				fieldOffsetBatchType,
				fieldOffsetBatchCreatedDateTime,
				fieldOffsetBatchCompletedDateTime,
				fieldOffsetBatchSignature,
				fieldOffsetBatchPendingApplication,
				fieldOffsetBatchAppliedDateTime,
				fieldOffsetBatchPendingSynchronization,
				fieldOffsetBatchSynchronizedDateTime,
				fieldOffsetBatchPendingDeletion,
				fieldOffsetBatchDeletedDateTime,
				fieldOffsetBatchErrorCode,

				endBatchFieldOffsets
			} BatchFieldOffsets_t;


			/* --- Constructors and destructor --- */
			BatchTags(FieldOffsetsList * pFieldList = 0);
			~BatchTags();


			static const std::string _batchNumber_;
			static const std::string _batchScaleId_;
			static const std::string _batchVersion_;
			static const std::string _batchRemoteVersion_;
			static const std::string _batchOrigin_;
			static const std::string _batchType_;
			static const std::string _batchCreatedDateTime_;
			static const std::string _batchCompletedDateTime_;
			static const std::string _batchSignature_;
			static const std::string _batchPendingApplication_;
			static const std::string _batchAppliedDateTime_;
			static const std::string _batchPendingSynchronization_;
			static const std::string _batchSynchronizedDateTime_;
			static const std::string _batchPendingDeletion_;
			static const std::string _batchDeletedDateTime_;


		protected:
			const TagsList & getMyTagsList() const { return _batchTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _batchTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};

}



#endif /* BATCHTAGS_H */
