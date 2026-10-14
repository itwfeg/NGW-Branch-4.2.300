/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductText6Tags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductText6Tags::dataType = 0;
std::string ProductText6Tags::dataTypeName("ProductText6Tags"); 
#endif

const std::string ProductText6Tags::productText6Number = "y6";
const std::string ProductText6Tags::productText6Text   = "Y6";

bool     ProductText6Tags::_tagsInitialized_ = false;
TagsList ProductText6Tags::productTextTags(endTextFieldOffsets);
TagsMap  ProductText6Tags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductText6Tags::ProductText6Tags(FieldOffsetsList *pFieldList)
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
		productTextTags[fieldOffsetTextNum]       = productText6Number;
		productTextTags[fieldOffsetTextErrorCode] = emptyTag;
		productTextTags[fieldOffsetTextText]      = productText6Text;
		productTextTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductText6Tags::~ProductText6Tags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
