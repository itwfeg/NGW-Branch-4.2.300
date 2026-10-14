/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef MARQUEETEXTTAGS_H
#define MARQUEETEXTTAGS_H

#include "Tags.h"

class MarqueeTextTags : public Tags
{
	friend class ProductTags;

	public:
		MarqueeTextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~MarqueeTextTags();

		static const std::string marqueeNumber;
		static const std::string marqueeText;

	protected:
		const TagsList &getMyTagsList() const {return marqueeTextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		 static bool _tagsInitialized_;
		 static TagsList marqueeTextTags;
		 static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//MARQUEETEXTTAGS_H
