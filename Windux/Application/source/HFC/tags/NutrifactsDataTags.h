/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NUTRIFACTSTAGS_H
#define NUTRIFACTSTAGS_H

#include "Tags.h"

class NutrifactsDataTags : public Tags
{
	public:
		NutrifactsDataTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~NutrifactsDataTags();

		static const std::string nutriFactNum;
		static const std::string nutriDataColumn1;
		static const std::string nutriDataColumn2;
		static const std::string nutriDataColumn3;
		static const std::string nutriDataColumn4;

	protected:
		const TagsList &getMyTagsList() const {return nutriDataTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList nutriDataTags;
		static TagsMap  fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//NUTRIFACTSTAGS_H
