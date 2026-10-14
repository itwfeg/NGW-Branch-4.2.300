#ifndef SYNCHRONIZATIONCHANGETAGS_H
#define SYNCHRONIZATIONCHANGETAGS_H



#include "Tags.h"



namespace hfc
{


	class SynchronizationChangeTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum SynchronizationChangeFieldOffsets
			{
//				fieldOffsetSynchronizationChangeIdentifier = 1,
				fieldOffsetSynchronizationChangeBatchNumber = 1,
				fieldOffsetSynchronizationChangeDevice,
				fieldOffsetSynchronizationChangeEvent,
				fieldOffsetSynchronizationChangeFormat,
				fieldOffsetSynchronizationChangeChange,
				fieldOffsetSynchronizationChangeErrorCode,
				endSynchronizationChangeFieldOffsets
			} SynchronizationChangeFieldOffsets_t;


			/* --- Constructors and destructor --- */
			SynchronizationChangeTags(FieldOffsetsList * pFieldList = 0);
			~SynchronizationChangeTags();


//			static const std::string _synchronizationChangeIdentifier_;
			static const std::string _synchronizationChangeBatchNumber_;
			static const std::string _synchronizationChangeDevice_;
			static const std::string _synchronizationChangeEvent_;
			static const std::string _synchronizationChangeFormat_;
			static const std::string _synchronizationChangeChange_;


		protected:
			const TagsList & getMyTagsList() const { return _synchronizationChangeTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _synchronizationChangeTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};

}



#endif /* SYNCHRONIZATIONCHANGETAGS_H */
