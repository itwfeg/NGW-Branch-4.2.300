/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTSTATISTICTAGS_H
#define PRODUCTSTATISTICTAGS_H

#include "Tags.h"

class ProductStatisticTags : public Tags
{
	public:
		ProductStatisticTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ProductStatisticTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string productNum;
		static const std::string language;
		static const std::string hourNum;
		static const std::string selectedCount;
		static const std::string printedCount;
		static const std::string selectedTime;
		static const std::string recordedDateTime;

		static TagsList productStatisticTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PRODUCTSTATISTICTAGS_H 
