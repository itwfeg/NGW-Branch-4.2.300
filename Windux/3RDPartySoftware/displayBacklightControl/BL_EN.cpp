// BL_EN.cpp : Defines the entry point for the console application.
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

void show_help(void)
{
	printf("Usage:\n");
	printf("BL_EN [1|2] [0|1]\n");
	printf("0: off, 1: on\n");
}
int _tmain(int argc, char* argv[])
{
	if(argc < 2)
	{
		show_help();
		return 1;
	}
	
	int ch,data;
	ch = (int) strtol(argv[1], (char **)NULL, 10);
	data = (int) strtol(argv[2], (char **)NULL, 10);

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
			//GPIO28 = BL1_EN, 0x50F bit 4
			//GPIO39 = BL2_EN, 0x538 bit 7
		
			switch (ch) {
				case 1:
					gfpOut32(0x50F, (gfpInp32(0x50F) & ~0x10) | (data?0x10:0));
					printf("0x50F= %X\n",gfpInp32(0x50F));
					break;
				case 2:
					gfpOut32(0x538, (gfpInp32(0x538) & ~0x80) | (data?0x80:0));
					printf("0x534= %X\n",gfpInp32(0x538));
					break;
			}
		}
	}
	return 0;
}

