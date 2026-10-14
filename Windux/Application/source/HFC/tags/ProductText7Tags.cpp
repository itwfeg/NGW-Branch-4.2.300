/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductText7Tags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductText7Tags::dataType = 0;
std::string ProductText7Tags::dataTypeName("ProductText7Tags"); 
#endif

const std::string ProductText7Tags::productText7Number = "y7";
const std::string ProductText7Tags::productText7Text   = "Y7";

bool     ProductText7Tags::_tagsInitialized_ = false;
TagsList ProductText7Tags::productTextTags(endTextFieldOffsets);
TagsMap  ProductText7Tags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductText7Tags::ProductText7Tags(FieldOffsetsList *pFieldList)
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
		productTextTags[fieldOffsetTextNum]       = productText7Number;
		productTextTags[fieldOffsetTextErrorCode] = emptyTag;
		productTextTags[fieldOffsetTextText]      = productText7Text;
		productTextTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductText7Tags::~ProductText7Tags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
