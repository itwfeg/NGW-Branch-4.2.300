/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductText8Tags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductText8Tags::dataType = 0;
std::string ProductText8Tags::dataTypeName("ProductText8Tags"); 
#endif

const std::string ProductText8Tags::productText8Number = "y8";
const std::string ProductText8Tags::productText8Text   = "Y8";

bool     ProductText8Tags::_tagsInitialized_ = false;
TagsList ProductText8Tags::productTextTags(endTextFieldOffsets);
TagsMap  ProductText8Tags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductText8Tags::ProductText8Tags(FieldOffsetsList *pFieldList)
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
		productTextTags[fieldOffsetTextNum]       = productText8Number;
		productTextTags[fieldOffsetTextErrorCode] = emptyTag;
		productTextTags[fieldOffsetTextText]      = productText8Text;
		productTextTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductText8Tags::~ProductText8Tags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
