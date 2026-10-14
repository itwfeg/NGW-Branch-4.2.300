#ifndef VERSIONMANAGER_H
#define VERSIONMANAGER_H

#include "PAL_platformheader.h"


#define REBOOT_SCALE ExitWindowsEx(EWX_REBOOT, 0)

#include <map>
#include <list>
//#include <Wininet.h> // replaced by PAL header

#include "HobVersion.h"
#include "HobDependency.h"

#include "VersionManagerEvent.h"
#include "properties/propertiestree.h"

#include "CEvolMsgReceiver.h"
#include "evolution.h"
#include "filesys/directory.h"
#include "CHobartTimer.h"
#include "CCriticalSection.h"

class SoftwareSystem;
class HobComplexVersion;
class UsbRequestUpdate;
class CfgInternal;
class UpdateSoftwareAutomatically;
class ImportManager;

namespace util_filesystem
{
class Directory;
class File;
}
class PeripheralRequestCodeUpdate;

class VersionManager : Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer
{
		 public:

			~VersionManager();
			virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

			static VersionManager *getInstance();

			STATUS loadCurrentSystemComponents();
			STATUS setSystemControllerHardwareVersion(HobVersion &ver);
			void allowOnlyNewerVersionsToBeUpgraded(bool mode) {onlyNewerVersions_ = mode;}
			HobDependency::DependencyResult validateCurrentSystem();
			STATUS updateFirmWareFromTFTP(UpdateSoftwareAutomatically* msg);
			STATUS updateFirmWareFromServer(UpdateSoftwareAutomatically* msg);
			STATUS updateFirmWareFromThumbDrive(const std::string& path);
			STATUS writeSystemControllerSerialEEP(std::string& path);
			STATUS readSystemControllerSerialEEP(std::string& path);

			void addAHardwareComponent(USBD_NODE_ID nodeId, HobVersion *version);
			bool removeAHardwareComponent(USBD_NODE_ID nodeId);
			bool dumpHardwareComponents();

			HobComplexVersion *getSystemControllerSoftwareVersion();
			HobVersion * getOSVersion();
			std::string getOSVersionAsString();
			HobVersion *getSystemControllerHardwareVersion();
			HobVersion * getAHWComponent(HobVersion::VersionType searchType);

			STATUS handleRemoteDatabaseFiles(UpdateSoftwareAutomatically* msg);
			STATUS getRemoteDBFile(std::string& destPath);

			STATUS disableHobartServicesOnBootUp();

			virtual void timeoutProcess( __int32 timer_id );

			typedef std::list<std::string> fileList;
			typedef std::list<std::string>::iterator fileListIterator;

			typedef std::map<USBD_NODE_ID, HobVersion *> ComponentList;
			typedef std::map<USBD_NODE_ID, HobVersion *>::iterator ComponentListIterator;

			const static USBD_NODE_ID SYSTEM_CONTROLLER_NODEID;

			void logInstallationMsg(std::string msg);

			void validateFtpPath(std::string &path);

		 private:
			enum VersionManagerTimer
			{
				delayBeforeRebootAvailable = 1
			};

			VersionManager();
			static VersionManager * instance;

			void handleUSBRequestUpdate(const char *recvBuffer);

			void tryRequestShutdown(bool convertDB);
			STATUS requestShutdown(bool convertDB);
			STATUS handleRemoteGraphicsFiles(UpdateSoftwareAutomatically* msg);

			bool checkForRepeatFailureCondition(PeripheralRequestCodeUpdate & msg);
			STATUS loadOSVersion();
			STATUS loadSystemControllerHardwareVersion();
			void getLastDir(std::string & searchPath, std::string & subDir);

			STATUS getSrcFromThumbDrive(util_filesystem::File& srcZip, SoftwareSystem** pNewSC);
			STATUS getSrcFromFTP(PAL_INET_HANDLE hFTP, util_filesystem::File& srcZip, SoftwareSystem** pNewSC);
			STATUS getIniFromFTP(PAL_INET_HANDLE hFTP, util_filesystem::File& srcIni, SoftwareSystem** pNewSC);

        #if defined( PAL_WIN32_RUN )
			STATUS disableAutoStartService(SC_HANDLE schSCManager, const std::string& serviceName);
			STATUS disableAutoStartIE();
			STATUS enableAppInstallerOnReboot();
			STATUS disableAppLaunchOnBootUp();

        #elif   defined( __linux__ )
            //
            // ***** WARNING: We may need another mechanism in Linux, or not
            //
			//STATUS disableAutoStartService();
			//STATUS disableAutoStartIE();
			//STATUS enableAppInstallerOnReboot();
			//STATUS disableAppLaunchOnBootUp();

		#endif

			void copyFtpDirectory(UpdateSoftwareAutomatically* msg, PAL_INET_HANDLE hInet, util_filesystem::Directory & currentSrcDir, util_filesystem::Directory & currentDestDir);
			void clearVersionIniComponents();

			SoftwareSystem *currentSoftwareSystem_;
			SoftwareSystem *newSoftwareSystem_;

			bool rebootRequested_;
			bool safeToReboot_;

			ComponentList hardwareComponentList_;

			bool convertDb_;
			bool onlyNewerVersions_;

			bool deletedFirmware_;

            PAL_INET_HANDLE hInet_, hFTP_;
			ImportManager * importManager_;

			util_filesystem::Directory dbTmpDir_;
			util_filesystem::Directory::const_iterator dbDirIter_;

			Util::CSmartCriticalSection versionMgrCs;

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
};

#endif //VERSIONMANAGER_H
