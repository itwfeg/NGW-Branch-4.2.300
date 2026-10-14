#include "UpgradeVersions.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"


#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UpgradeVersions::_dataType_ = 0;
std::string UpgradeVersions::_dataTypeName_("UpgradeVersions"); 
#endif


UpgradeVersions::UpgradeVersions()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


UpgradeVersions::UpgradeVersions(const HobVersion &curVer, const HobVersion *newVer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	currentVersion_.setVersionType(curVer.getVersionType());
	currentVersion_.setMajorRevision(curVer.getMajorRevision());
	currentVersion_.setMinorRevision(curVer.getMinorRevision());

	if (newVer)
	{
		newVersion_.setVersionType(newVer->getVersionType());
		newVersion_.setMajorRevision(newVer->getMajorRevision());
		newVersion_.setMinorRevision(newVer->getMinorRevision());
	}
}


UpgradeVersions::UpgradeVersions(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	int j = 0;
	currentVersion_ = HobComplexVersion(&buffer[j]);
	currentVersion_.serialize();
	j += currentVersion_.getSerializedLength();

	newVersion_ = HobComplexVersion(&buffer[j]);
}

UpgradeVersions::UpgradeVersions(const UpgradeVersions &uv)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	currentVersion_ = const_cast <UpgradeVersions&> (uv).getCurrentVersion();
	newVersion_ = const_cast <UpgradeVersions&> (uv).getNewVersion();
}

UpgradeVersions::~UpgradeVersions()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *UpgradeVersions::serialize() const
{
	// Now, for the derived class items

	serializedData_ = "";
	const char *pCurrentVersion = currentVersion_.serialize();
	serializedData_.append(pCurrentVersion, currentVersion_.getSerializedLength());

	const char *pNewVersion = newVersion_.serialize();
	serializedData_.append(pNewVersion, newVersion_.getSerializedLength());

	return serializedData_.data();
}

HobVersion& UpgradeVersions::getCurrentVersion()
{
	return currentVersion_;
}

const HobVersion& UpgradeVersions::getCurrentVersion() const
{
	return currentVersion_;
}

HobVersion& UpgradeVersions::getNewVersion()
{
	return newVersion_;
}

const HobVersion& UpgradeVersions::getNewVersion() const
{
	return newVersion_;
}

void UpgradeVersions::show()
{
	if (Debug::isTracing("UpgradeVersions"))
	{
		const std::string curStr = currentVersion_.getRevisionString();
		const std::string newStr = newVersion_.getRevisionString();
		std::string curDesc, newDesc;
		currentVersion_.getTypeDescriptor(curDesc);
		newVersion_.getTypeDescriptor(newDesc);
		Debug::trace("UpgradeVersions", "*****  show  *****");
		Debug::trace("UpgradeVersions", "Current Version:   " + curDesc + " :  " + curStr);
		Debug::trace("UpgradeVersions", "    New Version:   " + newDesc + " :  " + newStr);
		Debug::trace("UpgradeVersions", "*****************");
	}
}
