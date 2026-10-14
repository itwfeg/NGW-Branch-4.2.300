/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */


#ifndef DEPTMANAGER_H
#define DEPTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "DeptEvent.h"
#include "Dept.h"

class SessionMessage;
class DBSession;
class HobList;

#include "DBDeptsRM.h"
#include "DBDept.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class DeptManager : public DataManager
{
	public:

		DeptManager();
		DeptManager(hfc::HFCManagerContainer *container);
		virtual ~DeptManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Dept* getFirstRecord() { return firstDept(); }
		Dept* getNextRecord()  { return nextDept(); }
		EvolDeviceType getDevice() const { return DeptDev; }

		Dept *firstDept();
		Dept *nextDept();
		int countAllDepts();

		Dept *getCurrentDept();
		void setCurrentDept(Dept *newDept);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(Dept *recordToSave);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Dept *currentDept;

		edb::DBQuery<edb::DBDept> *allDeptsQuery;
		edb::DBQuery<edb::DBDept>::const_iterator *allDeptsIter;

		void init();

		bool handleRead(Dept *op);
		void handleReadAll();
		void handleWrite(Dept *op);
		void handleDelete(SessionMessage *msg);
		void handleDelete(HobRecordNum deptNum);
		void handleDeleteDeptRelatedRecords(HobRecordNum deptNum);
		void handleUpdateAndSave(Dept *modifiedDept = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(Dept *op);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //DEPTMANAGER_H
