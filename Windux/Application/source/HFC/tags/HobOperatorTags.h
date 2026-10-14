/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBOPERATORTAGS_H
#define HOBOPERATORTAGS_H

#include "Tags.h"

class HobOperatorTags : public Tags
{
	public:
		static const std::string opNumber;
		static const std::string opRFID;
		static const std::string opName;
		static const std::string opPLang;
		static const std::string accessLevel;
		static const std::string sawNote;

		static const std::string opNote;

		HobOperatorTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~HobOperatorTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static bool _tagsInitialized_;
		static TagsList hobOperatorTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HOBOPERATORTAGS_H
