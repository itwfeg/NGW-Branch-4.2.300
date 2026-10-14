/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SPECIALSTAGS_H
#define SPECIALSTAGS_H

#include "Tags.h"

class SpecialsTags : public Tags
{
	public:
		SpecialsTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~SpecialsTags();

		static const std::string specialsNum;
		static const std::string description;
		static const std::string image1;
		static const std::string image2;
		static const std::string defaultLength;

	protected:
		const TagsList &getMyTagsList() const {return specialsTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList specialsTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//SPECIALSTAGS_H 
