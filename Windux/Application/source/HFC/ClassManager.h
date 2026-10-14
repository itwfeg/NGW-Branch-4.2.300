/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CLASSMANAGER_H
#define CLASSMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "ClassEvent.h"
#include "Class.h"

class SessionMessage;
class DBSession;

#include "DBClassesRM.h"
#include "DBClass.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class ClassManager : public DataManager
{
	public:

		ClassManager();
		ClassManager(hfc::HFCManagerContainer *container);
		virtual ~ClassManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Class* getFirstRecord() { return firstClass(); }
		Class* getNextRecord()  { return nextClass(); }
		EvolDeviceType getDevice() const { return ClassDev; }

		Class *firstClass();
		Class *nextClass();
		int countAllClasses();

		Class *getCurrentClass();
		void setCurrentClass(Class *newClass);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(Class *recordToSave);

		void handleClassCreateMessage(Class *createClass);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Class *currentClass;

		edb::DBQuery<edb::DBClass> *allClassesQuery;
		edb::DBQuery<edb::DBClass>::const_iterator *allClassesIter;

		void init();

		void handleRead(Class *msgClass);
		void handleWrite(Class *modifiedClass);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Class *modifiedClass = 0);
		void handleRestore(SessionMessage *msg);
		
		void handleGetClassTreeMessage();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CLASSMANAGER_H
