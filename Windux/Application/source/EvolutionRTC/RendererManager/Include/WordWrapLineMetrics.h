#ifndef WORDWRAPLINEMETRICS_H
#define WORDWRAPLINEMETRICS_H



#include <list>
#include <vector>

#include "PAL_platformheader.h"

#include "CSmartFontHandle.h"



class WordWrapLineMetrics
{
	public:
    #if defined(PAL_WIN32_RUN)
		WordWrapLineMetrics(HDC fontDriver);
		WordWrapLineMetrics(HDC fontDriver, Util::CSmartFontHandle fontId, UINT16 fontAscent);
    #else
		WordWrapLineMetrics(Util::CSmartFontHandle &fontDriver);
		WordWrapLineMetrics(const Util::CSmartFontHandle &fontId, UINT16 fontAscent);
    #endif
		~WordWrapLineMetrics();

		void show();

    #if defined(PAL_WIN32_RUN)
		void setFontDriver(HDC fontDriver);
		HDC getFontDriver();
    #else
		void setFontDriver(const Util::CSmartFontHandle &fontDriver);
		Util::CSmartFontHandle getFontDriver();
    #endif

		void setLineLengthInPixels(UINT16 lineLength);
		UINT16 getLineLengthInPixels();
		void incLineLengthInPixels(UINT16 incLength = 1);

		void setBlockLengthInPixels(UINT16 lineLength);
		UINT16 getBlockLengthInPixels(UINT16 charLocation = 0);
		void incBlockLengthInPixels(UINT16 incLength = 1);

		void setMaxLineHeightInPixels(UINT16 lineHeight);
		UINT16 getMaxLineHeightInPixels();
		void newLineHeight(UINT16 newHeight = 0);
		void newLineHeight(UINT16 newAscent, UINT16 newDescent, UINT16 newLeading);

		UINT16 getMaxLineAscentInPixels() const;
		UINT16 getMaxLineAscentWithoutLeadingInPixels() const;
		UINT16 getMaxLineDescentInPixels() const;

		void setLineLengthInChars(UINT16 lineLength);
		UINT16 getLineLengthInChars();
		void incLineLengthInChars(UINT16 incLength = 1);

		void setTextStart(const char *textStart);
		const char *getTextStart();

		void setTextAreaFull();
		void clearTextAreaFull();
		bool isTextAreaFull();

		UINT16 getNumFonts();

		void addFontMetrics(UINT16 charLocation, const Util::CSmartFontHandle &fontId, UINT16 fontAscent);
		/*lcg rev 0.1 const UGL_FONT_METRICS *getFontMetrics(UINT16 charLocation); */

		Util::CSmartFontHandle getFontId(UINT16 charLocation, UINT16 &lastCharLocation, UINT16 & fontAscent);

//sjjc		UINT16 getMaxFontHeightInPixels() const;
//sjjc		UINT16 getMaxFontAscentInPixels() const;

		void init();
		void startNextLine();

		void addFontChangeWordToLine(WordWrapLineMetrics *newWord);

		void removeLeadingWhitespace();
		void removeTrailingWhitespace();

		void addWhiteSpaceChar(UINT8 whiteSpaceChar);
		void removeWhiteSpaceChar(UINT8 whiteSpaceChar);

		bool isWhiteSpaceChar(UINT8 whiteSpaceChar);

		static const UINT16 MaxLineLengthInChars = 120;

	private:
    #if defined(PAL_WIN32_RUN)
		HDC fontDriver;
    #else
		Util::CSmartFontHandle fontDriver;
    #endif

		UINT16 lineLengthInPixels;
		UINT16 maxLineAscentInPixels;
		UINT16 maxLineAscentWithoutLeadingInPixels;
		UINT16 maxLineDescentInPixels;
		UINT16 maxLineHeightInPixels;
		UINT16 lineLengthInChars;

		char *textStart;
		bool textAreaFull;

		std::list<UINT16>::iterator pixelLengthListPos;
		
		std::list<UINT16> fontChangeCharLoc;
		std::list<UINT16> pixelLengthList;
		std::list<UINT16> fontAscentList;
		std::list<Util::CSmartFontHandle> fontIdList;
		/*lcg rev 0.1 std::list<UGL_FONT_METRICS *> fontMetricsList;*/

		std::vector<UINT8> whiteSpaceChars;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};



#endif //WORDWRAPLINEMETRICS_H
