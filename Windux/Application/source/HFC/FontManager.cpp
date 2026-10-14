/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FontManager.h"
#include "HobString.h"
#include "SystemFontsList.h"
#include "HfcErrorFont.h"
#include "SessionMessage.h"
#include "Session.h"
#include "ResponseMessageManager.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

LOCAL int dataType = 0;
LOCAL string dataTypeName = "FontManager";
#endif

FontManager::FontManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = registerDataType(dataTypeName);

	addInstance(dataType);
#endif

	Debug::trace("FontManager", "FontManager()");
	init();
}

FontManager::FontManager(Session *session)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = registerDataType(dataTypeName);

	addInstance(dataType);
#endif

	Debug::trace("FontManager", "FontManager(Session *session)");
	init();
	mySession = session;
}

FontManager::~FontManager()
{
#ifdef USE_ACCOUNTANT
	removeInstance(dataType);
#endif

	Debug::trace("FontManager", "~FontManager()");
}

// private
void FontManager::init()
{
	mySession = NULL;
	fontFileNames = (FontFileNames *)NULL;
}


Font *FontManager::firstFont()
{
	if (fontFileNames)
	{
		delete fontFileNames;
		fontFileNames = (FontFileNames *)NULL;
	}
	fontFileNames = new FontFileNames(); // Font::FONTTYPE_CUSTOM this is the only way that fonts are returned

	Font *font = (Font *)NULL;
	HobString fontName;
	//fontName = fontName.substr(0, fontName.size() - 2);
	int found = fontFileNames->first(fontName);
	if (found >= 0)
		font = new Font(fontName);
	return font;
}

/* If the last record was not read, this method will return NULL.
 */
Font *FontManager::lastFont()
{
	if (fontFileNames)
	{
		delete fontFileNames;
		fontFileNames = (FontFileNames *)NULL;
	}
	fontFileNames = new FontFileNames(Font::FONTTYPE_ALL);

	Font *font = (Font *)NULL;
	HobString fontName;
	//fontName = fontName.substr(0, fontName.size() - 2);
	int found = fontFileNames->last(fontName);
	if (found >= 0)
		font = new Font(fontName);
	return font;
}


Font *FontManager::setPositionAtFont(const HobString &beginFromFontName)
{
	if (beginFromFontName.size())
	{
		Font *current = firstFont();
		bool foundBeginFrom = false;
		while (true)
		{
			if (!current)
				break;

			// Need to check >= in the following check because the requested one may not exist.
			// If it doesn't, then stop looking when the found font name is greater than
			// (since they are sorted alphabetically) the one requested.
			std::string name = current->getFontNameBase();
			if (name >= beginFromFontName)
			{
				foundBeginFrom = true;
				break;
			}
			if (current)
			{
				delete current;
				current = (Font *)NULL;
			}

			/* Don't loop on the following line, because if they request font name 'zoro', but the 'highest' name
			 * is 'yoyo', this loop will never end, since we'll keep finding next font names, but none will have
			 * a 'higher' name (or equal to) to break out of the loop.
			 */
			current = nextFont(false);
		}
		if (foundBeginFrom)
			return current;
	}
	return (Font *)NULL;
}


Font *FontManager::nextFont(bool shouldLoop = false, const HobString &beginFromFontName)
{
	Font *fg = (Font *)NULL;
	if (!fontFileNames)
		fg = firstFont();

	if (beginFromFontName.size())
	{
		Font *current = setPositionAtFont(beginFromFontName);
		if (current)
		{
			/* If the font found has a font name greater than the one requested,
			 * then this is the next one, go ahead and return it.
			 */
			std::string name = current->getFontNameBase();
			if (name > beginFromFontName)
				return current;
			else
				// Found the one requested; delete it so we can go to the next one.
				delete current;
		}
		else
		{
			if (shouldLoop)
				return firstFont();
			else
				return (Font *)NULL;
		}
	}
	else if (fg)
		return fg;

	Font *font = (Font *)NULL;
	HobString fontName("");
	int found = fontFileNames->next(fontName);
	if (found < 0)
	{
		if (shouldLoop)
			return firstFont();
		else
		{
			/* reclaim the memory from the query and the iterator used to traverse all LabelTypes in the db. */
			delete fontFileNames;
			fontFileNames = (FontFileNames *)NULL;

			return (Font *)NULL;
		}
	}

	font = new Font(fontName);
	return font;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstLabelType() method, the first call to this method will
 * result in the first record being returned.
 */
Font *FontManager::previousFont(bool shouldLoop, const HobString &beginFromFontName)
{
	Font *fg = (Font *)NULL;
	if (!fontFileNames)
		fg = firstFont();

	if (beginFromFontName.size())
	{
		Font *current = setPositionAtFont(beginFromFontName);
		if (current)
				delete current;
		else
			/* If we didn't find one from the setPositionAt...(), then we went past the end before finding
			 * the one requested, so return the last one.
			 */
			return lastFont();
	}
	else if (fg)
		return fg;

	Font *font = (Font *)NULL;
	HobString fontName("");
	int found = fontFileNames->previous(fontName);
	if (found < 0)
	{
		if (shouldLoop)
			return lastFont();
		else
		{
			/* reclaim the memory from the query and the iterator used to traverse all Fonts. */
			delete fontFileNames;
			fontFileNames = (FontFileNames *)NULL;

			return (Font *)NULL;
		}
	}

	font = new Font(fontName);
	return font;
}


int FontManager::countAllFonts()
{
	int count = 0;
	FontFileNames *ffn = new FontFileNames(Font::FONTTYPE_ALL);
	if (ffn)
	{
		count = ffn->size();
		delete ffn;
	}
	return count;
}

int FontManager::countAllCustomFonts()
{
	int count = 0;
	FontFileNames *ffn = new FontFileNames(Font::FONTTYPE_CUSTOM);
	if (ffn)
	{
		count = ffn->size();
		delete ffn;
	}
	return count;
}

void FontManager::handleMessage(SessionMessage *msg)
{
	handleFontMessage(msg);
}

bool FontManager::handleFontMessage(SessionMessage *msg)
{
	bool rv = false;
	// Valid as long as no process calls this method with a message containing
	// some other device than FontDev.
	if (!msg)
		return rv;

	switch(msg->getEventType())
	{
		case DtRead:
			Debug::trace("FontManager", "handling DtRead event");
			handleRead(msg);
			break;

		case DtDelete:
		{
			Debug::trace("FontManager", "handling DtDelete event");
			Font *deleteFont = dynamic_cast<Font *> (msg->getHfcObject());
			rv = handleDelete(deleteFont);
			break;
		}

		case DtDeleteAll:
			Debug::trace("FontManager", "handling DtDeleteAll event");
			rv = handleDeleteAll();
			break;

		case DtRestore:
		{
			Debug::trace("FontManager", "handling DtRestore event");
			Font *myFont = dynamic_cast<Font *> (msg->getHfcObject());
			if (myFont)
				rv = handleRestore(myFont);
			break;
		}

		case DtFirst:
			Debug::trace("FontManager", "handling DtFirst event");
			handleFirst();
			break;

		case DtLast:
			Debug::trace("FontManager", "handling DtLast event");
			handleLast();
			break;

		case DtNext:
		{
			Debug::trace("FontManager", "handling DtNext event");
			Font *referenceFont = dynamic_cast<Font *> (msg->getHfcObject());
			handleNext(referenceFont);
			break;
		}

		case DtPrevious:
		{
			Debug::trace("FontManager", "handling DtPrevious event");
			Font *referenceFont = dynamic_cast<Font *> (msg->getHfcObject());
			handlePrevious(referenceFont);
			break;
		}

		case FontReadAllNames:
			Debug::trace("FontManager", "handling FontReadAllNames event");
			handleReadAllNames();
			break;

		default:
			break;
	}

	return rv;
}

void FontManager::handleRead(SessionMessage *msg)
{
	if (!msg)
		return;

	const Font *font = dynamic_cast<const Font *> (msg->getHfcObject());
	Font *lookedUpFont = Font::findFont(font->getFontName());

	if (lookedUpFont)
	{
		if (lookedUpFont->getErrorCode() != RECORD_OK)
		{
			// Send error message to browser indicating Font not found.
			if (mySession && mySession->getResponseMessageManager())
				mySession->getResponseMessageManager()->addMessage(ErrFontDev, FontErr, HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			// We have a new Font.
			{
				// Send the Font found back to the Browser.
				SessionMessage fontData(FontDev, DtWrite);
				fontData.setFormatType(BrowserFormat);
				fontData.setHfcObject(lookedUpFont);

				// Send the Font to the UI (Browser)
				int bytesSent = mySession->send(&fontData, Messenger::DEST_UI);
				if (bytesSent < 0)
				{
					// todo - check correct number of bytes sent?
				}
			}
		}
		delete lookedUpFont;
	}
}


bool FontManager::handleDelete(Font *font)
{
	bool rv = false;

	if (font)
		rv = font->deleteRecord();
	else
		Debug::trace("FontManager", "handleDelete - font object is null");

	return rv;
}


bool FontManager::handleDeleteAll()
{
	bool rv = false;

	Debug::trace("FontManager", "handleDeleteAll - deleting all custom fonts");
	Font *pFont = firstFont();
	while (pFont)
	{
		if (pFont->isCustom())
		{
			rv |= pFont->deleteRecord();
		}
		delete pFont;
		pFont = nextFont();
	}

	return rv;
}


bool FontManager::handleRestore(Font *restoreFont)
{
	bool rv = false;

	if (restoreFont)
		rv = restoreFont->saveRecord();
	else
		Debug::trace("FontManager", "handleRestoreFont - font message object is null");

	return rv;
}


void FontManager::handleFirst()
{
	if (!mySession)
		return;

	Font *font = firstFont();
	if (!font)
		font = new Font();

	// Send the Font to the UI (Browser)
	SessionMessage gMsg(FontDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(font);
	int bytesSent = mySession->send(&gMsg, Messenger::DEST_UI);
	if (bytesSent < 0)
	{
		// todo - check correct number of bytes sent?
	}

	if (font)
		delete font;
}

void FontManager::handleLast()
{
	if (!mySession)
		return;

	Font *font = lastFont();
	if (!font)
		font = new Font();

	// Send the Font to the UI (Browser)
	SessionMessage gMsg(FontDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(font);
	int bytesSent = mySession->send(&gMsg, Messenger::DEST_UI);
	if (bytesSent < 0)
	{
		// todo - check correct number of bytes sent?
	}

	if (font)
		delete font;
}

void FontManager::handleNext(const Font *referenceFont)
{
	if (!mySession)
		return;

	Font *font = nextFont(true, (referenceFont) ? referenceFont->getFontName() : HobString(""));
	if (!font)
		font = new Font();

	// Send the Font to the UI (Browser)
	SessionMessage gMsg(FontDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(font);
	int bytesSent = mySession->send(&gMsg, Messenger::DEST_UI);
	if (bytesSent < 0)
	{
		// todo - check correct number of bytes sent?
	}

	if (font)
		delete font;
}

void FontManager::handlePrevious(const Font *referenceFont)
{
	if (!mySession)
		return;

	Font *font = previousFont(true, (referenceFont) ? referenceFont->getFontName() : HobString(""));
	if (!font)
		font = new Font();

	// Send the Font to the UI (Browser)
	SessionMessage gMsg(FontDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(font);
	int bytesSent = mySession->send(&gMsg, Messenger::DEST_UI);
	if (bytesSent < 0)
	{
		// todo - check correct number of bytes sent?
	}

	if (font)
		delete font;
}

void FontManager::handleReadAllNames()
{
	HobList fList;
	SystemFontsList sflList;
	char *fname;

	fname = sflList.getFontName();
	while (fname)
	{
		fList.push_back(HobString(fname));
		fname = sflList.getFontName();
	}

	SessionMessage namesMsg(FontDev, FontWriteAllNames);
	namesMsg.setFormatType(BrowserFormat);
	namesMsg.setHfcObject(&fList);

	int bytesSent = mySession->send(&namesMsg, Messenger::DEST_UI);
	if (bytesSent < 0)
	{
		// todo - check correct number of bytes sent?
	}
/*
	FontFileNames *ffn = new FontFileNames();
	if (ffn)
	{
		if (ffn->getListCount() > 0)
		{
			// Send the Font file names found back to the Browser.
			SessionMessage nameData(FontDev, FontReadAllNames);
			nameData.setFormatType(BrowserFormat);
			nameData.setHfcObject(ffn);

			int bytesSent = mySession->send(&nameData, Messenger::DEST_UI);
			if (bytesSent < 0)
			{
				// todo - check correct number of bytes sent?
			}
		}
		else
		{
			//	Send a message indicating no files were found in this directory.
		}
		delete ffn;
	}
*/
}
/*
void Session::handleFontMessage(SessionMessage *msg)
{
	Debug::trace("Session", getName() + HobString(" - handling Font DataType message"));

	if (!msg)
		return;

	switch(msg->getEventType())
	{
		case FontReadAllNames:
		{
			HobList fList;
			SystemFontsList sflList;
			char *fname;

			fname = sflList.getFontName();
			while (fname)
			{
				fList.push_back(HobString(fname));
				fname = sflList.getFontName();
			}

			SessionMessage namesMsg(FontDev, FontWriteAllNames);
			namesMsg.setFormatType(BrowserFormat);
			namesMsg.setHfcObject(&fList);

			int bytesSent = send(&namesMsg, Messenger::DEST_UI);
			if (bytesSent < 0)
			{
				// todo - check correct number of bytes sent?
			}
		}
			break;

		default:
			break;
	}
}
*/
