/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYSTYLEMANAGER_H
#define SOFTKEYSTYLEMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "SoftkeyStyle.h"
#include "Softkey.h"
#include "SoftkeyDevEvent.h"

#include "DBSoftKeyStylesRM.h"
#include "DBSoftKeyStyle.h"
#include "DBQuery.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class SoftkeyStyleManager : public DataManager
{
	public:

		SoftkeyStyleManager();
		SoftkeyStyleManager(hfc::HFCManagerContainer *container);
		virtual ~SoftkeyStyleManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		SoftkeyStyle* getFirstRecord() { return firstSoftkeyStyle(); }
		SoftkeyStyle* getNextRecord()  { return nextSoftkeyStyle(); }
		EvolDeviceType getDevice() const { return SoftkeyDev; }
		UINT8 getWriteEvent() const { return WriteSoftkeyStyle; }
		UINT8 getDeleteEvent() const { return DeleteSoftkeyStyle; }
		UINT8 getDeleteAllEvent() const { return DeleteAllSoftkeyStyles; }

		void handleMessage(SessionMessage *msg);


		SoftkeyStyle *firstSoftkeyStyle();
		SoftkeyStyle *nextSoftkeyStyle();
		int countAllSoftkeyStyles();

		SoftkeyStyle *getCurrentSoftkeyStyle();
		void setCurrentSoftkeyStyle(SoftkeyStyle *newSoftkeyStyle);

		void handleDeleteAll();
		void handleSaveRecord(SoftkeyStyle *recordToSave);

		SoftkeyStyle *createNewSoftkeyStyle(HobRecordNum recordNum = 0);
		void sendCurrentSoftkeyStyle();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		SoftkeyStyle *currentSoftkeyStyle;
		edb::DBQuery<edb::DBSoftKeyStyle> *allSoftkeyStylesQuery;
		edb::DBQuery<edb::DBSoftKeyStyle>::const_iterator *allSoftkeyStylesIter;

//		bool restoreTrackingFlag;

		void init();

		void handleRead(SoftkeyStyle *msgSoftkeyStyle);
		void handleReadAll();
		void handleWrite(SoftkeyStyle *modifiedSoftkeyStyle);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(SoftkeyStyle *modifiedSoftkeyStyle);
		void handleRestore(SessionMessage *msg);
		void handleCreate(SoftkeyStyle *msgSoftkeyStyle);
		void handleReadAllSoftkeyStyleNumbers();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYSTYLEMANAGER_H
