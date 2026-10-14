#ifndef EVOLWRAPPERERRORHISTORYMANAGER
#define EVOLWRAPPERERRORHISTORYMANAGER

#include <string>
#include "DataManager.h"
#include "DBWrapperErrorHistoryRM.h"
#include "DBWrapperErrorHistory.h"
#include "EvolWrapperErrorHistory.h"
#include "DBQuery.h"
	
	class DBSession;
	class SessionMessage;
	class EvolWrapperErrorHistory;
	class HfcEvolWrapperErrorHistoryList;		

namespace hfc
{
	class HFCManagerContainer;
}


	class EvolWrapperErrorHistoryManager : public DataManager
	{
		public:
			/* --- Constructors and destructor --- */
			EvolWrapperErrorHistoryManager(hfc::HFCManagerContainer *container);
			~EvolWrapperErrorHistoryManager();

			void handleMessage(SessionMessage *msg);

			void saveEvolWrapperErrorHistory(EvolWrapperErrorHistory* evolWrapperErrorHistory);
			
			int deleteAllEvolWrapperErrorHistories();
			
			int getWrapperErrorRecordCount();

			bool initializeEvolWrapperErrorHistoryQuery();
			EvolWrapperErrorHistory *getFirstEvolWrapperErrorHistory();
			EvolWrapperErrorHistory *getNextEvolWrapperErrorHistory();

			void destroyQueriesAndIters();
			void retrieveWrapperErrorHistory();
			void clearErrorList();

			void setMySession(DBSession *session) {mySession_ = session;}

			static const int maxWrapperErrorRecords_ = 99;	
			
		private:
			
			EvolWrapperErrorHistory *createEvolWrapperErrorHistory( edb::DBWrapperErrorHistory *dbWrapperErrorHistoryRec);
			
			DBSession *mySession_;

			hfc::HFCManagerContainer *mgrContainer_;
			Util::CSmartPtr< edb::DBQuery<edb::DBWrapperErrorHistory> > allWrapperErrorHistoryQuery_, query_;
			Util::CSmartPtr< edb::DBQuery<edb::DBWrapperErrorHistory>::iterator > allWrapperErrorHistoryIterator_, iterator_;
			
			HfcEvolWrapperErrorHistoryList *pErrorList;				

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};

//}

#endif