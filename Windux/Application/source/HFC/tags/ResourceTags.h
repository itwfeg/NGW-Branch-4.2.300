/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RESOURCETAGS_H
#define RESOURCETAGS_H

#include "Tags.h"

typedef enum ResourceTagsFieldOffsets
{
	fieldOffsetName = 1,
	fieldOffsetIsCustom,
	fieldOffsetResourceData,
	endResourceTagsFieldOffsets
}   ResourceTagsFieldOffsets_t;

class ResourceTags : public Tags
{
	public:
		ResourceTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ResourceTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string resourceName;
		static const std::string resourceResourceData;

		static TagsList resourceTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//RESOURCETAGS_H 
