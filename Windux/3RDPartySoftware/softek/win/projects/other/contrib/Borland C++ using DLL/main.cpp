//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "main.h"

//---------------------------------------------------------------------------

typedef long __stdcall (*Tp_stSetLineJump)(long nNewValue);
Tp_stSetLineJump p_stSetLineJump;

typedef BOOL __stdcall (*Tp_stSetReadCode39)(BOOL bNewValue);
Tp_stSetReadCode39 p_stSetReadCode39;

typedef short __stdcall (*Tp_stSetScanDirection)(short bNewValue);
Tp_stSetScanDirection p_stSetScanDirection;

typedef short __stdcall (*Tp_stScanBarCode)(LPCTSTR file);
Tp_stScanBarCode p_stScanBarCode;

typedef LPCSTR __stdcall (*Tp_stGetBarString)(short index);
Tp_stGetBarString p_stGetBarString;

typedef BOOL __stdcall (*Tp_stSetMultipleRead)(BOOL bNewValue);
Tp_stSetMultipleRead p_stSetMultipleRead;

HINSTANCE hLib;        // Handle of the dll
AnsiString filename;   // Filename of the image to be scanned

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_scanClick(TObject *Sender)
{
	char str[200];
	short	nBarStrings ;
	short	n ;

	Memo1->Lines->Clear();

	// Load "SoftekBarcodeDLL.dll"
	hLib = LoadLibrary("..\\..\\..\\..\\bin\\SoftekBarcodeDLL.dll");
	if (hLib == NULL)
	{
		MessageBox(NULL,"Error occurred while loading dll","",64);
		return;
	}

	// Assign the function pointers to functions contained in the dll
	p_stSetLineJump = (Tp_stSetLineJump) GetProcAddress(hLib, "stSetLineJump");
	p_stSetReadCode39 = (Tp_stSetReadCode39) GetProcAddress(hLib, "stSetReadCode39");
	p_stSetScanDirection = (Tp_stSetScanDirection) GetProcAddress(hLib, "stSetScanDirection");
	p_stScanBarCode = (Tp_stScanBarCode) GetProcAddress(hLib, "stScanBarCode");
	p_stGetBarString = (Tp_stGetBarString) GetProcAddress(hLib, "stGetBarString");
	p_stSetMultipleRead = (Tp_stSetMultipleRead) GetProcAddress(hLib, "stSetMultipleRead");

	// Set some properties
	p_stSetLineJump(1) ;
	p_stSetReadCode39(1) ;
	p_stSetScanDirection(15) ;
	p_stSetMultipleRead(TRUE);

	// Call the ScanBarCode method
	nBarStrings = p_stScanBarCode(filename.c_str()) ;

	// Check the result
	if (nBarStrings <= -6)
	{
		MessageBox(NULL,"Evaulation version has expired or is not valid","",64);
	}
	else if (nBarStrings < 0)
	{
		sprintf(str,"Error number %d", nBarStrings) ;
		MessageBox(NULL,str,"",64);
	}
	else if (nBarStrings == 0)
	{
		MessageBox(NULL,"No barcode found on this image","",64);
	}
	else
	{
		for (n = 1; n <= nBarStrings; n++)
		{
			Memo1->Lines->Add(p_stGetBarString(n));
		}
	}

	// Unload dll
	FreeLibrary(hLib);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button_openClick(TObject *Sender)
{
	if(OpenDialog->Execute())       // Open dialog
	{
		filename=OpenDialog->FileName;
		char * s = filename.c_str();
		Image1->Picture->LoadFromFile(s); //Display image
	}
}
//---------------------------------------------------------------------------

