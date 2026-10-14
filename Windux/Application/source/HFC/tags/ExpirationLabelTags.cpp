/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ExpirationLabelTags.h"
#include "DeptTags.h"
#include "ProductTags.h"
#include "ExpirationLabel.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ExpirationLabelTags::dataType = 0;
std::string ExpirationLabelTags::dataTypeName("ExpirationLabelTags"); 
#endif

const std::string ExpirationLabelTags::expirationDateTime = "el";

bool     ExpirationLabelTags::_tagsInitialized_ = false;
TagsList ExpirationLabelTags::expirationLabelTags(endExpirationLabelFieldOffsets);
TagsMap  ExpirationLabelTags::fieldMap;


ExpirationLabelTags::ExpirationLabelTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for this class
		expirationLabelTags[0]                                     = emptyTag;
		expirationLabelTags[fieldOffsetExpLabelPluNumber]          = ProductTags::prodNumber;
		expirationLabelTags[fieldOffsetExpLabelPluDescription]     = ProductTags::prodDescription;
		expirationLabelTags[fieldOffsetExpLabelExpirationDateTime] = expirationDateTime;
		expirationLabelTags[fieldOffsetExpLabelDeptNum]            = DeptTags::deptNum;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ExpirationLabelTags::~ExpirationLabelTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}