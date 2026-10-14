#ifndef READBATCHESRECORDTAGS_H
#define READBATCHESRECORDTAGS_H



#include "Tags.h"



namespace hfc
{


	class ReadBatchesRecordTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum ReadBatchesRecordFieldOffsets
			{
				fieldOffsetReadBatchesBatchType = 1,
				fieldOffsetReadBatchesReferenceVersion,

				endReadBatchesRecordFieldOffsets
			} ReadBatchesRecordFieldOffsets_t;


			/* --- Constructors and destructor --- */
			ReadBatchesRecordTags(FieldOffsetsList * pFieldList = 0);
			~ReadBatchesRecordTags();


			static const std::string _readBatchesBatchType_;
			static const std::string _readBatchesReferenceVersion_;


		protected:
			const TagsList & getMyTagsList() const { return _headerTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			/* --- Instance class members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			static bool _initialized_;
			static TagsList _headerTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum
	};

}



#endif /* READBATCHESRECORDTAGS_H */
