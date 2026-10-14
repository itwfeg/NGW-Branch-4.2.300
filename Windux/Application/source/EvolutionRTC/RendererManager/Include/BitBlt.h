#ifndef BITBLT_H
#define BITBLT_H


#include "UltimaObjects.h"

//This is need here so that the Tornado compiler understands that these are C functions and not methods to
//a class. This will prevent name mangling.
#ifdef __cplusplus
extern "C" {
#endif

void PatternOP(BMWORD *pattern, UltimaBitMap *destBitmap, UltimaRectangle *destRect);

/*******    Patterns    *******/
/*
extern PATTERN Grey;
extern PATTERN LtGrey;
extern PATTERN VLtGrey;
extern PATTERN DkGrey;
extern PATTERN VDkGrey;
*/
#ifdef __cplusplus
}
#endif



void CopyOP(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap, UltimaRectangle *destRect);
void InvertOP(UltimaBitMap *bitMap, UltimaRectangle *rect);
void RectSetOP(UltimaBitMap *bitMap, UltimaRectangle *rect, UINT16 value);
void CopyXOP(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap, UltimaRectangle *destRect);
void Rotate90 (UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap, UltimaRectangle *destRect);
void Rotate180(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap, UltimaRectangle *destRect);
void Rotate270(UltimaBitMap *srcBitmap, UltimaRectangle *srcRect, UltimaBitMap *destBitmap, UltimaRectangle *destRect);



UINT16 inline swapWordBytes(UINT16 src)
{
	return ((src & 0x00FF)<<0x08) | ((src & 0xFF00)>>0x08);
}


UINT16 inline transposeBits(UINT16 srcWord, int srcFirst, int srcRange, int destLast)
{
	UINT16 destWord = 0;

	for ( int srcBit = 0x0001 << srcFirst, destBit = 0x0001 << destLast;
			srcBit & srcRange; srcBit <<= 1, destBit >>= 1 )
	{
		if ( srcWord & srcBit )
			destWord |= destBit;
	}

	return destWord;
}


UINT16 inline transposeWord(UINT16 srcWord)
{
	UINT16 destWord = 0;

	for ( int srcBit = 0x0001, destBit = 0x8000; srcBit & 0xFFFF; srcBit <<= 1, destBit >>= 1 )
	{
		if ( srcWord & srcBit )
			destWord |= destBit;
	}

	return destWord;
}



//#define swapWordBytes(a)  ((((a) & 0x00FF)<<0x08) | (((a) & 0xFF00)>>0x08))



#endif	//BITBLT_H
