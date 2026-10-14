/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXT6TAGS_H
#define PRODUCTTEXT6TAGS_H

#include "Tags.h"

class ProductText6Tags : public Tags
{
	friend class ProductTags;

	public:
		ProductText6Tags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductText6Tags();

		static const std::string productText6Number;
		static const std::string productText6Text;

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

#endif	//PRODUCTTEXT6TAGS_H
