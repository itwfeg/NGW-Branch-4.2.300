/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WRAPPERTAGS_H
#define WRAPPERTAGS_H

#include "Tags.h"

class WrapperTags : public Tags
{
	public:
		WrapperTags(FieldOffsetsList *pFieldList = 0);
		~WrapperTags();

		static const std::string wrapperTimings;
		static const std::string versionNumber;

	protected:
		const TagsList &getMyTagsList() const {return wrapperTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList wrapperTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//WRAPPERTAGS_H 
