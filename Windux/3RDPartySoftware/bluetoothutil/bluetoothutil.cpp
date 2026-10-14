// bluetoothutil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "bluetoothapis.h"

int Scan(UCHAR timout)
{
	_tprintf(_T("Scanning ...\r\n"));
	BLUETOOTH_DEVICE_SEARCH_PARAMS params = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS), TRUE, TRUE, TRUE, TRUE, TRUE, timout, NULL };
	BLUETOOTH_DEVICE_INFO info = { sizeof(BLUETOOTH_DEVICE_INFO) };
	HBLUETOOTH_DEVICE_FIND find = BluetoothFindFirstDevice(&params, &info);
	if (find != NULL)
	{
		do
		{
			_tprintf(_T("\t%02X:%02X:%02X:%02X:%02X:%02X\t%s\r\n"),
				info.Address.rgBytes[0], info.Address.rgBytes[1], info.Address.rgBytes[2], info.Address.rgBytes[3], info.Address.rgBytes[4], info.Address.rgBytes[5],
				info.szName);
		}
		while (BluetoothFindNextDevice(find, &info));
		BluetoothFindDeviceClose(find);
	}
	else
	{
		DWORD lastError = ::GetLastError();
		_tprintf(_T("Error %d finding first device\r\n"), lastError);
		return 1;
	}
	return 0;
}

void PrintUsage(int argc, WCHAR* argv[])
{
	_tprintf(TEXT("Usage: %s { -s | -t [timeout] }\r\n"), argv[0]);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int operation = -1;
	UCHAR timeout = 10;

	int i;
	for (i = 0; i < argc; ++ i)
	{
		_TCHAR *arg = argv[i];
		if ((arg[0] == '-') || (arg[0] == '/'))
		{
			switch (arg[1])
			{
			case 's':
			case 'S':
				if (operation == -1)
				{
					operation = 1;
				}
				else
				{
					PrintUsage(argc, argv);
					return 1;
				}
				break;
			case 't':
			case 'T':
				{
					++ i;
					if (i < argc)
					{
						timeout = _wtoi(argv[i]);
					}
					else
					{
						PrintUsage(argc, argv);
						return 1;
					}
				}
				break;
			}
		}
	}

	int exitCode = 1;
	switch (operation)
	{
	case 1:
		exitCode = Scan(timeout);
		break;
	default:
		exitCode = 1;
		PrintUsage(argc, argv);
		break;
	}

	return exitCode;
}