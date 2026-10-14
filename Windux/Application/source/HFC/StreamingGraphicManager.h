/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STREAMINGGRAPHICMANAGER_H
#define STREAMINGGRAPHICMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "StreamingGraphic.h"

class SessionMessage;
class DBSession;
class StreamingGraphicsList;

#include "DBStreamingGraphicsRM.h"
#include "DBStreamingGraphic.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class StreamingGraphicManager : public DataManager
{
	public:

		StreamingGraphicManager();
		StreamingGraphicManager(hfc::HFCManagerContainer *container);
		virtual ~StreamingGraphicManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		StreamingGraphic* getFirstRecord() { return firstStreamingGraphic(); }
		StreamingGraphic* getNextRecord()  { return nextStreamingGraphic(); }
		EvolDeviceType getDevice() const { return StreamingGraphicDev; }

		StreamingGraphic *firstStreamingGraphic();
		StreamingGraphic *nextStreamingGraphic();
		int countAllStreamingGraphics();

		StreamingGraphic *getCurrentStreamingGraphic();
		void setCurrentStreamingGraphic(StreamingGraphic *newStreamingGraphic);

		StreamingGraphicsList *getCurrentStreamingGraphicsList();
		void setCurrentStreamingGraphicsList(StreamingGraphicsList *newList);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		StreamingGraphic *currentStreamingGraphic;

		edb::DBQuery<edb::DBStreamingGraphic> *allStreamingGraphicsQuery;
		edb::DBQuery<edb::DBStreamingGraphic>::const_iterator *allStreamingGraphicsIter;

		StreamingGraphicsList	*streamingGraphicsList;
		HobRecordNum			lastSlideNumber;

		void init();

		void handleRead(StreamingGraphic *msgStreamingGraphic);
		void handleReadAll();
		void handleWrite(StreamingGraphic *modifiedStreamingGraphic);
		void handleDelete(StreamingGraphic *msgStreamingGraphic);
		void handleSave(StreamingGraphic *modifiedStreamingGraphic);
		void handleRestore(StreamingGraphic *restoreStreamingGraphic);
		void handleReadAllGraphicNames();
		void handleSelectSlide(StreamingGraphic *msgStreamingGraphic);
		void handleMoveSlideUp();
		void handleMoveSlideDown();
		void handleInsertNewSlide();
		void handleDeleteSlide();
		void updateCurrentStreamingGraphic(StreamingGraphic *modifiedSlide, int event);
		HobRecordNum getNextAvailStreamingGraphicNum();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //STREAMINGGRAPHICMANAGER_H
