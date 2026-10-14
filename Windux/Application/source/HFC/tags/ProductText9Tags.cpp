/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductText9Tags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductText9Tags::dataType = 0;
std::string ProductText9Tags::dataTypeName("ProductText9Tags"); 
#endif

const std::string ProductText9Tags::productText9Number = "y9";
const std::string ProductText9Tags::productText9Text   = "Y9";

bool     ProductText9Tags::_tagsInitialized_ = false;
TagsList ProductText9Tags::productTextTags(endTextFieldOffsets);
TagsMap  ProductText9Tags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductText9Tags::ProductText9Tags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for this class
		productTextTags[0]                        = emptyTag;
		productTextTags[fieldOffsetTextNum]       = productText9Number;
		productTextTags[fieldOffsetTextErrorCode] = emptyTag;
		productTextTags[fieldOffsetTextText]      = productText9Text;
		productTextTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductText9Tags::~ProductText9Tags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
