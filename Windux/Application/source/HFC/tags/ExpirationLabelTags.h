/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EXPIRATIONLABELTAGS_H
#define EXPIRATIONLABELTAGS_H

#include "Tags.h"


class ExpirationLabelTags : public Tags
{
	public:
		ExpirationLabelTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ExpirationLabelTags();

		// The following tags are defined in their corresponding tags definition class.
		// prodNumber - defined in ProductTags.h
		// prodDescription - defined in ProductTags.h

		static const std::string expirationDateTime;

	protected:
		const TagsList &getMyTagsList() const {return expirationLabelTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList expirationLabelTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //EXPIRATIONLABELTAGS_H
