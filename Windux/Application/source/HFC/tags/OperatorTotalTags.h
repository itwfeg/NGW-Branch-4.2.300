#ifndef OPERATORTOTALTAGS_H
#define OPERATORTOTALTAGS_H



#include "TotalsTags.h"



namespace hfc
{


	class OperatorTotalTags : public TotalsTags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			OperatorTotalTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
			~OperatorTotalTags();


			/* --- Class data members --- */


		protected:
			const TagsList & getMyTagsList() const { return _operatorTotalTags_; }
			TagsMap & getMyTagsMap() const { return _fieldMap_; }


		private:
			static bool _initialized_;
			static TagsList _operatorTotalTags_;
			static TagsMap _fieldMap_;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif	//OPERATORTOTALTAGS_H 
