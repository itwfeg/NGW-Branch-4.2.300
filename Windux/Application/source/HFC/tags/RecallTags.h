/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RECALLTAGS_H
#define RECALLTAGS_H

#include "Tags.h"

class RecallTags : public Tags
{
	public:
		static const std::string recallNum;
		static const std::string recallName;
		static const std::string recallText;
		static const std::string receivedDateTime;
		static const std::string acknowledged;

		RecallTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~RecallTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static bool _tagsInitialized_;
		static TagsList recallTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//RECALLTAGS_H
