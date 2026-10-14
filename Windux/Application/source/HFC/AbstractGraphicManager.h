/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef ABSTRACTGRAPHICMANAGER_H
#define ABSTRACTGRAPHICMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "GraphicEvent.h"
#include "Graphic.h"
#include "GraphicFileNames.h"
#include "HFCManagerContainer.h"

class SessionMessage;
class DBSession;


class AbstractGraphicManager : public DataManager
{
	public:

		AbstractGraphicManager();
		virtual ~AbstractGraphicManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		short getCurrentSubType() const { return currentSubType; }
		void setCurrentSubType(short subType) { currentSubType = subType; }

		/* DataManager virtual methods. */
		Graphic* getFirstRecord() { return firstGraphic(); }
		Graphic* getNextRecord()  { return nextGraphic(); }
		EvolDeviceType getDevice() const { return GraphicDev; }

		Graphic *setPositionAtGraphic(const HobString &beginFromGraphicName);
		Graphic *firstGraphic();
		Graphic *nextGraphic(bool shouldLoop = false, const HobString &beginFromGraphicName = HobString(""));
		Graphic *previousGraphic(bool shouldLoop = false, const HobString &beginFromGraphicName = HobString(""));
		Graphic *lastGraphic();

		int countAllGraphics();
		int countAllGraphics(short graphicType);

		void handleMessage(SessionMessage *msg);

		virtual bool handleGraphicMessage(SessionMessage * /* msg */) {return false;}
		virtual void handleDeleteAll() {}

		void handleReadAllGraphicNamesPerType(UINT8 myEvent);


	protected:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		GraphicFileNames *graphicFileNames;

		short currentType;
		short currentSubType; // whether we're looking for user graphics, system graphics, or both - applicable to product, category, and label graphics
		hfc::HFCManagerContainer::HFCRecordManagerType myHFCRecordManagerType;

		void handleRead(SessionMessage *msg);
		bool handleRestore(SessionMessage *msg);
		bool handleDelete(SessionMessage *msg);

		virtual const std::string getFullPathToImages()           { return ""; }
		virtual const std::string getRelativePathToImages()       { return ""; }
		virtual const std::string getFullPathToSystemImages()     { return ""; }
		virtual const std::string getRelativePathToSystemImages() { return ""; }


	private:

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //ABSTRACTGRAPHICMANAGER_H
