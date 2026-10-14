/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTextCfgTags.h"
#include "ProductTextCfg.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTextCfgTags::dataType = 0;
std::string ProductTextCfgTags::dataTypeName("ProductTextCfgTags"); 
#endif

const std::string ProductTextCfgTags::textType   = "TT";
const std::string ProductTextCfgTags::printerNum = "PN";
const std::string ProductTextCfgTags::labelNum   = "LN";

bool     ProductTextCfgTags::_tagsInitialized_ = false;
TagsList ProductTextCfgTags::productTextCfgTags(endProductTextCfgFieldOffsets);
TagsMap  ProductTextCfgTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


ProductTextCfgTags::ProductTextCfgTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		productTextCfgTags[0]                        = emptyTag;
		productTextCfgTags[fieldOffsetPTCTextType]   = textType;
		productTextCfgTags[fieldOffsetPTCPrinterNum] = printerNum;
		productTextCfgTags[fieldOffsetPTCLabelNum]   = labelNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductTextCfgTags::~ProductTextCfgTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
