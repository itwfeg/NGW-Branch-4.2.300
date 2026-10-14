/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    TOAST.C

Abstract:

    Lists all the toaster device and interfaces present in the system 
    and opens the last interface to send an invalidate device Ioctl request
    or read requests.

Environment:

    usermode console application

Revision History:

  Eliyas Yakub  Nov 2, 1998
    

--*/

#include <basetyps.h>
#include <stdlib.h>
#include <wtypes.h>
#include <setupapi.h>
#include <initguid.h>
#include <stdio.h>
#include <string.h>
#include <winioctl.h>
#include <conio.h>

#include "SMBUSInterface.h"
#define USAGE  \
"Usage: Toast <-h> {-h option causes the device to hide from Device Manager UI}\n"

BOOL PrintToasterDeviceInfo();

int _cdecl main (int argc, char *argv[])
{
    HDEVINFO                            hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA            deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA    deviceInterfaceDetailData = NULL;
    ULONG                               predictedLength = 0;
    ULONG                               requiredLength = 0, bytes=0;
    HANDLE                              file;
    int                                 i, ch;
    char                                buffer[10];
    BOOL                                bHide = FALSE;
	ULONG startAddress;
/*
    if(argc == 2) {
        if(argv[1][0] == '-') {
            if(argv[1][1] == 'h' || argv[1][1] == 'H') {
                bHide = TRUE;
            } else {
                printf(USAGE);
                exit(0);
            }
        }
        else {
            printf(USAGE);
            exit(0);
        }
    }*/
    if(argc == 2) {
		startAddress = atoi(argv[1]);
	}


    //
    // Print a list of devices of Toaster Class
    // 
 /*   if(!PrintToasterDeviceInfo())
    {
        printf("No toaster devices present\n");
        return 0;
    }*/
    
    //
    // Open a handle to the device interface information set of all 
    // present toaster class interfaces.
    //

    hardwareDeviceInfo = SetupDiGetClassDevs (
                       (LPGUID)&GUID_DEVINTERFACE_SMBUS,
                       NULL, // Define no enumerator (global)
                       NULL, // Define no
                       (DIGCF_PRESENT | // Only Devices present
                       DIGCF_DEVICEINTERFACE)); // Function class devices.
    if(INVALID_HANDLE_VALUE == hardwareDeviceInfo)
    {
        printf("SetupDiGetClassDevs failed: %x\n", GetLastError());
        return 0;
    }
    
    deviceInterfaceData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

    printf("\nList of Toaster Device Interfaces\n");
    printf("---------------------------------\n");

    i = 0;

    //
    // Enumerate devices of toaster class
    //
    
    do {
        if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
                                 0, // No care about specific PDOs
                                 (LPGUID)&GUID_DEVINTERFACE_SMBUS,
                                 i, //
                                 &deviceInterfaceData)) {
                                 
            if(deviceInterfaceDetailData) {
                free (deviceInterfaceDetailData);
                deviceInterfaceDetailData = NULL;
            }

            //
            // Allocate a function class device data structure to 
            // receive the information about this particular device.
            //

            //
            // First find out required length of the buffer
            //
            
            if(!SetupDiGetDeviceInterfaceDetail (
                    hardwareDeviceInfo,
                    &deviceInterfaceData,
                    NULL, // probing so no output buffer yet
                    0, // probing so output buffer length of zero
                    &requiredLength,
                    NULL)) { // not interested in the specific dev-node
                if(ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
                    printf("SetupDiGetDeviceInterfaceDetail failed %d\n", GetLastError());
                    SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
                    return FALSE;                
                }
     
            }

            predictedLength = requiredLength;

            deviceInterfaceDetailData = malloc (predictedLength);

            if(deviceInterfaceDetailData) {
                deviceInterfaceDetailData->cbSize = 
                                sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
            } else {
                printf("Couldn't allocate %d bytes for device interface details.\n", predictedLength);
                SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
                return FALSE;
            }


            if (! SetupDiGetDeviceInterfaceDetail (
                       hardwareDeviceInfo,
                       &deviceInterfaceData,
                       deviceInterfaceDetailData,
                       predictedLength,
                       &requiredLength,
                       NULL)) {
                printf("Error in SetupDiGetDeviceInterfaceDetail\n");
                SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
                free (deviceInterfaceDetailData);
                return FALSE;
            }
            printf("%d) %s\n", ++i,
                    deviceInterfaceDetailData->DevicePath);
        }
        else if (ERROR_NO_MORE_ITEMS != GetLastError()) {
            free (deviceInterfaceDetailData);
            deviceInterfaceDetailData = NULL;
            continue;
        }
        else 
            break;

    } while (TRUE); 


    SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);

    if(!deviceInterfaceDetailData)
    {
        printf("No device interfaces present\n");
        return 0;
    }

    //
    // Open the last toaster device interface
    //

    printf("\nOpening the last interface:\n %s\n", 
                    deviceInterfaceDetailData->DevicePath);
    
    file = CreateFile ( deviceInterfaceDetailData->DevicePath,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL, // no SECURITY_ATTRIBUTES structure
                        OPEN_EXISTING, // No special create flags
                        0, // No special attributes
                        NULL);

    if (INVALID_HANDLE_VALUE == file) {
        printf("Error in CreateFile: %x", GetLastError());
        free (deviceInterfaceDetailData);
        return 0;
    }

    //
    // Invalidate the Device State
    //
        
    if(1)//bHide)
    {
		SMBUS_IOCTL_HEADER readCommand;
		SMBUS_IOCTL_WRITE writeCommand;
		UCHAR readData [40];
/*
		readCommand.deviceAddress = 0x2E;
		readCommand.command = 2;
		readCommand.length = 20;
		readCommand.type = 0;

		writeCommand.header.deviceAddress = 0x2E;
		writeCommand.header.command = 2;
		writeCommand.header.length = 22;
		writeCommand.header.type = 0;
		writeCommand.body[0] = 0xF0;

        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_BYTE_DATA_WRITE,
                              &writeCommand, sizeof(writeCommand),
                              NULL, 0,
                              &bytes, NULL)) {
            printf("write Byte Data failed:0x%x\n", GetLastError());
            free (deviceInterfaceDetailData);            
            CloseHandle(file);
            return 0;
        }
        printf("\nWrite data byte completed successfully bytes = %d\n", bytes);
 
        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_BYTE_DATA_READ,
                              &readCommand, sizeof(readCommand),
                              readData, sizeof(readData),
                              &bytes, NULL)) {
            printf("Read Byte Data failed:0x%x\n", GetLastError());
            free (deviceInterfaceDetailData);            
            CloseHandle(file);
            return 0;
        }
        printf("\nRead data byte completed successfully bytes = %d, val = %x\n", bytes, readData[0]);
      */

		writeCommand.header.deviceAddress = 0x2E;
		writeCommand.header.command = (UCHAR)startAddress;
		writeCommand.header.length = 0;
		writeCommand.header.type = 0;
		writeCommand.body[0] = 0x12;
		writeCommand.body[1] = 0x34;

		printf("write Word Data address:0x%x\n", writeCommand.header.command);
        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_WORD_DATA_WRITE,
                              &writeCommand, sizeof(writeCommand),
                              NULL, 0,
                              &bytes, NULL)) {
            printf("write Word Data failed:0x%x\n", GetLastError());

        }
		else
	        printf("\nWrite data word completed successfully bytes = %d\n", bytes);

		writeCommand.header.deviceAddress = 0x2E;
		writeCommand.header.command = (UCHAR)(startAddress + 2);
		writeCommand.header.length = 0;
		writeCommand.header.type = 0;
		writeCommand.body[0] = 0x56;
		writeCommand.body[1] = 0x78;

		printf("write Word Data address:0x%x\n", writeCommand.header.command);
        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_WORD_DATA_WRITE,
                              &writeCommand, sizeof(writeCommand),
                              NULL, 0,
                              &bytes, NULL)) {
            printf("write Word Data failed:0x%x\n", GetLastError());

        }
		else
			printf("\nWrite data word completed successfully bytes = %d\n", bytes);
 
		writeCommand.header.deviceAddress = 0x2E;
		writeCommand.header.command = (UCHAR)(startAddress + 4);
		writeCommand.header.length = 0;
		writeCommand.header.type = 0;
		writeCommand.body[0] = 0x9a;
		writeCommand.body[1] = 0xbc;

		printf("write Word Data address:0x%x\n", writeCommand.header.command);
        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_WORD_DATA_WRITE,
                              &writeCommand, sizeof(writeCommand),
                              NULL, 0,
                              &bytes, NULL)) {
            printf("write Word Data failed:0x%x\n", GetLastError());

        }
		else
			printf("\nWrite data word completed successfully bytes = %d\n", bytes);
 
/*		readCommand.deviceAddress = 0x2E;
		readCommand.command = 4;
		readCommand.length = 0;
		readCommand.type = 0;

        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_WORD_DATA_READ,
                              &readCommand, sizeof(readCommand),
                              readData, sizeof(readData),
                              &bytes, NULL)) {
            printf("Read word Data failed:0x%x\n", GetLastError());
 
        }
		else
			printf("\nRead data word completed successfully bytes = %d, val = %x:%x\n", bytes, readData[0], readData[1]);
  */
		readCommand.deviceAddress = 0x2E;
		readCommand.command = (UCHAR) startAddress;
		readCommand.length = 6;
		readCommand.type = 0;

		printf("read block Data address:0x%x\n", writeCommand.header.command);
        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_BLOCK_DATA_READ,
                              &readCommand, sizeof(readCommand),
                              readData, sizeof(readData),
                              &bytes, NULL)) {
            printf("Read block Data failed:0x%x\n", GetLastError());

        }
		else
			printf("\nRead data block completed successfully bytes = %d, val = %x:%x:%x:%x:%x:%x\n", 
				bytes, readData[0], readData[1], readData[2], readData[3], readData[4], readData[5]);

		readCommand.deviceAddress = 0x2E;
		readCommand.command = (UCHAR) startAddress;
		readCommand.length = 6;
		readCommand.type = 0;

		printf("read block Data address:0x%x\n", writeCommand.header.command);
        if (!DeviceIoControl (file,
                              IOCTL_SMBUS_BLOCK_DATA_READ,
                              &readCommand, sizeof(readCommand),
                              readData, sizeof(readData),
                              &bytes, NULL)) {
            printf("Read block Data failed:0x%x\n", GetLastError());

        }
		else
			printf("\nRead data block completed successfully bytes = %d, val = %x:%x:%x:%x:%x:%x\n", 
				bytes, readData[0], readData[1], readData[2], readData[3], readData[4], readData[5]);
  

    }
    

    // 
    // Read/Write to the toaster device.
    //
    
    printf("\nPress 'q' to exit, any other key to read...\n");
    fflush(stdin);
    ch = _getche();

/*    while(tolower(ch) != 'q' )
    {
    
        if(!ReadFile(file, buffer, sizeof(buffer), &bytes, NULL))
        {
            printf("Error in ReadFile: %x", GetLastError());
            free (deviceInterfaceDetailData);
            return 0;
        }
        printf("Read Successful\n");
        ch = _getche();
    }*/

    free (deviceInterfaceDetailData);
    CloseHandle(file);
    return 0;
}


/*
BOOL 
PrintToasterDeviceInfo()
{
    HDEVINFO        hdi;
    DWORD           dwIndex=0;
    SP_DEVINFO_DATA deid;
    BOOL            fSuccess=FALSE;
    CHAR           szCompInstanceId[MAX_PATH];
    CHAR           szCompDescription[MAX_PATH];
    CHAR           szFriendlyName[MAX_PATH];
    DWORD           dwRegType;
    BOOL            fFound=FALSE;

    // get a list of all devices of class 'GUID_DEVCLASS_TOASTER'
    hdi = SetupDiGetClassDevs(&GUID_DEVCLASS_TOASTER, NULL, NULL, 
                                            DIGCF_PRESENT);

    if (INVALID_HANDLE_VALUE != hdi)
    {

        // enumerate over each device
        while (deid.cbSize = sizeof(SP_DEVINFO_DATA),
               SetupDiEnumDeviceInfo(hdi, dwIndex, &deid))
        {
            dwIndex++;

            // the right thing to do here would be to call this function
            // to get the size required to hold the instance ID and then
            // to call it second time with a buffer large enough for that size.
            // However, that would tend to obscure the control flow in
            // the sample code. Lets keep things simple by keeping the
            // buffer large enough.

            // get the device instance ID
            fSuccess = SetupDiGetDeviceInstanceId(hdi, &deid,
                                                  (BYTE*)szCompInstanceId,
                                                  MAX_PATH, NULL);
            if (fSuccess)
            {
                // get the description for this instance
                fSuccess =
                    SetupDiGetDeviceRegistryProperty(hdi, &deid,
                                                     SPDRP_DEVICEDESC,
                                                     &dwRegType,
                                                     (BYTE*) szCompDescription,
                                                     MAX_PATH,
                                                     NULL);
                if (fSuccess)
                {
                    memset(szFriendlyName, 0, MAX_PATH);
                    SetupDiGetDeviceRegistryProperty(hdi, &deid,
                                                     SPDRP_FRIENDLYNAME,
                                                     &dwRegType,
                                                     (BYTE*) szFriendlyName,
                                                     MAX_PATH,
                                                     NULL);
                    fFound = TRUE;
                    printf("Instance ID : %s\n", szCompInstanceId);
                    printf("Description : %s\n", szCompDescription);
                    printf("FriendlyName: %s\n\n", szFriendlyName);
                }
            }
        }

        // release the device info list
        SetupDiDestroyDeviceInfoList(hdi);
    }

    if(fFound)
        return TRUE;
    else 
        return FALSE;
}
*/
