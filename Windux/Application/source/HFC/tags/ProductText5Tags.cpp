/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductText5Tags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductText5Tags::dataType = 0;
std::string ProductText5Tags::dataTypeName("ProductText5Tags"); 
#endif

const std::string ProductText5Tags::productText5Number = "y5";
const std::string ProductText5Tags::productText5Text   = "Y5";

bool     ProductText5Tags::_tagsInitialized_ = false;
TagsList ProductText5Tags::productTextTags(endTextFieldOffsets);
TagsMap  ProductText5Tags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductText5Tags::ProductText5Tags(FieldOffsetsList *pFieldList)
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
		productTextTags[fieldOffsetTextNum]       = productText5Number;
		productTextTags[fieldOffsetTextErrorCode] = emptyTag;
		productTextTags[fieldOffsetTextText]      = productText5Text;
		productTextTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductText5Tags::~ProductText5Tags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
