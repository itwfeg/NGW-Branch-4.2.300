/* Access Scale Project
 * COPYRIGHT (c) 2010.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CUTTESTTOTALMANAGER_H
#define CUTTESTTOTALMANAGER_H

#include "evolution.h"
#include "DataManager.h"

class SessionMessage;
class DBSession;
class CuttingTest;
class CutTestTotal;
class CuttingTestReport;

namespace hfc
{
	class HFCManagerContainer;
}

#include "DBQuery.h"


class CutTestTotalManager : public DataManager
{
	public:
		/* --- Types and subtypes --- */
		typedef std::map< HobRecordNum, Util::CSmartPtr<CutTestTotal> > cutTestTotalsByProductContainer_type;
		typedef std::map< HobRecordNum, Util::CSmartPtr<cutTestTotalsByProductContainer_type> > cutTestTotalsContainer_type;

		CutTestTotalManager(hfc::HFCManagerContainer *container);
		~CutTestTotalManager();


		/* --- Getter and setter members --- */
		DBSession * getMySession() const { return mySession_; }
		void setMySession(DBSession * session) { mySession_ = session; }

		void handleMessage(SessionMessage *msg);

		int initializeAllCutTestTotalsQuery();
		Util::CSmartPtr<CutTestTotal> getFirstCutTestTotal();
		Util::CSmartPtr<CutTestTotal> getNextCutTestTotal();

		CuttingTestReport *createCutTestReport(CuttingTest* inputMessage, int& totalCount);


	private:

		DBSession *mySession_;
		hfc::HFCManagerContainer *mgrContainer_;
		cutTestTotalsContainer_type cachedCutTestTotals_;
		cutTestTotalsContainer_type::iterator cutTestIterator_;
		cutTestTotalsByProductContainer_type::iterator totalIterator_;


		/* --- Instance function members --- */
		void clearCachedCutTestTotals();
		Util::CSmartPtr<CutTestTotal> getCachedCutTestTotal(HobRecordNum cutTestNumber, HobRecordNum productNumber, HobRecordNum deptNum);


#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CUTTESTTOTALMANAGER_H
