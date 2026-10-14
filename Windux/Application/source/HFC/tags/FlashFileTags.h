/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FLASHFILETAGS_H
#define FLASHFILETAGS_H

#include "Tags.h"

class FlashFileTags : public Tags
{
	public:
		FlashFileTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~FlashFileTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string flashFileName;
		static const std::string flashFileDate;

		static TagsList flashFileTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//FLASHFILETAGS_H 
