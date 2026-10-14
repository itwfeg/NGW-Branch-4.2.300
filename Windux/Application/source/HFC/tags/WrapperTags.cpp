/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WrapperTags.h"
#include "Debug.h"
#include "HfcEvolWrapperPackageTimings.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperTags::_dataType_ = 0;
std::string WrapperTags::_dataTypeName_("WrapperTags"); 
#endif

const std::string WrapperTags::wrapperTimings		= "wt";
const std::string WrapperTags::versionNumber		= "V#";

bool     WrapperTags::_tagsInitialized_ = false;
TagsList WrapperTags::wrapperTags(endHfcEvolWrPkgTimingsOffsets);
TagsMap  WrapperTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


WrapperTags::WrapperTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if ( !_tagsInitialized_ )
	{
		wrapperTags[0] 		= emptyTag;
		wrapperTags[fieldOffsetEvolWrPkgTimingsVersion]		= versionNumber;
		wrapperTags[fieldOffsetEvolWrPkgTimingsTimings]		= wrapperTimings;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

WrapperTags::~WrapperTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

}
