#ifndef SYNCHRONIZATIONRECORDTAGS_H
#define SYNCHRONIZATIONRECORDTAGS_H



#include "Tags.h"



namespace hfc
{


	class SynchronizationRecordTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum SynchronizationRecordFieldOffsets
			{
				fieldOffsetSynchronizationCommand = 1,

				endSynchronizationRecordFieldOffsets
			} SynchronizationRecordFieldOffsets_t;


			/* --- Constructors and destructor --- */
			SynchronizationRecordTags(FieldOffsetsList * pFieldList = 0);
			~SynchronizationRecordTags();


			static const std::string _synchronizationCommand_;


		protected:
			const TagsList & getMyTagsList() const { return _headerTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

			static bool _initialized_;
			static TagsList _headerTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum
	};

}



#endif /* SYNCHRONIZATIONRECORDTAGS_H */
