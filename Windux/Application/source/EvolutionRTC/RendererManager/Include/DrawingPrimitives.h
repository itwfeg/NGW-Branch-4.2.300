#ifndef DRAWINGPRIMITIVES_H
#define DRAWINGPRIMITIVES_H



#include "Barcode.h"          // for ModuleType


#include "CSmartFontHandle.h"
#include "UltimaObjects.h"

class WordWrapLineMetrics;
class GenericBarcode;

#define WIDTH 640
#define HEIGHT 480


//
//manually calculates the space required for white space
//temp solution because of problems with retrieving 
//space char size from the fonts.
//
#define WHITE_SPACE_ADVANCE(max_adv)                            ((((max_adv)>>6)/3)-1)


typedef enum
{
	GetWord,		// initial state
	EndOfLine,		// hard carriage return was encountered while building the word
	FullWord,		// add current word to the current line
	FullWordTabbed, // like previous but the word ended in a tab
	FontChange, 
	FontChangeInWord,
	Quit
} WordStatus;


typedef enum 
{
    InvertFrame,
    SetFrame,
    ClearFrame
} FrameType;


#define CR		'\n'	//hard carriage return
#define ESC 	0x1b	//font change sequence
#define Space 	' '


//Function Prototypes
void drawPoint(Point *point, BitMap *bitmap, Pen *pen);
void drawVerticalLine(Point *start, Point *end, BitMap *bitmap, Pen *pen);
void drawHorizontalLine(Point *start, Point *end, BitMap *bitmap, Pen *pen);
void drawLine(Point *startPoint, Point *endPoint, BitMap *bitmap, Pen *pen);
void drawFrame(BitMap *bitMap, Hobart::Rectangle *rectangle, FrameType type, Pen *pen);


#if defined(PAL_WIN32_RUN)
void drawSymbols(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, HDC hdc);
#else
void drawSymbols(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, Util::CSmartFontHandle fndl, BitMap &txt_bmp);

/// @name Rendering
/// Text rendering.
//@{
/// Calculates the string width and height.  Routine works with horizontally
/// oriented fonts.
bool GetTextExtentPoint32A(
	/// [in] Valid handle to the library.
	Util::CSmartFontHandle fhndl, 
	/// [in] String whoes extent is needed.
	char* str, 
	/// [in] Lenght of the string.
	int32_t len, 
	/// [out] Size structure returned with width and height
	/// of the string.
	PAL_SIZE *sz);
//@}
#endif

void drawSymbol(BitMap *bitmap, GenericBarcode *barcode, UINT8 symbolCode);
void drawCenter(BitMap *bitmap, GenericBarcode *barcode);
void drawModule(BitMap *bitmap, GenericBarcode *barcode, ModuleType type, INT16 width, INT16 height);
void drawMargin(BitMap *bitmap, GenericBarcode *barcode, INT16 rotation);
void drawGuard(BitMap *bitmap, GenericBarcode *barcode);

/* ALV - 10/2008 */
UINT16 computeEAN128CChecksum(UINT8 *data, UINT16 count, std::list<size_t> &barcode);
void drawEAN128CSymbol(BitMap *bitmap, GenericBarcode *barcode, UINT16 symbolCode);
void drawEAN128CSymbol270(BitMap *bitmap, GenericBarcode *barcode, UINT16 symbolCode);
#ifdef PAL_WIN32_RUN
void drawEAN128CSymbols   (BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, HDC hdc);
void drawEAN128CSymbols270(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, HDC hdc);
#else
void drawEAN128CSymbols   (BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, Util::CSmartFontHandle fndl);
void drawEAN128CSymbols270(BitMap *bitmap, GenericBarcode *barcode, UINT8 *data, INT16 count, INT16 start, Util::CSmartFontHandle fndl);
#endif

void fillRectangle(BitMap *bitMap, Hobart::Rectangle *rectangle, INT16 color);
void eraseRectangle(BitMap *pDestBitMap, Hobart::Rectangle *imageRect, UINT16 fillWord);
void rotateImage(BitMap *sourceBitMap, Hobart::Rectangle *sourceRectangle, BitMap *destinationBitMap, Hobart::Rectangle *destinationRectangle, INT16 angle);

INT16 drawWordWrapLine(INT16 xMax, INT16 yMax, WordWrapLineMetrics *wordWrapLineMetrics, bool subtractDescent);
WordStatus buildWord(unsigned char *sourceBuffer, UINT16 *count, unsigned char *newFont);
UINT16 skipFontChangeSequence(const char *pChar);

#if defined(PAL_WIN32_RUN)
Util::CSmartFontHandle generateFontId(HDC hdc, const char * fontString);
#else
Util::CSmartFontHandle generateFontId(const char * fontString);
int32_t TextOut(Util::CSmartFontHandle fhndl, int32_t str_x, int32_t str_y, const char *data, int32_t count, BitMap &txt_bmp);
#endif

void domRectToUltima(Hobart::Rectangle *domRect, UltimaRectangle &ultimaRect);
void domBitMapToUltima(BitMap *domBitMap, UltimaBitMap &ultBitMap);
void domPenToUltima(Pen *domPen, UltimaPen &ultimaPen);

#if defined(PAL_WIN32_RUN)
void GetAndFixTextMetrics(HDC hdc, TEXTMETRIC* tm);
#else
///Routine accepts CSmartFontHandle handle instead of HDC handle 
///as in windows
void GetAndFixTextMetrics(Util::CSmartFontHandle &fhndl, TEXTMETRIC* tm);
#endif


#endif //DRAWINGPRIMITIVES_H
