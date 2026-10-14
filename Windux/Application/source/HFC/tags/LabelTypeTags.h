/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef LABELTYPETAGS_H
#define LABELTYPETAGS_H

#include "Tags.h"

class LabelTypeTags : public Tags
{
	public:
		LabelTypeTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~LabelTypeTags();

		static const std::string resourceNumber;
		static const std::string resourceLength;
		static const std::string resourceData;
		static const std::string sequence;
		static const std::string sequenceMax;
		static const std::string rotationOnLabelStock;

	protected:
		const TagsList &getMyTagsList() const {return labelTypeTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList labelTypeTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//LABELTYPETAGS_H 
