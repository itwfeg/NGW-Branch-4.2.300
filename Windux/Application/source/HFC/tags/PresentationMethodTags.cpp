/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PresentationMethodTags.h"
#include "PresentationMethod.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PresentationMethodTags::dataType = 0;
std::string PresentationMethodTags::dataTypeName("PresentationMethodTags"); 
#endif

const std::string PresentationMethodTags::presentationMethodType			= "Pt";
const std::string PresentationMethodTags::presentationMethodDescription = "Pd";

TagsList PresentationMethodTags::presentationMethodTags(endPresentationMethodFieldOffsets);
TagsMap  PresentationMethodTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


PresentationMethodTags::PresentationMethodTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the table of tags for the Presentation Methods class
		presentationMethodTags[0] 									= emptyTag;
		presentationMethodTags[fieldOffsetPMType]				= presentationMethodType;
		presentationMethodTags[fieldOffsetPMDescription]	= presentationMethodDescription;

		putTagsInMap(pFieldList);
	}
}

PresentationMethodTags::~PresentationMethodTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &PresentationMethodTags::getMyTagsList() const
{
	return presentationMethodTags;
}

TagsMap &PresentationMethodTags::getMyTagsMap() const
{
	return fieldMap;
}
