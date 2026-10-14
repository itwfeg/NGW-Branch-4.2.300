/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXT7TAGS_H
#define PRODUCTTEXT7TAGS_H

#include "Tags.h"

class ProductText7Tags : public Tags
{
	friend class ProductTags;

	public:
		ProductText7Tags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductText7Tags();

		static const std::string productText7Number;
		static const std::string productText7Text;

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

#endif	//PRODUCTTEXT7TAGS_H
