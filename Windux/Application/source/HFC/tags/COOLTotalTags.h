#ifndef COOLTOTALTAGS_H
#define COOLTOTALTAGS_H



#include "TotalsTags.h"



namespace hfc
{


	class COOLTotalTags : public TotalsTags
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			COOLTotalTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
			~COOLTotalTags();


			/* --- Class data members --- */
			static const std::string _firstLogDate_;
			static const std::string _firstLogTime_;
			static const std::string _lastLogDate_;
			static const std::string _lastLogtime_;


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



#endif	//COOLTOTALTAGS_H 
