#ifndef HEADERRECORDTAGS_H
#define HEADERRECORDTAGS_H



#include "Tags.h"



namespace hfc
{


	class HeaderRecordTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum HeaderRecordFieldOffsets
			{
				fieldOffsetHeaderDataType = 1,
				fieldOffsetHeaderScaleRole,
				fieldOffsetHeaderHostname,
				fieldOffsetHeaderIPAddress,
				fieldOffsetHeaderCurrentTime,

				endHeaderRecordFieldOffsets
			} HeaderRecordFieldOffsets_t;


			/* --- Constructors and destructor --- */
			HeaderRecordTags(FieldOffsetsList * pFieldList = 0);
			~HeaderRecordTags();


			static const std::string _headerDataType_;
			static const std::string _headerScaleRole_;
			static const std::string _headerHostname_;
			static const std::string _headerIPAddress_;
			static const std::string _headerCurrentTime_;


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



#endif /* HEADERRECORDTAGS_H */
