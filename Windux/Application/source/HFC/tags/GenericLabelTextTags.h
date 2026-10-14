/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GENERICLABELTEXTTAGS_H
#define GENERICLABELTEXTTAGS_H

#include "Tags.h"

class GenericLabelTextTags : public Tags
{
	public:
		GenericLabelTextTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~GenericLabelTextTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string textNum;
		static const std::string textText;

		static TagsList genericLabelTextTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//GENERICLABELTEXTTAGS_H 
