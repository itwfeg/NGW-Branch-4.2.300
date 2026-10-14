/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "FontEvent.h"
#include "Font.h"
#include "FontFileNames.h"

class SessionMessage;
class Session;


class FontManager : public DataManager
{
	public:

		FontManager();
		FontManager(Session *session);
		virtual ~FontManager();

		Font *setPositionAtFont(const HobString &beginFromFontName);
		Font *firstFont();
		Font *nextFont(bool shouldLoop = false, const HobString &beginFromFontName = HobString(""));
		Font *previousFont(bool shouldLoop = false, const HobString &beginFromFontName = HobString(""));
		Font *lastFont();
		int countAllFonts();
		int countAllCustomFonts();

		bool handleFontMessage(SessionMessage *msg);
		void handleMessage(SessionMessage *msg);

		bool handleDeleteAll();


	private:

		Session *mySession;

		FontFileNames *fontFileNames;

		void init();

		void handleRead(SessionMessage *msg);
		bool handleRestore(Font *restoreFont);
		bool handleDelete(Font *font);
		void handleFirst();
		void handleLast();
		void handleNext(const Font *referenceFont);
		void handlePrevious(const Font *referenceFont);
		void handleReadAllNames();
};
#endif //FONTMANAGER_H
