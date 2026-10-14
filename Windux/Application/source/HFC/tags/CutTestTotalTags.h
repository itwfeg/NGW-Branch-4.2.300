#ifndef CUTTESTTOTALTAGS_H
#define CUTTESTTOTALTAGS_H



#include "TotalsTags.h"



namespace hfc
{

	class CutTestTotalTags : public TotalsTags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			CutTestTotalTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
			~CutTestTotalTags();


			/* --- Product data members --- */


		protected:
			const TagsList & getMyTagsList() const { return _totalTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _totalTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif	//CUTTESTTOTALTAGS_H 
