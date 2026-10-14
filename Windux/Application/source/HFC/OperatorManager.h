/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef OPERATORMANAGER_H
#define OPERATORMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "HobOperator.h"
//#include "OperatorChange.h"
#include "OperatorEvent.h"
#include "SynchronizationManager.h"

class SessionMessage;
class DBSession;
class HobList;
class WriteSingleField;

#include "DBOperatorsRM.h"
#include "DBOperator.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
	class BatchManager;
	class Batch;
}


/** There must always be at least one operator, real or virtual, with level 9 access. */
class OperatorManager : public DataManager
{
	public:

		OperatorManager();
		OperatorManager(hfc::HFCManagerContainer *container);
		virtual ~OperatorManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		HobOperator* getFirstRecord() { return firstOperator(); }
		HobOperator* getNextRecord()  { return nextOperator(); }
		EvolDeviceType getDevice() const { return OperatorDev; }

		HobOperator *firstOperator();
		HobOperator *nextOperator();
		int countAllOperators();

		HobOperator *getCurrentOperator();
		void setCurrentOperator(HobOperator *newOp);

		HobOperator *getLoggedInOperator();
		void setLoggedInOperator(HobOperator *newOp);


		void handleMessage(SessionMessage *msg);

		void renumberCurrentOperator(const HobString &newOperatorNumber);

		void handleDeleteAll();
		void handleSaveRecord(HobOperator *recordToSave);

		void destroyQueriesAndIters();


		static bool isSupervisorLoginRequired();
		static bool isSupervisorLoginRequired(DBManager *myDatabase);
		static bool isSupervisorLoginRequired(DBAccess *dbAccess);
		static bool checkSupervisorLogin(DBOperatorsRM *opRM);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		HobOperator *loggedInOperator;		//	The Operator who is currently logged in
		HobOperator *currentOperator;

		edb::DBQuery<edb::DBOperator> *allOperatorsQuery;
		edb::DBQuery<edb::DBOperator>::const_iterator *allOperatorsIter;

		HobList *idList;
		HobList *accessLevelList;
		HobList *nameList;
		HobList *languageList;

		void init();

		void handleRead(HobOperator *op);
		void handleReadAll();
		void handleWrite(HobOperator *op);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(HobOperator *modifiedOper);
		void handleRestore(SessionMessage *msg);
		void handleCreate(HobOperator *op);

		void handleLogin(HobOperator *op);
		void handleLoginRFID(HobOperator *op);
		void handleLogout();

		void checkIfRFIDExists(SessionMessage *msg);
		void checkIfIDExists(SessionMessage *msg);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //OPERATORMANAGER_H
