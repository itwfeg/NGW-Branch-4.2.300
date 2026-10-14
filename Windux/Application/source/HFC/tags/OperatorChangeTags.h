#ifndef OPERATORCHANGETAGS_H
#define OPERATORCHANGETAGS_H



#include "Tags.h"



namespace hfc
{


	class OperatorChangeTags : public Tags
	{
		public:
			/* --- Types and subtypes --- */
			static const std::string _changedField_;
			static const std::string _newValue_;
			static const std::string _oldValue_;
			static const std::string _changeDate_;
			static const std::string _changeTime_;
			static const std::string _changeDateTime_;


			/* --- Constructors and destructor --- */
			OperatorChangeTags(FieldOffsetsList * pFieldList = 0);
			~OperatorChangeTags();


		protected:
			const TagsList &getMyTagsList() const { return _totalTags_; }
			TagsMap &getMyTagsMap() const { return _fieldMap_; }


		private:
			static TagsList _totalTags_;
			static TagsMap _fieldMap_;       //example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif	//OPERATORCHANGETAGS_H 
