/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PAL_platformheader.h"
#include "Smbus.h"
//#include <initguid.h>   // replaced by PAL header
#include <winioctl.h>   // replaced by PAL header
#include "SMBUSInterface.h"
#include "Integer.h"
#include "Hex.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int Smbus::_dataType_ = 0;
	std::string Smbus::_dataTypeName_("Smbus"); 
#endif


using namespace std;


Smbus::Smbus(UCHAR devAddr): deviceInterfaceDetailData_(0), file_(0), 
	fileOpened_(false), deviceAddress_(devAddr)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", "Smbus");
#endif

	openHandle();

}

Smbus::~Smbus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", "~Smbus");
#endif

	if(deviceInterfaceDetailData_) 
	{
		free (deviceInterfaceDetailData_);
		deviceInterfaceDetailData_ = NULL;
	}

	closeHandle();
}


bool Smbus::openHandle()
{
	HDEVINFO hardwareDeviceInfo;
	SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
	ULONG predictedLength = 0;
	ULONG requiredLength = 0;
	int i = 0;

	hardwareDeviceInfo = SetupDiGetClassDevs (
				(LPGUID)&GUID_DEVINTERFACE_SMBUS,
				NULL, // Define no enumerator (global)
				NULL, // Define no
				(DIGCF_PRESENT | // Only Devices present
				DIGCF_DEVICEINTERFACE)); // Function class devices.
	if(INVALID_HANDLE_VALUE == hardwareDeviceInfo)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "failed to get hardware device info");
#endif
		return false;
	}
    
	deviceInterfaceData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);
	
	bool constTrue = true;

	do 
	{
		if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
					0, // No care about specific PDOs
					(LPGUID)&GUID_DEVINTERFACE_SMBUS,
					i, //
					&deviceInterfaceData)) 
		{
	                                 

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
				NULL)) 
			{ // not interested in the specific dev-node
				if(ERROR_INSUFFICIENT_BUFFER != GetLastError()) 
				{
	#ifndef EVOL_NDEBUG
					Debug::trace("Smbus", string("SetupDiGetDeviceInterfaceDetail failed") + Integer::toString(GetLastError()));
	#endif
					SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
					return false;                
				}
	     
			}

			predictedLength = requiredLength;

			if(deviceInterfaceDetailData_) 
			{
				free (deviceInterfaceDetailData_);
				deviceInterfaceDetailData_ = NULL;
			}

			deviceInterfaceDetailData_ = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);

			if(deviceInterfaceDetailData_) 
			{
				deviceInterfaceDetailData_->cbSize = 
				sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
			} 
			else 
			{
	#ifndef EVOL_NDEBUG
				Debug::trace("Smbus", string("Couldn't allocate ") + Integer::toString(predictedLength) + string("bytes for device interface details."));
	#endif
				SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
				return false;
			}


			if (! SetupDiGetDeviceInterfaceDetail (
				hardwareDeviceInfo,
				&deviceInterfaceData,
				deviceInterfaceDetailData_,
				predictedLength,
				&requiredLength,
				NULL)) 
			{
	#ifndef EVOL_NDEBUG
				Debug::trace("Smbus", "Error in SetupDiGetDeviceInterfaceDetail");
	#endif
				SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
				return false;
			}

		}
		else if (ERROR_NO_MORE_ITEMS != GetLastError()) 
		{
			continue;
		}
		else 
			break;
		i++;
	} while (constTrue); 


	SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);

	if(!deviceInterfaceDetailData_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "No device interfaces present");
#endif
		return false;
	}

	closeHandle();
    
	file_ = CreateFile ( deviceInterfaceDetailData_->DevicePath,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL, // no SECURITY_ATTRIBUTES structure
			OPEN_EXISTING, // No special create flags
			0, // No special attributes
			NULL);

    if (INVALID_HANDLE_VALUE == file_) 
    {
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Error in CreateFile:") + Integer::toString(GetLastError()));
#endif
		return false;
	}

	fileOpened_ = true;

	return true;
}

void Smbus::closeHandle()
{

	if (file_)
	{
		CloseHandle(file_);
		file_ = NULL;
		fileOpened_ = false;
	}

}

Smbus::SmbusStatus Smbus::readByte(UCHAR * readData)
{
	SMBUS_IOCTL_HEADER readCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read Byte started: deviceAddress: ") + Hex::toString(deviceAddress_));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Read Byte failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	readCommand.deviceAddress = deviceAddress_;
	readCommand.command = 0;
	readCommand.length = 0;
	readCommand.type = 0;


	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_BYTE_READ,
			&readCommand, sizeof(readCommand),
			readData, 1,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Read Byte failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read Byte Succeeded: bytes read:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}

Smbus::SmbusStatus Smbus::readDataByte(UCHAR command, UCHAR * readData)
{
	SMBUS_IOCTL_HEADER readCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read Byte Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Read Byte Data failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	readCommand.deviceAddress = deviceAddress_;
	readCommand.command = command;
	readCommand.length = 0;
	readCommand.type = 0;


	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_BYTE_DATA_READ,
			&readCommand, sizeof(readCommand),
			readData, 1,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Read Byte Data failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read Byte Data Succeeded: bytes read:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}

Smbus::SmbusStatus Smbus::readDataWord(UCHAR command, UCHAR * readData)
{
	SMBUS_IOCTL_HEADER readCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read Word Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Read word Data failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	readCommand.deviceAddress = deviceAddress_;
	readCommand.command = command;
	readCommand.length = 0;
	readCommand.type = 0;


	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_WORD_DATA_READ,
			&readCommand, sizeof(readCommand),
			readData, 2,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Read Word Data failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read Word Data Succeeded: bytes read:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}

Smbus::SmbusStatus Smbus::readDataBlock(UCHAR command, UCHAR length, UCHAR * readData)
{
	SMBUS_IOCTL_HEADER readCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read block Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command) + string("length: ") + Hex::toString(length));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Read block Data failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	if (length == 0)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Read block Data failed: zero length read");
#endif
		return STATUS_INVALID_PARAM; // MSVC2010

	}

	readCommand.deviceAddress = deviceAddress_;
	readCommand.command = command;
	readCommand.length = length;
	readCommand.type = 0;


	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_BLOCK_DATA_READ,
			&readCommand, sizeof(readCommand),
			readData, length,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Read block Data failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Read block Data Succeeded: bytes read:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}

Smbus::SmbusStatus Smbus::writeByte(UCHAR command)
{
	SMBUS_IOCTL_HEADER writeCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write Byte started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "write Byte failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	writeCommand.deviceAddress = deviceAddress_;
	writeCommand.command = command;
	writeCommand.length = 0;
	writeCommand.type = 0;


	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_BYTE_WRITE,
			&writeCommand, sizeof(writeCommand),
			NULL, 0,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Write Byte failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write Byte Succeeded: bytes written:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}

Smbus::SmbusStatus Smbus::writeDataByte(UCHAR command, UCHAR writeData)
{
	SMBUS_IOCTL_WRITE writeCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write Byte Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Write Byte Data failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	writeCommand.header.deviceAddress = deviceAddress_;
	writeCommand.header.command = command;
	writeCommand.header.length = 0;
	writeCommand.header.type = 0;
	writeCommand.body[0] = writeData;


	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_BYTE_DATA_WRITE,
			&writeCommand, sizeof(writeCommand),
			NULL, 0,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Write Byte Data failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write Byte Data Succeeded: bytes written:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}


Smbus::SmbusStatus Smbus::writeDataWord(UCHAR command, UCHAR * writeData)
{
	SMBUS_IOCTL_WRITE writeCommand;
	ULONG bytes = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write Word Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Write word Data failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	writeCommand.header.deviceAddress = deviceAddress_;
	writeCommand.header.command = command;
	writeCommand.header.length = 0;
	writeCommand.header.type = 0;
	writeCommand.body[0] = writeData[0];
	writeCommand.body[1] = writeData[1];

 
	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_WORD_DATA_WRITE,
			&writeCommand, sizeof(writeCommand),
			NULL, 0,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", string("Write Word Data failed:") + Integer::toString(GetLastError()));
#endif

		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write Word Data Succeeded: bytes read:") + Integer::toString(bytes));
#endif

	return STATUS_SUCCEEDED;

}

Smbus::SmbusStatus Smbus::writeDataBlock(UCHAR command, UCHAR length, UCHAR * writeData)
{
	UCHAR* pMemory = 0; 
	PSMBUS_IOCTL_WRITE pWriteCommand;
	ULONG bytes = 0;
	UCHAR writeBlockSize = 0;

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("write block Data started: deviceAddress: ") + Hex::toString(deviceAddress_) +
		string("command: ") + Hex::toString(command) + string("length: ") + Hex::toString(length));
#endif

	if (!fileOpened_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Smbus", "Write block Data failed: file not open");
#endif
		return STATUS_FILE_NOT_OPEN;        
	}

	switch (length)
	{
		case 0:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("Smbus", "Write block Data failed: zero length read");
#endif
			return STATUS_INVALID_PARAM; // MSVC2010
		}
		break;
		case 1:
		{
			return writeDataByte(command, writeData[0]);
		}
		break;
		case 2:
		{
			return writeDataWord(command, writeData);
		}
		break;

	}

	writeBlockSize = sizeof(SMBUS_IOCTL_WRITE) + length - sizeof(pWriteCommand->body);
	pMemory = (UCHAR *)malloc (writeBlockSize);

	pWriteCommand = (PSMBUS_IOCTL_WRITE)pMemory;

	pWriteCommand->header.deviceAddress = deviceAddress_;
	pWriteCommand->header.command = command;
	pWriteCommand->header.length = length;
	pWriteCommand->header.type = 0;

	memcpy (pWriteCommand->body, writeData, length);

 
	if (!DeviceIoControl (file_,
			IOCTL_SMBUS_BLOCK_DATA_WRITE,
			pWriteCommand, writeBlockSize,
			NULL, 0,
			&bytes, NULL)) 
	{
#ifndef EVOL_NDEBUG
        // ***** WARNING: We should NOT call PAL_getLastError() becuase DeviceIoControl() is not PAL_xxx
        // Also, this is Win32 only file.
		Debug::trace("Smbus", string("Write block Data failed:") + Integer::toString(GetLastError()));
#endif

		free (pMemory);
		return STATUS_FAILED;
	}

#ifndef EVOL_NDEBUG
	Debug::trace("Smbus", string("Write block Data Succeeded: bytes read:") + Integer::toString(bytes));
#endif

	free(pMemory);

	return STATUS_SUCCEEDED;

}