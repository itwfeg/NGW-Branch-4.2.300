// appInstaller.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "properties/properties.h"
#include "platform/platform.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "HobVersion.h"
#include <fstream>
#include <iostream>
#include "PAL_platformheader.h"
//#include <windows.h> // replaced by PAL header
//#include <shellapi.h> // replaced by PAL header
//#include <winsock2.h> // replaced by PAL header
#include "CZip.h"
#include "UsbManager.h"
#include "FCError.h"


#define BACKUP_INTERNAL_CONFIG_FILE_NAME "C:\\Program Files\\Hobart\\Access Scale\\config.dat"
#define APC_ZIP_SOURCE_FILE "source.bin"

#import "AT91Boot_DLL.tlb" rename_namespace("AT91BOOTDLL_Lib")

using namespace AT91BOOTDLL_Lib;
typedef int AT91BOOT_HANDLE_TYPE;

#define DBGU_CIDR_MASK ~0x000F001F
#define PROG_FMR 0x00480100

bool connectToPort(int portNum)
{
    int retStatus;
    bool connected = false;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 0 );

    err = WSAStartup( wVersionRequested, &wsaData );

    if (!err)
    {
        SOCKET socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socketHandle != INVALID_SOCKET )
        {
            sockaddr_in address;

            address.sin_family = AF_INET;
            address.sin_port = htons(portNum);
            address.sin_addr.s_addr = inet_addr("127.0.0.1");
            if (connect(socketHandle, (struct sockaddr *) &address, sizeof(address)) != SOCKET_ERROR)
            {
                connected = true;
            }

            retStatus = closesocket(socketHandle);
        }
        else
        {
            int temp = WSAGetLastError();
            std::cout << "error = " << temp << std::endl;
        }

        WSACleanup();
    }
    return connected;
}


void cleanADir(const util_filesystem::Directory & currentDir, std::string targetToDelete)
{
    util_filesystem::Directory::const_iterator iter;
    const std::string & tmp = currentDir.getFullPath();
    std::string searchStr("");

    searchStr = tmp.substr(tmp.length() - 3);

    if (searchStr == "jar")
    {

        currentDir.select(targetToDelete, util_filesystem::Directory::SELECT_FILES);

        iter = currentDir.begin();
        while ( iter != currentDir.end() )
        {       
            util_filesystem::File newFile(currentDir.getFullPath(), iter->getName());

            std::cout << "Deleting File:" << newFile.getFullPath() << std::endl << std::endl;
            if (newFile.exists())
                newFile.remove();

            ++iter;
        }

    }

    currentDir.selectAllSubdirectories();

    iter = currentDir.begin();
    while ( iter != currentDir.end() )
    {
        util_filesystem::Directory newDir("");
        newDir.appendPath(currentDir.getFullPath());

        newDir.appendPath(iter->getName());

        cleanADir(newDir, targetToDelete);

        ++iter;
    }

}

void cleanMOMCache()
{

    util_filesystem::Directory myDir("C:\\Documents and Settings");


    cleanADir(myDir, std::string("mom15.jar*.*"));

}

void disableAutoStartService(SC_HANDLE schSCManager, const std::string& serviceName)
{
    SC_HANDLE schService = 0;
    SC_LOCK sclLock = 0; 

    // Need to acquire database lock before reconfiguring. 
    std::cout << "Stopping service: " << serviceName << std::endl;
    do
    {
        sclLock = LockServiceDatabase(schSCManager);
        if (!sclLock)
        {
            std::cout << ".";
            Sleep (1000);
        }

    }
    while (!sclLock);

    std::cout << std::endl << "got the scl lock!" << std::endl;


    if (sclLock)
    {

        schService = OpenServiceA( 
                schSCManager,           // SCManager database 
                serviceName.c_str(),           // name of service 
                SERVICE_ALL_ACCESS); // need CHANGE access 
        if (schService)
        {
            SERVICE_STATUS ssStatus;

            if (!ControlService(
                        schService,  // handle to service 
                        SERVICE_CONTROL_STOP,           // number of arguments 
                        &ssStatus))
            {
                std::cout << "Service Not Currently Running!" << std::endl;
            }
            else 
            {
                std::cout << "Service stopped, exit code: " << ssStatus.dwServiceSpecificExitCode << std::endl;
            }



        }
        else
        {
            std::cout << "disableAutoStartService:FAILED to open service." << std::endl;
        }



    }
    else
    {
        std::cout << "disableAutoStartService:FAILED to lock service database." << std::endl;
    }

    if (schService)
        CloseServiceHandle(schService);

    if (sclLock)
        UnlockServiceDatabase(sclLock);

    std::cout << std::endl;
}

void startAService (SC_HANDLE schSCManager, const std::string& serviceName, const std::string& serviceDisplayName,
        const std::string& pathToServiceExe, bool startNow)
{
    std::cout << "Attempting to start service (" << serviceName << ")" << std::endl;

    SC_HANDLE schService = 0;
    SC_LOCK sclLock = 0; 

    // Need to acquire database lock before reconfiguring. 

    do
    {
        sclLock = LockServiceDatabase(schSCManager);
        if (!sclLock)
        {
            std::cout << ".";
            Sleep (1000);
        }

    }
    while (!sclLock);

    std::cout << std::endl << "got the scl lock!" << std::endl;

    if (sclLock)
    {

        schService = OpenServiceA( 
                schSCManager,           // SCManager database 
                serviceName.c_str(),           // name of service 
                SERVICE_ALL_ACCESS); // need CHANGE access 
        if (schService)
        {

            if (! ChangeServiceConfig( 
                        schService,        // handle of service 
                        SERVICE_NO_CHANGE, // service type: no change 
                        SERVICE_AUTO_START,       // change service start type 
                        SERVICE_NO_CHANGE, // error control: no change 
                        NULL,              // binary path: no change 
                        NULL,              // load order group: no change 
                        NULL,              // tag ID: no change 
                        NULL,              // dependencies: no change 
                        NULL,              // account name: no change 
                        NULL,              // password: no change 
                        NULL) )            // display name: no change
            {
                std::cout << "Failed to change Service to auto start!!!!" << std::endl;
            }
            else 
            {
                std::cout << "Service found and changed to auto start!!!!" << std::endl;
            }
        }
        else
        {
            std::cout << "Failed to open Service!!!!" << std::endl;

            DWORD dw = GetLastError(); 
            if (dw == ERROR_SERVICE_DOES_NOT_EXIST)
            {
                std::cout << "Service does not exist!!!!" << std::endl;

                schService = CreateServiceA( 
                        schSCManager,              // SCManager database 
                        serviceName.c_str(),        // name of service 
                        serviceDisplayName.c_str(),// service name to display 
                        SERVICE_ALL_ACCESS,        // desired access 
                        SERVICE_WIN32_OWN_PROCESS, // service type 
                        SERVICE_AUTO_START,      // start type 
                        SERVICE_ERROR_NORMAL,      // error control type 
                        pathToServiceExe.c_str(),// path to service's binary 
                        NULL,                      // no load ordering group 
                        NULL,                      // no tag identifier 
                        NULL,                      // no dependencies 
                        NULL,                      // LocalSystem account 
                        NULL);                     // no password 

                if (schService)
                {
                    std::cout << "Service created!!!!" << std::endl;
                }
                else
                {
                    std::cout << "Failed to create Service!!!!" << std::endl;
                }


            }

        }

        if (sclLock)
            UnlockServiceDatabase(sclLock); 


        if (schService && startNow)
        {

            if (!StartService(
                        schService,  // handle to service 
                        0,           // number of arguments 
                        NULL) )      // no arguments 
            {
                std::cout << "Failed to start Service!!!!" << std::endl;
            }
            else 
            {
                std::cout << "Service started!!!!" << std::endl;
            }
        }

    }
    else
    {
        std::cout << "FAILED to lock service database!!!! " << std::endl;
    }


    // Close the handle to the service. 

    if (schService)
        CloseServiceHandle(schService); 

}

void startWebService (SC_HANDLE schSCManager, util_env::Properties *properties, bool startNow)
{
    const std::string *pScaleServiceName = properties->getProperty("webServiceName");
    const std::string *pScaleServiceExe = properties->getProperty("webServiceExe");
    const std::string *pBaseInstallPath = properties->getProperty("evolutionDirectory");
    util_filesystem::File fileSSExe (*pBaseInstallPath, *pScaleServiceExe);

    startAService (schSCManager, *pScaleServiceName, *pScaleServiceName, fileSSExe.getFullPath(), startNow);
}

void startScaleService (SC_HANDLE schSCManager, util_env::Properties *properties, bool startNow)
{
    const std::string *pScaleServiceName = properties->getProperty("scaleServiceName");
    const std::string *pScaleServiceExe = properties->getProperty("scaleServiceExe");
    const std::string *pBaseInstallPath = properties->getProperty("evolutionDirectory");
    util_filesystem::File fileSSExe (*pBaseInstallPath, *pScaleServiceExe);

    startAService (schSCManager, *pScaleServiceName, *pScaleServiceName, fileSSExe.getFullPath(), startNow);
}

void reenableLaunchWebBrowserAtStartUp()
{

    HKEY hKey;
    DWORD dwDisp;

    std::cout << "Creating key (SOFTWARE\\microsoft\\windows\\CurrentVersion\\Run)" << std::endl;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T ("SOFTWARE\\microsoft\\windows\\CurrentVersion\\Run"), 
                0, NULL, REG_OPTION_NON_VOLATILE,
                KEY_WRITE, NULL, &hKey, &dwDisp)) 
    {
        std::cout << "Attempt to create key failed" << std::endl;
    }
    else
    {

        TCHAR valueData[256] = { _T("C:\\Program Files\\Hobart\\Access Scale\\appLauncher.exe") };
        DWORD valueSize = (lstrlen(valueData)+1);
        valueSize*= sizeof(TCHAR);

        if (RegSetValueEx(hKey,              // subkey handle 
                    _T("appLauncher"),        // value name 
                    0,                         // must be zero 
                    REG_SZ,             // value type 
                    (LPBYTE) valueData,        // pointer to value data 
                    valueSize)) // length of value data 
        {
            std::cout << "Attempt to set value failed" << std::endl;
        }
    }

    if (hKey)
        RegCloseKey(hKey); 

}

void removeAppInstallerFromRegistry()
{
    HKEY hKey = 0;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T ("SOFTWARE\\microsoft\\windows\\CurrentVersion\\Run"), 
                0, KEY_WRITE, &hKey))
    {
        std::cout << "failed to open run key" << std::endl;
    }
    else
    {
        if (RegDeleteValue(hKey, _T("appInstaller")))
        {
            std::cout << "failed to delete appInstaller registry value" << std::endl;
        }
        else
        {
            std::cout << "deleted appInstaller registry value" << std::endl;
        }
    }

    if (hKey)
        RegCloseKey(hKey);


}

#if 0 // def DISABLE_APP_INSTALLER_APC_UPDATE 
// this section disabled for compatibility with UsbManager AT91 support
int installApcFimrware(util_env::Properties *properties)
{
    const std::string* pPath = properties->getProperty("firmwareDirectory");

    if (pPath == 0)
        return -1;

    util_filesystem::File apcSrcZip(*pPath, "APC.zip");

    if (!apcSrcZip.exists())
        return -2;

    pPath = properties->getProperty("VersionManagerTempDir");
    util_filesystem::Directory vmTmpDir("");
    vmTmpDir.appendPath(*pPath);

    Hobart::CZip::unZipSingleFile(apcSrcZip.getFullPath().c_str(), APC_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

    util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), APC_ZIP_SOURCE_FILE);

    if (firmSourceFile.exists())
    {
        int i = 0;
        unsigned char *pTest[10] = {0};         
        AT91BOOT_HANDLE_TYPE h_handle = 0;

        std::cout << "Attempting to flash APC with new firmware from peripheral source library." << std::endl;

        if (FAILED(CoInitialize(NULL)))
        {
            return -3;
        }

        try
        {
            int uValue, error_code;

            // COM Object Creation
            IAT91BootDLLPtr pAT91BootDLL(__uuidof(AT91BootDLL));

            // MANDATORY: Allocate memory for each string in the table
            for (i=0; i<10; i++)
                pTest[i] = (unsigned char *)malloc(100);

            // Scan all devices connected
            pAT91BootDLL->AT91Boot_Scan((unsigned char *)pTest);

            // Open first Device of the list
            pAT91BootDLL->AT91Boot_Open(pTest[0], &h_handle);

            for (i=0; i<10; i++)
            {
                if (pTest[i] != 0)
                    free (pTest[i]);
            }

            if (h_handle != 0)
            {

                pAT91BootDLL->AT91Boot_Read_Int(h_handle, &uValue, 0xFFFFF240, NULL);

                // Assume Target is a SAM7S256 (SAM7S-EK)
                // Otherwise, modify that part by yourself, it will be a good training! ;-)
                if ((uValue & DBGU_CIDR_MASK) == (0x270B0940 & DBGU_CIDR_MASK))
                {
                    short vendId, prodId;

                    pAT91BootDLL->AT91Boot_Read_Short(h_handle, &vendId, 0x108000, &error_code);
                    pAT91BootDLL->AT91Boot_Read_Short(h_handle, &prodId, 0x108002, &error_code);

                    if ((vendId == HOBART_VENDOR_ID) && (prodId == HOBART_APC_PRINTER_ID))
                    {
                        HANDLE hFile = {NULL};
                        DWORD dwSize, bytes_read;
                        unsigned char *buffer;

                        std::cout << "Atmel device detected. Proceeding with flash." << std::endl;

                        // Open the file and get ready to read it.
                        // Don't be confused by the CreateFile() function,
                        // we're only opening an existing file.
                        hFile = CreateFileA(firmSourceFile.getFullPath().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
                        if(hFile == INVALID_HANDLE_VALUE)
                        {
                            std::cout << "failed to open the atmel firmware source file." << std::endl;

                        }
                        else
                        {
                            // Get the file size so we know how long to make our buffer.
                            dwSize = GetFileSize(hFile, NULL);
                            // Create a CString buffer of the proper length
                            // and fill it with spaces.
                            buffer = (unsigned char *)malloc(dwSize);
                            // Read the file into the buffer
                            ReadFile(hFile, buffer, dwSize, &bytes_read, NULL);
                            // We're done with the file handle so close it.
                            CloseHandle(hFile);

                            // MANDATORY: Program EFC_FMR register
                            // Assume Target is at 48MHz
                            pAT91BootDLL->AT91Boot_Write_Int(h_handle, PROG_FMR, 0xFFFFFF60, &error_code);

                            if (error_code != AT91C_BOOT_DLL_OK)
                            {
                                std::cout << "Failed to setup atmel before flashing." << std::endl ;
                            }
                            else
                            {

                                // Send File into SAM7S256 Flash
                                pAT91BootDLL->AT91Boot_SAM7S256_Send_Flash(h_handle, 0x8200, buffer, dwSize, &error_code);
                                if (error_code != AT91C_BOOT_DLL_OK)
                                {
                                    std::cout << "Failed to flash atmel." << std::endl ;
                                }
                                else
                                {

                                    std::cout << "Flash Succeeded. Starting HLX firmware NOW." << std::endl ;


                                    pAT91BootDLL->AT91Boot_Go(h_handle, 0x108200, &error_code);

                                    if (error_code == AT91C_BOOT_DLL_OK)
                                    {
                                        std::cout << "Started hlx firmware." << std::endl ;
                                    }
                                    else
                                    {
                                        std::cout << "Failed to start hlx firmware." << std::endl ;

                                        pAT91BootDLL->AT91Boot_Go(h_handle, 0x108200, &error_code);

                                        if (error_code == AT91C_BOOT_DLL_OK)
                                        {
                                            std::cout << "Started hlx firmware." << std::endl ;
                                        }
                                        else
                                        {
                                            std::cout << "Failed hlx firmware." << std::endl ;
                                        }
                                    }
                                }
                            }

                            free (buffer);
                        }
                    }
                    else
                    {
                        std::cout << "invalid vendor id/product id pair found at 0x108000." << std::endl ;

                    }
                }
                else
                {
                    std::cout << "Failed to find sam7s256 driver." << std::endl ;
                }

            }
            else
            {
                std::cout << "Failed to open sam7s256 driver." << std::endl ;
            }

            if (h_handle)
                pAT91BootDLL->AT91Boot_Close(h_handle);

        }
        catch (const _com_error& Err)
        {
            std::cout << "Caught error. Failed to locate atmel device. " << Err.ErrorMessage() << std::endl ;
        }



        CoUninitialize();
    }

    return 0;
}
#endif // DISABLE_APP_INSTALLER_APC_UPDATE 


int processVersionInfo(const std::string & logEntry, int * swtype, int * maj, int * min, int * build )
{

    std::string::size_type p = logEntry.find('.');

    if (p != std::string::npos)
    {
        std::string type = logEntry.substr(0, p);
        std::string remainder = logEntry.substr(p+1);


        sscanf(type.c_str(), "%d", swtype);

        if (remainder.size() > 0)
        {
            p = remainder.find('.');

            std::string majstr; 

            if (p == std::string::npos)
            {
                majstr = remainder;
                remainder = "";
            }
            else
            {
                majstr = remainder.substr(0, p);
                remainder = remainder.substr(p+1);
            }

            sscanf(majstr.c_str(), "%d", maj);

            if (remainder.size() > 0)
            {
                p = remainder.find('.');

                std::string minstr;

                if (p == std::string::npos)
                {
                    minstr = remainder;
                    remainder = "";
                }
                else
                {
                    minstr = remainder.substr(0, p);
                    remainder = remainder.substr(p+1);
                }

                sscanf(minstr.c_str(), "%d", min);

                if (remainder.size() > 0)
                {

                    sscanf(remainder.c_str(), "%d", build);

                    return 1;
                }
            }
        }
    }

    return 0;
}

int checkLogForNewFirmware(std::string path, int targetType)
{

    std::ifstream appInstLog (path.c_str());

    if (appInstLog.rdbuf( )->is_open( ))
    {
        char entry[32];
        int bytesRead = 0;

        do
        {

            appInstLog.getline(entry, 32);

            bytesRead = appInstLog.gcount();

            if (bytesRead > 0)
            {
                std::string logEntry(entry);
                int nType = 0, maj = 0, min = 0, build = 0;

                processVersionInfo(logEntry, &nType, &maj, &min, &build);

                if (nType == targetType)
                {
                    return 1;
                }
            }
        }
        while (bytesRead > 0);
    }

    return 0;

}


bool shutdownSystem()
{
    HANDLE hToken; 
    TOKEN_PRIVILEGES tkp; 

    if (!OpenProcessToken(GetCurrentProcess(), 
                TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
        return( false ); 

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
            &tkp.Privileges[0].Luid); 

    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

    // Get the shutdown privilege for this process. 

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
            (PTOKEN_PRIVILEGES)NULL, 0); 

    if (GetLastError() != ERROR_SUCCESS) 
        return false; 

    if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
                SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
                SHTDN_REASON_MINOR_UPGRADE |
                SHTDN_REASON_FLAG_PLANNED)) 
        return false; 

    return true;

}
int _tmain(int argc, _TCHAR* argv[])
{
    //try to load config.dat from the new software location. if that fails load it from the old software location.
    util_env::Properties *properties = new util_env::Properties(DEFAULT_INTERNAL_CONFIG_FILE_NAME);
    SC_HANDLE schSCManager = 0;

    const std::string *pPath;

    if ((!properties) || ( properties->getProperty("VersionManagerTempDir") == 0))
    {
        properties = new util_env::Properties(BACKUP_INTERNAL_CONFIG_FILE_NAME);

        if (!properties)
        {
            std::cout << "FAILED to find config.dat in either the new or old software" << std::endl << std::endl;
            Sleep (5000);//sleep for 5s.
            return -1;
        }

    }

    pPath = properties->getProperty("VersionManagerTempDir");

    if (pPath == 0)
        return -1;

    util_filesystem::Directory vmTmpDir("");
    vmTmpDir.appendPath(*pPath);


    std::cout << "Hobart Access Scale Installer" << std::endl << std::endl;
    std::cout << "Installing new software! Please wait!" << std::endl;
    std::cout << "Scale Application will automatically start after installation is complete!" << std::endl;
    std::cout << "Please do NOT reboot or shutdown scale now!" << std::endl << std::endl << std::endl;

    util_filesystem::File appInstallerLog(vmTmpDir.getFullPath(), "appInstaller.log");

    if (appInstallerLog.exists())
    {

#if 0 // def DISABLE_APP_INSTALLER_APC_UPDATE 
        // disabled for compatibility with UsbManager AT91 support

        if (checkLogForNewFirmware(appInstallerLog.getFullPath(), HobVersion::ApcPrinterSW))
        {
            installApcFimrware(properties);

            util_filesystem::File APCSourceFile (vmTmpDir.getFullPath(), APC_ZIP_SOURCE_FILE);

            APCSourceFile.remove();
        }
#endif

        if (checkLogForNewFirmware(appInstallerLog.getFullPath(), HobVersion::SystemControllerSW))
        {
            util_filesystem::Directory srcBaseDir("");
            srcBaseDir.appendPath(vmTmpDir.getFullPath());
            srcBaseDir.appendPath("SC");

            if (srcBaseDir.exists())
            {
                const std::string *pDestDbPath = properties->getProperty("evolutionDirectory");
                util_filesystem::Directory targetBaseDir ("");
                targetBaseDir.appendPath(*pDestDbPath);

                std::cout << "New system controller software exists" << std::endl;
                std::cout << "path :" << srcBaseDir.getFullPath() << std::endl << std::endl;
                std::cout << "cleaning Mom cache!" << std::endl << std::endl;

                cleanMOMCache();


                schSCManager = OpenSCManager( 
                        NULL,                    // local machine 
                        NULL,                    // ServicesActive database 
                        SC_MANAGER_ALL_ACCESS);  // full access rights


                if (schSCManager)
                {
                    const std::string *pScaleServiceName = properties->getProperty("webServiceName");
                    disableAutoStartService(schSCManager, *pScaleServiceName);
                }

                Sleep (1000);

                //install new hobart drivers if they exist.

                util_filesystem::Directory srcDriversDir ("");
                srcDriversDir.appendPath(srcBaseDir.getFullPath());
                srcDriversDir.appendPath("drivers");

                if (srcDriversDir.exists())
                {
                    std::cout << "Installing new drivers ..." << std::endl;

                    util_filesystem::File targetDrvAPCInf ("C:/windows/INF", "wghUsb.inf");

                    targetDrvAPCInf.remove();

                    util_filesystem::File targetDrvAPCSys ("C:/windows/system32/drivers", "weighUsb.sys");

                    targetDrvAPCSys.remove();

                    util_filesystem::File targetDrvSmbusSys ("C:/windows/system32/drivers", "smbus.sys");

                    targetDrvSmbusSys.remove();

                    util_filesystem::File srcDrvAPCInf (srcDriversDir.getFullPath(), "wghUsb.inf");

                    targetDrvAPCInf << srcDrvAPCInf;

                    util_filesystem::File srcDrvAPCSys (srcDriversDir.getFullPath(), "weighUsb.sys");

                    targetDrvAPCSys << srcDrvAPCSys;

                    util_filesystem::File srcDrvSmbusSys (srcDriversDir.getFullPath(), "smbus.sys");

                    targetDrvSmbusSys << srcDrvSmbusSys;

                    srcDriversDir.remove(true);
                }

                {//save the current version.ini file
                    util_filesystem::File origVerIni (targetBaseDir.getFullPath(), *properties->getProperty("VersionInfoFileName"));
                    util_filesystem::File newVerIni (srcBaseDir.getFullPath(), *properties->getProperty("VersionInfoFileName"));

                    if (newVerIni.exists())
                        newVerIni.remove();

                    origVerIni.moveTo(newVerIni);

                }

                {//save the service key if one exists
                    util_filesystem::Directory sourceServiceDir("");
                    sourceServiceDir.appendPath(targetBaseDir.getFullPath());
                    sourceServiceDir.appendPath("Service");


                    if (sourceServiceDir.exists())
                    {
                        util_filesystem::Directory destinationServiceDir("");
                        sourceServiceDir.appendPath(srcBaseDir.getFullPath());
                        sourceServiceDir.appendPath("Service");

                        if (!destinationServiceDir.exists())
                            destinationServiceDir.create();

                        destinationServiceDir << sourceServiceDir;
                    }


                }

                util_filesystem::Directory newEmptyDbDir ("");
                newEmptyDbDir.appendPath(srcBaseDir.getFullPath());
                newEmptyDbDir.appendPath("dbempty");

                const std::string *pVerIndData = properties->getProperty("versionIndependentBackupFile");
                util_filesystem::File fTargetVerIndData (*pVerIndData);
                util_filesystem::File fSrcVerIndData (srcBaseDir.getFullPath(), fTargetVerIndData.getFileName());

                if (fTargetVerIndData.exists())
                {
                    if (newEmptyDbDir.exists())
                    {

                        const std::string *pDestDbPath = properties->getProperty("dbLocation");
                        util_filesystem::Directory targetWorkingDBDir("");
                        targetWorkingDBDir.appendPath(*pDestDbPath);

                        std::cout << "New db exists: " << std::endl;
                        std::cout << "Copying from " << newEmptyDbDir.getFullPath() << std::endl;
                        std::cout << "Copying to: " << targetWorkingDBDir.getFullPath() << std::endl << std::endl;


                        targetWorkingDBDir.remove(true);

                        targetWorkingDBDir.create();

                        targetWorkingDBDir << newEmptyDbDir;

                    }

                    fTargetVerIndData.moveTo(fSrcVerIndData); 
                }

                std::cout << "Remove existing system controller software" << std::endl;
                std::cout << "Path :" << targetBaseDir.getFullPath() << std::endl << std::endl;

                const std::string *pTargetSystemPath = properties->getProperty("systemDir");

                util_filesystem::Directory targetSystemDir ("");
                targetSystemDir.appendPath(*pTargetSystemPath);

                targetSystemDir.remove(true);


                util_filesystem::Directory targetEmptyDbDir ("");
                targetEmptyDbDir.appendPath(targetBaseDir.getFullPath());
                targetEmptyDbDir.appendPath("dbempty");

                targetEmptyDbDir.remove(true);

                util_filesystem::Directory targetScriptsDir ("");
                targetScriptsDir.appendPath(targetBaseDir.getFullPath());
                targetScriptsDir.appendPath("scripts");

                targetScriptsDir.remove(true);

                const std::string *pTargetWebPagesPath = properties->getProperty("webPagesDir");

                util_filesystem::Directory targetWebPagesDir ("");
                targetWebPagesDir.appendPath(*pTargetWebPagesPath);

                targetWebPagesDir.removeContents(false);

                util_filesystem::Directory targetImagesDir ("");
                targetImagesDir.appendPath(targetWebPagesDir.getFullPath());
                targetImagesDir.appendPath("images");

                targetImagesDir.removeContents(false);

                util_filesystem::Directory targetEppDir ("");
                targetEppDir.appendPath(targetImagesDir.getFullPath());
                targetEppDir.appendPath("epp");

                targetEppDir.remove(true);

                util_filesystem::Directory targetEscDir ("");
                targetEscDir.appendPath(targetImagesDir.getFullPath());
                targetEscDir.appendPath("esc");

                targetEscDir.remove(true);

                util_filesystem::Directory targetWinDir ("");
                targetWinDir.appendPath(targetBaseDir.getFullPath());
                targetWinDir.appendPath("win");

                targetWinDir.remove(true);

                targetBaseDir.removeContents();

                std::cout << "Copying over new system controller software: " << std::endl;
                std::cout << "Copying from " << srcBaseDir.getFullPath() << std::endl;
                std::cout << "Copying to " << targetBaseDir.getFullPath() << std::endl << std::endl << std::endl;

                if (fSrcVerIndData.exists())
                    fSrcVerIndData.moveTo(fTargetVerIndData);

                targetBaseDir << util_filesystem::contents << util_filesystem::recursive << srcBaseDir;

                srcBaseDir.remove(true);


            }
        }

        appInstallerLog.remove();
    }


    if (!schSCManager)

        schSCManager = OpenSCManager( 
                NULL,                    // local machine 
                NULL,                    // ServicesActive database 
                SC_MANAGER_ALL_ACCESS);  // full access rights

    if (schSCManager)
    {

        if (!connectToPort(80))
        {
            std::cout << "Setup registry to allow proper reboot!" << std::endl;

            startWebService (schSCManager, properties, true);

            startScaleService(schSCManager, properties, true);

            reenableLaunchWebBrowserAtStartUp();
            removeAppInstallerFromRegistry();

            std::string pathToAppLauncher ("\"");
            pathToAppLauncher += *properties->getProperty("pathToLaunchUI");
            pathToAppLauncher += "\"";

            Sleep (2000);

            ShellExecuteA(NULL,
                    "open",
                    pathToAppLauncher.c_str(),
                    NULL,
                    NULL,
                    SW_SHOW
                    );

        }
        else
        {
            startWebService (schSCManager, properties, false);

            startScaleService(schSCManager, properties, false);

            reenableLaunchWebBrowserAtStartUp();
            removeAppInstallerFromRegistry();


            std::cout << "Web server is running!!! System will automatically reboot in 10 Seconds!" << std::endl;

            for (int i = 0; i < 10; i ++)
            {
                Sleep (1000);
                std::cout << ".";
            }
            shutdownSystem();
        }

        CloseServiceHandle(schSCManager);
        /*
           std::cout << "Waiting to reboot! System will automatically reboot in 10 Seconds!" << std::endl;

           for (int i = 0; i < 10; i ++)
           {
           Sleep (1000);
           std::cout << ".";
           }

           shutdownSystem();
           */
    }

    delete properties;
    return 0;
}

