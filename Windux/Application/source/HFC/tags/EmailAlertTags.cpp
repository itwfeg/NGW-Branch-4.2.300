/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "EmailAlertTags.h"
#include "HobEmailAlert.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EmailAlertTags::dataType = 0;
std::string EmailAlertTags::dataTypeName("EmailAlertTags"); 
#endif

const std::string EmailAlertTags::emailAlertNumber 				= "A#";
const std::string EmailAlertTags::emailAlertSubject1 			= "A1";
const std::string EmailAlertTags::emailAlertSubject2 			= "A2";
const std::string EmailAlertTags::emailAlertNotifyOnFailure 	= "Af";
const std::string EmailAlertTags::emailAlertNotifyOnResolution	= "Ar";
const std::string EmailAlertTags::emailAlertNotifyHandsFreeDeviceOnFailure		= "aF";
const std::string EmailAlertTags::emailAlertNotifyHandsFreeDeviceOnResolution	= "aR";
const std::string EmailAlertTags::emailAlertDelay				= "Ad";

TagsList EmailAlertTags::emailAlertTags(endHobEmailAlertFieldOffsets);
TagsMap  EmailAlertTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


EmailAlertTags::EmailAlertTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (pFieldList)
	{
		//initialize the the table of tags for the Class HobEmailAlert
		emailAlertTags[0] 								= emptyTag;
		emailAlertTags[fieldOffsetEANum]				= emailAlertNumber;
		emailAlertTags[fieldOffsetEASubject]			= emailAlertSubject1;
		emailAlertTags[fieldOffsetEANotifyOnFailure]	= emailAlertNotifyOnFailure;
		emailAlertTags[fieldOffsetEANotifyOnResolution] = emailAlertNotifyOnResolution;
		emailAlertTags[fieldOffsetEANotifyHandsFreeDeviceOnFailure]    = emailAlertNotifyHandsFreeDeviceOnFailure;
		emailAlertTags[fieldOffsetEANotifyHandsFreeDeviceOnResolution] = emailAlertNotifyHandsFreeDeviceOnResolution;
		emailAlertTags[fieldOffsetEADelay]				= emailAlertDelay;

		putTagsInMap(pFieldList);
	}
}

EmailAlertTags::~EmailAlertTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const TagsList &EmailAlertTags::getMyTagsList() const
{
	return emailAlertTags;
}

TagsMap &EmailAlertTags::getMyTagsMap() const
{
	return fieldMap;
}
