/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EXPANDEDTEXTTAGS_H
#define EXPANDEDTEXTTAGS_H

#include "Tags.h"

class ExpandedTextTags : public Tags
{
	friend class ProductTags;

	public:
		ExpandedTextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ExpandedTextTags();

		static const std::string expandedTextNumber;
		static const std::string expandedTextText;

	protected:
		const TagsList &getMyTagsList() const {return expandedTextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList expandedTextTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//EXPANDEDTEXTTAGS_H
