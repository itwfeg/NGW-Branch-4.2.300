#include <string>
#include <math.h>


#include "RendererManagerUnitTester.h"


#include "PAL_platformheader.h"
//#include <windowsx.h> // replaced by "PAL" header


#include "RendererManager/Include/ImageControl.h"
#include "RendererManager/Include/HobartLabel.h"
#include "RendererManager/Include/FormRecord.h"
#include "RendererManager/Include/Label.h"
#include "RendererManager/Include/Barcode.h"
#include "RendererManager/Include/Renderer.h"
#include "RendererManager/Include/BitMap.h"
#include "RendererManager/Include/Pen.h"
#include "RendererManager/Include/CRendererUtil.h"



int CALLBACK _EnumerateFont_(CONST ENUMLOGFONTEX* logicalFont, CONST NEWTEXTMETRICEX* physicalFont, DWORD fontType, LPARAM param);
void _DebugLogicalFont_(const LOGFONT& logicalFont);
const wchar_t* _DebugLogicalFontWeight_(int weight);
const wchar_t* _DebugLogicalFontCharSet_(int charset);
const wchar_t* _DebugLogicalFontOutPrecision_(int outPrecision);
const wchar_t* _DebugLogicalFontClipPrecision_(int clipPrecision);
const wchar_t* _DebugLogicalFontQuality_(int quality);
const wchar_t* _DebugLogicalFontPitch_(int pitch);
const wchar_t* _DebugLogicalFontFamily_(int family);
void _DebugPhysicalFont_(const NEWTEXTMETRICEX& physicalFont);
const wchar_t* _DebugPhysicalFontPitch_(int pitch);
std::wstring _DebugPhysicalFontFlags_(unsigned int flags);
const wchar_t* _DebugFontType_(unsigned int fontType);

void _DebugDeviceContext_(HDC hdc);
const wchar_t* _DebugDeviceCapTechnology_(int tech);
std::wstring _DebugDeviceRasterCapabilities_(int flags);
std::wstring _DebugDeviceCurveCapabilities_(int flags);
std::wstring _DebugDeviceLineCapabilities_(int flags);
std::wstring _DebugDevicePolygonalCapabilities_(int flags);
std::wstring _DebugDeviceTextCapabilities_(int flags);
std::wstring _DebugDeviceClipCapabilities_(int flags);
std::wstring _DebugDeviceShadeBlendCapabilities_(int flags);
std::wstring _DebugDeviceColorManagementCapabilities_(int flags);



#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererManagerUnitTester::dataType = 0;
std::string RendererManagerUnitTester::dataTypeName("RendererManagerUnitTester"); 
#endif



RendererManagerUnitTester::RendererManagerUnitTester() 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}


RendererManagerUnitTester::~RendererManagerUnitTester()
{

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


bool RendererManagerUnitTester::runLineTests()
{
	bool result = runTest1();

	result = runTest2() ? result : false;

	return result;
}


bool RendererManagerUnitTester::runBarcodeTests()
{
	bool result = runTest11();

	//result = runTest12() ? result : false;

	return result;
}


bool RendererManagerUnitTester::runGraphicTests()
{
	bool result = runTest21();

	//result = runTest22() ? result : false;

	return result;
}


bool RendererManagerUnitTester::runFontTests()
{
	bool result = runTest31();

	result = runTest32() ? result : false;

	return result;
}


bool RendererManagerUnitTester::runTextTests()
{
	bool result = runTest41();

//	result = runTest42() ? result : false;

	return result;
}


bool RendererManagerUnitTester::runLabelTests()
{
	bool result = runTest51();

//	result = runTest52() ? result : false;

	return result;
}


bool RendererManagerUnitTester::runTest1()
{
	HobartLabel* label = createLabel1();
	LabelStockDescriptor* lsd = label->getLabelStockDescriptor();
	ImageControl* ic = new ImageControl(label->getFormsList(), lsd);

	BitMap bitmap;
	UINT32 bitmapSize = lsd->getLength() * (lsd->getWidthDots() / 8);
	bitmap.allocateBitMap(bitmapSize);
	bitmap.setBitMapBounds(0, lsd->getWidthDots(), 0, lsd->getLength());
	bitmap.setBitMapPitch(((bitmap.getBitMapBoundsRight() - bitmap.getBitMapBoundsLeft() - 1) / 16 + 1));

	ic->setImageBitMapSize(bitmapSize);
	ic->setHorizFormWidth(lsd->getWidthDots());
	ic->setImageBitMap(&bitmap);
	ic->setPrintingRotatedBitmap(false);

	ic->setImageFlags(PreImage);
	ic->setImageLines(0);
	ic->setImageValid(false);
	ic->setImageOptions(0);

	Pen pen;
	pen.setPenColor(Black);
	pen.setPenState(Down);
	pen.setPenLocation(0,0);
	pen.setPenSize(1,1);
	ic->setPen(&pen);


	ic->initFormListIter();
	while ( !ic->isCurrentFormNull() )
	{
		FormRecord* form = ic->getFormFromList();
		BitMap* bitmap = ic->getImageBitMap();

		renderField(ic, form, bitmap, 0);
	}

	std::wstring fileName( L"BitmapTest1.bmp" );
//	Hobart::CRendererUtil::SaveBitmapToFile( ic->getImageBitMap(), fileName );

	bitmap.freeBitMap();
	delete ic;
	delete label;

	return true;
}


bool RendererManagerUnitTester::runTest2()
{
	HobartLabel* label = createLabel2();
	LabelStockDescriptor* lsd = label->getLabelStockDescriptor();
	ImageControl* ic = new ImageControl(label->getFormsList(), lsd);

	BitMap bitmap;
	UINT32 bitmapSize = lsd->getLength() * (lsd->getWidthDots() / 8);
	bitmap.allocateBitMap(bitmapSize);
	bitmap.setBitMapBounds(0, lsd->getWidthDots(), 0, lsd->getLength());
	bitmap.setBitMapPitch(((bitmap.getBitMapBoundsRight() - bitmap.getBitMapBoundsLeft() - 1) / 16 + 1));

	ic->setImageBitMapSize(bitmapSize);
	ic->setHorizFormWidth(lsd->getWidthDots());
	ic->setImageBitMap(&bitmap);
	ic->setPrintingRotatedBitmap(false);

	ic->setImageFlags(PreImage);
	ic->setImageLines(0);
	ic->setImageValid(false);
	ic->setImageOptions(0);

	Pen pen;
	pen.setPenColor(Black);
	pen.setPenState(Down);
	pen.setPenLocation(0,0);
	pen.setPenSize(1,1);
	ic->setPen(&pen);


	ic->initFormListIter();
	while ( !ic->isCurrentFormNull() )
	{
		FormRecord* form = ic->getFormFromList();
		BitMap* bitmap = ic->getImageBitMap();

		renderField(ic, form, bitmap, 0);
	}

	std::wstring fileName( L"BitmapTest2.bmp" );
//	Hobart::CRendererUtil::SaveBitmapToFile( ic->getImageBitMap(), fileName );

	bitmap.freeBitMap();
	delete ic;
	delete label;

	return true;
}


bool RendererManagerUnitTester::runTest11()
{
	HobartLabel* label = createLabel11();
	LabelStockDescriptor* lsd = label->getLabelStockDescriptor();
	ImageControl* ic = new ImageControl(label->getFormsList(), lsd);

	BitMap bitmap;
	INT16 pitch = (lsd->getWidthDots() + 15) / 16;
	bitmap.allocateBitMap(lsd->getLength() * pitch);
	bitmap.setBitMapBounds(0, lsd->getWidthDots(), 0, lsd->getLength());
	bitmap.setBitMapPitch(pitch);

	ic->setImageBitMapSize(lsd->getLength() * pitch * 2);
	ic->setHorizFormWidth(lsd->getWidthDots());
	ic->setImageBitMap(&bitmap);
	ic->setPrintingRotatedBitmap(false);

	ic->setImageFlags(PostImage);
	ic->setImageLines(0);
	ic->setImageValid(false);
	ic->setImageOptions(0);

	Pen pen;
	pen.setPenColor(Black);
	pen.setPenState(Down);
	pen.setPenLocation(0,0);
	pen.setPenSize(1,1);
	ic->setPen(&pen);


	ic->initFormListIter();
	while ( !ic->isCurrentFormNull() )
	{
		FormRecord* form = ic->getFormFromList();
		BitMap* bitmap = ic->getImageBitMap();

		renderField(ic, form, bitmap, 0);
	}

	std::wstring fileName( L"BitmapTest11.bmp" );
//	Hobart::CRendererUtil::SaveBitmapToFile( ic->getImageBitMap(), fileName );

	bitmap.freeBitMap();
	delete ic;
	delete label;

	return true;
}


bool RendererManagerUnitTester::runTest21()
{
	HobartLabel* label = createLabel21();
	ImageControl* ic = renderLabel(label);
	BitMap* bitmap = ic->getImageBitMap();

	std::wstring fileName( L"BitmapTest21.bmp" );
//	Hobart::CRendererUtil::SaveBitmapToFile( bitmap, fileName );

	bitmap->freeBitMap();
	delete ic;
	delete label;

	return true;
}


bool RendererManagerUnitTester::runTest31()
{
	HDC hdc = CreateCompatibleDC(NULL);
	LOGFONT fontSpec;

	fontSpec.lfHeight = - (LONG) ( (14 * (FLOAT)203.2) / 72 + (FLOAT)0.5) ;
//	fontSpec.lfCharSet = DEFAULT_CHARSET;
	fontSpec.lfCharSet = ANSI_CHARSET;
	fontSpec.lfPitchAndFamily = 0;
//	wcscpy(fontSpec.lfFaceName, L"");
	wcscpy(fontSpec.lfFaceName, L"Sp");

	EnumFontFamiliesEx(hdc, &fontSpec, (FONTENUMPROCW)&_EnumerateFont_, (LPARAM)this, 0);

	return true;
}


bool RendererManagerUnitTester::runTest32()
{
	HobartLabel* label = createLabel32();
	ImageControl* ic = renderLabel(label);
	BitMap* bitmap = ic->getImageBitMap();

	std::wstring fileName( L"BitmapTest32.bmp" );
//	Hobart::CRendererUtil::SaveBitmapToFile( bitmap, fileName );

	bitmap->freeBitMap();
	delete ic;
	delete label;

	return true;
}


bool RendererManagerUnitTester::runTest41()
{
	BitMap hobBitmap;

	{
		HBITMAP bitmap = CreateBitmap(400,300,1,1,NULL);
		HDC hdc = CreateCompatibleDC(NULL);

		HBITMAP bitmap_bk = SelectBitmap( hdc, bitmap );
		SetBkColor(hdc,COLOR_WHITE );
		//draw something.
		HBRUSH black_brush = CreateHatchBrush( HS_DIAGCROSS,COLOR_BLACK );

		RECT rct;
		rct.left=1;
		rct.right=399;
		rct.top=1;
		rct.bottom=299;
		FillRect(hdc, &rct, black_brush);

		DeleteObject( black_brush );
		SelectBitmap(hdc, bitmap_bk);
		DeleteDC(hdc);

		Hobart::CRendererUtil::GetHobartBitmap(bitmap, &hobBitmap);
		DeleteObject(bitmap);
	}
	

	{
		int XIncrement = 0; 
		TEXTMETRIC tm; 
		SIZE sz; 
		LPCWSTR lpszString1 = L"It"; 
		LPCWSTR lpszString2 = L" is "; 
//		LPCWSTR lpszString3 = L"Huge!"; 
//		HRESULT hr;
//		size_t * pcch;


		HBITMAP bitmap2 = CreateBitmap(200,100,1,1,NULL);
		HDC hdc2 = CreateCompatibleDC(NULL);
		HBITMAP bk2 = SelectBitmap( hdc2, bitmap2 );
		SetTextColor(hdc2, COLOR_BLACK);
		SetBkMode(hdc2, TRANSPARENT );

		
		long h = -MulDiv(10, GetDeviceCaps(hdc2, LOGPIXELSY), 72);

		LOGFONT lFont;
		lFont.lfHeight = 2*h;
		lFont.lfWidth = 0;
		lFont.lfEscapement = 0;// -10*15;
		lFont.lfOrientation = 0; //-10*15;
		lFont.lfWeight = FW_BOLD;
		lFont.lfItalic = TRUE;
		lFont.lfUnderline = FALSE;
		lFont.lfStrikeOut = FALSE;
		lFont.lfCharSet = ANSI_CHARSET;
		lFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lFont.lfQuality = DEFAULT_QUALITY;
		lFont.lfPitchAndFamily = FF_DONTCARE;
		wcscpy(lFont.lfFaceName, L"Courier New");

		
		HFONT font = CreateFontIndirect(&lFont); 					
		SelectFont( hdc2, font );
		TextOut( hdc2, 0, 40, lpszString1, 2 );
		DeleteObject( font );

		GetTextExtentPoint32(hdc2, lpszString1, 2, &sz); 
		XIncrement += sz.cx; 

		// Retrieve the overhang value from the TEXTMETRIC 
		// structure and subtract it from the x-increment. 
		// (This is only necessary for non-TrueType raster 
		// fonts.) 
		GetTextMetrics(hdc2, &tm); 
		XIncrement -= tm.tmOverhang; 


		lFont.lfHeight =  h; //20
		lFont.lfItalic = FALSE;
		//lFont.lfEscapement = -10*15;
		//lFont.lfOrientation = -10*15;
		font = CreateFontIndirect(&lFont); 
		SelectFont( hdc2, font );
		TextOut( hdc2, XIncrement, 50, L" is ", 4);
		DeleteObject( font );

		GetTextExtentPoint32(hdc2, lpszString2, 4, &sz); 
		XIncrement += sz.cx; 

		lFont.lfHeight = 4 * h; //20
		lFont.lfStrikeOut = TRUE;
		lFont.lfItalic = FALSE;
		font = CreateFontIndirect(&lFont); 
		SelectFont( hdc2, font );
		TextOut( hdc2, XIncrement, 20, L"Huge!", 5 );
		DeleteObject( font );

		SelectBitmap( hdc2, bk2 );
		DeleteDC( hdc2 );

		Hobart::CRendererUtil::Draw(&hobBitmap, bitmap2, 100, 100, SRCCOPY);
		DeleteObject(bitmap2);
	}
	
	{
		std::wstring file( L"BitmapTest41.bmp" );
//		Hobart::CRendererUtil::SaveBitmapToFile( &hobBitmap, file );
	}

	hobBitmap.freeBitMap();
	
	return true;
}


bool RendererManagerUnitTester::runTest51()
{
	wchar_t debugString[255];
	HobartLabel* label = createLabel51();
	ImageControl* ic = renderLabel(label);
	BitMap* bitmap = ic->getImageBitMap();

	int byte = 119 * bitmap->getBitMapPitch() * 2 ;
	swprintf(debugString, L"At end: Pixel (0,119) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,119) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 1));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,119) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 2));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,119) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 3));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,119) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 4));
	OutputDebugStringW(debugString);

	byte = 120 * bitmap->getBitMapPitch() * 2 ;
	swprintf(debugString, L"At end: Pixel (0,120) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,120) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 1));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,120) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 2));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,120) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 3));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,120) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 4));
	OutputDebugStringW(debugString);

	byte = 121 * bitmap->getBitMapPitch() * 2 ;
	swprintf(debugString, L"At end: Pixel (0,121) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,121) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 1));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,121) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 2));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,121) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 3));
	OutputDebugStringW(debugString);
	swprintf(debugString, L"At end: Pixel (0,121) = %x\n", *((unsigned char *)bitmap->getBitMapBase() + byte + 4));
	OutputDebugStringW(debugString);

	std::wstring fileName( L"BitmapTest51.bmp" );
//	Hobart::CRendererUtil::SaveBitmapToFile( bitmap, fileName );

	bitmap->freeBitMap();
	delete ic;
	delete label;

	return true;
}


HobartLabel* RendererManagerUnitTester::createLabel1()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel1");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(WideLabelWidth);
	lsd.setLength(Length3_500_Lines);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
	FormRecord* horizLine = new FormRecord(2, 1, lsd.getWidthDots() - 2*2, 0,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 1, 0);
	list.push_back(horizLine);

	horizLine = new FormRecord(2, lsd.getLength() - 1 - 1, lsd.getWidthDots() - 2*2, 0,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 1, 0);
	list.push_back(horizLine);

	FormRecord* vertLine = new FormRecord(1, 2, 0, lsd.getLength() - 2*2,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 1, 0);
	list.push_back(vertLine);

	vertLine = new FormRecord(lsd.getWidthDots() - 1 - 1, 2, 0, lsd.getLength() - 2*2,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 1, 0);
	list.push_back(vertLine);


	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


HobartLabel* RendererManagerUnitTester::createLabel2()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel2");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(WideLabelWidth);
	lsd.setLength(Length3_500_Lines);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
	FormRecord* horizLine = new FormRecord(4, 2, lsd.getWidthDots() - 2*4, 0,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 2, 0);
	list.push_back(horizLine);

	horizLine = new FormRecord(4, lsd.getLength() - 2*2, lsd.getWidthDots() - 2*4, 0,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 2, 0);
	list.push_back(horizLine);

	FormRecord* vertLine = new FormRecord(2, 4, 0, lsd.getLength() - 2*4,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 2, 0);
	list.push_back(vertLine);

	vertLine = new FormRecord(lsd.getWidthDots() - 2*2, 4, 0, lsd.getLength() - 2*4,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 2, 0);
	list.push_back(vertLine);


	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


HobartLabel* RendererManagerUnitTester::createLabel11()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel11");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(WideLabelWidth);
	lsd.setLength(Length3_500_Lines);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
/*	FormRecord* barcode = new FormRecord(2, 2, lsd.getWidthDots() - 2*2, lsd.getLength() - 2*2,
			Fixed | PostImage | BarcodeEntity | RectBorder, BARCODE, CenterJustify, 0, UPCA, 0); */
	FormRecord* barcode = new FormRecord(2, 2, 351, 100,
			Fixed | PostImage | BarcodeEntity | RectBorder, BARCODE, CenterJustify, 0, UPCA, 0);
	barcode->setData((UINT32*)"123456789012");
	list.push_back(barcode);


	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


HobartLabel* RendererManagerUnitTester::createLabel21()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel21");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(WideLabelWidth);
	lsd.setLength(Length3_500_Lines);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
/*	FormRecord* graphic = new FormRecord(2, 2, lsd.getWidthDots() - 2*2, lsd.getLength() - 2*2,
			Fixed | PreImage | GraphicEntity | RectBorder, USER_GRAPHIC, CenterJustify, 0, 0, 0); */
/*	FormRecord* graphic = new FormRecord(2, 2, 350, 100,
			Fixed | PreImage | GraphicEntity | RectBorder, USER_GRAPHIC, CenterJustify, 0, 0, 0); */
	FormRecord* graphic = new FormRecord(2, 2, 250, 50,
			Fixed | PreImage | GraphicEntity | RectBorder, USER_GRAPHIC, CenterJustify, 0, 0, 0);
	HobartLabel* label = createLabel29();
	ImageControl* ic = renderLabel(label);
	BitMap* bitmap = ic->getImageBitMap();
	graphic->setBitMapData(bitmap);
	list.push_back(graphic);

	delete label;
	delete ic;

	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


HobartLabel* RendererManagerUnitTester::createLabel29()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel21");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(353);
	lsd.setLength(102);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
	FormRecord* barcode = new FormRecord(1, 1, 351, 100,
			Fixed | PreImage | BarcodeEntity, BARCODE, CenterJustify, 0, UPCA, 0);
	barcode->setData((UINT32*)"123456789012");
	list.push_back(barcode);


	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


HobartLabel* RendererManagerUnitTester::createLabel32()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel21");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(WideLabelWidth);
	lsd.setLength(Length3_500_Lines);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
	FormRecord* text = new FormRecord(2, 2, lsd.getWidthDots() - 2*2, lsd.getLength() - 2*2,
			Fixed | PreImage | TextEntity | RectBorder, DESCRIPTION, LeftJustify, 0, 0, 0);
	text->setData((UINT32*)"This is the description to be rendered with the Windows rendering"
		" engine. If it works correctly, that would be a miracle (or mirage)."
		" Can this work at all? No one can tell. But a little bit of luck may"
		" go a long way here.");
	text->setFamilyName((const UINT8*)"Times New Roman");
	text->setPointSize(18);
	list.push_back(text);


	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


HobartLabel* RendererManagerUnitTester::createLabel51()
{
	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId((UINT16)0xFFFF);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("RendererManager TestLabel21");


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidthDots(WideLabelWidth);
	lsd.setLength(Length3_000_Lines);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);


	// Create the forms list
	HobartLabel::FormsListType list;
	FormRecord* form;
//000C008201A6003C0111010100000000000053500000000000000000000000000000000000000000000000000000000000000000000000000000000A00000000000000000000
//	form = new FormRecord(12, 126, 422, 64,
//	form = new FormRecord(5, 52, 437, 58,
	form = new FormRecord(5, 52, 437, 62,
			Fixed | PreImage | TextEntity, DESCRIPTION, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"BEEF GROUND 10\" CHUCK""\x0A""CHUCK GROUND BEEF");
//	form->setData((UINT32*)"\x0A""CHUCK GROUND BEEF");
	form->setFamilyName((const UINT8*)"Sp");
	form->setPointSize(14);
	list.push_back(form);
//000D000B0104006E01220000000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000
/*	form = new FormRecord(13, 11, 260, 110,
			Fixed | PreImage | BarcodeEntity | RectBorder, BARCODE, LeftJustify, 0, 0, 0);
	form->setData((UINT32*)"123456789012");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(8);
	list.push_back(form);
//0111000B00A0006E01141500000000000000436F7572696572204E65770000000000000000000000000000000000000000000000000000000000000000000000000000000000
	form = new FormRecord(273, 11, 10, 110,
			Fixed | PreImage | GraphicEntity | RectBorder, STORE_GRAPHIC, LeftJustify, 0, 0, 0);
	list.push_back(form);
//000C01CF01A6000001181300000002000000436F7572696572204E65770000000000000000000000000000000000000000000000000000000000000C00000000000000000000
	form = new FormRecord(12, 463, 422, 0,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 2, 0);
	list.push_back(form);
//012F01E3008B001C01210801000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000800000000000000000000
	form = new FormRecord(303, 483, 139, 28,
			Fixed | PreImage | TextEntity, SELL_BY, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"10/2/06");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(8);
	list.push_back(form);
//009A02030000003001181300000003000000436F7572696572204E65770000000000000000000000000000000000000000000000000000000000000C00000000000000000000
	form = new FormRecord(154, 515, 0, 48,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 3, 0);
	list.push_back(form);
//000C020201AE000001181300000002000000436F7572696572204E65770000000000000000000000000000000000000000000000000000000000000C00000000000000000000
	form = new FormRecord(12, 514, 430, 0,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 2, 0);
	list.push_back(form);
//012C01CF0000006401181300000003000000436F7572696572204E65770000000000000000000000000000000000000000000000000000000000000C00000000000000000000
	form = new FormRecord(300, 463, 0, 100,
			Fixed | PreImage | LineEntity, USER_LINE, LeftJustify, 0, 3, 0);
	list.push_back(form);
//012F01D1008B001201111801000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000
	form = new FormRecord(303, 465, 139, 18,
			Fixed | PreImage | TextEntity, GENERIC_TEXT_3, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"Sell By");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(5);
	list.push_back(form);
//000C0204008E001201111901000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000
	form = new FormRecord(12, 516, 142, 18,
			Fixed | PreImage | TextEntity, GENERIC_TEXT_4, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"Net Weight");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(5);
	list.push_back(form);
//000C0216008E002301210A01000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000A00000000000000000000
	form = new FormRecord(12, 534, 142, 35,
			Fixed | PreImage | TextEntity, NET_WEIGHT, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"3.21 lb.");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(10);
	list.push_back(form);
//009D0204008F001201111A01000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000
	form = new FormRecord(157, 516, 143, 18,
			Fixed | PreImage | TextEntity, GENERIC_TEXT_5, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"Unit Price");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(5);
	list.push_back(form);
//009D0216008F002301210D01000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000A00000000000000000000
	form = new FormRecord(157, 534, 143, 35,
			Fixed | PreImage | TextEntity, UNIT_PRICE, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"$5.38");
//	form->setData((UINT32*)"$5.3");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(10);
	list.push_back(form);
//012F0204008B001201111B01000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000
	form = new FormRecord(303, 516, 139, 18,
			Fixed | PreImage | TextEntity, GENERIC_TEXT_6, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"Total Price");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(5);
	list.push_back(form);
//012F0216008B002901210E01000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000C00000000000000000000
	form = new FormRecord(303, 534, 139, 41,
			Fixed | PreImage | TextEntity, TOTAL_PRICE, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"$12.34");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(12);
	list.push_back(form);
//000C01DB0120002301110501000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000A00000000000000000000
	form = new FormRecord(12, 475, 288, 35,
			Fixed | PreImage | TextEntity, SPECIAL_MESSAGE, CenterJustify, 0, 0, 0);
	form->setData((UINT32*)"Good Purchase!");
	form->setFamilyName((const UINT8*)"Times New Roman");
	form->setPointSize(10);
	list.push_back(form);
//000C011B01AA00B101144800000000000000416C627361666568616E640000000000000000000000000000000000000000000000000000000000000000000000000000000000
	form = new FormRecord(12, 283, 426, 177,
			Fixed | PreImage | GraphicEntity, INDIRECT_GRAPHIC_REFERENCE, LeftJustify, 0, 0, 0);
	form->setData((UINT32*)"Albsafehand");
	list.push_back(form); */
//000C00C001A6005A0A110300000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000500000000000000000000
//000B00FB01A8004B1A11 0300000000000000486F6261727400000000000000000000000000000000000000000000000000000000000000000000000700000000000000000000
/*	form = new FormRecord(11, 251, 424, 200,
			RectBorder | Variable | Floating | PreImage | TextEntity, EXPANDED_TEXT_1, LeftJustify, 0, 0, 0);
	form->setData((UINT32*)"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE""\x0A""EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE""\x0A""EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
	form->setData((UINT32*)"INGREDIENTS: ""\x1B\x1B""fn=Arial;ps=12\x1B""TURKEY BREAST, ""\x1B\x1B""fn=Arial;ps=8\x1B""WATER, LESS THAN  1.5 % OF SALT, ""\x1B\x1B""fn=Arial;ps=14\x1B""SUGAR""\x1B\x1B""fn=Arial;ps=7\x1B"", SODIUM PHOSPHATE,  NATURAL FLAVOR."); */
//	form->setData((UINT32*)"EEEEEEEEEEEEEEEE""\x1B\x1B""fn=Hobart;ps=10\x1B""EEEEEEEEE""\x0A""\x1B\x1B""fn=Hobart;ps=7\x1B""EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE""\x0A""EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
/*	form->setData((UINT32*)"Ingredients: Ingredient1 (Ingredient11, Ingredient 12, Ingredient13, "
			"and Ingredient14), Ingredient2, Ingredient3, Ingredient4 (Ingredient41, Ingredient42, "
			"Ingredient43, Ingredient44, and Ingredient45), Ingredient5 (Ingredient51 and Ingredient52), "
			"Ingredient6 (Ingredient61 and Ingredient62), Ingredient7, Ingredient8 (Ingredient81, "
			"Ingredient82, Ingredient83, Ingredient84, Ingredient85, Ingredient86, Ingredient87, "
			"Ingredient88, and Ingredient89), and Ingredient9."); */
/*	form->setData((UINT32*)"Ingredients Ingredient1 Ingredient11 Ingredient 12 Ingredient13 "
			"and Ingredient14 Ingredient2 Ingredient3 Ingredient4 Ingredient41 Ingredient42 "
			"Ingredient43 Ingredient44 and Ingredient45 Ingredient5 Ingredient51 and Ingredient52 "
			"Ingredient6 Ingredient61 and Ingredient62 Ingredient7 Ingredient8 Ingredient81 "
			"Ingredient82 Ingredient83 Ingredient84 Ingredient85 Ingredient86 Ingredient87 "
			"Ingredient88 and Ingredient89 and Ingredient9"); */
/*	form->setData((UINT32*)"Ingredients Ingredient1 Ingredient11 Ingredient 12 Ingredient13 "
			"and Ingredient14 Ingredient2 Ingredient3 Ingredient4 Ingredient41 Ingredient42 "
			"Ingredient43 Ingredient44 and Ingredient45 Ingredient5 Ingredient51 and Ingredient52 "); */
/*			"Ingredient6 Ingredient61 and Ingredient62 Ingredient7 Ingredient8 Ingredient81 "
			"Ingredient82 Ingredient83 Ingredient84 Ingredient85 Ingredient86 Ingredient87 "
			"Ingredient88 and Ingredient89 and Ingredient9"); */
/*	form->setFamilyName((const UINT8*)"Hobart");
	form->setPointSize(7);
	list.push_back(form); */
//0092025401210066011448000000000000006C6F6F6B20666F722070726566657272656400000000000000000000000000000000000000000000000000000000000000000000
/*	form = new FormRecord(146, 596, 289, 102,
			Fixed | PreImage | GraphicEntity | RectBorder, INDIRECT_GRAPHIC_REFERENCE, LeftJustify, 0, 0, 0);
	form->setData((UINT32*)"look for preferred");
	list.push_back(form); */


	// Create and return the label
	return new HobartLabel(header, lsd, list);
}


ImageControl* RendererManagerUnitTester::renderLabel(HobartLabel* label, bool preImage)
{
	LabelStockDescriptor* lsd = label->getLabelStockDescriptor();
	ImageControl* ic = new ImageControl(label->getFormsList(), lsd);

	BitMap* bitmap = new BitMap;
	INT16 pitch = (lsd->getWidthDots() + 15) / 16;
	bitmap->allocateBitMap(lsd->getLength() * pitch);
	bitmap->setBitMapBounds(0, lsd->getWidthDots(), 0, lsd->getLength());
	bitmap->setBitMapPitch(pitch);

	ic->setImageBitMapSize(lsd->getLength() * pitch * 2);
	ic->setHorizFormWidth(lsd->getWidthDots());
	ic->setImageBitMap(bitmap);
	ic->setPrintingRotatedBitmap(false);

	ic->setImageFlags((UINT16)(preImage ? PreImage : PostImage));
	ic->setImageLines(0);
	ic->setImageValid(false);
	ic->setImageOptions(0);

	Pen pen;
	pen.setPenColor(Black);
	pen.setPenState(Down);
	pen.setPenLocation(0,0);
	pen.setPenSize(1,1);
	ic->setPen(&pen);


	ic->initFormListIter();
	while ( !ic->isCurrentFormNull() )
		renderField(ic, ic->getFormFromList(), bitmap, 0);

	return ic;
}


int CALLBACK _EnumerateFont_(CONST ENUMLOGFONTEX* logicalFont, CONST NEWTEXTMETRICEX* physicalFont, DWORD fontType, LPARAM param)
{
	UNREFERENCED_PARAMETER(param);

	wchar_t debugString[255];
	static int counter = 0;
	//RendererManagerUnitTester* unitTester = (RendererManagerUnitTester*)param;

	swprintf(debugString, L"%d Enumerating new font\n", ++counter);
	OutputDebugStringW(debugString);

	if ( counter )
	{
		_DebugLogicalFont_(logicalFont->elfLogFont);

		swprintf(debugString, L"\tLogicalFont FullName = %s\n", logicalFont->elfFullName);
		OutputDebugStringW(debugString);

		swprintf(debugString, L"\tLogicalFont Style = %s\n", logicalFont->elfStyle);
		OutputDebugStringW(debugString);

		swprintf(debugString, L"\tLogicalFont Script = %s\n", logicalFont->elfScript);
		OutputDebugStringW(debugString);

		_DebugPhysicalFont_(*physicalFont);

		swprintf(debugString, L"\tFontType = %s\n", _DebugFontType_(fontType));
		OutputDebugStringW(debugString);
	}

	return 1;
}


void _DebugLogicalFont_(const LOGFONT& logicalFont)
{
	wchar_t debugString[255];

	swprintf(debugString, L"\t\tLogicalFont Height = %d logical units\n", logicalFont.lfHeight);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Width = %d logical units\n", logicalFont.lfWidth);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Escapement = %d.%d degrees\n", logicalFont.lfEscapement/10, logicalFont.lfEscapement%10);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Orientation = %d.%d degrees\n", logicalFont.lfOrientation/10, logicalFont.lfOrientation%10);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Weight = %s\n", _DebugLogicalFontWeight_(logicalFont.lfWeight));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Italic = %s\n", logicalFont.lfItalic ? L"TRUE" : L"FALSE");
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Underline = %s\n", logicalFont.lfUnderline ? L"TRUE" : L"FALSE");
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont StrikeOut = %s\n", logicalFont.lfStrikeOut ? L"TRUE" : L"FALSE");
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont CharSet = %s\n", _DebugLogicalFontCharSet_(logicalFont.lfCharSet));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont OutPrecision = %s\n", _DebugLogicalFontOutPrecision_(logicalFont.lfOutPrecision));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont ClipPrecision = %s\n", _DebugLogicalFontClipPrecision_(logicalFont.lfClipPrecision));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont Quality = %s\n", _DebugLogicalFontQuality_(logicalFont.lfQuality));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont PitchAndFamily = %s %s\n", _DebugLogicalFontFamily_(logicalFont.lfPitchAndFamily&0xfff0), _DebugLogicalFontPitch_(logicalFont.lfPitchAndFamily&0x000f));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tLogicalFont FaceName = %s\n", logicalFont.lfFaceName);
	OutputDebugStringW(debugString);
}


void _DebugPhysicalFont_(const NEWTEXTMETRICEX& physicalFont)
{
	wchar_t debugString[255];

	swprintf(debugString, L"\t\tPhysicalFont Height = %d units (ascent + descent)\n", physicalFont.ntmTm.tmHeight);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Ascent = %d units above baseline\n", physicalFont.ntmTm.tmAscent);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Descent = %d units below baseline\n", physicalFont.ntmTm.tmDescent);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont InternalLeading = %d units\n", physicalFont.ntmTm.tmInternalLeading);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont ExternalLeading = %d units\n", physicalFont.ntmTm.tmExternalLeading);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont AveCharWidth = %d units (letter x)\n", physicalFont.ntmTm.tmAveCharWidth);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont MaxCharWidth = %d units (widest character's width)\n", physicalFont.ntmTm.tmMaxCharWidth);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Weight = %s\n", _DebugLogicalFontWeight_(physicalFont.ntmTm.tmWeight));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Overhang = %d units (extra width)\n", physicalFont.ntmTm.tmOverhang);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont DigitizedAspectX = %d units\n", physicalFont.ntmTm.tmDigitizedAspectX);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont DigitizedAspectY = %d units\n", physicalFont.ntmTm.tmDigitizedAspectY);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont FirstChar = %#.4x\n", physicalFont.ntmTm.tmFirstChar);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont LastChar = %#.4x\n", physicalFont.ntmTm.tmLastChar);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont DefaultChar = %#.4x\n", physicalFont.ntmTm.tmDefaultChar);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont BreakChar = %#.4x\n", physicalFont.ntmTm.tmBreakChar);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Italic = %s\n", physicalFont.ntmTm.tmItalic ? L"TRUE" : L"FALSE");
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Underlined = %s\n", physicalFont.ntmTm.tmUnderlined ? L"TRUE" : L"FALSE");
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont StruckOut = %s\n", physicalFont.ntmTm.tmStruckOut ? L"TRUE" : L"FALSE");
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont PitchAndFamily = %s %s\n", _DebugLogicalFontFamily_(physicalFont.ntmTm.tmPitchAndFamily&0xfff0), _DebugPhysicalFontPitch_(physicalFont.ntmTm.tmPitchAndFamily&0x000f));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont CharSet = %s\n", _DebugLogicalFontCharSet_(physicalFont.ntmTm.tmCharSet));
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont Flags = %s\n", _DebugPhysicalFontFlags_(physicalFont.ntmTm.ntmFlags).c_str());
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont SizeEM = %d notational units\n", physicalFont.ntmTm.ntmSizeEM);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont CellHeight = %d notational units\n", physicalFont.ntmTm.ntmCellHeight);
	OutputDebugStringW(debugString);

	swprintf(debugString, L"\t\tPhysicalFont AvgWidth = %d notational units\n", physicalFont.ntmTm.ntmAvgWidth);
	OutputDebugStringW(debugString);
}


const wchar_t* _DebugLogicalFontWeight_(int weight)
{
	switch ( weight )
	{
		case FW_DONTCARE:   return L"FW_DONTCARE";
		case FW_THIN:       return L"FW_THIN";
		case FW_EXTRALIGHT: return L"FW_EXTRALIGHT";
		case FW_LIGHT:      return L"FW_LIGHT";
		case FW_NORMAL:     return L"FW_NORMAL";
		case FW_MEDIUM:     return L"FW_MEDIUM";
		case FW_SEMIBOLD:   return L"FW_SEMIBOLD";
		case FW_BOLD:       return L"FW_BOLD";
		case FW_EXTRABOLD:  return L"FW_EXTRABOLD";
		case FW_HEAVY:      return L"FW_HEAVY";
	}

	return L"FW_UNKNOWN";
}


const wchar_t* _DebugLogicalFontCharSet_(int charset)
{
	switch ( charset )
	{
		case ANSI_CHARSET:         return L"ANSI_CHARSET";
		case DEFAULT_CHARSET:      return L"DEFAULT_CHARSET";
		case SYMBOL_CHARSET:       return L"SYMBOL_CHARSET";
		case MAC_CHARSET:          return L"MAC_CHARSET";
		case SHIFTJIS_CHARSET:     return L"SHIFTJIS_CHARSET";
		case HANGUL_CHARSET:       return L"HANGUL_CHARSET";
		case JOHAB_CHARSET:        return L"JOHAB_CHARSET";
		case GB2312_CHARSET:       return L"GB2312_CHARSET";
		case CHINESEBIG5_CHARSET:  return L"CHINESEBIG5_CHARSET";
		case GREEK_CHARSET:        return L"GREEK_CHARSET";
		case TURKISH_CHARSET:      return L"TURKISH_CHARSET";
		case VIETNAMESE_CHARSET:   return L"VIETNAMESE_CHARSET";
		case HEBREW_CHARSET:       return L"HEBREW_CHARSET";
		case ARABIC_CHARSET:       return L"ARABIC_CHARSET";
		case BALTIC_CHARSET:       return L"BALTIC_CHARSET";
		case RUSSIAN_CHARSET:      return L"RUSSIAN_CHARSET";
		case THAI_CHARSET:         return L"THAI_CHARSET";
		case EASTEUROPE_CHARSET:   return L"EASTEUROPE_CHARSET";
		case OEM_CHARSET:          return L"OEM_CHARSET";
	}

	return L"UNKNOWN_CHARSET";
}


const wchar_t* _DebugLogicalFontOutPrecision_(int outPrecision)
{
	switch ( outPrecision )
	{
		case OUT_DEFAULT_PRECIS:          return L"OUT_DEFAULT_PRECIS";
		case OUT_STRING_PRECIS:           return L"OUT_STRING_PRECIS";
		case OUT_CHARACTER_PRECIS:        return L"OUT_CHARACTER_PRECIS";
		case OUT_STROKE_PRECIS:           return L"OUT_STROKE_PRECIS";
		case OUT_TT_PRECIS:               return L"OUT_TT_PRECIS";
		case OUT_DEVICE_PRECIS:           return L"OUT_DEVICE_PRECIS";
		case OUT_RASTER_PRECIS:           return L"OUT_RASTER_PRECIS";
		case OUT_TT_ONLY_PRECIS:          return L"OUT_TT_ONLY_PRECIS";
		case OUT_OUTLINE_PRECIS:          return L"OUT_OUTLINE_PRECIS";
		case OUT_SCREEN_OUTLINE_PRECIS:   return L"OUT_SCREEN_OUTLINE_PRECIS";
		case OUT_PS_ONLY_PRECIS:          return L"OUT_PS_ONLY_PRECIS";
	}

	return L"OUT_UNKNOWN";
}


const wchar_t* _DebugLogicalFontClipPrecision_(int clipPrecision)
{
	switch ( clipPrecision )
	{
		case CLIP_DEFAULT_PRECIS:      return L"CLIP_DEFAULT_PRECIS";
		case CLIP_CHARACTER_PRECIS:    return L"CLIP_CHARACTER_PRECIS";
		case CLIP_STROKE_PRECIS:       return L"CLIP_STROKE_PRECIS";
		case CLIP_MASK:                return L"CLIP_MASK";
		case CLIP_LH_ANGLES:           return L"CLIP_LH_ANGLES";
		case CLIP_TT_ALWAYS:           return L"CLIP_TT_ALWAYS";
		case CLIP_EMBEDDED:            return L"CLIP_EMBEDDED";
	}

	return L"CLIP_UNKNOWN";
}


const wchar_t* _DebugLogicalFontQuality_(int quality)
{
	switch ( quality )
	{
		case DEFAULT_QUALITY:           return L"DEFAULT_QUALITY";
		case DRAFT_QUALITY:             return L"DRAFT_QUALITY";
		case PROOF_QUALITY:             return L"PROOF_QUALITY";
		case NONANTIALIASED_QUALITY:    return L"NONANTIALIASED_QUALITY";
		case ANTIALIASED_QUALITY:       return L"ANTIALIASED_QUALITY";
	}

	return L"UNKNOWN_QUALITY";
}


const wchar_t* _DebugLogicalFontPitch_(int pitch)
{
	switch ( pitch )
	{
		case DEFAULT_PITCH:        return L"DEFAULT_PITCH";
		case FIXED_PITCH:          return L"FIXED_PITCH";
		case VARIABLE_PITCH:       return L"VARIABLE_PITCH";
		case MONO_FONT:            return L"MONO_FONT";
	}

	return L"UNKNOWN_PITCH";
}


const wchar_t* _DebugLogicalFontFamily_(int family)
{
	switch ( family )
	{
		case FF_DONTCARE:        return L"FF_DONTCARE";
		case FF_ROMAN:           return L"FF_ROMAN";
		case FF_SWISS:           return L"FF_SWISS";
		case FF_MODERN:          return L"FF_MODERN";
		case FF_SCRIPT:          return L"FF_SCRIPT";
		case FF_DECORATIVE:      return L"FF_DECORATIVE";
	}

	return L"FF_UNKNOWN";
}


const wchar_t* _DebugPhysicalFontPitch_(int pitch)
{
	switch ( pitch )
	{
		case  0 /* 0000 */:        return L"RASTER MONOSPACED";
		case  1 /* 0001 */:        return L"RASTER PROPORTIONAL";
		case  2 /* 0010 */:        return L"VECTOR MONOSPACED";
		case  3 /* 0011 */:        return L"VECTOR PROPORTIONAL";
		case  4 /* 0100 */:        return L"UNKNOWN MONOSPACED";
		case  5 /* 0101 */:        return L"UNKNOWN PROPORTIONAL";
		case  6 /* 0110 */:        return L"UNKNOWN MONOSPACED";
		case  7 /* 0111 */:        return L"UNKNOWN PROPORTIONAL";
		case  8 /* 1000 */:        return L"DEVICE RASTER MONOSPACED";
		case  9 /* 1001 */:        return L"DEVICE RASTER PROPORTIONAL";
		case 10 /* 1010 */:        return L"DEVICE VECTOR MONOSPACED";
		case 11 /* 1011 */:        return L"DEVICE VECTOR PROPORTIONAL";
		case 12 /* 1100 */:        return L"DEVICE UNKNOWN MONOSPACED";
		case 13 /* 1101 */:        return L"DEVICE UNKNOWN PROPORTIONAL";
		case 14 /* 1110 */:        return L"DEVICE UNKNOWN MONOSPACED";
		case 15 /* 1111 */:        return L"DEVICE UNKNOWN PROPORTIONAL";
	}

	return L"UNKNOWN_PITCH";
}


std::wstring _DebugPhysicalFontFlags_(unsigned int flags)
{
	std::wstring debugString;

	if ( flags & NTM_ITALIC )
		debugString.append(L"NTM_ITALIC | ");

	if ( flags & NTM_BOLD )
		debugString.append(L"NTM_BOLD | ");

	if ( flags & NTM_REGULAR )
		debugString.append(L"NTM_REGULAR | ");

	if ( flags & NTM_NONNEGATIVE_AC )
		debugString.append(L"NTM_NONNEGATIVE_AC | ");

	if ( flags & NTM_PS_OPENTYPE )
		debugString.append(L"NTM_PS_OPENTYPE | ");

	if ( flags & NTM_TT_OPENTYPE )
		debugString.append(L"NTM_TT_OPENTYPE | ");

	if ( flags & NTM_MULTIPLEMASTER )
		debugString.append(L"NTM_MULTIPLEMASTER | ");

	if ( flags & NTM_TYPE1 )
		debugString.append(L"NTM_TYPE1 | ");

	if ( flags & NTM_DSIG )
		debugString.append(L"NTM_DSIG | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);

	return debugString;
}


const wchar_t* _DebugFontType_(unsigned int fontType)
{
	switch ( fontType )
	{
		case RASTER_FONTTYPE:        return L"RASTER_FONTTYPE";
		case DEVICE_FONTTYPE:        return L"DEVICE_FONTTYPE";
		case TRUETYPE_FONTTYPE:      return L"TRUETYPE_FONTTYPE";
	}

	return L"UNKNOWN_FONTTYPE";
}


void _DebugDeviceContext_(HDC hdc)
{
	int result;
	wchar_t debugString[255];

	swprintf(debugString, L"\n\n");
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, DRIVERVERSION);
	swprintf(debugString, L"\tDRIVERVERSION = %d\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, TECHNOLOGY);
	swprintf(debugString, L"\tTECHNOLOGY = %s\n", _DebugDeviceCapTechnology_(result));
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, HORZSIZE);
	swprintf(debugString, L"\tHORZSIZE = %d millimiters (physical screen)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, VERTSIZE);
	swprintf(debugString, L"\tVERTSIZE = %d millimiters (physical screen)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, HORZRES);
	swprintf(debugString, L"\tHORZRES = %d pixels (physical screen)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, VERTRES);
	swprintf(debugString, L"\tVERTRES = %d pixels (physical screen)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, LOGPIXELSX);
	swprintf(debugString, L"\tLOGPIXELSX = %d pixels per logical inch horizontally\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, LOGPIXELSY);
	swprintf(debugString, L"\tLOGPIXELSY = %d pixels per logical inch vertically\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, BITSPIXEL);
	swprintf(debugString, L"\tBITSPIXEL = %d color bits per pixel\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, PLANES);
	swprintf(debugString, L"\tPLANES = %d color planes\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, NUMBRUSHES);
	swprintf(debugString, L"\tNUMBRUSHES = %d device-specific brushes\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, NUMPENS);
	swprintf(debugString, L"\tNUMPENS = %d device-specific pens\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, NUMMARKERS);
	swprintf(debugString, L"\tNUMMARKERS = %d device-specific markers\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, NUMFONTS);
	swprintf(debugString, L"\tNUMFONTS = %d device-specific fonts\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, NUMCOLORS);
	swprintf(debugString, L"\tNUMCOLORS = %d entries in color table (if BITSPIXEL <= 8)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, ASPECTX);
	swprintf(debugString, L"\tASPECTX = %d relative width of a device pixel\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, ASPECTY);
	swprintf(debugString, L"\tASPECTY = %d relative height of a device pixel\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, ASPECTXY);
	swprintf(debugString, L"\tASPECTXY = %d diagonal width of a device pixel\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, PDEVICESIZE);
	swprintf(debugString, L"\tPDEVICESIZE = %d bytes required for device descriptor (reserved)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, SIZEPALETTE);
	swprintf(debugString, L"\tSIZEPALETTE = %d entries in system palette\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, NUMRESERVED);
	swprintf(debugString, L"\tNUMRESERVED = %d reserved entries in system palette\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, COLORRES);
	swprintf(debugString, L"\tCOLORRES = %d bpp (actual color resolution)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, PHYSICALWIDTH);
	swprintf(debugString, L"\tPHYSICALWIDTH = %d units width in physical page (printers)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, PHYSICALHEIGHT);
	swprintf(debugString, L"\tPHYSICALHEIGHT = %d units height in physical page (printers)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, PHYSICALOFFSETX);
	swprintf(debugString, L"\tPHYSICALOFFSETX = %d horizontal units until printable area (printer)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, PHYSICALOFFSETY);
	swprintf(debugString, L"\tPHYSICALOFFSETY = %d vertical units until printable area (printer)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, SCALINGFACTORX);
	swprintf(debugString, L"\tSCALINGFACTORX = %d scale factor for x-axis (printer)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, SCALINGFACTORY);
	swprintf(debugString, L"\tSCALINGFACTORY = %d scale factor for y-axis (printer)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, VREFRESH);
	swprintf(debugString, L"\tVREFRESH = %d Hz vertical refresh rate (display)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, BLTALIGNMENT);
	swprintf(debugString, L"\tBLTALIGNMENT = %d pixels preferred bit alignment (display)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, DESKTOPVERTRES);
	swprintf(debugString, L"\tDESKTOPVERTRES = %d pixels desktop vertical height (display)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, DESKTOPHORZRES);
	swprintf(debugString, L"\tDESKTOPHORZRES = %d pixels desktop horizontal width (display)\n", result);
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, RASTERCAPS);
	swprintf(debugString, L"\tRASTERCAPS = %s\n", _DebugDeviceRasterCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, CURVECAPS);
	swprintf(debugString, L"\tCURVECAPS = %s\n", _DebugDeviceCurveCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, LINECAPS);
	swprintf(debugString, L"\tLINECAPS = %s\n", _DebugDeviceLineCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, POLYGONALCAPS);
	swprintf(debugString, L"\tPOLYGONALCAPS = %s\n", _DebugDevicePolygonalCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, TEXTCAPS);
	swprintf(debugString, L"\tTEXTCAPS = %s\n", _DebugDeviceTextCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, CLIPCAPS);
	swprintf(debugString, L"\tCLIPCAPS = %s\n", _DebugDeviceClipCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, SHADEBLENDCAPS);
	swprintf(debugString, L"\tSHADEBLENDCAPS = %s\n", _DebugDeviceShadeBlendCapabilities_(result).c_str());
	OutputDebugStringW(debugString);

	result = GetDeviceCaps(hdc, COLORMGMTCAPS);
	swprintf(debugString, L"\tCOLORMGMTCAPS = %s\n", _DebugDeviceColorManagementCapabilities_(result).c_str());
	OutputDebugStringW(debugString);
}


const wchar_t* _DebugDeviceCapTechnology_(int tech)
{
	switch ( tech )
	{
		case DT_PLOTTER:         return L"DT_PLOTTER";
		case DT_RASDISPLAY:      return L"DT_RASDISPLAY";
		case DT_RASPRINTER:      return L"DT_RASPRINTER";
		case DT_RASCAMERA:       return L"DT_RASCAMERA";
		case DT_CHARSTREAM:      return L"DT_CHARSTREAM";
		case DT_METAFILE:        return L"DT_METAFILE";
		case DT_DISPFILE:        return L"DT_DISPFILE";
	}

	return L"DT_UNKNOWN";
}


std::wstring _DebugDeviceRasterCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & RC_BITBLT )
		debugString.append(L"RC_BITBLT | ");

	if ( flags & RC_BANDING )
		debugString.append(L"RC_BANDING | ");

	if ( flags & RC_SCALING )
		debugString.append(L"RC_SCALING | ");

	if ( flags & RC_BITMAP64 )
		debugString.append(L"RC_BITMAP64 | ");

	if ( flags & RC_GDI20_OUTPUT )
		debugString.append(L"RC_GDI20_OUTPUT | ");

	if ( flags & RC_GDI20_STATE )
		debugString.append(L"RC_GDI20_STATE | ");

	if ( flags & RC_SAVEBITMAP )
		debugString.append(L"RC_SAVEBITMAP | ");

	if ( flags & RC_DI_BITMAP )
		debugString.append(L"RC_DI_BITMAP | ");

	if ( flags & RC_PALETTE )
		debugString.append(L"RC_PALETTE | ");

	if ( flags & RC_DIBTODEV )
		debugString.append(L"RC_DIBTODEV | ");

	if ( flags & RC_BIGFONT )
		debugString.append(L"RC_BIGFONT | ");

	if ( flags & RC_STRETCHBLT )
		debugString.append(L"RC_STRETCHBLT | ");

	if ( flags & RC_FLOODFILL )
		debugString.append(L"RC_FLOODFILL | ");

	if ( flags & RC_STRETCHDIB )
		debugString.append(L"RC_STRETCHDIB | ");

	if ( flags & RC_OP_DX_OUTPUT )
		debugString.append(L"RC_OP_DX_OUTPUT | ");

	if ( flags & RC_DEVBITS )
		debugString.append(L"RC_DEVBITS | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"RC_NONE");

	return debugString;
}


std::wstring _DebugDeviceCurveCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & CC_CIRCLES )
		debugString.append(L"CC_CIRCLES | ");

	if ( flags & CC_PIE )
		debugString.append(L"CC_PIE | ");

	if ( flags & CC_CHORD )
		debugString.append(L"CC_CHORD | ");

	if ( flags & CC_ELLIPSES )
		debugString.append(L"CC_ELLIPSES | ");

	if ( flags & CC_WIDE )
		debugString.append(L"CC_WIDE | ");

	if ( flags & CC_STYLED )
		debugString.append(L"CC_STYLED | ");

	if ( flags & CC_WIDESTYLED )
		debugString.append(L"CC_WIDESTYLED | ");

	if ( flags & CC_INTERIORS )
		debugString.append(L"CC_INTERIORS | ");

	if ( flags & CC_ROUNDRECT )
		debugString.append(L"CC_ROUNDRECT | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"CC_NONE");

	return debugString;
}


std::wstring _DebugDeviceLineCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & LC_POLYLINE )
		debugString.append(L"LC_POLYLINE | ");

	if ( flags & LC_MARKER )
		debugString.append(L"LC_MARKER | ");

	if ( flags & LC_POLYMARKER )
		debugString.append(L"LC_POLYMARKER | ");

	if ( flags & LC_WIDE )
		debugString.append(L"LC_WIDE | ");

	if ( flags & LC_STYLED )
		debugString.append(L"LC_STYLED | ");

	if ( flags & LC_WIDESTYLED )
		debugString.append(L"LC_WIDESTYLED | ");

	if ( flags & LC_INTERIORS )
		debugString.append(L"LC_INTERIORS | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"LC_NONE");

	return debugString;
}


std::wstring _DebugDevicePolygonalCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & PC_POLYGON )
		debugString.append(L"PC_POLYGON | ");

	if ( flags & PC_RECTANGLE )
		debugString.append(L"PC_RECTANGLE | ");

	if ( flags & PC_WINDPOLYGON )
		debugString.append(L"PC_WINDPOLYGON | ");

	if ( flags & PC_TRAPEZOID )
		debugString.append(L"PC_TRAPEZOID | ");

	if ( flags & PC_SCANLINE )
		debugString.append(L"PC_SCANLINE | ");

	if ( flags & PC_WIDE )
		debugString.append(L"PC_WIDE | ");

	if ( flags & PC_STYLED )
		debugString.append(L"PC_STYLED | ");

	if ( flags & PC_WIDESTYLED )
		debugString.append(L"PC_WIDESTYLED | ");

	if ( flags & PC_INTERIORS )
		debugString.append(L"PC_INTERIORS | ");

	if ( flags & PC_POLYPOLYGON )
		debugString.append(L"PC_POLYPOLYGON | ");

	if ( flags & PC_PATHS )
		debugString.append(L"PC_PATHS | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"PC_NONE");

	return debugString;
}


std::wstring _DebugDeviceTextCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & TC_OP_CHARACTER )
		debugString.append(L"TC_OP_CHARACTER | ");

	if ( flags & TC_OP_STROKE )
		debugString.append(L"TC_OP_STROKE | ");

	if ( flags & TC_CP_STROKE )
		debugString.append(L"TC_CP_STROKE | ");

	if ( flags & TC_CR_90 )
		debugString.append(L"TC_CR_90 | ");

	if ( flags & TC_CR_ANY )
		debugString.append(L"TC_CR_ANY | ");

	if ( flags & TC_SF_X_YINDEP )
		debugString.append(L"TC_SF_X_YINDEP | ");

	if ( flags & TC_SA_DOUBLE )
		debugString.append(L"TC_SA_DOUBLE | ");

	if ( flags & TC_SA_INTEGER )
		debugString.append(L"TC_SA_INTEGER | ");

	if ( flags & TC_SA_CONTIN )
		debugString.append(L"TC_SA_CONTIN | ");

	if ( flags & TC_EA_DOUBLE )
		debugString.append(L"TC_EA_DOUBLE | ");

	if ( flags & TC_IA_ABLE )
		debugString.append(L"TC_IA_ABLE | ");

	if ( flags & TC_UA_ABLE )
		debugString.append(L"TC_UA_ABLE | ");

	if ( flags & TC_SO_ABLE )
		debugString.append(L"TC_SO_ABLE | ");

	if ( flags & TC_RA_ABLE )
		debugString.append(L"TC_RA_ABLE | ");

	if ( flags & TC_VA_ABLE )
		debugString.append(L"TC_VA_ABLE | ");

	if ( flags & TC_RESERVED )
		debugString.append(L"TC_RESERVED | ");

	if ( flags & TC_SCROLLBLT )
		debugString.append(L"TC_SCROLLBLT | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"TC_NONE");

	return debugString;
}


std::wstring _DebugDeviceClipCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & CP_RECTANGLE )
		debugString.append(L"CP_RECTANGLE | ");

	if ( flags & CP_REGION )
		debugString.append(L"CP_REGION | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"CP_NONE");

	return debugString;
}


std::wstring _DebugDeviceShadeBlendCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & SB_CONST_ALPHA )
		debugString.append(L"SB_CONST_ALPHA | ");

	if ( flags & SB_PIXEL_ALPHA )
		debugString.append(L"SB_PIXEL_ALPHA | ");

	if ( flags & SB_PREMULT_ALPHA )
		debugString.append(L"SB_PREMULT_ALPHA | ");

	if ( flags & SB_GRAD_RECT )
		debugString.append(L"SB_GRAD_RECT | ");

	if ( flags & SB_GRAD_TRI )
		debugString.append(L"SB_GRAD_TRI | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"SB_NONE");

	return debugString;
}


std::wstring _DebugDeviceColorManagementCapabilities_(int flags)
{
	std::wstring debugString;

	if ( flags & CM_DEVICE_ICM )
		debugString.append(L"CM_DEVICE_ICM | ");

	if ( flags & CM_GAMMA_RAMP )
		debugString.append(L"CM_GAMMA_RAMP | ");

	if ( flags & CM_CMYK_COLOR )
		debugString.append(L"CM_CMYK_COLOR | ");


	if ( debugString.length() )
		debugString.erase(debugString.length() - 3);
	else
		debugString.append(L"CM_NONE");

	return debugString;
}
