/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyTags.h"
#include "DeptTags.h"

#include "SoftkeyStyle.h"
#include "SoftkeyFeature.h"
#include "Softkey.h"
#include "SoftkeyGroup.h"

#include "HobOperatorTags.h"
#include "CategoryTags.h"
#include "ProductTags.h"


/******************************************************************************
*   The Softkey Tags defined                                                  *
******************************************************************************/

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SKTags::dataType = 0;
std::string SKTags::dataTypeName("SKTags"); 
#endif

SKTags::SKTags()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}
SKTags::~SKTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const std::string SKTags::styleNum			= "Sn";
const std::string SKTags::vAlign			= "vA";
const std::string SKTags::textColor			= "tC";
const std::string SKTags::disabledTextColor	= "tD";
const std::string SKTags::bgColor			= "bC";
const std::string SKTags::toggledBgColor	= "cB";
const std::string SKTags::bgImage			= "bI";
const std::string SKTags::toggledBgImage    = "iB";
const std::string SKTags::imageDir			= "fd";
const std::string SKTags::fgImageLocation	= "fD";

const std::string SKTags::featureNum		= "F#";
const std::string SKTags::hGridSize			= "hG";
const std::string SKTags::vGridSize			= "vG";
const std::string SKTags::hSpacing			= "hS";
const std::string SKTags::vSpacing			= "vS";
const std::string SKTags::groupNum			= "gn";
const std::string SKTags::fixedRowColMethod = "FM";
const std::string SKTags::defaultButton     = "Fb";

const std::string SKTags::flashkeyNum		= "f#";		//	FK_NUM_TAG - Quantum/Ultima only;
const std::string SKTags::flashkeyName		= "fd";		//	FK_DESC_TAG - Quantum/Ultima only;
const std::string SKTags::hPosition			= "hp";
const std::string SKTags::vPosition			= "vp";
const std::string SKTags::hSize				= "fh";		//	FK_H_SIZE_TAG
const std::string SKTags::vSize				= "fv";		//	FK_V_SIZE_TAG
const std::string SKTags::name1				= "kn";
const std::string SKTags::name2				= "kN";
const std::string SKTags::action			= "ft";		//	FK_REC_TYPE_TAG
const std::string SKTags::actionSubtype		= "as";
const std::string SKTags::actionParam1Int	= "fn";		//	FK_REC_NUM
const std::string SKTags::actionParam2Str	= "fS";

const std::string SKTags::scriptName      = "fx";
const std::string SKTags::scriptInterval  = "fX";


/******************************************************************************
*   SoftkeyStyleTags                                                          *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int SoftkeyStyleTags::dataType = 0;
std::string SoftkeyStyleTags::dataTypeName("SoftkeyStyleTags"); 
#endif

bool     SoftkeyStyleTags::_tagsInitialized_ = false;
TagsList SoftkeyStyleTags::softkeyStyleTags(endSoftkeyStyleFieldOffsets);
TagsMap  SoftkeyStyleTags::fieldMap;

SoftkeyStyleTags::SoftkeyStyleTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for any Totals class
		softkeyStyleTags[0] 										= emptyTag;
		softkeyStyleTags[fieldOffsetSKSStyleNumber]					= SKTags::styleNum;
		softkeyStyleTags[fieldOffsetSKSTextVerticalAlignment]		= SKTags::vAlign;
		softkeyStyleTags[fieldOffsetSKSTextColor]					= SKTags::textColor;
		softkeyStyleTags[fieldOffsetSKSGrayedOutTextColor]			= SKTags::disabledTextColor;
		softkeyStyleTags[fieldOffsetSKSBackgroundColor]				= SKTags::bgColor;
		softkeyStyleTags[fieldOffsetSKSBackgroundImage]				= SKTags::bgImage;
		softkeyStyleTags[fieldOffsetSKSImageDirectory]				= SKTags::imageDir;
		softkeyStyleTags[fieldOffsetSKSForegroundImageLocation]		= SKTags::fgImageLocation;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

SoftkeyStyleTags::~SoftkeyStyleTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

/******************************************************************************
*   SoftkeyFeatureTags                                                        *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int SoftkeyFeatureTags::dataType = 0;
std::string SoftkeyFeatureTags::dataTypeName("SoftkeyFeatureTags"); 
#endif

bool     SoftkeyFeatureTags::_tagsInitialized_ = false;
TagsList SoftkeyFeatureTags::softkeyFeatureTags(endSoftkeyFeatureFieldOffsets);
TagsMap  SoftkeyFeatureTags::fieldMap;

SoftkeyFeatureTags::SoftkeyFeatureTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for any Totals class
		softkeyFeatureTags[0] 									= emptyTag;
		softkeyFeatureTags[fieldOffsetSKFNumber]				= SKTags::featureNum;
		softkeyFeatureTags[fieldOffsetSKFHorizontalGridSize]	= SKTags::hGridSize;
		softkeyFeatureTags[fieldOffsetSKFVerticalGridSize]		= SKTags::vGridSize;
		softkeyFeatureTags[fieldOffsetSKFHorizontalSpacing]		= SKTags::hSpacing;
		softkeyFeatureTags[fieldOffsetSKFVerticalSpacing]		= SKTags::vSpacing;
		softkeyFeatureTags[fieldOffsetSKFDefSoftkeyGroup]		= SKTags::groupNum;
		softkeyFeatureTags[fieldOffsetSKFSoftkeyStyle]			= SKTags::styleNum;
		softkeyFeatureTags[fieldOffsetSKFDeptNum]               = DeptTags::deptNum;
		softkeyFeatureTags[fieldOffsetSKFFixedRowColMethod]     = SKTags::fixedRowColMethod;
		softkeyFeatureTags[fieldOffsetSKFDefaultButton]         = SKTags::defaultButton;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

SoftkeyFeatureTags::~SoftkeyFeatureTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

/******************************************************************************
*   SoftkeyTags                                                               *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int SoftkeyTags::dataType = 0;
std::string SoftkeyTags::dataTypeName("SoftkeyTags"); 
#endif

bool     SoftkeyTags::_tagsInitialized_ = false;
TagsList SoftkeyTags::softkeyTags(endSoftkeyFieldOffsets);
TagsMap  SoftkeyTags::fieldMap;

SoftkeyTags::SoftkeyTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for any Totals class
		softkeyTags[0] 										= emptyTag;
		softkeyTags[fieldOffsetSKeyGroupNum]				= SKTags::groupNum;
		softkeyTags[fieldOffsetSKeyHorizontalPos]			= SKTags::hPosition;
		softkeyTags[fieldOffsetSKeyVerticalPos]				= SKTags::vPosition;
		softkeyTags[fieldOffsetSKeyHorizontalSize]			= SKTags::hSize;
		softkeyTags[fieldOffsetSKeyVerticalSize]			= SKTags::vSize;
		softkeyTags[fieldOffsetSKeyBackgroundColor]			= SKTags::bgColor;
		softkeyTags[fieldOffsetSKeyToggledBackgroundColor]	= SKTags::toggledBgColor;
		softkeyTags[fieldOffsetSKeyBackgroundImage]			= SKTags::bgImage;
		softkeyTags[fieldOffsetSKeyToggledBackgroundImage]	= SKTags::toggledBgImage;
		softkeyTags[fieldOffsetSKeyName1]					= SKTags::name1;
		softkeyTags[fieldOffsetSKeyName2]					= SKTags::name2;
		softkeyTags[fieldOffsetSKeyAction]					= SKTags::action;
		softkeyTags[fieldOffsetSKeyActionIntegerParam]		= SKTags::actionParam1Int;
		softkeyTags[fieldOffsetSKeyActionStringParam]		= SKTags::actionParam2Str;
		softkeyTags[fieldOffsetSKeyActionCategoryNum]		= CategoryTags::categoryNum;
		softkeyTags[fieldOffsetSKeyActionOperatorNum]		= HobOperatorTags::opNumber;
		softkeyTags[fieldOffsetSKeyActionProductNum]		= ProductTags::prodNumber;
		softkeyTags[fieldOffsetSKeyActionFeatureNum]		= SKTags::featureNum;
		softkeyTags[fieldOffsetSKeyTextColor]				= SKTags::textColor;
		softkeyTags[fieldOffsetSKeyFlashkeyNum]				= SKTags::flashkeyNum;
		softkeyTags[fieldOffsetSKeyFlashkeyName]			= SKTags::flashkeyName;
		softkeyTags[fieldOffsetSKeyDeptNumber]				= DeptTags::deptNum;
		softkeyTags[fieldOffsetSKeyScriptName]          = SKTags::scriptName;
		softkeyTags[fieldOffsetSKeyScriptInterval]      = SKTags::scriptInterval;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

SoftkeyTags::~SoftkeyTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
/******************************************************************************
*   SoftkeyGroupTags                                                          *
******************************************************************************/
#ifdef USE_ACCOUNTANT
int SoftkeyGroupTags::dataType = 0;
std::string SoftkeyGroupTags::dataTypeName("SoftkeyGroupTags"); 
#endif

bool     SoftkeyGroupTags::_tagsInitialized_ = false;
TagsList SoftkeyGroupTags::softkeyGroupTags(endSoftkeyGroupFieldOffsets);
TagsMap  SoftkeyGroupTags::fieldMap;

SoftkeyGroupTags::SoftkeyGroupTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for any Totals class
		softkeyGroupTags[0] 								= emptyTag;
		softkeyGroupTags[fieldOffsetSKGGroupNumber]			= SKTags::groupNum;
		softkeyGroupTags[fieldOffsetSKGHorizontalGridSize]	= SKTags::hGridSize;
		softkeyGroupTags[fieldOffsetSKGVerticalGridSize]	= SKTags::vGridSize;
		softkeyGroupTags[fieldOffsetSKGHorizontalSpacing]	= SKTags::hSpacing;
		softkeyGroupTags[fieldOffsetSKGVerticalSpacing]		= SKTags::vSpacing;
		softkeyGroupTags[fieldOffsetSKGDeptNum]             = DeptTags::deptNum;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

SoftkeyGroupTags::~SoftkeyGroupTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
