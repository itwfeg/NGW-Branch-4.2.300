/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "DeptTags.h"
#include "Dept.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DeptTags::dataType = 0;
std::string DeptTags::dataTypeName("DeptTags"); 
#endif

const std::string DeptTags::deptNum 	= "d#";
const std::string DeptTags::deptName1	= "dm";
const std::string DeptTags::deptName2	= "dM";

bool     DeptTags::_tagsInitialized_ = false;
TagsList DeptTags::deptTags(endDeptFieldOffsets);
TagsMap  DeptTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


DeptTags::DeptTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for the Dept class
		deptTags[0] 							= emptyTag;
		deptTags[fieldOffsetDeptNum]		= deptNum;
		deptTags[fieldOffsetDeptName1]	= deptName1;
		deptTags[fieldOffsetDeptName2]	= deptName2;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

DeptTags::~DeptTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
