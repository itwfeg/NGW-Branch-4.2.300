/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXT9TAGS_H
#define PRODUCTTEXT9TAGS_H

#include "Tags.h"

class ProductText9Tags : public Tags
{
	friend class ProductTags;

	public:
		ProductText9Tags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductText9Tags();

		static const std::string productText9Number;
		static const std::string productText9Text;

	protected:
		const TagsList &getMyTagsList() const {return productTextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList productTextTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PRODUCTTEXT9TAGS_H
