#ifndef HFCCOMMHEADERTAGS_H
#define HFCCOMMHEADERTAGS_H



#include "Tags.h"



namespace hfc
{


	class HfcCommHeaderTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum HfcCommHeaderFieldOffsets
			{
				fieldOffsetHeaderDataType = 1,
				fieldOffsetHeaderHostname,
				fieldOffsetHeaderIPAddress,
				fieldOffsetHeaderCurrentTime,

				endHfcCommHeaderFieldOffsets
			} HfcCommHeaderFieldOffsets_t;


			/* --- Constructors and destructor --- */
			HfcCommHeaderTags(FieldOffsetsList * pFieldList = 0);
			~HfcCommHeaderTags();


			static const std::string _headerDataType_;
			static const std::string _headerHostname_;
			static const std::string _headerIPAddress_;
			static const std::string _headerCurrentTime_;


		protected:
			const TagsList & getMyTagsList() const { return _headerTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _headerTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};

}



#endif /* HFCCOMMHEADERTAGS_H */
