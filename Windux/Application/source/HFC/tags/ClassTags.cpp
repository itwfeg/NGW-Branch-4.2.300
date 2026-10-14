/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ClassTags.h"
#include "DeptTags.h"
#include "Class.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ClassTags::dataType = 0;
std::string ClassTags::dataTypeName("ClassTags"); 
#endif

const std::string ClassTags::classNumber			= "c#";
const std::string ClassTags::className 				= "dt";
const std::string ClassTags::reportingClassNumber	= "rc";

bool     ClassTags::_tagsInitialized_ = false;
TagsList ClassTags::classTags(endClassFieldOffsets);
TagsMap  ClassTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ClassTags::ClassTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		classTags[0] 							= emptyTag;
		classTags[fieldOffsetClassNum]			= classNumber;
		classTags[fieldOffsetClassName]			= className;
		classTags[fieldOffsetReportingClassNum]	= reportingClassNumber;
		classTags[fieldOffsetClassDeptNum]		= DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ClassTags::~ClassTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}