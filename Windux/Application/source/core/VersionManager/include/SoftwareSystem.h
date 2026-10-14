#ifndef SOFTWARESYSTEM_H
#define SOFTWARESYSTEM_H

//#include "vxWorks.h"
#include <string>
#include <map>
namespace util_env
{
class Properties;
class PropertiesTree;
}
namespace util_filesystem
{
class Directory;
class File;
}
class ShutdownSystemRequest;
class PeripheralRequestCodeUpdate;
class PeripheralCodeUpdate;

#include "HobVersion.h"
#include "HobDependency.h"
#include "HobComplexVersion.h"
#include "VersionManager.h"
//#include "usbdLib.h"

class SoftwareSystem
{
	public:
		typedef enum PeripheralVersionCheck
		{
			NotAvailable,
			AvailableUpgrade,
			AvailableInvalidHW,
			AvailableInvalidHWAndSW,
			AvailableSameVersion,
			retryCntrOver
		}   PeripheralVersionCheck_t;

		SoftwareSystem(bool convert);
		SoftwareSystem(const util_filesystem::File& fSoftwareProps, bool installed = false);
		SoftwareSystem(const SoftwareSystem& pSystem);
		~SoftwareSystem();

		const std::string* toString();

		HobDependency::DependencyResult validateSoftwareSystem(VersionManager::ComponentList &hardwareVersions);
		void addAComponent(HobComplexVersion *pNewComponent);
		void mergeSystems(SoftwareSystem *pNewSystem, bool onlyNewerVersions, bool bForce = false);
		void addSystem(SoftwareSystem *pNewSystem);
		STATUS flashSystem(VersionManager::ComponentList * pHardwareMap);
		void savePropertiesToFile(bool clearProperties = false);
		PeripheralVersionCheck doesUSBComponentNeedToBeLoaded(PeripheralRequestCodeUpdate *pUSBMsg,
			VersionManager::ComponentList &hardwareVersions, HobVersion::VersionType &newVerType);
		bool requestPeripheralUpgrade(PeripheralRequestCodeUpdate *pUSBMsg,
			VersionManager::ComponentList &hardwareVersions, PeripheralCodeUpdate **pReplyMsg);
		bool doesAComponentNeedInstalling(HobVersion::VersionType checkType);
		bool doesAnyComponentNeedInstalling();
		HobComplexVersion * getSoftwareVersionOfComponentType(HobVersion::VersionType checkType);
		bool doesDBNeedBackingUp();
		bool doesSystemNeedRebooting();
		bool isEmpty (){return componentList_.empty();}
		HobComplexVersion *findMajorComponent();
		bool removeAllWrapperSWComponents();
		bool removeAllCLASWComponents();//Added 9/24 Alex Zavala
	private:

		void logEntryInAppInstallerLog(HobComplexVersion *pComponent);
		void logEntryInInstallationLog(HobComplexVersion *pComponent);

		STATUS extractPeripheralTar(USBD_NODE_ID nodeId, HobVersion::VersionType verType);
		HobDependency::DependencyResult validateNewHardware(HobVersion *pNewHard);

		std::map<HobVersion::VersionType, HobComplexVersion *> componentList_;

		util_env::PropertiesTree* softProps_;

		util_filesystem::File verIniFile_;

		mutable std::string serialized_;
		
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif	//SOFTWARESYSTEM_H
