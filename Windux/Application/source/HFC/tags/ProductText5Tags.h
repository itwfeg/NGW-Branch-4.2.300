/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXT5TAGS_H
#define PRODUCTTEXT5TAGS_H

#include "Tags.h"

class ProductText5Tags : public Tags
{
	friend class ProductTags;

	public:
		ProductText5Tags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductText5Tags();

		static const std::string productText5Number;
		static const std::string productText5Text;

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

#endif	//PRODUCTTEXT5TAGS_H
