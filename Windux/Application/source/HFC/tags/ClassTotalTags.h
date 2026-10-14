#ifndef CLASSTOTALTAGS_H
#define CLASSTOTALTAGS_H



#include "TotalsTags.h"



namespace hfc
{


	class ClassTotalTags : public TotalsTags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			ClassTotalTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
			~ClassTotalTags();


			/* --- Class data members --- */


		protected:
			const TagsList & getMyTagsList() const { return _classTotalTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _classTotalTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif	//CLASSTOTALTAGS_H 
