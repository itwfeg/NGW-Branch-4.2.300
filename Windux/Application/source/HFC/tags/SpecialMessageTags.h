/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SPECIALMESSAGETAGS_H
#define SPECIALMESSAGETAGS_H

#include "Tags.h"

class SpecialMessageTags : public Tags
{
	friend class ProductTags;

	public:
		SpecialMessageTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~SpecialMessageTags();

		static const std::string specialTextNumber;
		static const std::string specialTextText;

	protected:
		const TagsList &getMyTagsList() const {return specialtextTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList specialtextTags;
		static TagsMap fieldMap;	//example pair: s#,fieldOffsetTextNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//SPECIALMESSAGETAGS_H
