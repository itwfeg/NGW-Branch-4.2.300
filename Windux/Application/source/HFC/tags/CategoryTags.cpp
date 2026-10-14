/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CategoryTags.h"
#include "DeptTags.h"
#include "SoftkeyTags.h"

#include "ConfigurationTags.h"
#include "Category.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CategoryTags::dataType = 0;
std::string CategoryTags::dataTypeName("CategoryTags"); 
#endif

const std::string CategoryTags::categoryNum 		= "C#";
const std::string CategoryTags::parentCategoryNum 	= "rC";
const std::string CategoryTags::categoryName1 		= "n1";
const std::string CategoryTags::categoryName2 		= "n2";
const std::string CategoryTags::graphicName 		= "i1";
const std::string CategoryTags::displayOrder 		= "do";

bool     CategoryTags::_tagsInitialized_ = false;
TagsList CategoryTags::categoryTags(endCategoryFieldOffsets);
TagsMap  CategoryTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


CategoryTags::CategoryTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		categoryTags[0] 								= emptyTag;
		categoryTags[fieldOffsetCategoryNum]			= categoryNum;
		categoryTags[fieldOffsetParentCategoryNum]		= parentCategoryNum;
		categoryTags[fieldOffsetCategoryName1]			= categoryName1;
		categoryTags[fieldOffsetCategoryName2]			= categoryName2;
		categoryTags[fieldOffsetCategoryGraphicName]	= graphicName;
		categoryTags[fieldOffsetCategoryBgColor]		= SKTags::bgColor;
		categoryTags[fieldOffsetCategoryTextColor]		= SKTags::textColor;
		categoryTags[fieldOffsetCategoryPlaylist]		= ConfigurationTags::getGCDPlaylistFilenameTag();
		categoryTags[fieldOffsetCategoryDeptNum]        = DeptTags::deptNum;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

CategoryTags::~CategoryTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}