/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CATEGORYTAGS_H
#define CATEGORYTAGS_H

#include "Tags.h"

class CategoryTags : public Tags
{
	public:
		CategoryTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~CategoryTags();

		static const std::string categoryNum;
		static const std::string parentCategoryNum;
		static const std::string categoryName1;
		static const std::string categoryName2;
		static const std::string graphicName;
		static const std::string displayOrder;

	protected:
		const TagsList &getMyTagsList() const {return categoryTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList categoryTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//CATEGORYTAGS_H 
