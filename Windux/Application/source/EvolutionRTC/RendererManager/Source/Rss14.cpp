/*
 * Rss14.cpp
 *
 *  Created on: Jul 8, 2013
 *      Author: aaron swift
 */
#include "Rss14.h"
#include "DrawingPrimitives.h"
#include "Debug.h"
#include "Integer.h"
#include <algorithm>

#include "ErrorHandling.h"
#include "CRendererUtil.h"

const int Rss14::CCSizes[] =
{	78,108,138,167,197, 				/* cca sizes */
	208,264,352,496,672,840,1016,1184, 	/* ccb sizes	*/
	0 };

/* element 1 weighting for characters N determined by adjacent finder */
const int Rss14::parWts[24] =
{ 0,1,20,189,193,62,185,113,150,46,76,43,16,109,
 70,134,148,6,120,79,103,161,55,45 };

/* odd elements N & max, even N & max, odd mul, combos:	 */
const int Rss14::tbl174[30] =
{
	12,7,	5,2,	4,	348,
	10,5,	7,4,	20,	1040,
	8,4,	9,5,	52,	1560,
	6,3,	11,6,	104,1040,
	4,1,	13,8,	204,204 };

const unsigned char Rss14::iswhat[256] = 		 			/* byte look up table with IS_XXX bits */
{
	0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	 					/* 32 control characters: */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	8,8,8,0xf,0,8,8,8,8,8,0xc,8,0xc,0xc,0xc,0xc,   			/* 32 punctuation and numeric characters: */
	0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,0xd,
	8,8,8,8,8,8,
	0,					  									/* 32 upper case and punctuation characters: */
	0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,
	0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,
	0,0,0,0xc,8,
	0,											   			/* 32 lower case and punctuation characters: */
	8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,
	0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/* extended ASCII */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

Rss14::Rss14( BitMap *bitmap, Hobart::Rectangle *bounds, unsigned char *data, INT16 length, INT16 charHeight, INT16 leading, INT16 ascent, INT16 faceMax, Util::CSmartFontHandle fontId, int modsize, int rotation, BitMap& txt_bmp, bool isGS1DataBarExpanded ) :
		isGS1DataBarExpanded_(isGS1DataBarExpanded)
{
	Debug::trace("Renderer.RSS14", "Rss14 entered ");
	GenericBarcode barcode;

	int i, j;
	int SYM_H = 34;
	unsigned short margin_offset, field_width, barcode_width, startPosition;
	int segs, lNdx, lNdx1, lMods;
	int evenRow, rev;
	int sepCnt;
	int chexSize;
	int rPadl, rPadcc;
	unsigned char sav1, sav2, sav3, sav4;

	INT16 height_temp = bounds->getRectangleBottom();


    /* The Barcode data is now stored in the static label form in human
       readable format, so strip parens before drawing barcode. */
	i = j = 0;
	while (data[i] != '\0')
	{
		if ( (data[i] == '(') || (data[i] == ')') )
		{
			i++;
			continue;
		}
		dataBuffer[j++] = data[i++];
	}
	dataBuffer[j] = '\0';

    Debug::trace("Renderer.RSS14", "set pens to black and down");
    barcode.setPens();

    Debug::trace("Renderer.RSS14", "calculate the symbol height");
    if (rotation == 0)
    {
    	barcode.setSymbolHeight( (bounds->getRectangleBottom() - bounds->getRectangleTop() - charHeight - leading - 1) );
    	Debug::trace("Renderer.RSS14", "Symbol Height: " + Integer::toString(barcode.getSymbolHeight()));
    }
    else if (rotation == 270)
    {
    	barcode.setSymbolHeight( (bounds->getRectangleRight() - bounds->getRectangleLeft() - charHeight - leading - 1) );
    }

    barcode.setGuardHeight( (barcode.getSymbolHeight() + (charHeight / 2)) );
    Debug::trace("Renderer.RSS14", "Guard Height: " + Integer::toString(barcode.getGuardHeight()));

    barcode.setCharOffset( (((7 * modsize) - faceMax) / 2) );
    Debug::trace("Renderer.RSS14", "Character Offset:" + Integer::toString(barcode.getCharOffset()));

    if (barcode.getCharOffset() < 0) barcode.setCharOffset( barcode.getCharOffset() * -1 );
    if (modsize == 0)
    {
        if (rotation == 0)
            modsize = 1;
        else if (rotation == 270)
            modsize = 1;
    }

    Debug::trace("Renderer.RSS14", "set the module size");
    barcode.setModuleSize( modsize );

#ifdef EAN_USE_MARGIN
  #ifdef EAN_USE_HALF_LEFT_MARGIN
    margin_offset = ((modsize*LEFT_MARG_MODS_UPC)/2);
  #else
    margin_offset = (modsize*LEFT_MARG_MODS_UPC);
  #endif
#else
    margin_offset = 10;
#endif

    Debug::trace("Renderer.RSS14", "calculate the field width");
    if (rotation == 270)
	{
        field_width = ( bounds->getRectangleBottom() - bounds->getRectangleTop() );
	}
    else
	{
        field_width = ( bounds->getRectangleRight() - bounds->getRectangleLeft() );
	}

    Debug::trace("Renderer.RSS14", "field_width: " + Integer::toString(field_width));
    Debug::trace("Renderer.RSS14", "field_height: " + Integer::toString( (bounds->getRectangleBottom() - bounds->getRectangleTop()) ));
    Debug::trace("Renderer.RSS14", "field_top: " + Integer::toString( (bounds->getRectangleTop()) ));
    Debug::trace("Renderer.RSS14", "field_bottom: " + Integer::toString( bounds->getRectangleBottom() ));

	for (i = 0; i < ( Rss14::MAX_DBL_SEGS * Rss14::SYM_W +2 ); i++)
	{
		chexPattern[i] = 0;
	}
	for (i = 0; i < Rss14::MAX_DBL_SEGS * Rss14::ELMNTS + 4; i++)
	{
		linPattern[i] = 0;
	}
	for (i = 0; i < Rss14::MAX_DBL_SEGS * (Rss14::ELMNTS + 12) + 4; i++)
	{
		sepPattern[i] = 0;
	}
	/*sepPattern[--i] = '\0';*/
	for (i = 0; i < Rss14::MAX_DBL_SEGS; i++)
	{
	  for (j = 0; j < Rss14::ELMNTS; j++)
		dblPattern[i][j] = 0;
	}

	//params.dataStr = BCStrBuf;
	params.dataStr = &dataBuffer[0];

	params.pixMult = 1; 	/* init params */
	params.undercut = 0;
	params.Yundercut = 0;
	params.sepHt = 4;

	params.segWidth = (isGS1DataBarExpanded_) ? 16 : 6;	/* Maximum segments per row */

	rowWidth = params.segWidth; /* save for getUnusedBitCnt */

	/*Create human readable string with spaces */
	/* 90 614141 000015 */
	Debug::trace("Renderer.RSS14", "Create human readable string with space");
	if ((segs = encode(params.dataStr, dblPattern)) > 0)
	{
		linPattern[0] = linPattern[1] = 1; /* left guard pattern */
		lNdx = 2;
		for (i = 0; i < segs-1; i += 2) {
			for (j = 0; j < 8+5+8; j++) { /* copy double segments */
				linPattern[lNdx++] = dblPattern[i/2][j];
			}
		}
		if (i == segs-1) {
			for (j = 0; j < 8+5; j++) { /* copy last odd segment if one exists */
				linPattern[lNdx++] = dblPattern[i/2][j];
			}
		}
		linPattern[lNdx] = linPattern[lNdx+1] = 1; /* right guard pattern */
		j = (segs <= params.segWidth) ? segs : params.segWidth;
		i = (segs+j-1)/j; /* number of linear rows */
		lNdx = 2 + (j/2)*(8+5+8) + (j&1)*(8+5) + 2;
		/*lNdx = (j/2)*(8+5+8) + (j&1)*(8+5); In New Code This Way  EFP...*/
		lMods = 2 + (j/2)*(17+15+17) + (j&1)*(17+15) + 2;
		for (i = 0; i < MAX_DBL_SEGS*SYM_W+2; i++) {
			chexPattern[i] = 1; /* chex = all 1X elements */
		}
		chexPattern[0] = 5; 	/* except first and last  */
		if ((lMods&1) == 0) {
			chexPattern[lMods-8] = 4;
			chexSize = lMods-7;
		}
		else {
			chexPattern[lMods-9] = 5;
			chexSize = lMods-8;
		}
		rPadl = 0;
		rPadcc = lMods - L_PAD - MAX_WIDTH;
		if (rPadcc < 0) {
			rPadl = -rPadcc;
			rPadcc = 0;
		}
	}

    Debug::trace("Renderer.RSS14", "segs: " + Integer::toString(segs));
    Debug::trace("Renderer.RSS14", "lNdx: " + Integer::toString(lNdx));
    Debug::trace("Renderer.RSS14", "lMods: " + Integer::toString(lMods));

	int textLines( 1 );
	if ( !isGS1DataBarExpanded_ )
		textLines = (18 < length) ? 2 : 1;

    int sepPatternCount( 0 );
	if (!isGS1DataBarExpanded_)
	{
		if (12 < segs)
			sepPatternCount = 2;
		else if (6 < segs)
			sepPatternCount = 1;
	}

    SYM_H = bounds->getRectangleBottom() - bounds->getRectangleTop();
    SYM_H -= sepPatternCount * 12; // 12 is the max height of sep pattern
    SYM_H -= textLines * labs(charHeight);
    SYM_H /= sepPatternCount + 1;

    Debug::trace("Renderer.RSS14", "SYM_H: " + Integer::toString(SYM_H));

    if ( SYM_H < 12 )
    {
        Debug::trace("Renderer.RSS14", "symbol height too small!");
    	return;
    }

	height_temp -= (textLines * labs(charHeight));
	startPosition = height_temp;


    barcode_width = margin_offset + (lMods * barcode.getModuleSize());
    Debug::trace("Renderer.RSS14", "barcode_width: " + Integer::toString(barcode_width));


	// If the barcode won't fit in space allotted in label type using 2-pixel-wide bars, then reset it up for 1-pixel-wide bars and try again
	if ((barcode_width > field_width) && (barcode_width > 0))
	{
		modsize = 1;

		barcode.setCharOffset((((7 * modsize) - faceMax) / 2));
		Debug::trace("Renderer.RSS14", "Character Offset2:" + Integer::toString(barcode.getCharOffset()));
		if (barcode.getCharOffset() < 0)
			barcode.setCharOffset(barcode.getCharOffset() * -1);

		Debug::trace("Renderer.RSS14", "set the module size to 1 due to barcode won't fit in space allotted in label type");
		barcode.setModuleSize(modsize);

#ifdef EAN_USE_MARGIN
#ifdef EAN_USE_HALF_LEFT_MARGIN
		margin_offset = ((modsize*LEFT_MARG_MODS_UPC) / 2);
#else
		margin_offset = (modsize*LEFT_MARG_MODS_UPC);
#endif
#else
		margin_offset = 10;
#endif

		barcode_width = margin_offset + (lMods * barcode.getModuleSize());
		Debug::trace("Renderer.RSS14", "new barcode_width for modsize=1: " + Integer::toString(barcode_width));
	}


	if((barcode_width <= field_width) && (barcode_width > 0))
    {
		line1 = TRUE; 			/* so first line is not Y undercut */

		Debug::trace("Renderer.RSS14", "draw the barcode");
		/* note: barcode is drawn bottom up	*/
		i = 0;
		evenRow = TRUE; 		/* start with even */


		while (segs > i+params.segWidth) {
			i += params.segWidth;
			evenRow = !evenRow; /* alternate row parity	*/
		}

		if (segs - i > 0) {
			evenRow = !evenRow; /* alternate row parity for partial last row */
			j = segs;
			lNdx1 = ((j/2)*(8+5+8)+(j&1)*(8+5)+4) - ((i/2)*(8+5+8)+(i&1)*8);

			/* left guard pattern */
			sav1 = linPattern[(i/2)*(8+5+8)+(i&1)*8];
			sav2 = linPattern[(i/2)*(8+5+8)+(i&1)*8+1];
			linPattern[(i/2)*(8+5+8)+(i&1)*8] = 1;
			linPattern[(i/2)*(8+5+8)+(i&1)*8+1] = 1;
			rev = evenRow ^ (i/2)&1;

			height_temp -= params.pixMult*SYM_H;
			barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);
			Debug::trace("Renderer.RSS14", "height_temp: " + Integer::toString(height_temp));

			/* can't reverse if odd # finders so offset it right by one	*/
			if (rev && (((lNdx1-4)%8)&1))
			{
				elements(bitmap, lNdx1, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
								  params.pixMult*SYM_H, &barcode, (i/2+1)&1, TRUE, FALSE);

				sepCnt = cnvSeparator(lNdx1, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
											sepPattern, ((i/2+1)&1));

				height_temp -= params.sepHt;
				barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);

				elements(bitmap, sepCnt, sepPattern, params.sepHt, &barcode, TRUE, TRUE, FALSE);
			}
			/* otherwise normal row	*/
			else
			{
				/*draw lower barcode */
				elements(bitmap, lNdx1, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
								  params.pixMult*SYM_H, &barcode, (i/2+1)&1, FALSE, rev);
				if (i > 0) {
					sepCnt = cnvSeparator(lNdx1, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
											sepPattern, ((i/2+1)&1));

					height_temp -= params.sepHt;
					barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);

					/*draw lower seperator */
					elements(bitmap, sepCnt, sepPattern, params.sepHt, &barcode, TRUE, FALSE, rev);
				}
			}
			evenRow = !evenRow;
			/* restore elements	*/
			linPattern[(i/2)*(8+5+8)+(i&1)*8] = sav1;
			linPattern[(i/2)*(8+5+8)+(i&1)*8+1] = sav2;
		}

		for (i -= params.segWidth ; i >= 0; i -= params.segWidth)
		{
			j = i + params.segWidth; /* last segment number + 1 in this row	*/
			/* left guard pattern  */
			sav1 = linPattern[(i/2)*(8+5+8)+(i&1)*8];
			sav2 = linPattern[(i/2)*(8+5+8)+(i&1)*8+1];
			linPattern[(i/2)*(8+5+8)+(i&1)*8] = 1;
			linPattern[(i/2)*(8+5+8)+(i&1)*8+1] = 1;
			/* right guard pattern */
			sav3 = linPattern[(j/2)*(8+5+8)+(j&1)*(8+5)+2];
			sav4 = linPattern[(j/2)*(8+5+8)+(j&1)*(8+5)+3];
			linPattern[(j/2)*(8+5+8)+(j&1)*(8+5)+2] = 1;
			linPattern[(j/2)*(8+5+8)+(j&1)*(8+5)+3] = 1;

			/* chex pattern	*/
			height_temp -= params.sepHt;
			barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);
			Debug::trace("Renderer.RSS14", "height_temp(4): " + Integer::toString(height_temp));

			elements(bitmap, chexSize, chexPattern, params.sepHt, &barcode, TRUE, FALSE, FALSE);

			sepCnt = cnvSeparator(lNdx, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
											sepPattern, ((i/2+1)&1));

			height_temp -= params.sepHt;
			barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);
			Debug::trace("Renderer.RSS14", "height_temp(5): " + Integer::toString(height_temp));

			elements(bitmap, sepCnt, sepPattern, params.sepHt, &barcode, TRUE, FALSE, evenRow ^ (i/2)&1);

			/*draw upper barcode */
			height_temp -= params.pixMult*SYM_H;
			barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);
			Debug::trace("Renderer.RSS14", "height_temp(6): " + Integer::toString(height_temp));

			elements(bitmap, lNdx, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
							params.pixMult*SYM_H, &barcode, (i/2+1)&1, FALSE, evenRow ^ (i/2)&1);
			if (i > 0) {
				sepCnt = cnvSeparator(lNdx, &linPattern[(i/2)*(8+5+8)+(i&1)*8],
											sepPattern, ((i/2+1)&1));

				height_temp -= params.sepHt;
				barcode.resetPen((bounds->getRectangleLeft() + margin_offset), height_temp);
				Debug::trace("Renderer.RSS14", "height_temp(7): " + Integer::toString(height_temp));

				elements(bitmap, sepCnt, sepPattern, params.sepHt, &barcode, TRUE, FALSE, evenRow ^ (i/2)&1);
			}
			evenRow = !evenRow;
			/* restore elements	*/
			linPattern[(i/2)*(8+5+8)+(i&1)*8] = sav1;
			linPattern[(i/2)*(8+5+8)+(i&1)*8+1] = sav2;
			linPattern[(j/2)*(8+5+8)+(j&1)*(8+5)+2] = sav3;
			linPattern[(j/2)*(8+5+8)+(j&1)*(8+5)+3] = sav4;
		}
#if defined(PAL_WIN32_RUN)
		HBITMAP winBitmap = NULL;

		HDC fontDrvId = CreateCompatibleDC(NULL);
		Assert(fontDrvId != NULL, Util::CNTException());
		Assert(GetMapMode(fontDrvId) == MM_TEXT, Util::CNTException());
		Assert(SelectFont(fontDrvId, fontId.Get()) != NULL, Util::CNTException());

//		HBITMAP winBitmapOld = SelectBitmap(fontDrvId, winBitmap);
//		Assert( winBitmapOld != NULL, Util::CNTException());

		SetTextColor(fontDrvId, COLOR_BLACK);
		SetBkMode(fontDrvId, TRANSPARENT);
#endif

		Debug::trace("Renderer.RSS14", "draw the barcode text");
		int textLen = (textLines == 1) ? strlen((const char*)data) : 18;
#if defined(PAL_WIN32_RUN)
		Assert(TextOutA(fontDrvId, (bounds->getRectangleLeft() + margin_offset), startPosition, (char *)data, textLen) != 0, Util::CNTException());
#else
		TextOut(fontId, (bounds->getRectangleLeft() + margin_offset), startPosition, (char *)data, textLen, txt_bmp);
#endif
		if ( 1 < textLines )
		{
			Debug::trace("Renderer.RSS14", "draw the barcode last characters");

			// remove terminator chars
			std::string dataStr( (char*) &data[18] );
			dataStr.erase(std::remove(dataStr.begin(), dataStr.end(), '#'), dataStr.end());

#if defined(PAL_WIN32_RUN)
			Assert(TextOutA( fontDrvId, (bounds->getRectangleLeft() + margin_offset), (startPosition + charHeight), (char *)(data += 18), (length - 18) ) != 0, Util::CNTException());
#else
			TextOut( fontId, (bounds->getRectangleLeft() + margin_offset), (startPosition + charHeight), dataStr.c_str(), dataStr.length(), txt_bmp );
#endif
		}

#if defined(PAL_WIN32_RUN)
		//SelectBitmap(fontDrvId, winBitmapOld);
		Hobart::CRendererUtil::Draw(bitmap, winBitmap);
		DeleteDC(fontDrvId);
		DeleteObject(winBitmap);
#endif

    }
    return;
}

int Rss14::encode(unsigned char *string, unsigned char bars[Rss14::MAX_DBL_SEGS][Rss14::ELMNTS])
{

const unsigned char finders[6][3] =
{
	{ 1,8,4 },
	{ 3,6,4 },
	{ 3,4,6 },
	{ 3,2,8 },
	{ 2,6,5 },
	{ 2,2,9 }
};

const int finderSets[10][11] =
{
	{ 1,	-1,	0,	 0,	0,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-2,	2,	 0,	0,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-3,	2,	-4,	0,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-5,	2,	-4,	3,	 0,	0,	 0,	0,	 0,	0},
	{ 1,	-5,	2,	-4,	4,	-6,	0,	 0,	0,	 0,	0},
	{ 1,	-5,	2,	-4,	5,	-6,	6,	 0,	0,	 0,	0},
	{ 1,	-1,	2,	-2,	3,	-3,	4,	-4,	0,	 0,	0},
	{ 1,	-1,	2,	-2,	3,	-3,	4,	-5,	5,	 0,	0},
	{ 1,	-1,	2,	-2,	3,	-3,	4,	-5,	6,	-6,	0},
	{ 1,	-1,	2,	-2,	3,	-4,	4,	-5,	5,	-6,	6}
};


	int i, j;
	int parity, weight;
	int symValue;
	int size, fndrNdx, fndrSetNdx;
	unsigned char bitField[Rss14::MAX_DBL_SEGS * 3];

	linFlag = TRUE;
	parity = 0;

	if ((i=check2DData(string)) != 0) {
		return 0;
	}
	size = pack(string, bitField);
	if (size < 0) {
		return 0;
	}

	/* note size is # of data chars, not segments  */
	if ((bitField[0]&0x40) == 0x40) {
		/* method 1, insert variable length symbol bit field */
		bitField[0] |= (((size+1)&1)<<5) + ((size > 13)?0x10:0);
	}
	if ((bitField[0]&0x60) == 0) {
		/* method 00, insert variable length symbol bit field  */
		bitField[0] |= (((size+1)&1)<<4) + ((size > 13)?8:0);
	}
	if ((bitField[0]&0x71) == 0x30) {
		/* method 01100 01101, insert variable length symbol bit field	*/
		bitField[0] |= (((size+1)&1)<<1) + ((size > 13)?1:0);
	}
	fndrSetNdx = (size - 2) / 2;

	for (i = 0; i < (size+2)/2; i++)
	{
		/* loop through all dbl segments	 */
		fndrNdx = finderSets[fndrSetNdx][i];
		/* fill left data char in dbl segment if not first (check char)	 */
		j = (fndrNdx >= 0) ? fndrNdx*2 : -fndrNdx*2+1;
		if (i > 0) {
			weight = parWts[2*(j-2)];
			symValue = getVal12(bitField, i*2-1);
			parity = symCharPat(&bars[i][0], symValue, parity, weight, TRUE);
		}
		/* fill finder for dbl segment */
		if (fndrNdx < 0) { /* reversed finder  */
			bars[i][12] = finders[-fndrNdx-1][0];
			bars[i][11] = finders[-fndrNdx-1][1];
			bars[i][10] = finders[-fndrNdx-1][2];
			bars[i][9] = 1;
			bars[i][8] = 1;
		}
		else { /* forward finder  */
			bars[i][8] = finders[fndrNdx-1][0];
			bars[i][9] = finders[fndrNdx-1][1];
			bars[i][10] = finders[fndrNdx-1][2];
			bars[i][11] = 1;
			bars[i][12] = 1;
		}
		/* fill right data char in dbl segment if it exists	 */
		if (size > i*2) {
			weight = parWts[2*(j-2)+1];
			symValue = getVal12(bitField, i*2);
			parity = symCharPat(&bars[i][8+5], symValue, parity, weight, FALSE);
		}
	}
	/* fill in first parity char   */
	symCharPat(bars[0], (size-3)*PARITY_MOD + parity, 0, weight, TRUE);
	return(size+1);
}

void Rss14::elements(BitMap *bitmap, int elmCnt, unsigned char pattern[], int height, GenericBarcode *barcode,
 					   int whtFirst, int rightShift, int reverse)
{
	unsigned char i;

	if (rightShift)
	{
	    drawModule(bitmap, barcode, LightModule, 1, height);
	}

	if ((reverse) && ((elmCnt & 1) == 0)) {
		whtFirst = !whtFirst; // invert white first if revesed even elements
	}

	if (reverse)
	{
		if (whtFirst)
		{
			for (i = 0; i < elmCnt; i++)
			{
			    drawModule(bitmap, barcode, LightModule, pattern[(elmCnt - 1) - i], height);

				if (i < elmCnt)
				{
					i++;
				    drawModule(bitmap, barcode, DarkModule, pattern[(elmCnt - 1) - i], height);
				}
			}
		}
		else
		{
			for (i = 0; i < elmCnt; i++)
			{
			    drawModule(bitmap, barcode, DarkModule, pattern[(elmCnt - 1) - i], height);

				if (i < elmCnt)
				{
					i++;
				    drawModule(bitmap, barcode, LightModule, pattern[(elmCnt - 1) - i], height);
				}
			}
		}
	}
	else
	{
		if (whtFirst)
		{
			for (i = 0; i < elmCnt; i++)
			{
			    drawModule(bitmap, barcode, LightModule, pattern[i], height);
				i++;

				if (i < elmCnt)
				{
				    drawModule(bitmap, barcode, DarkModule, pattern[i], height);
				}
			}
		}
		else
		{
			for (i = 0; i < elmCnt; i++)
			{
			    drawModule(bitmap, barcode, DarkModule, pattern[i], height);
				i++;

				if (i < elmCnt)
				{
				    drawModule(bitmap, barcode, LightModule, pattern[i], height);
				}
			}
		}
	}

	return;
}

/* copies pattern for separator adding 9 narrow elements inside each finder	*/
int Rss14::cnvSeparator(int elmCnt, unsigned char pattern[], unsigned char sepPattern[],
		int whtFirst)
{

int i, j, k;

	k = 0;
	i = 0;

	while (k <= 4)
	{
		k += pattern[i];
		i++;
	}

	if ((whtFirst && (i&1)==1) || (!whtFirst && (i&1)==0))
	{
		sepPattern[0] = 4;
		sepPattern[1] = k-4;
		j = 2;
	}
	else
	{
		sepPattern[0] = k;
		j = 1;
	}

	for ( ; i < elmCnt-2; i++, j++)
	{
		sepPattern[j] = pattern[i];
		if (pattern[i] + pattern[i+1] + pattern[i+2] == 13)
		{
			if ((j&1)==1)
			{
				/* finder is light,dark,light */
				for (k = 0; k < pattern[i]; k++)
				{
					sepPattern[j+k] = 1; /* bwbw... over light */
				}
				j += k-1;
				if ((k&1) == 0)
				{
					i++;
					sepPattern[j] += pattern[i]; /* trailing w for e1, append to next w	*/
				}
				else
				{
					i++;
					j++;
					sepPattern[j] = pattern[i]; /* trailing b, next is w e2	*/
				}

				i++;
				j++;

				for (k = 0; k < pattern[i]; k++)
				{
					sepPattern[j+k] = 1; /* bwbw... over light e3 */
				}

				j += k-1;
				if ((k&1) == 0)
				{
					i++;
					sepPattern[j] += pattern[i]; /* trailing w for e3, append to next w	*/
				}
				else
				{
					i++;
					j++;
					sepPattern[j] = pattern[i]; /* trailing b for e3, next is w	*/
				}
			}
			else
			{
				/* finder is dark,light,dark  */
				i++;
				if (pattern[i] > 1)
				{
					j++;
					for (k = 0; k < pattern[i]; k++)
					{
						sepPattern[j+k] = 1; /* bwbw... over light e2  */
					}
					j += k-1;
					if ((k&1) == 0)
					{
						i++;
						sepPattern[j] += pattern[i]; /* trailing w for e2, append to next w	*/
					}
					else
					{
						i++;
						j++;
						sepPattern[j] = pattern[i]; /* trailing b for e2, next is w	*/
					}
				}
				else
				{
					i++;
					sepPattern[j] = 10; /* 1X light e2 (val=3), so w/b/w = 10/1/2 */
					sepPattern[j+1] = 1;
					sepPattern[j+2] = 2;
					j+=2;
				}
			}
		}
	}

	k = pattern[i] + pattern[i+1];
	j--;

	while (k <= 4)
	{
	   k += sepPattern[j];
	   j--;
	}

	if ((j&1)==0)
	{
		j += 2;
		sepPattern[j-1] = k-4;
		sepPattern[j] = 4;
	}
	else
	{
		j++;
		sepPattern[j] = k;
	}
	return(j+1);
}

#define K	4
static int widths[K];

/* 	Fills in elements for a symbol character given a element array and
 	and a symbol char value. Updates the parity *weight. Will fill in
 	the array forward or reverse order for odd or even characters.
 	Returns the updated parity.	*/
int Rss14::symCharPat(unsigned char bars[], int symValue, int parity, int weight,
							 int forwardFlag)
{

int i, value, saveVal;
int elementN, elementMax;
int wgtOdd, wgtEven;
int iIndex;

	wgtOdd = weight;
	wgtEven = (weight * 3) % PARITY_MOD;

	/* get sym char val index into tbl174 */
	iIndex = 0;
	while (symValue >= tbl174[iIndex+5])
	{
		symValue -= tbl174[iIndex+5];
		iIndex += 6;
	}

	/* get odd elements N and max */
	elementN = tbl174[iIndex];
	elementMax = tbl174[iIndex+1];
	saveVal = value = symValue / tbl174[iIndex+4];

	/* generate and store odd element widths:  */
	getRSSwidths(value, elementN, K, elementMax, 0);
	for (i = 0; i < 4; i++)
	{
		if (forwardFlag)
		{
			bars[i*2] = (unsigned char)widths[i]; /* store in 0,2,4,6  */
		}
		else
		{
			bars[7 - i*2] = (unsigned char)widths[i]; /* else in 7,5,3,1 */
		}

		parity += wgtOdd * widths[i];
		parity = parity % PARITY_MOD;
		wgtOdd = (wgtOdd * 9) % PARITY_MOD;
	}

	/* calculate even elements value: */
	value = symValue - (tbl174[iIndex+4] * saveVal);
	elementN = tbl174[iIndex+2];
	elementMax = tbl174[iIndex+3];

	/* generate and store even element widths:	*/
	getRSSwidths(value, elementN, K, elementMax, 1);
	for (i = 0; i < 4; i++)
	{
		if (forwardFlag)
		{
			bars[1 + i*2] = (unsigned char)widths[i]; /* store in 1,3,5,7 */
		}
		else
		{
			bars[6 - i*2] = (unsigned char)widths[i]; /* else in 6,4,2,0  */
		}

		parity += wgtEven * widths[i];
		parity = parity % PARITY_MOD;
		wgtEven = (wgtEven * 9) % PARITY_MOD;
	}
	return(parity);
}

/* gets the next 12 bit sym char from bit string */
int Rss14::getVal12(unsigned char bitString[], int symNdx)
{
	int val, ndx;

	ndx = symNdx*3/2; /* index into bitString */
	if (symNdx & 1)
	{
		/* odd sym char count, so val = 4+8 bits */
		val = (int)bitString[ndx] & 0xF;
		val = (val << 8) + (int)bitString[ndx+1];
	}
	else
	{
		/* even sym char count, so val = 8+4 bits  */
		val = (int)bitString[ndx];
		val = (val << 4) + ((int)bitString[ndx+1] >> 4);
	}
	return(val);
}

/**********************************************************************
* getRSSwidths
* routine to generate widths for RSS elements for a given value.
* Calling arguments:
* val = required value
*	n = number of modules
* elements = elements in set (RSS-14 Expanded = 4; RSS-14 Limited = 7)
*	maxWidth = maximum module width of an element
*	noNarrow = 0 will skip patterns without a narrow element
* Return:
* static int widths[] = element widths
************************************************************************/
void Rss14::getRSSwidths(int val, int n, int elements, int maxWidth, int noNarrow)
{
	int bar;
	int elmWidth;
	int mxwElement;
	int subVal, lessVal;
	int narrowMask = 0;

	for (bar = 0; bar < elements-1; bar++)
	{
		for (elmWidth = 1, narrowMask |= (1<<bar);
				 ;
				 elmWidth++, narrowMask &= ~(1<<bar))
		{
			/* get all combinations */
			subVal = combins(n-elmWidth-1, elements-bar-2);
			/* less combinations with no narrow */
			if ((!noNarrow) && (narrowMask == 0) &&
					 (n-elmWidth-(elements-bar-1) >= elements-bar-1))
			{
				subVal -= combins(n-elmWidth-(elements-bar), elements-bar-2);
			}
			/* less combinations with elements greater than maxVal */
			if (elements-bar-1 > 1)
			{
				lessVal = 0;
				for (mxwElement = n-elmWidth-(elements-bar-2);
						 mxwElement > maxWidth;
						 mxwElement--)
				{
					lessVal += combins(n-elmWidth-mxwElement-1, elements-bar-3);
				}
				subVal -= lessVal * (elements-1-bar);
			}
			else if (n-elmWidth > maxWidth)
			{
				subVal--;
			}
			val -= subVal;
			if (val < 0) break;
		}
		val += subVal;
		n -= elmWidth;
		widths[bar] = elmWidth;
	}
	widths[bar] = n;
	return;
}

/* combins(n,r): returns the number of Combinations of r selected from n:
*		Combinations = n!   ( n-r! * r!) */
int Rss14::combins(int n, int r)
{

	int i, j;
	int maxDenom, minDenom;
	int val;

	if (n-r > r)
	{
		minDenom = r;
		maxDenom = n-r;
	}
	else
	{
		minDenom = n-r;
		maxDenom = r;
	}

	val = 1;
	j = 1;
	for (i = n; i > maxDenom; i--)
	{
		val *= i;
		if (j <= minDenom)
		{
			val /= j;
			j++;
		}
	}
	for ( ; j <= minDenom; j++)
	{
		val /= j;
	}
	return(val);
}


int Rss14::pack(unsigned char str[], unsigned char bitField[] )
{
	encodeT encode;

	encode.str = str;
	encode.bitField = bitField;
	encode.iStr = encode.iBit = 0;
	if (linFlag)
	{
		encode.iBit++; /* skip composite link bit if linear component */
		encode.mode = doLinMethods(encode.str, &encode.iStr,
											encode.bitField, &encode.iBit);
	}
	else
	{
		encode.mode = doMethods(&encode);
	}

	while (encode.mode != FINI_MODE)
	{
		switch (encode.mode)
		{
			case NUM_MODE:
			{
				encode.mode = procNUM(&encode);
				break;
			}
			case ALNU_MODE:
			{
				encode.mode = procALNU(&encode);
				break;
			}
			case ISO_MODE:
			{
				encode.mode = procISO(&encode);
				break;
			}
			default:
			{
				return(-1);
			}
		}
	}
	return(insertPad(&encode));
}

int Rss14::check2DData(unsigned char dataStr[])
{
	int i;

	for (i = 0; iswhat[dataStr[i]] != 0x80; i++)
	{
		if (iswhat[dataStr[i]] == 0)
		{
			return(i); /* error, unsupported character */
		}
	}
	return(0);
}

int Rss14::procNUM(encodeT  *encode) {

int bitCnt, char1, char2, what1, what2, i;

	/* check first char type */
	if ((what1 = iswhat[(char1 = encode->str[encode->iStr])]) == IS_FINI)
	{
		/* end of data	*/
		bitCnt = getUnusedBitCnt(encode->iBit, &i);
		if (bitCnt > 4)
		{
			bitCnt = 4;
		}

		if (bitCnt > 0)
		{
			/* insert full or partial 0000 alnu latch for pad  */
			putBits(encode->bitField, encode->iBit, bitCnt, 0);
			encode->iBit += bitCnt;
		}
		return(FINI_MODE);
	}

	if ((what1 & IS_NUM) == 0)
	{
		/* first not a "number", latch to ALNU	*/
		putBits(encode->bitField, encode->iBit, 4, 0);
		encode->iBit += 4;
		return(ALNU_MODE);
	}
	/* check 2nd char type	*/
	if (((what2 = iswhat[(char2 = encode->str[(encode->iStr)+1])]) == IS_FINI) &&
			((what1 & IS_FNC1) == 0))
	{
		/* single digit left, check for nearly at end of bits  */
		encode->iStr += 1;
		bitCnt = getUnusedBitCnt(encode->iBit, &i);
		if ((bitCnt >= 4) && (bitCnt < 7))
		{
			/* less than 7 bits, encode a bcd+1	 */
			putBits(encode->bitField, encode->iBit, 4, (size_t)(char1+1 -(int)'0'));
			bitCnt -= 4;
			if (bitCnt > 0)
			{
				/* 0 or 00 final pad  */
				putBits(encode->bitField, (encode->iBit)+4, bitCnt, 0);
			}
			encode->iBit += 4 + bitCnt;
		}
		else
		{
			/* encode as digit & FNC1 */
			putBits(encode->bitField, encode->iBit, 7, (size_t)(((char1-(int)'0') * 11) + 10 + 8));
			encode->iBit += 7;
			bitCnt -= 7;
			if ((bitCnt > 4) || (bitCnt < 0))
			{
				bitCnt = 4; /* either not near end or ran over to upper	 */
			}

			if (bitCnt > 0)
			{
				/* insert full or partial 0000 alnu latch */
				putBits(encode->bitField, encode->iBit, bitCnt, 0);
				encode->iBit += bitCnt;
			}
		}
		return(FINI_MODE);
	}

	if (((what1 & what2 & IS_FNC1) != 0) || ((what2 & IS_NUM) == 0))
	{
		/* dbl FNC1 or 2nd char not a digit, latch to alnu	*/
		putBits(encode->bitField, encode->iBit, 4, 0);
		encode->iBit += 4;
		return(ALNU_MODE);
	}
	else
	{
		/* both "digits", encode as 7-bits	*/
		encode->iStr += 2;
		if ((what1 & IS_FNC1) != 0)
		{
			char1 = 10;
		}
		else
		{
			char1 -= (int)'0';
		}
		if ((what2 & IS_FNC1) != 0)
		{
			char2 = 10;
		}
		else
		{
			char2 -= (int)'0';
		}
		putBits(encode->bitField, encode->iBit, 7, (size_t)((char1 * 11) + char2 + 8));
		encode->iBit += 7;
		return(NUM_MODE);
	}
}

int Rss14::procALNU(encodeT *encode)
{

int chr, i, what, whatN;

	/* check next char type	*/
	if ((what = iswhat[(chr = encode->str[encode->iStr])]) == IS_FINI)
	{
		/* end of data	*/
		return(FINI_MODE);
	}
	if ((what & IS_ALNU) == 0)
	{
		/* not a ALNU, latch to ISO	 */
		putBits(encode->bitField, encode->iBit, 5, 4);
		encode->iBit += 5;
		return(ISO_MODE);
	}
	if (((what & IS_NUM) != 0) &&
			(((what | iswhat[encode->str[(encode->iStr)+1]]) & IS_FNC1) == 0))
	{
		/* next is NUM, look for more  */
		for (i = 1; i < 6; i++)
		{
			if ((whatN = iswhat[encode->str[(encode->iStr)+i]]) == IS_FINI)
			{
				if (i >= 4)
				{
					/* latch numeric if >= 4 numbers at end	*/
					putBits(encode->bitField, encode->iBit, 3, 0);
					encode->iBit += 3;
					return(NUM_MODE);
				}
				else
				{
					/* else continue in ALNU */
					break;
				}
			}
			else if ((whatN & IS_NUM) == 0)
			{
				/* stay in ALNU if < 6 digits coming up	 */
				break;
			}
		}

		if (i == 6)
		{
			/* NUM if 6 or more digits coming up */
			putBits(encode->bitField, encode->iBit, 3, 0);
			encode->iBit += 3;
			return(NUM_MODE);
		}
	}
	/* process char in ALNU mode  */
	encode->iStr += 1;
	if ((what & IS_NUM) != 0)
	{
		/* FNC1 or 0-9 */
		if ((what & IS_FNC1) != 0)
		{
			chr = 0xf;
			encode->mode = NUM_MODE;
		}
		else
		{
			chr = chr - (int)'0' + 5;
		}
		putBits(encode->bitField, encode->iBit, 5, (size_t)chr);
		encode->iBit += 5;
	}
	else
	{
		if (chr == SYM_SEP)
		{
			/* ^ (symbol separator)	 */
			chr = 0x1F;
			encode->mode = NUM_MODE;
		}
		else if (chr >= (int)'A')
		{
			/* A-Z	 */
			chr = chr - (int)'A';
		}
		else if (chr >= ',')
		{
			/* ,-./	 */
			chr = chr - (int)',' + 0x1B;
		}
		else
		{
			/* * */
			chr = 0x1A;
		}
		putBits(encode->bitField, encode->iBit, 6, (size_t)(chr + 0x20));
		encode->iBit += 6;
	}
	return(encode->mode);
}

int Rss14::procISO(encodeT *encode)
{

	int chr, i, what, whatN, numCnt;

	/* check next char type	  */
	if ((what = iswhat[(chr = encode->str[encode->iStr])]) == IS_FINI)
	{
		/* end of data	*/
		return(FINI_MODE);
	}

	numCnt = 0;
	if (((what & IS_ALNU) != 0) && ((what & IS_FNC1) == 0))
	{
		/* next is ALNU (& not FNC1), look 9 for more ALNU */
		if ((what & IS_NUM) != 0)
		{
			/* also count leading "digits"	*/
			numCnt = 1;
		}

		for (i = 1; i < 10; i++)
		{
			whatN = iswhat[encode->str[(encode->iStr)+i]];
			if (whatN == IS_FINI)
			{
				if ((numCnt >= 4) || (numCnt <= -4))
				{
					/* latch numeric if >= 4 numbers at end	 */
					putBits(encode->bitField, encode->iBit, 3, 0);
					encode->iBit += 3;
					return(NUM_MODE);
				}

				if (i >= 5)
				{
					/* latch ALNU if >= 5 alphanumbers at end  */
					putBits(encode->bitField, encode->iBit, 5, 4);
					encode->iBit += 5;
					return(ALNU_MODE);
				}
				else
				{
					break;
				}
			}

			if ((whatN & IS_NUM) != 0)
			{
				if (numCnt > 0)
				{
					/* count leading diits */
					numCnt++;
				}
			}
			else if (numCnt > 0)
			{
				/* stop counting if not a digit	 */
				numCnt = -numCnt;
			}

			if ((whatN & IS_ALNU) == 0)
			{
				break;
			}
		}

		if (i == 10)
		{
			if ((numCnt >= 4) || (numCnt <= -4))
			{
				/* latch numeric if >= 4 numbers follow & no ISO chars in next 10 */
				putBits(encode->bitField, encode->iBit, 3, 0);
				encode->iBit += 3;
				return(NUM_MODE);
			}
			else
			{
				/* latch ALNU if no ISO only chars in next 10  */
				putBits(encode->bitField, encode->iBit, 5, 4);
				encode->iBit += 5;
				return(ALNU_MODE);
			}
		}
	}
	/* process char in ISO mode	 */
	encode->iStr += 1;
	if ((what & IS_NUM) != 0)
	{
		/* FNC1 or 0-9 */
		if ((what & IS_FNC1) != 0)
		{
			chr = 0xF;
			encode->mode = NUM_MODE;
		}
		else
		{
			chr = chr - (int)'0' + 5;
		}
		putBits(encode->bitField, encode->iBit, 5, (size_t)chr);
		encode->iBit += 5;
	}
	else if ((chr >= (int)'A') && (chr <= (int)'Z'))
	{
		/* A-Z	*/
		chr = chr - (int)'A' + 0x40;
		putBits(encode->bitField, encode->iBit, 7, (size_t)chr);
		encode->iBit += 7;
	}
	else if ((chr >= (int)'a') && (chr <= (int)'z'))
	{
		/* a-z	*/
		chr = chr - (int)'a' + 0x5A;
		putBits(encode->bitField, encode->iBit, 7, (size_t)chr);
		encode->iBit += 7;
	}
	else
	{
		if (chr == 32)
		{
			chr = 0xFC; /* sp  */
		}
		else if (chr == SYM_SEP)
		{
			chr = 0xFD; /* ^ (symbol separator)	 */
			encode->mode = NUM_MODE;
		}
		else if (chr == 95)
		{
			chr = 0xFB; /* _   */
		}
		else if (chr >= 58)
		{
			chr = chr - 58 + 0xF5; /* :-? */
		}
		else if (chr >= 37)
		{
			chr = chr - 37 + 0xEA; /* %-/  */
		}
		else
		{
			chr = chr - 33 + 0xE8; /* !-"	*/
		}
		putBits(encode->bitField, encode->iBit, 8, (size_t)chr);
		encode->iBit += 8;
	}
	return(encode->mode);
}

int Rss14::insertPad(encodeT *encode)
{

	int bitCnt, chr, size;

	bitCnt = getUnusedBitCnt(encode->iBit, &size);
	if (bitCnt < 0)
	{
		return(-1); /* too many bits */
	}

	while (bitCnt >= 5)
	{
		putBits(encode->bitField, encode->iBit, 5, 4);
		encode->iBit += 5;
		bitCnt -= 5;
	}
	if (bitCnt > 0)
	{
		chr = 4 >> (5-bitCnt);
		putBits(encode->bitField, encode->iBit, bitCnt, (size_t)chr);
		encode->iBit += bitCnt;
	}
	return(size);
}

int Rss14::doMethods(encodeT *encode)
{

	size_t bits;

	if (strlen( (const char *) encode->str) >= 8 && encode->str[0] == '1' &&
				(encode->str[1] == '1' || encode->str[1] == '7'))
	{
		/* method "10" */
		putBits(encode->bitField, 0, 2, 2); /* mfg exp date-lot encodation method bit flag 10  */
		bits = ((size_t)(encode->str[2]-'0')*10 +
				(size_t)(encode->str[3]-'0')) * 384; /* YY  */
		bits += ((size_t)(encode->str[4]-'0')*10 +
				(size_t)(encode->str[5]-'0') - 1) * 32; /* MM	 */
		bits += (size_t)(encode->str[6]-'0')*10 +
				(size_t)(encode->str[7]-'0'); /* DD  */
		putBits(encode->bitField, 2, 16, bits); /* date packed data	*/
		putBits(encode->bitField, 2+16, 1,
				(size_t)((encode->str[1] == '1') ? 0 : 1)); /* 0 1 bit for AI 11 17 */

		if (encode->str[8] == '1' && encode->str[9] == '0' &&
				encode->str[10] != '#')
		{
			encode->iStr = 2+6+2; /* lot data follows */
			encode->iBit = 2+16+1;
		}
		else
		{
			encode->str[7] = '#'; /* insert FNC1 to indicate no lot	 */
			encode->iStr = 2+6-1;
			encode->iBit = 2+16+1;
		}
		return(NUM_MODE);
	}
	return(encode->mode);
}

int Rss14::doLinMethods(unsigned char str[], int *iStr, unsigned char bitField[], int *iBit)
{

	size_t bits;
	long weight;
	char numStr[10];

	if (strlen((const char *) str ) >= 26)
	{
		strncpy((char *)numStr, (char *)&str[20], 6); /* possible weight field	*/
		numStr[6] = '\0';
	}
	/* look for AI 01  */
	if (strlen((const char *) str ) >=16 && str[0]=='0' && str[1]=='1')
	{

		/* look for fixed length with AI 01[9] + 3103[0-32767]	*/
		if (str[2]=='9' && (strlen((const char *) str )==16+10) &&
			str[16]=='3' && str[17]=='1' && str[18]=='0' && str[19]=='3' &&
			( weight = atol( (char *)numStr) ) <=32767L )
		{
			/* method 0100, AI's 01 + 3103	 */
			putBits(bitField, *iBit, 4, 4); /* write method	*/
			*iBit += 4;
			*iStr += 3; /* skip AI 01 and PI 9 */
			cnv12(str, iStr, bitField, iBit); /* write PID-12 */
			putBits(bitField, *iBit, 15, (size_t)weight); /* write weight	*/
			*iBit += 15;
			*iStr += 1+10; /* skip check digit & jump weight field	*/
		}

		/* look for fixed length with AI 01[9] + 3202[0-009999]	 */
		else if (str[2]=='9' && (strlen((const char *) str )==16+10) &&
			str[16]=='3' && str[17]=='2' && str[18]=='0' && str[19]=='2' &&
			( weight = atol( (char *) numStr)) <=9999L )
		{
			/* method 0101, AI s 01 + 3202	*/
			putBits(bitField, *iBit, 4, 5); /* write method	 */
			*iBit += 4;
			*iStr += 3; /* skip AI 01 and PI 9	*/
			cnv12(str, iStr, bitField, iBit); /* write PID-12  */
			putBits(bitField, *iBit, 15, (size_t)weight); /* write weight	 */
			*iBit += 15;
			*iStr += 1+10; /* skip check digit & jump weight field	 */
		}

		/* look for fixed length with AI 01[9] + 3203[0-022767]	 */
		else if (str[2]=='9' && (strlen( (const char *) str) == 16+10 ) &&
			str[16]=='3' && str[17]=='2' && str[18]=='0' && str[19]=='3' &&
			( weight = atol( (char *) numStr) ) <= 22767L )
		{
			/* method 0101, AI s 01 + 3203	*/
			putBits(bitField, *iBit, 4, 5); /* write method	*/
			*iBit += 4;
			*iStr += 3; /* skip AI 01 and PI 9 */
			cnv12(str, iStr, bitField, iBit); /* write PID-12 */
			putBits(bitField, *iBit, 15, (size_t)weight+10000); /* write weight */
			*iBit += 15;
			*iStr += 1+10; /* skip check digit   jump weight field */
		}

		/* look for AI 01[9] + 392[0-3]	*/
		else if ( str[2]=='9' && (strlen( (const char *) str) >= 16+4+1 ) &&
			str[16]=='3' && str[17]=='9' && str[18]=='2' &&
			(str[19]>='0' && str[19]<='3'))
		{
			/* method 01100, AI s 01 + 392x + G.P. */
			putBits(bitField, *iBit, 5+2, 0x0C<<2); /* write method + 2 VLS bits */
			*iBit += 5+2;
			*iStr += 3; /* skip AI 01 and PI 9	*/
			cnv12(str, iStr, bitField, iBit); /* write PID-12  */
			putBits(bitField, *iBit, 2, (size_t)(str[19]-'0')); /* write D.P.	*/
			*iBit += 2;
			*iStr += 1+4; /* skip check digit   jump price AI  */
		}

		/* look for AI 01[9] + 393[0-3]	 */
		else if ( str[2]=='9' && (strlen((const char *) str ) >= 16+4+3+1 ) &&
			str[16]=='3' && str[17]=='9' && str[18]=='3' &&
			(str[19]>='0' && str[19]<='3'))
		{
			/* method 01101, AI's 01 + 393x[NNN] + G.P.	 */
			putBits(bitField, *iBit, 5+2, 0x0D<<2); /* write method + 2 VLS bits */
			*iBit += 5+2;
			*iStr += 3; /* skip AI 01 and PI 9 */
			cnv12(str, iStr, bitField, iBit); /* write PID-12 */
			putBits(bitField, *iBit, 2, (size_t)(str[19]-'0')); /* write D.P.	*/
			*iBit += 2;
			*iStr += 1+4; /* skip check digit & jump price AI  */
			strncpy((char *)numStr, (char *)&str[20], 3); /* ISO country code	*/
			numStr[3] = '\0';
			putBits( bitField, *iBit, 10, (int) atol( (char *)numStr ) ); /* write ISO c.c. */
			*iBit += 10;
			*iStr += 3; /* jump ISO country code  */
		}

		/* look for fixed length with AI 01[9] + 310x 320x[0-099999]   */
		else if (str[2]=='9' && ( strlen( (const char *) str) == 16+10 ) &&
			str[16]=='3' && (str[17]=='1' || str[17]=='2') && str[18]=='0' &&
			( weight = atol( (char *) numStr )) <=99999L )
		{
			/* methods 0111000-0111001, AI s 01 + 3x0x no date */
			bits = 0x34+(str[17]-'1');
			putBits(bitField, *iBit, 7, bits); /* write method */
			*iBit += 7;
			*iStr += 3; /* skip AI 01 and PI 9 */
			cnv12(str, iStr, bitField, iBit); /* write PID-12 */
			weight = weight + ((long)(str[19] - '0') * 100000L); /* decimal digit  */
			putBits(bitField, *iBit, 4, (size_t)(weight>>16)); /* write weight   */
			putBits(bitField, *iBit+4, 16, (size_t)(weight&0xFFFF));
			*iBit += 20;
			*iStr += 10; /* jump weight field  */
			putBits(bitField, *iBit, 16, (size_t)38400); /* write no date	*/
			*iBit += 16;
		}

		/* look for fixed length + AI 01[9] + 310x/320x[0-099999] + 11/13/15/17	 */
		else if (str[2]=='9' && strlen( (const char *) str) == 16+10+8 &&
			str[16]=='3' && (str[17]=='1' || str[17]=='2') && str[18]=='0' &&
			(weight=atol( (char *) numStr )) <=99999L &&
			str[26]=='1' &&
			(str[27]=='1' || str[27]=='3' || str[27]=='5' || str[27]=='7'))
		{
			/* methods 0111000-0111111, AI s 01 + 3x0x + 1x	  */
			bits = 0x38+(str[27]-'1')+(str[17] - '1');
			putBits(bitField, *iBit, 7, bits); /* write method	*/
			*iBit += 7;
			*iStr += 3; /* skip AI 01 and PI 9	*/
			cnv12(str, iStr, bitField, iBit); /* write PID-12  */
			weight = weight + ((long)(str[19] - '0') * 100000L); /* decimal digit */
			putBits(bitField, *iBit, 4, (size_t)(weight>>16)); /* write weight  */
			putBits(bitField, *iBit+4, 16, (size_t)(weight&0xFFFF));
			*iBit += 20;
			*iStr += 11; /* jump check digit & weight field	 */
			str[26] = '1'; /* restore string's data	 */
			putBits(bitField, *iBit, 16, yymmdd(&str[*iStr+2])); /* write date */
			*iBit += 16;
			*iStr += 8; /* date field  */
		}
		else
		{
			/* method 1 (plus 2-bit variable lng sym bit fld), AI 01  */
			putBits(bitField, *iBit, 1+2, 1<<2); /* write method + 2 VLS bits */
			*iBit += 1+2;
			*iStr += 2;
			cnv13(str, iStr, bitField, iBit);
			*iStr += 1; /*skip check digit */
		}
	}
	else
	{
		/* method 00 (plus 2-bit variable lng sym bit fld), not AI 01  */
		putBits(bitField, *iBit, (2+2), 0); /* write method + 2 VLSB bits	*/
		*iBit += 2+2;
	}
	return(NUM_MODE);
}

size_t Rss14::yymmdd(unsigned char str[])
{

	size_t val;

	val = ((size_t)(str[0]-'0')*10 +
					(size_t)(str[1]-'0')) * 384; /* YY  */
	val += ((size_t)(str[2]-'0')*10 +
					(size_t)(str[3]-'0') - 1) * 32; /* MM	 */
	val += (size_t)(str[4]-'0')*10 +
					(size_t)(str[5]-'0'); /* DD	*/
	return(val);
}

/* converts 13 digits to 44 bits */
void Rss14::cnv13 (unsigned char str[], int *iStr, unsigned char bitField[], int *iBit)
{

	int i;

	putBits(bitField, *iBit, 4, (size_t)(str[*iStr] - '0')); /* high order 4 bits	 */
	*iBit += 4;
	*iStr += 1;
	for (i = 0; i < 4 ; i++)
	{
		putBits(bitField, *iBit, 10, (size_t)((size_t)(str[*iStr] - '0')*100 +
							(str[*iStr+1] - '0')*10 +
							str[*iStr+2] - '0')); /* 10 bit groups bits	*/
		*iBit += 10;
		*iStr += 3;
	}
	return;
}

/* converts 12 digits to 40 bits  */
void Rss14::cnv12 (unsigned char str[], int *iStr, unsigned char bitField[], int *iBit)
{
	int i;

	for (i = 0; i < 4 ; i++)
	{
		putBits(bitField, *iBit, 10, (size_t)((size_t)(str[*iStr] - '0')*100 +
							(str[*iStr+1] - '0')*10 +
							str[*iStr+2] - '0')); /* 10 bit groups bits	*/
		*iBit += 10;
		*iStr += 3;
	}
	return;
}

/*
returns number of bits left to closest 2d symbol,
 or -1 if iBit is larger than largest in type and width
 also returns CCSizes index or data char cnt for RSS14E in *size
*/
int Rss14::getUnusedBitCnt(int iBit, int *size)
{

	int i;

	if (linFlag)
	{
		*size = 0;
		if (iBit <= 252)
		{
			if ((*size = (iBit + 11) / 12) < 3)
			{
				*size = 3; /* 3 data sym chars minimum	*/
			}
			if ((((*size)+1+rowWidth) % rowWidth) == 1)
			{
				(*size)++; /* last row minimum of 2	 */
			}
			return(*size*12 - iBit);
		}
	}
	else
	{
		for (i = 0; CCSizes[i] != 0; i++)
		{
			if (iBit <= CCSizes[i])
			{
				*size = i;
				return(CCSizes[i] - iBit);
			}
		}
	}
	return(-1);
}

void Rss14::putBits(unsigned char bitField[], int bitPos, int length, size_t bits)
{
	int i;

	if ((bitPos+length > MAX_BYTES*8) || (length > 16))
	{
		return;
	}

	for (i = length-1; i >= 0; i--)
	{
		if ((bits & 1) != 0)
		{
			bitField[(bitPos+i)/8] |= 0x80 >> ((bitPos+i)%8);
		}
		else
		{
			bitField[(bitPos+i)/8] &= ~(0x80 >> ((bitPos+i)%8));
		}
		bits >>= 1;
	}
	return;
}

