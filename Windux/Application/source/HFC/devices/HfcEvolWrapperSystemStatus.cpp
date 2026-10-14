/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperSystemStatus.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperSystemStatus::_dataType_ = 0;
std::string HfcEvolWrapperSystemStatus::_dataTypeName_("HfcEvolWrapperSystemStatus"); 
#endif

HfcEvolWrapperSystemStatus::HfcEvolWrapperSystemStatus(const char *data) : EvolWrapperSystemStatus(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperSystemStatus", "Constructor() - building the lists.");

	HobString version;
	for (unsigned int x = 0; x < EvolWrapperSystemStatus::_maxBoards_; ++x)
	{
		const EvolWrapperNode node( getNode(x) );

		// SOF-1365: give all nodes to caller
		nodeList.push_back(Integer::toString(node.getNodeId()));
		configVersionList.push_back(Hex::toString(node.getConfigVersion()));
		deviceInitFlagsList.push_back(Hex::toString(node.getDeviceInitFlags()));
		errorFlagsList.push_back(Hex::toString(node.getErrorFlags()));
		wrapBoxChecksumList.push_back(Hex::toString(node.getWrapBoxChecksum()));

		version = Integer::toString(node.getAppFlash().getMajorRevision());
		version.append(".").append(Integer::toString(node.getAppFlash().getMinorRevision()));
		appVersionList.push_back(version);

		version = Integer::toString(node.getDataFlash().getMajorRevision());
		version.append(".").append(Integer::toString(node.getDataFlash().getMinorRevision()));
		dataVersionList.push_back(version);

		version = Integer::toString(node.getBootFlash().getMajorRevision());
		version.append(".").append(Integer::toString(node.getBootFlash().getMinorRevision()));
		bootVersionList.push_back(version);
	}
}


HfcEvolWrapperSystemStatus::~HfcEvolWrapperSystemStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

}

const char *HfcEvolWrapperSystemStatus::serialize(int format) const
{
	Debug::trace("HfcEvolWrapperSystemStatus", "serialize()");
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolWrSysStatusFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetEvolWrSysStatusReportSysError:
						HfcObject::serializedData.append(Hex::toString(getReportSysError()));
						break;
					case fieldOffsetEvolWrSysStatusNode:
						HfcObject::serializedData.append(nodeList.serialize(format));
						break;
					case fieldOffsetEvolWrSysStatusAppVersion:
						HfcObject::serializedData.append(appVersionList.serialize(format));
						break;
					case fieldOffsetEvolWrSysStatusDataVersion:
						HfcObject::serializedData.append(dataVersionList.serialize(format));
						break;
					case fieldOffsetEvolWrSysStatusBootVersion:
						HfcObject::serializedData.append(bootVersionList.serialize(format));
						break;

					case fieldOffsetEvolWrSysStatusConfigVersion:
						HfcObject::serializedData.append(configVersionList.serialize(format));
						break;
					case fieldOffsetEvolWrSysStatusDeviceInitFlags:
						HfcObject::serializedData.append(deviceInitFlagsList.serialize(format));
						break;
					case fieldOffsetEvolWrSysStatusErrorFlags:
						HfcObject::serializedData.append(errorFlagsList.serialize(format));
						break;
					case fieldOffsetEvolWrSysStatusWrapBoxChecksum:
						HfcObject::serializedData.append(wrapBoxChecksumList.serialize(format));
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			Debug::trace("HfcEvolWrapperSystemStatus", "serialize(Invalid format!!!)");
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
