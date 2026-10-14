#ifndef HOURLYTOTALTAGS_H
#define HOURLYTOTALTAGS_H



#include "TotalsTags.h"



namespace hfc
{


	class HourlyTotalTags : public TotalsTags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			HourlyTotalTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
			~HourlyTotalTags();


			/* --- Class data members --- */
			static const std::string _hourNum_;


		protected:
			const TagsList & getMyTagsList() const { return _hourlyTotalTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _hourlyTotalTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif	//HOURLYTOTALTAGS_H 
