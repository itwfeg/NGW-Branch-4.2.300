/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXT8TAGS_H
#define PRODUCTTEXT8TAGS_H

#include "Tags.h"

class ProductText8Tags : public Tags
{
	friend class ProductTags;

	public:
		ProductText8Tags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductText8Tags();

		static const std::string productText8Number;
		static const std::string productText8Text;

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

#endif	//PRODUCTTEXT8TAGS_H
