/* Access Back-room Scale Project
 * COPYRIGHT (c) 2008.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcUpgradeList.h"

#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcUpgradeList::_dataType_ = 0;
std::string HfcUpgradeList::_dataTypeName_("HfcUpgradeList"); 
#endif


HfcUpgradeList::HfcUpgradeList(const char *data) : UpgradeList(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	std::list<UpgradeVersions>::iterator fieldIter = versionData_.begin();
	while (fieldIter != versionData_.end())
	{
		add(fieldIter);
		++fieldIter;
	}
}

HfcUpgradeList::~HfcUpgradeList()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcUpgradeList::serialize(int format) const
{
	Debug::trace("HfcUpgradeList", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(components.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(curVersions.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(newVersions.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(isPending.serialize(format));
		}
		break;
	}

	return HfcObject::serializedData.data();
}

void HfcUpgradeList::add(std::list<UpgradeVersions>::iterator iter)
{
	HobVersion currVer = iter->getCurrentVersion();
	HobVersion newVer = iter->getNewVersion();
	bool isDifferent = false;

	//	Get the component from currVer as it should always be there.
	HobVersion::VersionType currVerType = currVer.getVersionType();
	HobVersion::VersionType newVerType = newVer.getVersionType();

	components.push_back(Integer::toString(currVerType));
	curVersions.push_back(HobString(Integer::toString(currVer.getMajorRevision())) + "." + HobString(Integer::toString(currVer.getMinorRevision())));
	if (newVerType != currVerType)
	{
		Debug::trace("HfcUpgradeList", "add(" + HobString(Integer::toString(currVerType)) + ":  current = " + currVer.getRevisionString() + "  new = None");
		newVersions.push_back("-");
	}
	else
	{
		Debug::trace("HfcUpgradeList", "add(" + HobString(Integer::toString(currVerType)) + ":  current = " + currVer.getRevisionString() + "  new = " + newVer.getRevisionString());
		newVersions.push_back(HobString(Integer::toString(newVer.getMajorRevision())) + "." + HobString(Integer::toString(newVer.getMinorRevision())));
	}
	if ((newVerType == currVerType) && (currVer.getRevisionString() != newVer.getRevisionString()))
		isDifferent = true;
	else
		isDifferent = false;
	isPending.push_back(Bool::toString(isDifferent));	//	If it's different, then it will be pending.
}
