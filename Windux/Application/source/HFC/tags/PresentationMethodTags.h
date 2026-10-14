/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRESENTATIONMETHODTAGS_H
#define PRESENTATIONMETHODTAGS_H

#include "Tags.h"

class PresentationMethodTags : public Tags
{
	public:
		PresentationMethodTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~PresentationMethodTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string presentationMethodType;
		static const std::string presentationMethodDescription;

		static TagsList presentationMethodTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PRESENTATIONMETHODTAGS_H 
