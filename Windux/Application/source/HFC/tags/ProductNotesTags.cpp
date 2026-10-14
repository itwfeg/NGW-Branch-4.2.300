/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductNotesTags.h"
#include "DeptTags.h"
#include "ProductText.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductNotesTags::dataType = 0;
std::string ProductNotesTags::dataTypeName("ProductNotesTags"); 
#endif

const std::string ProductNotesTags::productNotesTextNumber 	= "n#";
const std::string ProductNotesTags::productNotesTextText 	= "an";

bool     ProductNotesTags::_tagsInitialized_ = false;
TagsList ProductNotesTags::productNotesTags(endTextFieldOffsets);
TagsMap  ProductNotesTags::fieldMap;	//example pair: r#,fieldOffsetTextNum


ProductNotesTags::ProductNotesTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the the table of tags for this class
		productNotesTags[0]                        = emptyTag;
		productNotesTags[fieldOffsetTextNum]       = productNotesTextNumber; 
		productNotesTags[fieldOffsetTextErrorCode] = emptyTag;
		productNotesTags[fieldOffsetTextText]      = productNotesTextText;
		productNotesTags[fieldOffsetTextDeptNum]   = DeptTags::deptNum;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductNotesTags::~ProductNotesTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}