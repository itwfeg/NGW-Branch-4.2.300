/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GRAPHICMANAGER_H
#define GRAPHICMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "GraphicEvent.h"
#include "Graphic.h"
#include "GraphicFileNames.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class GraphicManager : public DataManager
{
	public:

		GraphicManager();
		GraphicManager(hfc::HFCManagerContainer *container);
		virtual ~GraphicManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		bool handleGraphicMessage(SessionMessage *msg);
		void handleMessage(SessionMessage *msg);

		//void importGraphicType(const string& baseSrcDir) {}
		void importGraphicType(short graphicType, const std::string& baseSrcDir, bool moveFile);

		void copyGraphicFile(const HobString &fileNamePath, const HobString &destDir, short graphicType = Graphic::TYPE_UNKNOWN);
		void moveGraphicFile(const HobString &fileNamePath, const HobString &destDir, short graphicType = Graphic::TYPE_UNKNOWN);

		void handleReadAllGraphicNamesPerType(UINT8 myEvent, short type);

	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		short currentType;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //GRAPHICMANAGER_H
