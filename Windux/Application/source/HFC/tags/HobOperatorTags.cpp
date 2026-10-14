/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobOperatorTags.h"
#include "HobOperator.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobOperatorTags::dataType = 0;
std::string HobOperatorTags::dataTypeName("HobOperatorTags"); 
#endif

const std::string HobOperatorTags::opNumber 		= "oi";
const std::string HobOperatorTags::opRFID			= "or";
const std::string HobOperatorTags::opName 		= "on";
const std::string HobOperatorTags::opPLang 		= "ol";
const std::string HobOperatorTags::accessLevel 	= "oc";
const std::string HobOperatorTags::sawNote 		= "os";
const std::string HobOperatorTags::opNote 		= "oN";

TagsList HobOperatorTags::hobOperatorTags(endHobOperatorFieldOffsets);
TagsMap  HobOperatorTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


HobOperatorTags::HobOperatorTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the HobOperator class
		hobOperatorTags[0] 							= emptyTag;
		hobOperatorTags[fieldOffsetOpNum] 		= opNumber;
		hobOperatorTags[fieldOffsetOpRFID] 		= opRFID;
		hobOperatorTags[fieldOffsetOpName] 		= opName;
		hobOperatorTags[fieldOffsetOpLang] 		= opPLang;
		hobOperatorTags[fieldOffsetAccessLevel] = accessLevel;
		hobOperatorTags[fieldOffsetSawNote] 	= sawNote;
		hobOperatorTags[fieldOffsetMode] 		= emptyTag; 

		putTagsInMap(pFieldList);
	}
}

HobOperatorTags::~HobOperatorTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &HobOperatorTags::getMyTagsList() const
{
	return hobOperatorTags;
}

TagsMap &HobOperatorTags::getMyTagsMap() const
{
	return fieldMap;
}
