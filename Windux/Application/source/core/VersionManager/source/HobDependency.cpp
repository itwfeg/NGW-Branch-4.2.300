#include <iostream>
#include <limits.h>

using namespace std;

#include "HobDependency.h"
#include "Integer.h"
#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobDependency::dataType = 0;
std::string HobDependency::dataTypeName("HobDependency"); 
#endif

HobDependency::HobDependency(const HobDependency& dep):
	versionType_(dep.versionType_), minMajRev_(dep.minMajRev_), maxMajRev_(dep.maxMajRev_), dependencyString_(dep.dependencyString_),
	serialized_("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HobDependency::HobDependency():
	versionType_(HobVersion::UnknownComponent), minMajRev_(0), maxMajRev_(USHRT_MAX), dependencyString_(""), serialized_("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HobDependency::HobDependency(const std::string &dep)
{
	std::string::size_type pos1 = 0, pos2 = 0;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	versionType_ = HobVersion::UnknownComponent;
	minMajRev_ = 0;
	maxMajRev_ = USHRT_MAX;

	pos1 = dep.find(DEPENDENCY_SEPERATOR);
	if (pos1 != std::string::npos)
	{

			pos2 = dep.find(HobVersion::VERSION_SEPERATOR);

			if (pos2 < pos1)
			{
				std::string vt(dep.substr(0, pos2));
				versionType_ = static_cast <HobVersion::VersionType>(Integer::valueOf(vt));

				std::string minRev(dep.substr(pos2+1, pos1));
				minMajRev_ = static_cast <UINT16>(Integer::valueOf(minRev));
			}

			pos2 = dep.rfind(HobVersion::VERSION_SEPERATOR);

			if (pos2 > pos1)
			{

				std::string vt(dep.substr(pos1+1, pos2));
				versionType_ = static_cast <HobVersion::VersionType>(Integer::valueOf(vt));

				std::string maxRev(dep.substr(pos2+1));
				maxMajRev_ = static_cast <UINT16>(Integer::valueOf(maxRev));

			}

	}
	dependencyString_ = dep;
}


HobDependency::~HobDependency()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

bool HobDependency::isAValidDependencyString(const std::string &dep)
{
	std::string::size_type pos1 = 0, pos2 = 0;

	pos1 = dep.find(DEPENDENCY_SEPERATOR);
	if (pos1 == std::string::npos)
	{
		Debug::trace("firmware.upgrade", HobString("isAValidDependencyString:not a valid dependency string"));
		return false;
	}

	pos2 = dep.find(HobVersion::VERSION_SEPERATOR);

	if (pos2 < pos1)
	{
		if (!HobVersion::isAValidMajorVersionString(dep.substr(0, pos1)))
		{
			Debug::trace("firmware.upgrade", HobString("isAValidDependencyString:not a valid dependency string"));
			return false;
		}
	}

	pos2 = dep.rfind(HobVersion::VERSION_SEPERATOR);

	if (pos2 > pos1)
	{
		if (!HobVersion::isAValidMajorVersionString(dep.substr(pos1+1, pos2)))
		{
			Debug::trace("firmware.upgrade", HobString("isAValidDependencyString:not a valid dependency string"));
			return false;
		}
	}

	return true;

}

void HobDependency::buildDependencyString()
{
char tmp[10];

	dependencyString_ = "";

	if (minMajRev_ != 0)
	{
		sprintf(tmp, "%d", versionType_);
		dependencyString_ += tmp;
		dependencyString_ += ".";
		sprintf(tmp, "%d", minMajRev_);
		dependencyString_ += tmp;
	}

	dependencyString_ += ">";
	if (maxMajRev_ != USHRT_MAX)
	{
		sprintf(tmp, "%d", versionType_);

		dependencyString_ += tmp;
		dependencyString_ += ".";

		sprintf(tmp, "%d", maxMajRev_);
		dependencyString_ += tmp;
	}

}

const std::string* HobDependency::toString()
{
char tmp[10];

	sprintf(tmp, "%d", versionType_);

	serialized_ = tmp;
	serialized_+= ":";
	sprintf(tmp, "%d", minMajRev_);
	serialized_+= tmp;
	serialized_+= ">";
	if (maxMajRev_ != USHRT_MAX)
	{
		sprintf(tmp, "%d", maxMajRev_);
		serialized_+= tmp;
	}

	return &serialized_;

}


HobDependency::DependencyResult HobDependency::dependencyCheck(HobVersion* ver)
{
HobVersion::VersionType vt = ver->getVersionType();
UINT16 majRev = ver->getMajorRevision();

	if ((versionType_ != HobVersion::UnknownComponent) && (vt != versionType_) )
			return NoDependency;

	if ((majRev >= minMajRev_) && (majRev <= maxMajRev_))
		return DependencySucceeded;

	return DependencyFailed;

}


void HobDependency::saveToPropertiesFile(util_env::Properties *pINI)
{
char tmp[10];

	if (pINI != NULL)
	{
		std::string key("component.");
		sprintf(tmp, "%d", versionType_);
		key += tmp;
		key += ".dependency";

		pINI->setProperty(key, dependencyString_);
	}	


}

