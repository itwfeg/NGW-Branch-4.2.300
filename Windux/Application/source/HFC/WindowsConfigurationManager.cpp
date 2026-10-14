/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"

#include "WindowsConfigurationManager.h"
#include "WindowsConfigurationEvents.h"
#ifdef LINUX_FIX
#include "messages/WirelessProfilesSelections.h"
#else
#include "Messages/WirelessProfilesSelections.h"
#endif
#include "Integer.h"
#include "Debug.h"
#ifdef PAL_WIN32_RUN
#include "windows.h"
#include "wlanapi.h"
#include "wincrypt.h"
#endif
#include <iostream>
#include <fstream>
#include "filesys/file.h"
#include "filesys/directory.h"
#include "SessionMessage.h"
#include "SessionManager.h"

#if defined(PAL_WIN32_RUN)      // ***** WARNING: Stub Linux only
typedef DWORD (CALLBACK* FUNCOPENHANDLE) (DWORD, PVOID, PDWORD, PHANDLE);
typedef DWORD (CALLBACK* FUNCCLOSEHANDLE) (HANDLE, PVOID);
typedef DWORD (CALLBACK* FUNCENUMINTERFACES) (HANDLE, PVOID, PWLAN_INTERFACE_INFO_LIST*);
typedef DWORD (CALLBACK* FUNCGETPROFILELIST) (HANDLE, const GUID*, PVOID, PWLAN_PROFILE_INFO_LIST*);
typedef DWORD (CALLBACK* FUNCGETPROFILE) (HANDLE, const GUID*, LPCWSTR, PVOID, LPWSTR*, DWORD*, PDWORD);
typedef DWORD (CALLBACK* FUNCSETPROFILE) (HANDLE, const GUID*, DWORD, LPCWSTR, LPCWSTR, BOOL, PVOID, DWORD*);
typedef VOID (CALLBACK* FUNCFREEMEMORY) (PVOID);
typedef BOOL (CALLBACK* FUNCENCRYPT) (DATA_BLOB*, LPCWSTR, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);
typedef BOOL (CALLBACK* FUNCUNENCRYPT) (DATA_BLOB*, LPWSTR*, DATA_BLOB*, PVOID, CRYPTPROTECT_PROMPTSTRUCT*, DWORD, DATA_BLOB*);

#define SIZEID 30

#endif  // PAL_WIN32_RUN (stub Linux)

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WindowsConfigurationManager::_dataType_ = 0;
std::string WindowsConfigurationManager::_dataTypeName_("WindowsConfigurationManager"); 
#endif


using namespace std;


WindowsConfigurationManager::WindowsConfigurationManager()
    : Hobart::CEvolMsgReceiver("WindowsConfigurationManager")

{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    Debug::trace("WindowsConfigurationManager", "WindowsConfigurationManager");

}

WindowsConfigurationManager::~WindowsConfigurationManager()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
	ShutdownNicely();

    Debug::trace("WindowsConfigurationManager", "~WindowsConfigurationManager");
}

void WindowsConfigurationManager::handleMessage(SessionMessage* msg)
{
    if ( !msg ) return;

    if ( msg->getDeviceType() == WindowsConfigurationDev )
    {
        switch ( msg->getEventType() )
        {
            case backupWirelessProfiles:
                {
                    WirelessProfilesSelections* message = dynamic_cast<WirelessProfilesSelections*>(msg->getHfcObject());
                    if ( !message ) return;

                    if (message->getDirection() == WPBackup)
                    {
#if defined ( __linux__ )
                        bool result = false;
#else
                        bool result = BackupWirelessProfiles(message->getFileName());
#endif

                        SessionManager * pSesMan = SessionManager::getInstance();
                        if (pSesMan)
                        {
                            Session* pSession = pSesMan->getPrimaryConsoleSession();

                            if (pSession)
                            {
                                if (result)
                                    pSession->addDialogMessage(ErrWindowsConfigurationDev, WindowsConfigurationErrBackupSuccessfull, HfcResponse::RESPONSESTYLE_OK);
                                else
                                    pSession->addDialogMessage(ErrWindowsConfigurationDev, WindowsConfigurationErrBackupFailed, HfcResponse::RESPONSESTYLE_OK);
                            }
                        }
                    }
                    else
                    {
                        bool result = RestoreWirelessProfiles(message->getFileName());

                        SessionManager * pSesMan = SessionManager::getInstance();
                        if (pSesMan)
                        {
                            Session* pSession = pSesMan->getPrimaryConsoleSession();

                            if (pSession)
                            {
                                if (result)
                                    pSession->addDialogMessage(ErrWindowsConfigurationDev, WindowsConfigurationErrRestoreSuccessfull, HfcResponse::RESPONSESTYLE_OK);
                                else
                                    pSession->addDialogMessage(ErrWindowsConfigurationDev, WindowsConfigurationErrRestoreFailed, HfcResponse::RESPONSESTYLE_OK);
                            }
                        }
                    }
                }
        }
    }
}

void WindowsConfigurationManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
    EvolMessage evolMessage( msg->Data() );
    switch (evolMessage.getDeviceType())
    {
        case WindowsConfigurationDev:
            {
                switch ( evolMessage.getEventType())
                {
                    case backupWirelessProfiles:
                        {

                            Debug::trace("WindowsConfigurationManager.processEvent", "Received CfgSaved message");

                        }
                        break;
                    default:

                        Debug::trace("WindowsConfigurationManager.processEvent", "received unknown ConfigurationDev message.");

                        break;
                }
            }
            break;


        default:
            Debug::trace("WindowsConfigurationManager.processEvent", "received unknown DevType message.");
            break;
    }

}

#if defined(PAL_WIN32_RUN)      // ***** WARNING: Stub Linux only
bool WindowsConfigurationManager::RestoreWirelessProfiles(const std::string &path)
{
    DWORD status;
    DWORD negVer;
    HANDLE winAPIHandle;
    FUNCOPENHANDLE pFuncOpenHandle;
    FUNCCLOSEHANDLE pFuncCloseHanlde;

    PAL_HMODULE hinstCryptLib = LoadLibrary(L"crypt32.dll");
    if (hinstCryptLib == NULL)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load library crypt32.dll!");

        return false;
    }

    PAL_HMODULE hinstLib = LoadLibrary(L"wlanapi.dll");
    if (hinstLib == NULL)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load library wlanapi.dll!");

        status = FreeLibrary(hinstCryptLib);
        return false;
    }

    pFuncOpenHandle = (FUNCOPENHANDLE) GetProcAddress(hinstLib, "WlanOpenHandle");
    pFuncCloseHanlde = (FUNCCLOSEHANDLE) GetProcAddress(hinstLib, "WlanCloseHandle");

    if (!pFuncOpenHandle || !pFuncCloseHanlde )
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to get function handle in wlanapi.dll!");

        status = FreeLibrary(hinstLib);
        status = FreeLibrary(hinstCryptLib);

        return false;
    }

    status = pFuncOpenHandle (1, NULL, &negVer,&winAPIHandle);
    if (status == ERROR_SUCCESS)
    {

        status = RestoreProfilesFromDirectory(path, hinstCryptLib, hinstLib, winAPIHandle);

        pFuncCloseHanlde (winAPIHandle, NULL);
    }
    else
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to open connection with wlanapi server!");
    }

    Debug::trace("WindowsConfigurationManager.Backup", "profile dump complete!");


    FreeLibrary(hinstLib);
    FreeLibrary(hinstCryptLib);

    return (status == ERROR_SUCCESS);
}
#else
bool WindowsConfigurationManager::RestoreWirelessProfiles(const std::string &UNUSED(path))
{
    return false;
}
#endif

#if defined(PAL_WIN32_RUN)      // ***** WARNING: Stub Linux only
DWORD WindowsConfigurationManager::RestoreProfilesFromDirectory(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle)
{

    DWORD status = 0;
    PWLAN_INTERFACE_INFO_LIST pIntfList = 0;
    FUNCENUMINTERFACES funcEnumInterfaces = (FUNCENUMINTERFACES) GetProcAddress(hinstLib, "WlanEnumInterfaces");
    FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

    if (!funcEnumInterfaces || !funcFreeMem)
    {
        Debug::trace("WindowsConfigurationManager.Restore", "Failed to load function from wlanapi.dll!");

        return ERROR_INVALID_FUNCTION;
    }


    // enumerate wireless interfaces
    if ((status = funcEnumInterfaces(
                    winAPIHandle,
                    NULL,               // reserved
                    &pIntfList
                    )) != ERROR_SUCCESS)
    {
        Debug::trace("WindowsConfigurationManager.Restore", "WlanEnumInterfaces failed!");

        if (pIntfList)
            funcFreeMem(pIntfList);

        return ERROR_ACCESS_DENIED;
    }

    util_filesystem::Directory workingDir("");
    workingDir.appendPath(path);

    workingDir.select("*.xml");
    util_filesystem::Directory::const_reverse_iterator dirIter = workingDir.rbegin();

    while (dirIter != workingDir.rend( ))
    {
        util_filesystem::File fullProfilePath (workingDir.getFullPath(), dirIter->getName());

        if (fullProfilePath.exists())
        {

            status = RestoreProfile(fullProfilePath.getFullPath(), hinstCryptLib, hinstLib, winAPIHandle, &pIntfList->InterfaceInfo[0].InterfaceGuid);
            if (status != ERROR_SUCCESS)
                break;
        }

        dirIter++;
    }

    if (pIntfList)
        funcFreeMem(pIntfList);

    return status;
}

DWORD WindowsConfigurationManager::RestoreProfile(const std::string& path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID)
{
    DWORD status = ERROR_ACCESS_DENIED;
    ifstream sourceFile(path.c_str(), ios::in|ios::binary|ios::ate);

    if (sourceFile)
    {
        FUNCSETPROFILE funcSetProfile = (FUNCSETPROFILE) GetProcAddress(hinstLib, "WlanSetProfile");
        FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");
        FUNCUNENCRYPT funcUnencrypt = (FUNCUNENCRYPT) GetProcAddress(hinstCryptLib, "CryptUnprotectData");

        if (funcSetProfile && funcFreeMem && funcUnencrypt)
        {

            ifstream::pos_type sizeFile = sourceFile.tellg();
            int buffSize = (int)sizeFile;
            char *xmlStr = new char[buffSize];

            if (xmlStr)
            {

                sourceFile.seekg (0, ios::beg);
                sourceFile.read (xmlStr, sizeFile);

                int bytesRead = sourceFile.gcount();

                if (bytesRead == sizeFile)
                {
                    DWORD resultantSize = 0;
                    PBYTE pDest = 0;

                    Debug::trace("WindowsConfigurationManager.Restore", std::string("Loading profile ") + path + std::string(" into WZC!"));

                    pDest = decrypt((PBYTE) xmlStr, sizeFile, &resultantSize);

                    if ((resultantSize >0) && pDest)
                    {

                        LPCWSTR pProfileXml = (LPCWSTR)pDest;
                        DWORD reasonCode = 0;

                        Debug::trace("WindowsConfigurationManager.Restore", "Unencrypt successfull!");

                        if ((status = funcSetProfile(
                                        winAPIHandle, 
                                        pGUID,
                                        0,
                                        pProfileXml,    
                                        NULL,       
                                        TRUE,    
                                        NULL,    
                                        &reasonCode     
                                        )) != ERROR_SUCCESS)
                        {
                            Debug::trace("WindowsConfigurationManager.Restore", "WlanSetProfile failed!");

                        }



                    }

                    if (pDest)
                        free (pDest);
                    /*
                       DATA_BLOB DataIn;
                       DATA_BLOB DataOut;

                       DataIn.pbData = (BYTE *)xmlStr;    
                       DataIn.cbData = buffSize;
                       DataOut.cbData = 0;
                       DataOut.pbData = NULL;

                       LPWSTR pDescrOut =  NULL;

                       if(funcUnencrypt(
                       &DataIn,
                       &pDescrOut, // A description string
                    // to be included with the
                    // encrypted data. 
                    NULL,                               // Optional entropy not used.
                    NULL,                               // Reserved.
                    NULL,                               // Pass NULL for the 
                    // prompt structure.
                    0,
                    &DataOut))
                    {

                    LPCWSTR pProfileXml = (LPCWSTR)DataOut.pbData;
                    DWORD reasonCode = 0;

                    Debug::trace("WindowsConfigurationManager.Restore", "Unencrypt successfull!");

                    if ((status = funcSetProfile(
                    winAPIHandle, 
                    pGUID,
                    0,
                    pProfileXml,    
                    NULL,       
                    TRUE,    
                    NULL,    
                    &reasonCode     
                    )) != ERROR_SUCCESS)
                    {
                    Debug::trace("WindowsConfigurationManager.Restore", "WlanSetProfile failed!");

                    }

                    }
                    else
                    Debug::trace("WindowsConfigurationManager.Restore", "Unencrypt Failed!");


                    if (DataOut.pbData)
                    LocalFree(DataOut.pbData);
                    */
                }
                else
                    Debug::trace("WindowsConfigurationManager.Restore", "Failed to Read all bytes from file!");

                delete[] xmlStr;
            }
            else
            {
                status = ERROR_OUTOFMEMORY;

                Debug::trace("WindowsConfigurationManager.Backup", "Failed to allocate memory for xml file contents!");
            }
        }
        else
        {
            Debug::trace("WindowsConfigurationManager.Backup", "Failed to get function handle in wlanapi.dll!");

            status = ERROR_INVALID_FUNCTION;
        }

        sourceFile.close();
    }
    else
        Debug::trace("WindowsConfigurationManager.Restore", "Failed to open file!");

    return status;
}

PBYTE WindowsConfigurationManager::decrypt(PBYTE src, DWORD size, PDWORD resultantSize)
{
    PBYTE pDest = 0;
    *resultantSize = (size - (SIZEID * sizeof(WCHAR)) ) / 2;

    pDest = (PBYTE)malloc(*resultantSize);

    if (pDest)
    {
        for (int s = SIZEID*sizeof(WCHAR), d = 0; s < (int)size; s+=2, d++)
        {
            BYTE high = (src[s] - 0x30) << 4;
            BYTE low = (src[s+1] - 0x30);
            pDest[d] = high | low;

        }
    }
    else
        *resultantSize = 0;

    return pDest;
}

bool WindowsConfigurationManager::BackupWirelessProfiles(const std::string &path)
{
    DWORD status;
    DWORD negVer;
    HANDLE winAPIHandle;
    FUNCOPENHANDLE pFuncOpenHandle;
    FUNCCLOSEHANDLE pFuncCloseHanlde;

    PAL_HMODULE hinstCryptLib = LoadLibrary(L"crypt32.dll");
    if (hinstCryptLib == NULL)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load library crypt32.dll!");

        return false;
    }

    PAL_HMODULE hinstLib = LoadLibrary(L"wlanapi.dll");
    if (hinstLib == NULL)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load library wlanapi.dll!");

        status = FreeLibrary(hinstCryptLib);
        return false;
    }

    pFuncOpenHandle = (FUNCOPENHANDLE) GetProcAddress(hinstLib, "WlanOpenHandle");
    pFuncCloseHanlde = (FUNCCLOSEHANDLE) GetProcAddress(hinstLib, "WlanCloseHandle");

    if (!pFuncOpenHandle || !pFuncCloseHanlde )
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to get function handle in wlanapi.dll!");

        status = FreeLibrary(hinstLib);
        status = FreeLibrary(hinstCryptLib);

        return false;
    }

    status = pFuncOpenHandle (1, NULL, &negVer,&winAPIHandle);
    if (status == ERROR_SUCCESS)
    {

        status = loadProfilesOnAllInterfaces(path, hinstCryptLib, hinstLib, winAPIHandle);

        pFuncCloseHanlde (winAPIHandle, NULL);
    }
    else
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to open connection with wlanapi server!");
    }

    Debug::trace("WindowsConfigurationManager.Backup", "profile dump complete!");


    FreeLibrary(hinstLib);
    FreeLibrary(hinstCryptLib);

    return (status == ERROR_SUCCESS);

}


/*************************************************************************************************************************
 * This function will load profiles on each wireless card interface. However, it is really intended for use with
 * a single interface system.
 *************************************************************************************************************************/

DWORD WindowsConfigurationManager::loadProfilesOnAllInterfaces(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle)
{
    DWORD status = 0;
    PWLAN_INTERFACE_INFO_LIST pIntfList = 0;
    FUNCENUMINTERFACES funcEnumInterfaces = (FUNCENUMINTERFACES) GetProcAddress(hinstLib, "WlanEnumInterfaces");
    FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");


    if (!funcEnumInterfaces || !funcFreeMem)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load function from wlanapi.dll!");

        return ERROR_INVALID_FUNCTION;
    }



    // enumerate wireless interfaces
    if ((status = funcEnumInterfaces(
                    winAPIHandle,
                    NULL,               // reserved
                    &pIntfList
                    )) != ERROR_SUCCESS)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "WlanEnumInterfaces failed!");

        return ERROR_SUCCESS;
    }

    Debug::trace("WindowsConfigurationManager.Backup", std::string("There are ") + Integer::toString(pIntfList->dwNumberOfItems) + std::string(" wireless intefaces."));


    for (int i = 0; i < (int)pIntfList->dwNumberOfItems; i++)
    {
        status = loadProfilesOnInterface(path, hinstCryptLib, hinstLib, winAPIHandle, &pIntfList->InterfaceInfo[i].InterfaceGuid);
    }

    if (pIntfList)
        funcFreeMem(pIntfList);

    return status;
}

DWORD WindowsConfigurationManager::loadProfilesOnInterface(const std::string &path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID)
{
    DWORD status = 0;
    PWLAN_PROFILE_INFO_LIST pProfileList = 0;
    FUNCGETPROFILELIST funcGetProfileList = (FUNCGETPROFILELIST) GetProcAddress(hinstLib, "WlanGetProfileList");
    FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

    if (!funcGetProfileList || !funcFreeMem)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load function from wlanapi.dll!");

        return ERROR_INVALID_FUNCTION;
    }

    if ((status = funcGetProfileList(
                    winAPIHandle,
                    pGUID,               // reserved
                    NULL,
                    &pProfileList
                    )) != ERROR_SUCCESS)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "WlanGetProfileList failed!");

        return ERROR_SUCCESS;
    }

    Debug::trace("WindowsConfigurationManager.Backup", std::string("There are ") + Integer::toString(pProfileList->dwNumberOfItems) + std::string(" profiles on the interface."));

    for (int i = 0; i < (int)pProfileList->dwNumberOfItems; i++)
    {

        if ((status = getProfile(path, hinstCryptLib, hinstLib, winAPIHandle, pGUID, 
                        pProfileList->ProfileInfo[i].strProfileName, i)) != ERROR_SUCCESS)
            break;

    }

    if (pProfileList)
        funcFreeMem(pProfileList);

    return status;
}

DWORD WindowsConfigurationManager::getProfile(const std::string& path, PAL_HMODULE hinstCryptLib, PAL_HMODULE hinstLib, HANDLE winAPIHandle, const GUID* pGUID, PWCHAR profileName, int profileIndex)
{
    DWORD status = 0;
    PWSTR strXml = 0;
    FUNCGETPROFILE funcGetProfile = (FUNCGETPROFILE) GetProcAddress(hinstLib, "WlanGetProfile");
    FUNCFREEMEMORY funcFreeMem = (FUNCFREEMEMORY) GetProcAddress(hinstLib, "WlanFreeMemory");

    if (!funcGetProfile || !funcFreeMem)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to load function from wlanapi.dll!");

        return ERROR_INVALID_FUNCTION;
    }

    if ((status = funcGetProfile(
                    winAPIHandle, 
                    pGUID, 
                    profileName,    // profile name
                    NULL,       // reserved
                    &strXml,    // XML string of the profile
                    NULL,       // not interested in the profile flags
                    NULL        // don't care about ACL
                    )) != ERROR_SUCCESS)
    {
        Debug::trace("WindowsConfigurationManager.Backup", "WlanGetProfile failed!");

        if (strXml)
            funcFreeMem(strXml);

        return status;
    }
    else
    {
        WCHAR idString[128];
        DWORD resultantSize = 0;
        PBYTE pDest = 0;

        swprintf(idString, L"Wireless Profile %s.", profileName);

        //write the null terminating character to file. hence (wcslen(strXml) + 1)
        pDest = encrypt((PBYTE) strXml, (DWORD)((wcslen(strXml) + 1) * sizeof(WCHAR)), &resultantSize, idString);
        if ((resultantSize > 0) && pDest)
        {

            char fName[32];

            sprintf(fName,"profile_%02d.xml", profileIndex + 1);

            util_filesystem::File outPath(path, fName);

            Debug::trace("WindowsConfigurationManager.Backup", std::string("Saving file ") + outPath.getFullPath());


            ofstream out(outPath.getFullPath().c_str(), ios::out | ios::trunc | ios::binary);

            if (out)
            {

                Debug::trace("WindowsConfigurationManager.Backup", std::string("writing %d bytes to file!") + Integer::toString(resultantSize));

                out.write((char *)pDest, resultantSize);

                out.close();
            }
            else
                Debug::trace("WindowsConfigurationManager.Backup", "Failed to save file!");


        }

        if (pDest)
            free(pDest);
        /*
           FUNCENCRYPT funcEncrypt = (FUNCENCRYPT) GetProcAddress(hinstCryptLib, "CryptProtectData");

           if (!funcEncrypt)
           {
           Debug::trace("WindowsConfigurationManager.Backup", "Unable to find function CryptProtectData!");

           if (strXml)
           funcFreeMem(strXml);

           return ERROR_INVALID_FUNCTION;
           }

           DATA_BLOB DataIn;
           DATA_BLOB DataOut;
           int sizeInputBytes = (int)wcslen(strXml);

           if (sizeInputBytes > 0)
           {
           DataIn.pbData = (BYTE *)strXml;    
           DataIn.cbData = (sizeInputBytes + 1)*sizeof(WCHAR); //add in the null WCHAR at the end of the string
           DataOut.cbData = 0;
           DataOut.pbData = NULL;

           WCHAR tmpName[128];

           swprintf(tmpName, L"Wireless Profile %s.", profileName);

           if(funcEncrypt(
           &DataIn,
           tmpName,                            // A description string
        // to be included with the
        // encrypted data. 
        NULL,                               // Optional entropy not used.
        NULL,                               // Reserved.
        NULL,                               // Pass NULL for the 
        // prompt structure.
        0,
        &DataOut))
        {
        char fName[32];

        sprintf(fName,"profile_%02d.xml", profileIndex + 1);

        util_filesystem::File outPath(path, fName);

        Debug::trace("WindowsConfigurationManager.Backup", std::string("Saving file ") + outPath.getFullPath());


        ofstream out(outPath.getFullPath().c_str(), ios::out | ios::trunc | ios::binary);

        if (out)
        {

        out.write((char *)DataOut.pbData, DataOut.cbData);

        out.close();
        }
        else
        Debug::trace("WindowsConfigurationManager.Backup", "Failed to save file!");


        }
        else
        {
        Debug::trace("WindowsConfigurationManager.Backup", "Encryption Failed! Not saving profile!");

        }

        if (DataOut.pbData)
        LocalFree(DataOut.pbData);
    }
    */
        if (strXml)
            funcFreeMem(strXml);
    }

    return ERROR_SUCCESS;
}

PBYTE WindowsConfigurationManager::encrypt(PBYTE src, DWORD size, PDWORD resultantSize, PWCHAR identifier)
{
    PBYTE pDest = 0;
    *resultantSize = SIZEID*sizeof(WCHAR) + (size * 2);

    pDest = (PBYTE)malloc(*resultantSize);

    if (pDest)
    {

        wcsncpy((PWCHAR)pDest, identifier, SIZEID);

        for (int s = 0, d = SIZEID*sizeof(WCHAR); s < (int)size; s++, d+=2)
        {
            BYTE high = ((src[s] & 0xF0)>>4) + 0x30;
            BYTE low = (src[s] & 0x0F) + 0x30;
            pDest[d] = high;
            pDest[d+1] = low;

        }
    }
    else
        *resultantSize = 0;

    return pDest;
}
#endif  // PAL_WIN32_RUN (Stub Linux only)

WindowsConfigurationManager *WindowsConfigurationManager::getInstance()
{
    if (instance == 0)
    {
        instance = new WindowsConfigurationManager();
    }
    return instance;
}


WindowsConfigurationManager *WindowsConfigurationManager::instance = (WindowsConfigurationManager *)NULL;



