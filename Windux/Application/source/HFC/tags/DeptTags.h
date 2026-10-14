/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef DEPTTAGS_H
#define DEPTTAGS_H

#include "Tags.h"

class DeptTags : public Tags
{
	public:
		DeptTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~DeptTags();

		static const std::string deptNum;
		static const std::string deptName1;
		static const std::string deptName2;

	protected:
		const TagsList &getMyTagsList() const {return deptTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList deptTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//DEPTTAGS_H 
