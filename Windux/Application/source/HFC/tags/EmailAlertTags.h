/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EMAILALERTTAGS_H
#define EMAILALERTTAGS_H

#include "Tags.h"

class EmailAlertTags : public Tags
{
	public:
		EmailAlertTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~EmailAlertTags();

	protected:
		const TagsList &getMyTagsList() const;
		TagsMap &getMyTagsMap() const;

	private:
		static const std::string emailAlertNumber;
		static const std::string emailAlertSubject1;
		static const std::string emailAlertSubject2;
		static const std::string emailAlertNotifyOnFailure;
		static const std::string emailAlertNotifyOnResolution;
		static const std::string emailAlertNotifyHandsFreeDeviceOnFailure;
		static const std::string emailAlertNotifyHandsFreeDeviceOnResolution;
		static const std::string emailAlertDelay;

		static TagsList emailAlertTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//EMAILALERTTAGS_H 
