/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SpecialsTags.h"
#include "DeptTags.h"
#include "Special.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SpecialsTags::dataType = 0;
std::string SpecialsTags::dataTypeName("SpecialsTags"); 
#endif

const std::string SpecialsTags::specialsNum 	= "S#";
const std::string SpecialsTags::description		= "sN";
const std::string SpecialsTags::image1 			= "i1";
const std::string SpecialsTags::image2 			= "i2";
const std::string SpecialsTags::defaultLength 	= "dl";

bool     SpecialsTags::_tagsInitialized_ = false;
TagsList SpecialsTags::specialsTags(endSpecialFieldOffsets);
TagsMap  SpecialsTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


SpecialsTags::SpecialsTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for the Specials class
		specialsTags[0] 								= emptyTag;
		specialsTags[fieldOffsetSpecialNum]				= specialsNum;
		specialsTags[fieldOffsetSpecialDescription]		= description;
		specialsTags[fieldOffsetSpecialImageName1]		= image1;
		specialsTags[fieldOffsetSpecialImageName2]		= image2;
		specialsTags[fieldOffsetSpecialDefaultLength]	= defaultLength;
		specialsTags[fieldOffsetSpecialDeptNum]			= DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

SpecialsTags::~SpecialsTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}