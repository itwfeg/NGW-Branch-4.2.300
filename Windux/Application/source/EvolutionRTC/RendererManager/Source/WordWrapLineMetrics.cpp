#include <iostream>
#include <algorithm>



#include "WordWrapLineMetrics.h"
#include "PAL_platformheader.h"
//#include <windowsx.h> // replaced by "PAL" header

#include "Debug.h"
#include "util/e_string/utilstring.h"
#include "CRendererUtil.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WordWrapLineMetrics::dataType = 0;
std::string WordWrapLineMetrics::dataTypeName("WordWrapLineMetrics"); 
#endif

using namespace std;
using namespace util_string;


#if defined(PAL_WIN32_RUN)
WordWrapLineMetrics::WordWrapLineMetrics(HDC fontDriver)
#else
WordWrapLineMetrics::WordWrapLineMetrics(Util::CSmartFontHandle &fontDriver)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->fontDriver = fontDriver;

	lineLengthInPixels = 0;
	maxLineAscentInPixels = 0;
	maxLineAscentWithoutLeadingInPixels = 0;
	maxLineDescentInPixels = 0;
	maxLineHeightInPixels = 0;
	lineLengthInChars = 0;
	textStart = NULL;
	textAreaFull = false;

	pixelLengthListPos = pixelLengthList.end();
}

#if defined(PAL_WIN32_RUN)
WordWrapLineMetrics::WordWrapLineMetrics(HDC fontDriver, Util::CSmartFontHandle fontId, UINT16 fontAscent)
#else
WordWrapLineMetrics::WordWrapLineMetrics(const Util::CSmartFontHandle &fontDriver, UINT16 fontAscent)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->fontDriver = fontDriver;

	lineLengthInPixels = 0;
	maxLineAscentInPixels = 0;
	maxLineAscentWithoutLeadingInPixels = 0;
	maxLineDescentInPixels = 0;
	maxLineHeightInPixels = 0;
	lineLengthInChars = 0;
	textStart = NULL;
	textAreaFull = false;

#if defined(PAL_WIN32_RUN)
	addFontMetrics(0, fontId, fontAscent);
#else
	addFontMetrics(0, fontDriver, fontAscent);
#endif
}


WordWrapLineMetrics::~WordWrapLineMetrics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

/*	while (fontIdList.empty() == false)
	{
		HFONT fontId = fontIdList.back();
		fontIdList.pop_back();

		DeleteFont(fontId);
	}
*/
/* //sjjc
	while (fontMetricsList.empty() == false)
	{
		UGL_FONT_METRICS *pFM = fontMetricsList.back();
		fontMetricsList.pop_back();

		delete pFM;
	}
*/
}

void WordWrapLineMetrics::show()
{
	Debug::trace("WordWrapLineMetrics", 20, "WordWrapLineMetrics:");

	Debug::trace("WordWrapLineMetrics", 20, "fontDriver: " + ptrtostring((void*)fontDriver));

	Debug::trace("WordWrapLineMetrics", 20, "lineLengthInPixels: " + uinttostring(lineLengthInPixels));
	Debug::trace("WordWrapLineMetrics", 20, "maxLineAscentInPixels: " + uinttostring(maxLineAscentInPixels));
	Debug::trace("WordWrapLineMetrics", 20, "maxLineAscentWithoutLeadingInPixels: " + uinttostring(maxLineAscentWithoutLeadingInPixels));
	Debug::trace("WordWrapLineMetrics", 20, "maxLineDescentInPixels: " + uinttostring(maxLineDescentInPixels));
	Debug::trace("WordWrapLineMetrics", 20, "maxLineHeightInPixels: " + uinttostring(maxLineHeightInPixels));
	Debug::trace("WordWrapLineMetrics", 20, "lineLengthInChars: " + uinttostring(lineLengthInChars));

	Debug::trace("WordWrapLineMetrics", 20, "textAreaFull: " + booltostring(textAreaFull));

	list<UINT16>::iterator charLocPos = fontChangeCharLoc.begin();
	list<Util::CSmartFontHandle>::iterator fontIdPos = fontIdList.begin();
	list<UINT16>::iterator fontAscentPos = fontAscentList.begin();
/* //sjjc	list<UGL_FONT_METRICS *>::iterator fontMetricsPos = fontMetricsList.begin(); */
	list<UINT16>::iterator tempPixelLengthListPos = pixelLengthList.begin();

	Debug::trace("WordWrapLineMetrics", 20, "numFonts: " + inttostring(fontChangeCharLoc.size()) +
			", " + inttostring(fontIdList.size()) + ", " + inttostring(fontAscentList.size()) + //sjjc ", " + inttostring(fontMetricsList.size()) +
			", " + inttostring(pixelLengthList.size()));

	while (charLocPos != fontChangeCharLoc.end())
	{
		Debug::trace("WordWrapLineMetrics", 20, "Font change encountered ----------------------");
		Debug::trace("WordWrapLineMetrics", 20, "Charactor Location: " + uinttostring(*charLocPos));
		Debug::trace("WordWrapLineMetrics", 20, "Font ID: " + ptrtostring((void*)(*fontIdPos)));
		Debug::trace("WordWrapLineMetrics", 20, "Font Ascent: " + uinttostring(*fontAscentPos));
		Debug::trace("WordWrapLineMetrics", 20, "pixelLength: " + uinttostring(*tempPixelLengthListPos));

/* //sjjc		UGL_FONT_METRICS *pFontMetrics = *fontMetricsPos;
		Debug::trace("WordWrapLineMetrics", 20, "FontName: " + string(pFontMetrics->faceName) +
				" Pixel Size: " + inttostring(pFontMetrics->pixelSize));
		Debug::trace("WordWrapLineMetrics", 20, "Font height: " + inttostring(pFontMetrics->height));
		Debug::trace("WordWrapLineMetrics", 20, "Font maxAscent: " + inttostring(pFontMetrics->maxAscent));
		Debug::trace("WordWrapLineMetrics", 20, "Font maxDescent: " + inttostring(pFontMetrics->maxDescent));
		Debug::trace("WordWrapLineMetrics", 20, "Font maxAdvance: " + inttostring(pFontMetrics->maxAdvance));
		Debug::trace("WordWrapLineMetrics", 20, "Font leading: " + inttostring(pFontMetrics->leading));
*/
		charLocPos++;
		fontIdPos++;
		fontAscentPos++;
//sjjc		fontMetricsPos++;
		tempPixelLengthListPos++;
	}
}

#if defined(PAL_WIN32_RUN)
void WordWrapLineMetrics::setFontDriver(HDC fontDriver)
#else
void WordWrapLineMetrics::setFontDriver(const Util::CSmartFontHandle &fontDriver)
#endif
{
	this->fontDriver = fontDriver;
}

#if defined(PAL_WIN32_RUN)
HDC WordWrapLineMetrics::getFontDriver()
#else
Util::CSmartFontHandle WordWrapLineMetrics::getFontDriver()
#endif
{
	return fontDriver;
}

void WordWrapLineMetrics::setLineLengthInPixels(UINT16 lineLength)
{
	lineLengthInPixels = lineLength;
}

UINT16 WordWrapLineMetrics::getLineLengthInPixels()
{
	return lineLengthInPixels;
}

void WordWrapLineMetrics::incLineLengthInPixels(UINT16 incLength)
{
	lineLengthInPixels = lineLengthInPixels + incLength;
}

void WordWrapLineMetrics::setBlockLengthInPixels(UINT16 lineLength)
{
	*pixelLengthListPos = lineLength;
}

UINT16 WordWrapLineMetrics::getBlockLengthInPixels(UINT16 charLocation)
{
	UINT16 blockLength = 0;

	list<UINT16>::iterator charLocPos = fontChangeCharLoc.begin();
	list<UINT16>::iterator tempIter = pixelLengthList.begin();

	while ((charLocPos != fontChangeCharLoc.end()) && (*charLocPos < charLocation))
	{
		charLocPos++;
		tempIter++;
	}

	if (tempIter != pixelLengthList.end())
	{
		blockLength = *pixelLengthListPos;
	}

	return blockLength;
}

void WordWrapLineMetrics::incBlockLengthInPixels(UINT16 incLength)
{
	*pixelLengthListPos = *pixelLengthListPos + incLength;
}

void WordWrapLineMetrics::setMaxLineHeightInPixels(UINT16 lineHeight)
{
	maxLineHeightInPixels = lineHeight;
}

UINT16 WordWrapLineMetrics::getMaxLineHeightInPixels()
{
	return maxLineHeightInPixels;
}

void WordWrapLineMetrics::newLineHeight(UINT16 newHeight)
{
	if (newHeight > maxLineHeightInPixels)
	{
		maxLineHeightInPixels = newHeight;
	}
}

void WordWrapLineMetrics::newLineHeight(UINT16 newAscent, UINT16 newDescent, UINT16 newLeading)
{
	UINT16 newAscentWithoutLeading = newAscent - newLeading;

	if ( newAscent > maxLineAscentInPixels )
		maxLineAscentInPixels = newAscent;

	if ( newAscentWithoutLeading > maxLineAscentWithoutLeadingInPixels )
		maxLineAscentWithoutLeadingInPixels = newAscentWithoutLeading;

	if ( newDescent > maxLineDescentInPixels )
		maxLineDescentInPixels = newDescent;

	if ( maxLineAscentInPixels + maxLineDescentInPixels > maxLineHeightInPixels )
		maxLineHeightInPixels = maxLineAscentInPixels + maxLineDescentInPixels;
}


UINT16 WordWrapLineMetrics::getMaxLineAscentInPixels() const
{
	return maxLineAscentInPixels;
}


UINT16 WordWrapLineMetrics::getMaxLineAscentWithoutLeadingInPixels() const
{
	return maxLineAscentWithoutLeadingInPixels;
}


UINT16 WordWrapLineMetrics::getMaxLineDescentInPixels() const
{
	return maxLineDescentInPixels;
}


void WordWrapLineMetrics::setLineLengthInChars(UINT16 lineLength)
{
	lineLengthInChars = lineLength;
}

UINT16 WordWrapLineMetrics::getLineLengthInChars()
{
	return lineLengthInChars;
}

void WordWrapLineMetrics::incLineLengthInChars(UINT16 incLength)
{
	lineLengthInChars = lineLengthInChars + incLength;
}

void WordWrapLineMetrics::setTextStart(const char *textStart)
{
	this->textStart = (char *) textStart;
}

const char *WordWrapLineMetrics::getTextStart()
{
	return (const char *) textStart;
}

void WordWrapLineMetrics::setTextAreaFull()
{
	textAreaFull = true;
}

void WordWrapLineMetrics::clearTextAreaFull()
{
	textAreaFull = false;
}

bool WordWrapLineMetrics::isTextAreaFull()
{
	return textAreaFull;
}

UINT16 WordWrapLineMetrics::getNumFonts()
{
	return (UINT16)fontIdList.size();
}

// We want to keep the fonts in order with characters,
// so we must find the correct place
// to insert this font first!!
void WordWrapLineMetrics::addFontMetrics(UINT16 charLocation, const Util::CSmartFontHandle &fontId, UINT16 fontAscent)
{
//sjjc	UGL_FONT_METRICS *pNewFM = new UGL_FONT_METRICS;
//sjjc	uglFontMetricsGet(fontId, pNewFM);

	list<UINT16>::iterator charLocPos = fontChangeCharLoc.begin();
	list<Util::CSmartFontHandle>::iterator fontIdPos = fontIdList.begin();
	list<UINT16>::iterator fontAscentPos = fontAscentList.begin();
//sjjc	list<UGL_FONT_METRICS *>::iterator fontMetricsPos = fontMetricsList.begin();
	pixelLengthListPos = pixelLengthList.begin();

	while ((charLocPos != fontChangeCharLoc.end()) && (*charLocPos < charLocation))
	{
		charLocPos++;
		fontIdPos++;
		fontAscentPos++;
//sjjc		fontMetricsPos++;
		pixelLengthListPos++;
	}
	
	// if there is already a font at this position, remove it and
	// insert the new one
	if ((charLocPos != fontChangeCharLoc.end()) && (*charLocPos == charLocation))
	{
		charLocPos = fontChangeCharLoc.erase(charLocPos);
		fontIdPos = fontIdList.erase(fontIdPos);
		fontAscentPos = fontAscentList.erase(fontAscentPos);
		pixelLengthListPos = pixelLengthList.erase(pixelLengthListPos);

//sjjc		UGL_FONT_METRICS *pFM = *fontMetricsPos;
//sjjc		delete pFM;

//sjjc		fontMetricsPos = fontMetricsList.erase(fontMetricsPos);
	}

	fontChangeCharLoc.insert(charLocPos, charLocation);
	fontIdList.insert(fontIdPos, fontId);
	fontAscentList.insert(fontAscentPos, fontAscent);
//sjjc	fontMetricsList.insert(fontMetricsPos, pNewFM);

	pixelLengthListPos = pixelLengthList.insert(pixelLengthListPos, 0);
}
/* //sjjc
const UGL_FONT_METRICS *WordWrapLineMetrics::getFontMetrics(UINT16 charLocation)
{
	UGL_FONT_METRICS *pFM = NULL;

	list<UINT16>::iterator charLocPos = fontChangeCharLoc.begin();
	list<UGL_FONT_METRICS *>::iterator fontMetricsPos = fontMetricsList.begin();
	pixelLengthListPos = pixelLengthList.begin();

	while ((charLocPos != fontChangeCharLoc.end()) && (*charLocPos <= charLocation))
	{
		charLocPos++;
		fontMetricsPos++;
		pixelLengthListPos++;
	}

	if (fontMetricsPos != fontMetricsList.end())
	{
		pFM = *fontMetricsPos;
	}

	return (const UGL_FONT_METRICS *) pFM;
}
*/
Util::CSmartFontHandle WordWrapLineMetrics::getFontId(UINT16 charLocation, UINT16 &lastCharLocation, UINT16 & fontAscent)
{
	Util::CSmartFontHandle fontId;

	list<UINT16>::iterator charLocPos = fontChangeCharLoc.begin();
	list<Util::CSmartFontHandle>::iterator fontIdPos = fontIdList.begin();
	list<UINT16>::iterator fontAscentPos = fontAscentList.begin();
	pixelLengthListPos = pixelLengthList.begin();

	list<UINT16>::iterator charLocPosPrev = fontChangeCharLoc.begin();
	list<Util::CSmartFontHandle>::iterator fontIdPosPrev = fontIdList.begin();
	list<UINT16>::iterator fontAscentPosPrev = fontAscentList.begin();

	charLocPos++;
	fontIdPos++;
	fontAscentPos++;

	while ((charLocPos != fontChangeCharLoc.end()) && (*charLocPos <= charLocation))
	{
		charLocPos++;
		fontIdPos++;
		fontAscentPos++;
		pixelLengthListPos++;

		charLocPosPrev++;
		fontIdPosPrev++;
		fontAscentPosPrev++;
	}

	fontId = *fontIdPosPrev;
	fontAscent = *fontAscentPosPrev;

	if (charLocPos == fontChangeCharLoc.end())
	{
		if ( 0 < lineLengthInChars )
			lastCharLocation = lineLengthInChars - 1; // 0 .. lineLengthInChars - 1
	}
	else
	{
		lastCharLocation = *charLocPos - 1;
	}

	return fontId;
}

void WordWrapLineMetrics::init()
{
	lineLengthInPixels = 0;
	maxLineAscentInPixels = 0;
	maxLineAscentWithoutLeadingInPixels = 0;
	maxLineDescentInPixels = 0;
	maxLineHeightInPixels = 0;
	lineLengthInChars = 0;
	textStart = NULL;
	textAreaFull = false;

	fontChangeCharLoc.clear();
	fontIdList.clear();
	fontAscentList.clear();

/*	while (fontIdList.empty() == false)
	{
		Util::CSmartFontHandle fontId = fontIdList.back();
		fontIdList.pop_back();

		DeleteFont(fontId);
	}
*/
/* //sjjc
	while (fontMetricsList.empty() == false)
	{
		UGL_FONT_METRICS *pFM = fontMetricsList.back();
		fontMetricsList.pop_back();

		delete pFM;
	}
*/
	pixelLengthList.clear();
	pixelLengthListPos = pixelLengthList.end();
}

void WordWrapLineMetrics::startNextLine()
{
	Util::CSmartFontHandle saveFontId = fontIdList.back();
	UINT16 saveFontAscent = fontAscentList.back();
//	fontIdList.pop_back();

	init();

	addFontMetrics(0, saveFontId, saveFontAscent);
}

void WordWrapLineMetrics::addFontChangeWordToLine(WordWrapLineMetrics *newWord)
{
	// update stati for new word!!

	lineLengthInPixels = lineLengthInPixels + newWord->getLineLengthInPixels();

	if ( newWord->getMaxLineAscentInPixels() > maxLineAscentInPixels )
		maxLineAscentInPixels = newWord->getMaxLineAscentInPixels();

	if ( newWord->getMaxLineAscentWithoutLeadingInPixels() > maxLineAscentWithoutLeadingInPixels )
		maxLineAscentWithoutLeadingInPixels = newWord->getMaxLineAscentWithoutLeadingInPixels();

	if ( newWord->getMaxLineDescentInPixels() > maxLineDescentInPixels )
		maxLineDescentInPixels = newWord->getMaxLineDescentInPixels();

	if (newWord->getMaxLineHeightInPixels() > maxLineHeightInPixels)
		maxLineHeightInPixels = newWord->getMaxLineHeightInPixels();

	// Add Fonts from new word to this LineMetrics!!!
	UINT16 lastCharLoc = 0;
	UINT16 fontLocation = 0;

	UINT16 ii = 0;
	while (ii < newWord->getNumFonts())
	{
		// just print the entire line with the first font!!
		UINT16 fontAscent = 0;
		Util::CSmartFontHandle fontId = newWord->getFontId(fontLocation, lastCharLoc, fontAscent);
		
		addFontMetrics(lineLengthInChars, fontId, fontAscent);
		setBlockLengthInPixels(newWord->getBlockLengthInPixels(fontLocation));

		lineLengthInChars += (lastCharLoc - fontLocation) + 1;

		fontLocation = lastCharLoc + 1;
		ii++;
	}

	if (newWord->isTextAreaFull() == true)
	{
		textAreaFull = true;
	}
}

void WordWrapLineMetrics::addWhiteSpaceChar(UINT8 whiteSpaceChar)
{
	if (whiteSpaceChars.size() > 0)
	{
		if (find(whiteSpaceChars.begin(), whiteSpaceChars.end(), whiteSpaceChar) == whiteSpaceChars.end())
		{
			whiteSpaceChars.push_back(whiteSpaceChar);
		}
	}
	else
	{
		whiteSpaceChars.push_back(whiteSpaceChar);
	}
}

void WordWrapLineMetrics::removeWhiteSpaceChar(UINT8 whiteSpaceChar)
{
	if (whiteSpaceChars.size() > 0)
	{
		vector<UINT8>::iterator pos = find(whiteSpaceChars.begin(), whiteSpaceChars.end(), whiteSpaceChar);

		if ( pos != whiteSpaceChars.end())
		{
			whiteSpaceChars.erase(pos);
		}
	}
}

bool WordWrapLineMetrics::isWhiteSpaceChar(UINT8 whiteSpaceChar)
{
	bool rv = false;

	if (whiteSpaceChars.size() > 0)
	{
		vector<UINT8>::iterator pos = find(whiteSpaceChars.begin(), whiteSpaceChars.end(), whiteSpaceChar);
		if ( pos != whiteSpaceChars.end())
		{
			rv = true;
		}
	}

	return rv;
}

void WordWrapLineMetrics::removeLeadingWhitespace()
{
	while (isWhiteSpaceChar(textStart[0]) == true)
	{
		textStart++;
	}
}

void WordWrapLineMetrics::removeTrailingWhitespace()
{
	// Needs to get character size for correct font and update
	// lists with new values.
}


/* UINT16 WordWrapLineMetrics::getMaxFontHeightInPixels() const
{
	UINT16 maxFontHeight = 0;
//sjjc	list<UGL_FONT_METRICS *>::const_iterator fontMetricsIter = fontMetricsList.begin();

	Debug::trace("WordWrapLineMetrics", 20, "getMaxFontHeightInPixels: Calculating maximum (currently " +
			uinttostring(maxFontHeight) + ")");

	while ( fontMetricsIter != fontMetricsList.end() )
	{
		UGL_FONT_METRICS *pFontMetrics = *fontMetricsIter;

		Debug::trace("WordWrapLineMetrics", 20, "getMaxFontHeightInPixels: Current font's height is " +
				uinttostring(pFontMetrics->height + pFontMetrics->leading));

		if ( maxFontHeight < pFontMetrics->height + pFontMetrics->leading )
		{
			maxFontHeight = pFontMetrics->height + pFontMetrics->leading;

			Debug::trace("WordWrapLineMetrics", 20, "getMaxFontHeightInPixels: New maximum is " +
					uinttostring(maxFontHeight));
		}

		++fontMetricsIter;
	}

	return maxFontHeight;
}
*/

/*UINT16 WordWrapLineMetrics::getMaxFontAscentInPixels() const
{
	UINT16 maxFontAscent = 0;
//sjjc	list<UGL_FONT_METRICS *>::const_iterator fontMetricsIter = fontMetricsList.begin();

	while ( fontMetricsIter != fontMetricsList.end() )
	{
		UGL_FONT_METRICS *pFontMetrics = *fontMetricsIter;

		if ( maxFontAscent < pFontMetrics->maxAscent )
			maxFontAscent = pFontMetrics->maxAscent;

		++fontMetricsIter;
	}

	return maxFontAscent;
}
*/
