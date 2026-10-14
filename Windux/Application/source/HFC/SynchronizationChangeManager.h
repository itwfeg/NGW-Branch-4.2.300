#ifndef SYNCHRONIZATIONCHANGEMANAGER_H
#define SYNCHRONIZATIONCHANGEMANAGER_H



#include <string>


#include "DataManager.h"


#include "DBSynchronizationChangesRM.h"


namespace edb
{
	class DBSynchronizationChange;
	class DBBatch;
}


#include "CSmartPtr.h"



namespace hfc
{


	class HFCManagerContainer;
	class SynchronizationChange;
	class Batch;


	class SynchronizationChangeManager : public DataManager
	{
		friend class BatchManager;


		public:
			/* --- Constructors and destructor --- */
			SynchronizationChangeManager(hfc::HFCManagerContainer* container);
			~SynchronizationChangeManager();


			/* --- Instance function members --- */
			bool initializeChangesQuery();
			SynchronizationChange* getFirstChange();
			SynchronizationChange* getNextChange();

			bool initializeBatchChangesQuery(int batchNumber);
			SynchronizationChange* getFirstChangeFromBatch(int batchNumber);
			SynchronizationChange* getNextChangeFromBatch(int batchNumber);

			void saveSynchronizationChange(SynchronizationChange* change, int batchNumber);
			int deleteSynchronizationChange(SynchronizationChange* change);
			int deleteBatchChanges(int batchNumber);
			int deleteAllSynchronizationChanges();

			void handleMessage(SessionMessage* msg);
			void destroyQueriesAndIters();


		private:
			hfc::HFCManagerContainer* mgrContainer_;
			Util::CSmartPtr< edb::DBSynchronizationChangesRM::query_type > dbQueryAllChanges_, dbQueryAllChangesInBatch_;
			Util::CSmartPtr< edb::DBSynchronizationChangesRM::iterator_type > dbIteratorAllChanges_, dbIteratorAllChangesInBatch_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			SynchronizationChange* createSynchronizationChange(const edb::DBSynchronizationChange * dbSynchronizationChangeRec);
	};


}



#endif /* SYNCHRONIZATIONCHANGEMANAGER_H */
