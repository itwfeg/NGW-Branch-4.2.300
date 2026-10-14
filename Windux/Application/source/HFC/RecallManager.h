/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RECALLMANAGER_H
#define RECALLMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "RecallEvent.h"
#include "Recall.h"
#include "FeaturedCategory.h"

class SessionMessage;
class DBSession;
class SoftKeyLists;

#include "DBRecallsRM.h"
#include "DBRecall.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class RecallManager : public DataManager
{
	public:

		RecallManager();
		RecallManager(hfc::HFCManagerContainer *container);
		virtual ~RecallManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Recall* getFirstRecord() { return firstRecall(); }
		Recall* getNextRecord()  { return nextRecall(); }
		EvolDeviceType getDevice() const { return RecallDev; }

		Recall *firstRecall();
		Recall *nextRecall();
		int countAllRecalls();

		Recall *getCurrentRecall();
		void setCurrentRecall(Recall *newRecall);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(Recall *recordToSave);

		Recall *createNewRecall(std::string recordName);
		void sendCurrentRecall();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Recall *currentRecall;

		edb::DBQuery<edb::DBRecall> *allRecallsQuery;
		edb::DBQuery<edb::DBRecall>::const_iterator *allRecallsIter;


		void init();

		void handleRead(Recall *msgRecall);
		void handleReadAll();
		void handleWrite(Recall *modifiedRecall);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Recall *modifiedRecall = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(Recall *msgRecall);

		void handleAcknowledgeRecall(Recall *msgRecall);
		void handleCheckForRecalls();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //RECALLMANAGER_H
