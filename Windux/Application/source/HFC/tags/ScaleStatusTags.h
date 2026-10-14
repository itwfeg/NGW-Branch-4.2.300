/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SCALESTATUSTAGS_H
#define SCALESTATUSTAGS_H

#include "Tags.h"

class ScaleStatusTags : public Tags
{
	public:
		ScaleStatusTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ScaleStatusTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string scaleCondition;
		static const std::string scaleError;
		static const std::string scaleStatus;
		static const std::string scaleType;
		static const std::string scaleModel;

		static TagsList scaleStatusTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//SCALESTATUSTAGS_H
