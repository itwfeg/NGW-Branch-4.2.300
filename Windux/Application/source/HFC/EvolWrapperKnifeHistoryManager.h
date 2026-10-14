#ifndef EVOLWRAPPERKNIFEHISTORYMANAGER
#define EVOLWRAPPERKNIFEHISTORYMANAGER

#include <string>
#include "DataManager.h"
#include "DBKnifeTimeHistoryRM.h"
#include "DBKnifeTimeHistory.h"
//#include "EvolWrapperKnifeHistory.h"
#include "DBQuery.h"
	
	
//class EvolWrapperKnifeHistory;
class Session;
class HfcEvolWrapperKnifeTimeHistory;
class HfcEvolWrapperEventCapture;

namespace hfc
{
	class HFCManagerContainer;
}

	class EvolWrapperKnifeHistoryManager : public DataManager
	{
		public:
			/* --- Constructors and destructor --- */
			EvolWrapperKnifeHistoryManager(hfc::HFCManagerContainer *container);
			~EvolWrapperKnifeHistoryManager();

#if 0
			EvolWrapperKnifeHistory *findEvolWrapperKnifeHistory(EvolWrapperKnifeHistory::KnifeHistoryType id);
			EvolWrapperKnifeHistory *findEvolWrapperKnifeHistory(const std::string & id);

			void saveEvolWrapperKnifeHistory(EvolWrapperKnifeHistory* evolWrapperKnifeHistory);
			
			int deleteEvolWrapperKnifeHistory(EvolWrapperKnifeHistory* evolWrapperKnifeHistory);
#endif
			int deleteAllEvolWrapperKnifeHistories();

#if 0
			bool initializeEvolWrapperKnifeHistoryQuery();
			EvolWrapperKnifeHistory *getFirstEvolWrapperKnifeHistory();
			EvolWrapperKnifeHistory *getNextEvolWrapperKnifeHistory();
#endif

		HfcEvolWrapperKnifeTimeHistory *getEvolWrapperKnifeTimeHistory();
		bool processWrapperKnifeTime(HfcEvolWrapperEventCapture *pKnifeTimeMsg);

#if 0
			void destroyQueriesAndIters();
#endif
		
		private:
			
#if 0
			EvolWrapperKnifeHistory *createEvolWrapperKnifeHistory(const edb::DBKnifeTimeHistory *dbknifeTimeHistoryRec);

			hfc::HFCManagerContainer *mgrContainer_;
			Util::CSmartPtr< edb::DBQuery<edb::DBKnifeTimeHistory> > allKnifeHistoryQuery_, query_;
			Util::CSmartPtr< edb::DBQuery<edb::DBKnifeTimeHistory>::iterator > allKnifeHistoryIterator_, iterator_;



#endif
			hfc::HFCManagerContainer *mgrContainer_;

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
};

#endif		//	EVOLWRAPPERKNIFEHISTORYMANAGER
