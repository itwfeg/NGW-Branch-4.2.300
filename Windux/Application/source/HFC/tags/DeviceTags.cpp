/* Evolution Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DeviceTags.h"
#include "Device.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
int DeviceTags::dataType = 0;
std::string DeviceTags::dataTypeName("DeviceTags"); 
#endif

const std::string DeviceTags::deviceType      = "dP";
const std::string DeviceTags::deviceSubType   = "ds";
const std::string DeviceTags::kind            = "dk";
const std::string DeviceTags::serialNumber    = "dn";
const std::string DeviceTags::hardwareVersion = "dH";
const std::string DeviceTags::softwareVersion = "dS";
const std::string DeviceTags::checkPresence   = "dK";
const std::string DeviceTags::installedDate   = "di";
const std::string DeviceTags::serviceHistory  = "dy";
const std::string DeviceTags::userName        = "dN";
const std::string DeviceTags::systemName      = "dd";
const std::string DeviceTags::ipAddress       = "dI";

bool     DeviceTags::_tagsInitialized_ = false;
TagsList DeviceTags::deviceTags(endDeviceFieldOffsets);
TagsMap  DeviceTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


DeviceTags::DeviceTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		deviceTags[0] = emptyTag;
		deviceTags[fieldOffsetDeviceType]      = deviceType;
		deviceTags[fieldOffsetDeviceSubType]   = deviceSubType;
		deviceTags[fieldOffsetKind]            = kind;
		deviceTags[fieldOffsetSerialNumber]    = serialNumber;
		deviceTags[fieldOffsetHardwareVersion] = hardwareVersion;
		deviceTags[fieldOffsetSoftwareVersion] = softwareVersion;
		deviceTags[fieldOffsetCheckPresence]   = checkPresence;
		deviceTags[fieldOffsetInstalledDate]   = installedDate;
		deviceTags[fieldOffsetServiceHistory]  = serviceHistory;
		deviceTags[fieldOffsetUserName]        = userName;
		deviceTags[fieldOffsetSystemName]      = systemName;
		deviceTags[fieldOffsetIPAddress]       = ipAddress;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

DeviceTags::~DeviceTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}