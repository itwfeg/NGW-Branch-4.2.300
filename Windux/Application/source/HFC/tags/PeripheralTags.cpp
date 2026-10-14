/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PeripheralTags.h"
#include "Peripheral.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PeripheralTags::dataType = 0;
std::string PeripheralTags::dataTypeName("PeripheralTags"); 
#endif

const std::string PeripheralTags::type					= "z1";
const std::string PeripheralTags::subType				= "z2";
const std::string PeripheralTags::kind					= "z3";
const std::string PeripheralTags::serialNumber		= "z4";
const std::string PeripheralTags::installationDate	= "z5";
const std::string PeripheralTags::serviceHistory	= "z6";
const std::string PeripheralTags::hardwareVersion	= "z7";
const std::string PeripheralTags::softwareVersion	= "z8";
const std::string PeripheralTags::checkingPresence	= "z9";

bool     PeripheralTags::_tagsInitialized_ = false;
TagsList PeripheralTags::peripheralTags(endPeripheralFieldOffsets);
TagsMap  PeripheralTags::fieldMap;  //example pair: r#,fieldOffsetTextNum


PeripheralTags::PeripheralTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for the Peripheral class
		peripheralTags[0] 												= emptyTag;
		peripheralTags[fieldOffsetPeripheralType]					= type;
		peripheralTags[fieldOffsetPeripheralSubType]				= subType;
		peripheralTags[fieldOffsetPeripheralKind]					= kind;
		peripheralTags[fieldOffsetPeripheralSerialNumber]		= serialNumber;
		peripheralTags[fieldOffsetPeripheralInstallationDate]	= installationDate;
		peripheralTags[fieldOffsetPeripheralServiceHistory]	= serviceHistory;
		peripheralTags[fieldOffsetPeripheralHardwareVersion]	= hardwareVersion;
		peripheralTags[fieldOffsetPeripheralSoftwareVersion]	= softwareVersion;
		peripheralTags[fieldOffsetPeripheralCheckPresence]		= checkingPresence;
		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

PeripheralTags::~PeripheralTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}