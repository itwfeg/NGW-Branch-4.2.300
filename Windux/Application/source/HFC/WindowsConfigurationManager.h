/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WINDOWSCONFIGURATIONMANAGER_H
#define WINDOWSCONFIGURATIONMANAGER_H

#include "Session.h"
#include "CEvolMsgReceiver.h"
class SessionMessage;

typedef enum WPDirection
{
    WPBackup = 1,
    WPRestore
} WPDirection_t;

class WindowsConfigurationManager : public Hobart::CEvolMsgReceiver
{
    public:

        ~WindowsConfigurationManager();


        void init();

        bool checkNetworkAvailability();

        void handleMessage(SessionMessage* msg);

        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ); //lcg rev 0.1

        static WindowsConfigurationManager *getInstance();


    private:


        WindowsConfigurationManager();

        bool RestoreWirelessProfiles(const std::string &path);
#if defined(PAL_WIN32_RUN)      // ***** WARNING: Stub Linux only
        DWORD RestoreProfilesFromDirectory(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle);
        DWORD RestoreProfile(const std::string& path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID);

        bool BackupWirelessProfiles(const std::string &path);
        DWORD loadProfilesOnInterface(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID);
        DWORD loadProfilesOnAllInterfaces(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle);
        DWORD getProfile(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID, PWCHAR profileName, int profileIndex);

        PBYTE decrypt(PBYTE src, DWORD size, PDWORD resultantSize);
        PBYTE encrypt(PBYTE src, DWORD size, PDWORD resultantSize, PWCHAR identifier);
#endif
        static WindowsConfigurationManager *instance;

#ifdef USE_ACCOUNTANT
        static int _dataType_;
        static std::string _dataTypeName_;
#endif
};

#endif //WINDOWSCONFIGURATIONMANAGER_H




