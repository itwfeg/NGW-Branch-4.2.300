/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CLASSTAGS_H
#define CLASSTAGS_H

#include "Tags.h"

class ClassTags : public Tags
{
	public:
		ClassTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~ClassTags();

		static const std::string classNumber;
		static const std::string className;
		static const std::string reportingClassNumber;

	protected:
		const TagsList &getMyTagsList() const {return classTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList classTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//CLASSTAGS_H 
