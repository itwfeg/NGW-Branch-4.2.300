/* Evolution Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DEVICETAGS_H
#define DEVICETAGS_H

#include "Tags.h"

class DeviceTags : public Tags
{
	public:
		DeviceTags(FieldOffsetsList *pFieldList = (FieldOffsetsList *)NULL);
		~DeviceTags();

		static const std::string deviceType;
		static const std::string deviceSubType;
		static const std::string kind;
		static const std::string serialNumber;
		static const std::string hardwareVersion;
		static const std::string softwareVersion;
		static const std::string checkPresence;
		static const std::string installedDate;
		static const std::string serviceHistory;
		static const std::string userName;
		static const std::string systemName;
		static const std::string ipAddress;     

	protected:
		const TagsList &getMyTagsList() const {return deviceTags;}
		TagsMap &getMyTagsMap() const {return fieldMap;}

	private:
		static bool _tagsInitialized_;
		static TagsList deviceTags;
		static TagsMap fieldMap;	//example pair: p#,fieldOffsetProductNum

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//DEVICETAGS_H 
