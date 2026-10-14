// soft_shutdown.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include <stdlib.h>

typedef void	(__stdcall *lpOut32)(short, short);
typedef short	(__stdcall *lpInp32)(short);
typedef BOOL	(__stdcall *lpIsInpOutDriverOpen)(void);
typedef BOOL	(__stdcall *lpIsXP64Bit)(void);

//Some global function pointers (messy but fine for an example)
lpOut32 gfpOut32;
lpInp32 gfpInp32;
lpIsInpOutDriverOpen gfpIsInpOutDriverOpen;
lpIsXP64Bit gfpIsXP64Bit;

int _tmain(int argc, _TCHAR* argv[])
{
//Dynamically load the DLL at runtime (not linked at compile time)
	HINSTANCE hInpOutDll ;
	hInpOutDll = LoadLibrary ( _T("InpOut32.DLL") ) ;	//The 32bit DLL. If we are building x64 C++ 
													//applicaiton then use InpOutx64.dll
	if ( hInpOutDll != NULL )
	{
		gfpOut32 = (lpOut32)GetProcAddress(hInpOutDll, "Out32");
		gfpInp32 = (lpInp32)GetProcAddress(hInpOutDll, "Inp32");
		gfpIsInpOutDriverOpen = (lpIsInpOutDriverOpen)GetProcAddress(hInpOutDll, "IsInpOutDriverOpen");
		gfpIsXP64Bit = (lpIsXP64Bit)GetProcAddress(hInpOutDll, "IsXP64Bit");

		if (gfpIsInpOutDriverOpen())
		{
			//GPIO base addr = 0x500
			//Set GPIO12 to input (0x505 bit4 = 1)
			gfpOut32(0x505, gfpInp32(0x505)| 1<<4);

			//Poll GPIO12 value (0x50D bit4 = 0)
			//shutdown system
			while (1)
			{
				if(0 == ((gfpInp32(0x50D) >> 4) & 0x1))
					//system("C:\\WINDOWS\\System32\\shutdown /s /t 0");
				    system("C:\\Python27\\python.exe \"C:\\Program Files\\Hobart\\Python\\requestScaleServiceShutdown.py\"");
				//Sleep 500ms
				Sleep(200);
			}
		}
	}
	return 0;
}

