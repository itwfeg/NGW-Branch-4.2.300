#ifndef HOBCOMPLEXVERSION_H
#define HOBCOMPLEXVERSION_H

//#include "vxWorks.h"
#include "evolution.h"
#include <string>
#include <map>

#include "HobVersion.h"
#include "HobDependency.h"
#include "properties/properties.h"
#include "filesys/directory.h"
//#include "usbdLib.h"
namespace util_env
{
class PropertiesTree;
}
namespace util_filesystem
{
class File;
}


class HobComplexVersion : public HobVersion
{

	public:

		HobComplexVersion();
		HobComplexVersion(const char *buffer);
		HobComplexVersion(const HobComplexVersion& cv);
		HobComplexVersion(const std::string& version, const std::string& dependencyList, bool installed = false);
		~HobComplexVersion();

		const std::string* toString();

		HobDependency::DependencyResult dependencyCheck(HobVersion* ver);

		bool getIsInstalled(){return installed_;}
		void setIsInstalled(bool installed){installed_ = installed;}

		util_filesystem::Directory* getSoftwareLocation() { return &softLocDir_; }

		void saveToPropertiesFile(util_env::PropertiesTree *pINI);

		static void getSoftPath(HobVersion::VersionType vt, std::string& path);
		static void getTempDir(HobVersion::VersionType vt, util_filesystem::Directory& tempDir, util_filesystem::File& tempZip);
		static void getExtractionDir(HobVersion::VersionType vt, util_filesystem::Directory& tempDir, USBD_NODE_ID UNUSED(nodeId));
		static HobVersion::VersionType getHardwareTypeForSoftwareType(HobVersion::VersionType swvt);
		const char *serialize() const;
		static HobVersion::VersionType getCorrespondingSoftTypeFromHardType(HobVersion::VersionType hardwareType);
		bool isUpgradeForcible();
	private:


		bool installed_;
		util_filesystem::Directory softLocDir_;

		std::map<HobVersion::VersionType, HobDependency *> dependencyList_;

		const static char DEPENDENCY_SEPERATOR = ',';

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};
#endif	//HOBCOMPLEXVERSION_H
