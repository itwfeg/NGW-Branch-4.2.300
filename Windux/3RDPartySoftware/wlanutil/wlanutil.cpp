// wlanutil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "wlanapi.h"

#define BUFFERSIZE 5000

void PrintUsage(int argc, WCHAR* argv[])
{
    _tprintf(TEXT("Usage: %s [xml_file] [profile_name]\n"), argv[0]);
}

int wmain(int argc, WCHAR* argv[])
{
    WCHAR szXMLFile[MAX_PATH];                  // Name of file containing userdata (in XML format)
    WCHAR szProfileName[MAX_PATH];              // Name of wi-fi connection profile that we are adding userdata to
    HANDLE hWlan = NULL;                        // Handle to Wlan interface
    HANDLE hFile = NULL;                        // Handle to XML file
    DWORD dwMaxClient = 2;                      // For WlanOpenHandle: Client version for Windows Vista and Windows Server 2008 (Source: MSDN)   
    DWORD dwCurVersion = 0;                     // Out parameter from WlanOpenHandle: The version of the WLAN API that will be used in this session. This value is usually the highest version supported by both the client and server.
    DWORD dwResult = 0;                         // Retval for API calls
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;   // List of all Wlan interfaces on system
    PWLAN_INTERFACE_INFO pIfInfo = NULL;        // Information on specific Wlan interface
    GUID guidWlanInterface;                     // GUID for Wlan interface
    DWORD dwBytesRead = 0;                      // Out parameter for ReadFile - actual number of bytes read into buffer
    char szXML[BUFFERSIZE] = {0};               // XML read from szXMLFile
    WCHAR wszXML[BUFFERSIZE] = {0};             // WCHAR version of XML read from szXMLFile

    // Check for correct number of arguments
    if (argc == 3)
    {
        try
        {
            wcscpy_s(szXMLFile, argv[1]);
            WIN32_FIND_DATA FindFileData;
            HANDLE hFind;

            hFind = FindFirstFile(szXMLFile, &FindFileData);
            if (hFind == INVALID_HANDLE_VALUE) 
            {
                wprintf(L"File not found\n");
                PrintUsage(argc, argv);
                return 1;
            } 
            else 
            {
                FindClose(hFind);
            }    

            wcscpy_s(szProfileName, argv[2]);
        }
        catch(...)
        {
            wprintf(L"Invalid parameter\n");
            PrintUsage(argc, argv);
            return 1;
        }
    }
    else
    {
        PrintUsage(argc, argv);
    }

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hWlan); 
    if (dwResult != ERROR_SUCCESS)
    {
        wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
        return 1;
    }

    // Find GUID for the WLAN interface
    dwResult = WlanEnumInterfaces(hWlan, NULL, &pIfList); 
    if (dwResult != ERROR_SUCCESS)
    {
        wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        // FormatMessage can be used to find out why the function failed
        return 1;
    }
    else 
    {
        // Assumption: always 1 and only 1 WLAN interface
        if (pIfList->dwNumberOfItems == 1)
        {
            pIfInfo = (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[0];

            if (pIfInfo)
            {
                guidWlanInterface = pIfInfo->InterfaceGuid;
            }
        }
    }

    if (pIfList != NULL) 
    {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    // Open XML File
    hFile = CreateFile(szXMLFile,               // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        wprintf(L"Unable to open file \"%s\" for read.\n", szXMLFile);
        return 1; 
    }

    // Read XML into szXML
    if(!ReadFile(hFile, (LPVOID)&szXML, BUFFERSIZE-1, &dwBytesRead, NULL))
    {
        wprintf(L"Unable to read from file.\n GetLastError=%08x\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }

    // File is ANSI text. 
    // Must convert to WideChar for use in WlanSetProfileEapXmlUserData API
    if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE-1)
    {
        szXML[dwBytesRead]='\0'; // Explict NULL character at end of char buffer
        MultiByteToWideChar(CP_ACP, NULL, szXML, BUFFERSIZE, wszXML, BUFFERSIZE);
    }
    else if (dwBytesRead == 0)
    {
        wprintf(L"No data read from file %s\n", szXMLFile);
    }
    else
    {
        wprintf(L"Error reading file %s\n", szXMLFile);
    }

    CloseHandle(hFile);
	wprintf(L"Starting\n");
    dwResult = WlanSetProfileEapXmlUserData(hWlan, &guidWlanInterface, szProfileName, WLAN_SET_EAPHOST_DATA_ALL_USERS, wszXML, NULL);
	 wprintf(L"finishing\n");
    WlanCloseHandle(hWlan, NULL);

    if (dwResult == ERROR_SUCCESS)
    {
         wprintf(L"success\n");
		return 0;
    }

    switch (dwResult)
    {
    case ERROR_ACCESS_DENIED:
        {
            wprintf(L"Access denied\n");
            break;
        }
    case ERROR_BAD_PROFILE:
        {
            wprintf(L"Bad profile\n");
            break;
        }
    case ERROR_INVALID_PARAMETER:
        {
            wprintf(L"Invalid parameter\n");
            break;
        }
    case ERROR_INVALID_HANDLE:
        {
            wprintf(L"Invalid handle\n");
            break;
        }
    case ERROR_NOT_ENOUGH_MEMORY:
        {
            wprintf(L"Not enough memory\n");
            break;
        }
    case ERROR_NOT_SUPPORTED:
        {
            wprintf(L"Not supported\n");
            break;
        }
    case ERROR_SERVICE_NOT_ACTIVE:
        {
            wprintf(L"Not active\n");
            break;
        }
    default:
        {
            wprintf(L"Failed with error: %u\n", dwResult);
            break;
        }
    }

    return 1;
}
