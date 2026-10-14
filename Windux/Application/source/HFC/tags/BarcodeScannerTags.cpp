/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "BarcodeScannerTags.h"
#include "HfcScannerProductRead.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BarcodeScannerTags::dataType = 0;
std::string BarcodeScannerTags::dataTypeName("BarcodeScannerTags"); 
#endif

const std::string BarcodeScannerTags::scannerCode         = "sC";
const std::string BarcodeScannerTags::scannerProductNum   = "p#";

bool     BarcodeScannerTags::_tagsInitialized_ = false;
TagsList BarcodeScannerTags::barcodeScannerTags(endBarcodeScannerFieldOffsets);
TagsMap  BarcodeScannerTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


BarcodeScannerTags::BarcodeScannerTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		barcodeScannerTags[0]                                = emptyTag;
		barcodeScannerTags[fieldOffsetBCScannerCode]         = scannerCode;
		barcodeScannerTags[fieldOffsetBCScannerProductNum]   = scannerProductNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

BarcodeScannerTags::~BarcodeScannerTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
