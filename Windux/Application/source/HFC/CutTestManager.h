/* Access Scale Project
 * COPYRIGHT (c) 2010.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CUTTESTMANAGER_H
#define CUTTESTMANAGER_H

#include "evolution.h"
#include "DataManager.h"

class SessionMessage;
class DBSession;
class CuttingTest;
class Softkey;
class HobOperator;
class CutTestDetailsOption1;
class CutTestHistoryListing;

namespace hfc
{
	class HFCManagerContainer;
}

#include "DBCuttingTestsRM.h"
#include "DBCuttingTest.h"
#include "DBQuery.h"

class CutTestManager : public DataManager
{
	public:

		CutTestManager();
		CutTestManager(hfc::HFCManagerContainer *container);
		~CutTestManager();


		/* --- Getter and setter members --- */
		DBSession * getMySession() const { return mySession_; }
		void setMySession(DBSession * session) { mySession_ = session; }

		void handleMessage(SessionMessage *msg);
		void handleSoftkeyMessage(Softkey *msgSoftkey);

		bool isCuttingTestOn();
		void turnCuttingTestOn();
		void turnCuttingTestOff();

		bool isWaitingForLoginToStartCutTest() const { return waitingForLoginToStartCutTest_; }
        bool isWaitingToStartCutTest() const { return waitingToStartCutTest_;  }
		bool checkForSuspendedCutTests(HobString opNum);

		CuttingTest *getCurrentCuttingTest();
		void setCurrentCuttingTest(CuttingTest *cuttingTest);

		//CutTestDetailsOption1 *readCuttingTestDetailsForReport(SessionMessage *msg);
		CutTestDetailsOption1 *readCuttingTestDetailsForReport(int testNumber);
		const HobString &readCuttingTestDetailsDescription(HobString plunum) const;

		HobRecordNum getCurrentCutTestNumber();

		CuttingTest *firstCuttingTest();
		CuttingTest *nextCuttingTest();

		int countAllCuttingTests(bool notDeleted = false);

		void destroyQueriesAndIters();

		// Made public so Session can call it, after login operator
		void initiateCuttingTestOption1();  // via softkey action

		bool isPLUValidForCutTestOption1(const HobRecordNum pluNum) const;


	private:

		DBSession *mySession_;
		hfc::HFCManagerContainer *mgrContainer_;

		void init();

		bool			cuttingTestOn_;
		CuttingTest		*currentCuttingTest;

		bool  waitingForLoginToStartCutTest_;
        bool waitingToStartCutTest_;

		mutable HobString currentDescription_;

		std::string meatSpecSheetPLUList_;

		edb::DBQuery<edb::DBCuttingTest> *allCuttingTestsQuery;
		edb::DBQuery<edb::DBCuttingTest>::const_iterator *allCuttingTestsIter;

		void handleRestore(SessionMessage *msg);

		void initiateCuttingTest();  // via softkey action
		void clearCuttingTestOption1();
		void beginCuttingTest(SessionMessage *msg);
		void setCurrentCutTestInProcess();
		void readCuttingTestDetails(SessionMessage *msg);
		void cancelCuttingTest();
		void endCuttingTest();

		int  deleteAllCuttingTests();


		HobOperator *checkForLoggedInOperator();
		void readCuttingTestOption1(SessionMessage *msg);
		void beginCuttingTestOption1(SessionMessage *msg);
		void endCuttingTestOption1();
		void completeCuttingTestOption1(SessionMessage *msg);
		void readCuttingTestDetailsOption1(SessionMessage *msg);
		void readCuttingTestHistoryOption1();
		void suspendCutTestOption1(SessionMessage *msg);
		void deleteCuttingTestOption1(SessionMessage *msg);
		void resumeCutTestOption1(SessionMessage *msg);
		void saveCuttingTestToUsbOption1(SessionMessage *msg);


		void destroyCuttingTestsQueryAndIter();

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CUTTESTMANAGER_H
