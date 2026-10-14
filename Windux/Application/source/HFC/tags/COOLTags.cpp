/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLTags.h"
#include "DeptTags.h"

#include "COOLCategory.h"
#include "COOLCountry.h"
#include "COOLProductionStep.h"
#include "COOLPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"
#include "COOLCategoryCountry.h"
#include "COOLCategoryProdStep.h"
#include "COOLCategoryPretext.h"
#include "CoolTotal.h"
#include "COOLListText.h"
#include "COOLList.h"

#include "ProductTags.h"
#include "Debug.h"


/******************************************************************************
*   The COOL Tags defined                                                     *
******************************************************************************/

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLTags::dataType = 0;
std::string COOLTags::dataTypeName("COOLTags"); 
#endif

COOLTags::COOLTags()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

COOLTags::~COOLTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const std::string COOLTags::coolCategoryNum   = "cb";
const std::string COOLTags::coolCategoryName1 = "c0";  // with a number zero, not the letter o
const std::string COOLTags::coolCategoryName2 = "C0";  // with a number zero, not the letter o

const std::string COOLTags::belongsToAllCOOLCategories = "CA";

const std::string COOLTags::coolCountryName1 = "c6";
const std::string COOLTags::coolCountryName2 = "C6";
const std::string COOLTags::coolCountryNum   = "c5";

const std::string COOLTags::coolProductionStepNum   = "c3";
const std::string COOLTags::coolProductionStepName1 = "c4";
const std::string COOLTags::coolProductionStepName2 = "cj";

const std::string COOLTags::coolPretextNum   = "ci";
const std::string COOLTags::coolPretextName1 = "ch";
const std::string COOLTags::coolPretextName2 = "cH";

const std::string COOLTags::coolTextNum  = "c1";
const std::string COOLTags::coolTextText = "c2";
const std::string COOLTags::coolTextType = "cq";

const std::string COOLTags::coolShortlistNum          = "cs";
const std::string COOLTags::coolShortlistFirstLink    = "c9";
const std::string COOLTags::coolShortlistOrderNum     = "CO"; // with a capital letter o, not the number zero.
const std::string COOLTags::coolShortlistListLink     = "c9";
const std::string COOLTags::coolShortlistListNextLink = "ca";

const std::string COOLTags::coolLogDateFirst = "ck";
const std::string COOLTags::coolLogTimeFirst = "cp";
const std::string COOLTags::coolLogDateLast	 = "cl";
const std::string COOLTags::coolLogTimeLast	 = "cn";



/******************************************************************************
*   COOLCategoryTags                                                          *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLCategoryTags::dataType = 0;
std::string COOLCategoryTags::dataTypeName("COOLCategoryTags"); 
#endif

bool     COOLCategoryTags::_tagsInitialized_ = false;
TagsList COOLCategoryTags::coolCategoryTags(endCOOLCategoryFieldOffsets);
TagsMap  COOLCategoryTags::fieldMap;

COOLCategoryTags::COOLCategoryTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolCategoryTags[0] 							 = emptyTag;
		coolCategoryTags[fieldOffsetCOOLCategoryNumber]	 = COOLTags::coolCategoryNum;
		coolCategoryTags[fieldOffsetCOOLCategoryName1]	 = COOLTags::coolCategoryName1;
		coolCategoryTags[fieldOffsetCOOLCategoryName2]	 = COOLTags::coolCategoryName2;
		coolCategoryTags[fieldOffsetCOOLCategoryDeptNum] = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLCategoryTags::~COOLCategoryTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

/******************************************************************************
*   COOLCountryTags                                                           *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLCountryTags::dataType = 0;
std::string COOLCountryTags::dataTypeName("COOLCountryTags"); 
#endif

bool     COOLCountryTags::_tagsInitialized_ = false;
TagsList COOLCountryTags::coolCountryTags(endCOOLCountryFieldOffsets);
TagsMap  COOLCountryTags::fieldMap;

COOLCountryTags::COOLCountryTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolCountryTags[0] 											= emptyTag;
		coolCountryTags[fieldOffsetCOOLCountryName1]				= COOLTags::coolCountryName1;
		coolCountryTags[fieldOffsetCOOLCountryName2]				= COOLTags::coolCountryName2;
		coolCountryTags[fieldOffsetCOOLCountryAllCategoriesFlag]	= COOLTags::belongsToAllCOOLCategories;
		coolCountryTags[fieldOffsetCOOLCountryCategory]				= COOLTags::coolCategoryNum;
		coolCountryTags[fieldOffsetCOOLCountryNum]					= COOLTags::coolCountryNum;
		coolCountryTags[fieldOffsetCOOLCountryDeptNum]              = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLCountryTags::~COOLCountryTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

/******************************************************************************
*   COOLCategoryCountryTags                                                   *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLCategoryCountryTags::dataType = 0;
std::string COOLCategoryCountryTags::dataTypeName("COOLCategoryCountryTags"); 
#endif

bool     COOLCategoryCountryTags::_tagsInitialized_ = false;
TagsList COOLCategoryCountryTags::coolCategoryCountryTags(endCOOLCategoryCountryFieldOffsets);
TagsMap  COOLCategoryCountryTags::fieldMap;

COOLCategoryCountryTags::COOLCategoryCountryTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	if ( !_tagsInitialized_ )
	{
		coolCategoryCountryTags[0] 										= emptyTag;
		coolCategoryCountryTags[fieldOffsetCOOLCatCountryCategoryNum]	= COOLTags::coolCategoryNum;
		coolCategoryCountryTags[fieldOffsetCOOLCatCountryCountryName]	= COOLTags::coolCountryName1;
		coolCategoryCountryTags[fieldOffsetCOOLCatCountryCountryNum]	= COOLTags::coolCountryNum;
		coolCategoryCountryTags[fieldOffsetCOOLCatCountryDeptNum]       = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLCategoryCountryTags::~COOLCategoryCountryTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

/******************************************************************************
*   COOLProductionStepTags                                                    *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLProductionStepTags::dataType = 0;
std::string COOLProductionStepTags::dataTypeName("COOLProductionStepTags"); 
#endif

bool     COOLProductionStepTags::_tagsInitialized_ = false;
TagsList COOLProductionStepTags::coolProductionStepTags(endCOOLProductionStepFieldOffsets);
TagsMap  COOLProductionStepTags::fieldMap;

COOLProductionStepTags::COOLProductionStepTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolProductionStepTags[0] 											   	= emptyTag;
		coolProductionStepTags[fieldOffsetCOOLProductionStepName1]				= COOLTags::coolProductionStepName1;
		coolProductionStepTags[fieldOffsetCOOLProductionStepName2]				= COOLTags::coolProductionStepName2;
		coolProductionStepTags[fieldOffsetCOOLProductionStepAllCategoriesFlag]	= COOLTags::belongsToAllCOOLCategories;
		coolProductionStepTags[fieldOffsetCOOLProductionStepCategory]			= COOLTags::coolCategoryNum;
		coolProductionStepTags[fieldOffsetCOOLProductionStepNum]				= COOLTags::coolProductionStepNum;
		coolProductionStepTags[fieldOffsetCOOLProductionStepDeptNum]            = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLProductionStepTags::~COOLProductionStepTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

/******************************************************************************
*   COOLCategoryProductionStepTags                                            *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLCategoryProductionStepTags::dataType = 0;
std::string COOLCategoryProductionStepTags::dataTypeName("COOLCategoryProductionStepTags"); 
#endif

bool     COOLCategoryProductionStepTags::_tagsInitialized_ = false;
TagsList COOLCategoryProductionStepTags::coolCategoryProductionStepTags(endCOOLCategoryProdStepFieldOffsets);
TagsMap  COOLCategoryProductionStepTags::fieldMap;

COOLCategoryProductionStepTags::COOLCategoryProductionStepTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	if ( !_tagsInitialized_ )
	{
		coolCategoryProductionStepTags[0] 										= emptyTag;
		coolCategoryProductionStepTags[fieldOffsetCOOLCatProdStepCategoryNum]	= COOLTags::coolCategoryNum;
		coolCategoryProductionStepTags[fieldOffsetCOOLCatProdStepStepName]		= COOLTags::coolProductionStepName1;
		coolCategoryProductionStepTags[fieldOffsetCOOLCatProdStepNum]			= COOLTags::coolProductionStepNum;
		coolCategoryProductionStepTags[fieldOffsetCOOLCatProdDeptNum]           = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLCategoryProductionStepTags::~COOLCategoryProductionStepTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
/******************************************************************************
*   COOLPretextTags                                                           *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLPretextTags::dataType = 0;
std::string COOLPretextTags::dataTypeName("COOLPretextTags"); 
#endif

bool     COOLPretextTags::_tagsInitialized_ = false;
TagsList COOLPretextTags::coolPretextTags(endCOOLPretextFieldOffsets);
TagsMap  COOLPretextTags::fieldMap;

COOLPretextTags::COOLPretextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	if ( !_tagsInitialized_ )
	{
		coolPretextTags[0] 											= emptyTag;
		coolPretextTags[fieldOffsetCOOLPretextNumber]				= COOLTags::coolPretextNum;
		coolPretextTags[fieldOffsetCOOLPretextName1]				= COOLTags::coolPretextName1;
		coolPretextTags[fieldOffsetCOOLPretextName2]				= COOLTags::coolPretextName2;
		coolPretextTags[fieldOffsetCOOLPretextAllCategoriesFlag]	= COOLTags::belongsToAllCOOLCategories;
		coolPretextTags[fieldOffsetCOOLPretextCategory]				= COOLTags::coolCategoryNum;
		coolPretextTags[fieldOffsetCOOLPretextTextNum]				= COOLTags::coolTextNum;
		coolPretextTags[fieldOffsetCOOLPretextTextText]				= COOLTags::coolTextText;
		coolPretextTags[fieldOffsetCOOLPretextTextTextType]			= COOLTags::coolTextType;
		coolPretextTags[fieldOffsetCOOLPretextDeptNum]              = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLPretextTags::~COOLPretextTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
/******************************************************************************
*   COOLCategoryPretextTags                                                   *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLCategoryPretextTags::dataType = 0;
std::string COOLCategoryPretextTags::dataTypeName("COOLCategoryPretextTags"); 
#endif

bool     COOLCategoryPretextTags::_tagsInitialized_ = false;
TagsList COOLCategoryPretextTags::coolCategoryPretextTags(endCOOLCategoryPretextFieldOffsets);
TagsMap  COOLCategoryPretextTags::fieldMap;

COOLCategoryPretextTags::COOLCategoryPretextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolCategoryPretextTags[0] 										= emptyTag;
		coolCategoryPretextTags[fieldOffsetCOOLCatPretextCategoryNum]	= COOLTags::coolCategoryNum;
		coolCategoryPretextTags[fieldOffsetCOOLCatPretextPretextNum]	= COOLTags::coolPretextNum;
		coolCategoryPretextTags[fieldOffsetCOOLCatPretextDeptNum]       = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLCategoryPretextTags::~COOLCategoryPretextTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
/******************************************************************************
*   COOLTextTags                                                              *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLTextTags::dataType = 0;
std::string COOLTextTags::dataTypeName("COOLTextTags"); 
#endif

bool     COOLTextTags::_tagsInitialized_ = false;
TagsList COOLTextTags::coolTextTags(endCOOLTextFieldOffsets);
TagsMap  COOLTextTags::fieldMap;

COOLTextTags::COOLTextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolTextTags[0] 							= emptyTag;
		coolTextTags[fieldOffsetCOOLTextRecordNum]	= COOLTags::coolTextNum;
		coolTextTags[fieldOffsetCOOLTextText]		= COOLTags::coolTextText;
		coolTextTags[fieldOffsetCOOLTextTextType]	= COOLTags::coolTextType;
		coolTextTags[fieldOffsetCOOLTextDeptNum]    = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLTextTags::~COOLTextTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
/******************************************************************************
*   COOLHobShortlistTags                                                         *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLHobShortlistTags::dataType = 0;
std::string COOLHobShortlistTags::dataTypeName("COOLHobShortlistTags"); 
#endif

bool     COOLHobShortlistTags::_tagsInitialized_ = false;
TagsList COOLHobShortlistTags::coolHobShortlistTags(endCOOLShortlistFieldOffsets);
TagsMap  COOLHobShortlistTags::fieldMap;

COOLHobShortlistTags::COOLHobShortlistTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolHobShortlistTags[0] 									= emptyTag;
		coolHobShortlistTags[fieldOffsetCOOLShortlistProductNum]	= emptyTag;
		coolHobShortlistTags[fieldOffsetCOOLShortlistTextNum]		= COOLTags::coolTextNum;
		coolHobShortlistTags[fieldOffsetCOOLShortlistOrder] 		= emptyTag;
		coolHobShortlistTags[fieldOffsetCOOLShortlistLink]			= COOLTags::coolShortlistListLink;
		coolHobShortlistTags[fieldOffsetCOOLShortlistNextLink]		= COOLTags::coolShortlistListNextLink;
		coolHobShortlistTags[fieldOffsetCOOLShortlistDeptNum]       = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLHobShortlistTags::~COOLHobShortlistTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
/******************************************************************************
*   COOLShortlistTags                                                         *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLShortlistTags::dataType = 0;
std::string COOLShortlistTags::dataTypeName("COOLShortlistTags"); 
#endif

bool     COOLShortlistTags::_tagsInitialized_ = false;
TagsList COOLShortlistTags::coolShortlistTags(endCOOLShortlistFieldOffsets);
TagsMap  COOLShortlistTags::fieldMap;

COOLShortlistTags::COOLShortlistTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolShortlistTags[0] 									= emptyTag;
		coolShortlistTags[fieldOffsetCOOLShortlistProductNum]	= ProductTags::prodNumber;		//	Support for older versions.
		coolShortlistTags[fieldOffsetCOOLShortlistNum]			= COOLTags::coolShortlistNum;
		coolShortlistTags[fieldOffsetCOOLShortlistTextNum]		= COOLTags::coolTextNum;
		coolShortlistTags[fieldOffsetCOOLShortlistOrder] 		= COOLTags::coolShortlistOrderNum;
		coolShortlistTags[fieldOffsetCOOLShortlistLink]			= COOLTags::coolShortlistListLink;
		coolShortlistTags[fieldOffsetCOOLShortlistNextLink]		= COOLTags::coolShortlistListNextLink;
		coolShortlistTags[fieldOffsetCOOLShortlistDeptNum]      = DeptTags::deptNum;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLShortlistTags::~COOLShortlistTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


/******************************************************************************
*   COOLListTextTags                                                          *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLListTextTags::dataType = 0;
std::string COOLListTextTags::dataTypeName("COOLListTextTags"); 
#endif

bool     COOLListTextTags::_tagsInitialized_ = false;
TagsList COOLListTextTags::coolListTextTags(endCOOLListTextFieldOffsets);
TagsMap  COOLListTextTags::fieldMap;

COOLListTextTags::COOLListTextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolListTextTags[0] 									= emptyTag;
		coolListTextTags[fieldOffsetCOOLListTextOrder]			= COOLTags::coolShortlistOrderNum;
		coolListTextTags[fieldOffsetCOOLListTextLinkNum]		= COOLTags::coolShortlistListLink;
		coolListTextTags[fieldOffsetCOOLListTextNextLinkNum]	= COOLTags::coolShortlistListNextLink;
		coolListTextTags[fieldOffsetCOOLListTextTextNum]		= COOLTags::coolTextNum;
		coolListTextTags[fieldOffsetCOOLListTextProductNum]		= ProductTags::prodNumber;
		coolListTextTags[fieldOffsetCOOLListTextDeptNum]        = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLListTextTags::~COOLListTextTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


/******************************************************************************
*   COOLListTags                                                              *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLListTags::dataType = 0;
std::string COOLListTags::dataTypeName("COOLListTags"); 
#endif

bool     COOLListTags::_tagsInitialized_ = false;
TagsList COOLListTags::coolListTags(endCOOLListFieldOffsets);
TagsMap  COOLListTags::fieldMap;

COOLListTags::COOLListTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolListTags[0] 								= emptyTag;
		coolListTags[fieldOffsetCOOLListProductNum]		= ProductTags::prodNumber;
		coolListTags[fieldOffsetCOOLFirstLinkNum]		= COOLTags::coolShortlistFirstLink;
		coolListTags[fieldOffsetCOOLListDeptNum]        = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLListTags::~COOLListTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


/******************************************************************************
*   COOLShortlistTextTags                                                     *
******************************************************************************/

#ifdef USE_ACCOUNTANT
int COOLShortlistTextTags::dataType = 0;
std::string COOLShortlistTextTags::dataTypeName("COOLShortlistTextTags"); 
#endif

bool     COOLShortlistTextTags::_tagsInitialized_ = false;
TagsList COOLShortlistTextTags::coolShortlistTextTags(6/*endCOOLShortlistTextFieldOffsets*/);
TagsMap  COOLShortlistTextTags::fieldMap;

COOLShortlistTextTags::COOLShortlistTextTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		coolShortlistTextTags[0] 												= emptyTag;
		coolShortlistTextTags[1/*fieldOffsetCOOLShortlistTextListNum*/]		= COOLTags::coolShortlistNum;
		coolShortlistTextTags[2/*fieldOffsetCOOLShortlistTextTextNum*/]		    = COOLTags::coolTextNum;
		coolShortlistTextTags[3/*fieldOffsetCOOLShortlistTextOrderNum*/]		= COOLTags::coolShortlistOrderNum;
		coolShortlistTextTags[4/*fieldOffsetCOOLShortlistTextListLink*/]		= COOLTags::coolShortlistListLink;
		coolShortlistTextTags[5/*fieldOffsetCOOLShortlistTextListNextLink*/]	= COOLTags::coolShortlistListNextLink;
		coolShortlistTextTags[6/*DeptNum*/]                                     = DeptTags::deptNum;

		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

COOLShortlistTextTags::~COOLShortlistTextTags() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
