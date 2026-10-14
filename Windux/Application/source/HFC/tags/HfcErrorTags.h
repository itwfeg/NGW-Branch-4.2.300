/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCERRORTAGS_H
#define HFCERRORTAGS_H



#include "Tags.h"


class HfcErrorTags : public Tags
{
	public:
		HfcErrorTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~HfcErrorTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string recordNum;
		static const std::string errorCode;
		static const std::string recordType;
		static const std::string failedTag;
		static const std::string keyValue;

		static TagsList hfcErrorTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCERRORTAGS_H 
