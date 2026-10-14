/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYGROUPMANAGER_H
#define SOFTKEYGROUPMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "SoftkeyGroup.h"
#include "Softkey.h"
#include "SoftkeyDevEvent.h"
#include "SoftkeyBasics.h"

#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyGroup.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKey.h"
#include "DBQuery.h"
#include "DBSoftKey.h"
#include "DBCategory.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class SoftkeyGroupManager : public DataManager
{
	public:

		SoftkeyGroupManager();
		SoftkeyGroupManager(hfc::HFCManagerContainer *container);
		virtual ~SoftkeyGroupManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		SoftkeyGroup* getFirstRecord() { return firstSoftkeyGroup(); }
		SoftkeyGroup* getNextRecord()  { return nextSoftkeyGroup(); }
		EvolDeviceType getDevice() const { return SoftkeyDev; }
		UINT8 getWriteEvent() const { return WriteSoftkeyGroup; }
		UINT8 getDeleteEvent() const { return DeleteSoftkeyGroup; }
		UINT8 getDeleteAllEvent() const { return DeleteAllSoftkeyGroups; }

		void handleMessage(SessionMessage *msg);

		SoftkeyGroup *firstSoftkeyGroup();
		SoftkeyGroup *nextSoftkeyGroup();
		int countAllSoftkeyGroups();

		SoftkeyGroup *getCurrentSoftkeyGroup();
		void setCurrentSoftkeyGroup(SoftkeyGroup *newSoftkeyGroup);
		void sendCurrentSoftkeyGroup();

		void handleDeleteAll() { handleDeleteAll(true, RECORD_NOT_FOUND); }
		void handleDeleteAll(HobRecordNum deptNum) { handleDeleteAll(true, deptNum); }
		void handleDeleteAll(bool createDefaultKeys, HobRecordNum deptNum = RECORD_NOT_FOUND);
		void handleSaveRecord(SoftkeyGroup *recordToSave);

		void destroyQueriesAndIters();

		// The last parameter is here but is used internally in the method.  This method is recursive, so it is needed on the recursive call,
		// but not by the original caller.  So I initialize it to 0 (null) so the original caller doesn't have to pass in anything.
		void determinePLUsInSoftkeyGroup(const HobRecordNum groupNum, const HobRecordNum departmentNum, std::string &pluNumberList, std::string *groupNumberList = 0);

		static SoftkeyGroup *prepareToLoadSoftkeyGroup(HobRecordNum featureNum, HobRecordNum groupNum, short langNum, DBAccess *dbAccess);
		static int loadSoftkeyGroup(SoftkeyGroup *mySoftkeyGroup, DBAccess *dbAccess, FixedRowColMethod fixedRowColMethod = FRCM_None);
		static SoftkeyGroup *loadFixedRowColSoftkeyGroup(HobRecordNum groupNum, short langNum, DBAccess *dbAccess);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		SoftkeyGroup *currentSoftkeyGroup;
		edb::DBQuery<edb::DBSoftKeyGroup> *allSoftkeyGroupsQuery;
		edb::DBQuery<edb::DBSoftKeyGroup>::const_iterator *allSoftkeyGroupsIter;

		edb::DBQuery<edb::DBCategory> *allCategoriesQuery;
		edb::DBQuery<edb::DBCategory>::const_iterator *allCategoriesIter;

		SoftkeyGroup *currentGridData;		//	formerly currentGridData

//		bool restoreTrackingFlag;

		void init();

		void handleLoadAllSoftkeyGroups();
		void handleLoadSoftkeyGroup(SoftkeyGroup *msgSG, int event = LoadSoftkeyGroup);
		void handleLoadCategorySoftkeyGroup(Softkey *msgSoftkey);

		void handleLoadProductSearchSoftkeyGroup(Softkey *msgSoftkey);
		void buildSoftkeyGridData(SoftkeyFeature *feature, SoftkeyStyle *style, edb::DBQuery<edb::DBProduct> *productsQuery, HobRecordNum categoryNum = 0);
		void handleLoadSpecialsSoftkeyGroup(Softkey *msgSoftkey);

		void handleRead(SoftkeyGroup *msgSoftkeyGroup);
		void handleReadAll();
		void handleWrite(SoftkeyGroup *modifiedSoftkeyGroup);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(SoftkeyGroup *modifiedSoftkeyGroup);
		void handleRestore(SessionMessage *msg);
		void handleCreate(SoftkeyGroup *msgSoftkeyGroup);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYGROUPMANAGER_H
