/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "NutrifactsDataTags.h"
#include "DeptTags.h"
#include "Nutrifacts.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NutrifactsDataTags::dataType = 0;
std::string NutrifactsDataTags::dataTypeName("NutrifactsDataTags"); 
#endif

const std::string NutrifactsDataTags::nutriFactNum		= "N#";
const std::string NutrifactsDataTags::nutriDataColumn1	= "N1";
const std::string NutrifactsDataTags::nutriDataColumn2	= "N2";
const std::string NutrifactsDataTags::nutriDataColumn3	= "N3";
const std::string NutrifactsDataTags::nutriDataColumn4	= "N4";

bool     NutrifactsDataTags::_tagsInitialized_ = false;
TagsList NutrifactsDataTags::nutriDataTags(endNutrifactsFieldOffsets);
TagsMap  NutrifactsDataTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


NutrifactsDataTags::NutrifactsDataTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for the NutrifactsData class
		nutriDataTags[0]                        = emptyTag;
		nutriDataTags[fieldOffsetNFTextNum]     = nutriFactNum;
		nutriDataTags[fieldOffsetNFColumn1Data] = nutriDataColumn1;
		nutriDataTags[fieldOffsetNFColumn2Data] = nutriDataColumn2;
		nutriDataTags[fieldOffsetNFColumn3Data] = nutriDataColumn3;
		nutriDataTags[fieldOffsetNFColumn4Data] = nutriDataColumn4;
		nutriDataTags[fieldOffsetNFDeptNum]     = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

NutrifactsDataTags::~NutrifactsDataTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}