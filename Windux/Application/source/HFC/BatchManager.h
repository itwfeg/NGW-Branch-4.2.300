#ifndef BATCHMANAGER_H
#define BATCHMANAGER_H



#include "DataManager.h"

#include "Batch.h"


#include "DBBatchesRM.h"
#include "DBBatch.h"
#include "DBQuery.h"


#include "CSmartPtr.h"


namespace hfc
{


	class HFCManagerContainer;
	class SynchronizationChange;
	class SynchronizationChangeManager;


	class BatchManager : public DataManager
	{
		public:
			/* --- Constructors and destructor --- */
			BatchManager(hfc::HFCManagerContainer* container);
			~BatchManager();


			/* --- Instance function members --- */
			Batch * findBatchNumber(int number);
			Batch * findBatchVersion(int version);

			bool initializeBatchesQuery();
			Batch * getFirstBatch();
			Batch * getNextBatch();

			bool initializeCompleteLocalBatchesQuery();
			Batch * getFirstCompleteLocalBatch();
			Batch * getNextCompleteLocalBatch();

			bool initializeCompleteBatchesQuery(int versionNumber = Batch::_invalidBatchVersion_);
			Batch * getFirstCompleteBatch(int versionNumber);
			Batch * getNextCompleteBatch(int versionNumber);

			bool initializeIncompleteBatchesQuery();
			Batch * getFirstIncompleteBatch();
			Batch * getNextIncompleteBatch();

			bool initializeCompleteNotAppliedBatchesQuery();
			Batch * getFirstCompleteNotAppliedBatch();
			Batch * getNextCompleteNotAppliedBatch();

			bool initializePendingSynchronizationBatchesQuery();
			Batch * getFirstPendingSynchronizationBatch();
			Batch * getNextPendingSynchronizationBatch();

			bool initializePendingDeleteBatchesQuery();
			Batch * getFirstPendingDeleteBatch();
			Batch * getNextPendingDeleteBatch();

			bool initializeSameSignatureBatchesQuery(const std::string& signature);
			Batch * getFirstSameSignatureBatch(const std::string& signature);
			Batch * getNextSameSignatureBatch(const std::string& signature);

			Batch * createBatch(int batchNumber, Batch::BatchType type, Batch::BatchOrigin origin);
			void saveBatch(Batch* batch);
			int deleteBatch(Batch* batch);
			int deleteAllBatches();

			void addRecordToBatch(Util::CSmartPtr<hfc::SynchronizationChange> record, Util::CSmartPtr<Batch> batch);

			bool arePendingSynchronizationBatchesPresent();
			bool arePendingSynchronizationLocalBatchesPresent();

			void handleMessage(SessionMessage* msg);
			void destroyQueriesAndIters();


			/* --- Class function members --- */
			static void initializeBatchNumber(DBAccess* dbAccess);


		private:
			hfc::HFCManagerContainer* mgrContainer_;
			Util::CSmartPtr< edb::DBQuery<edb::DBBatch> > query_;
			Util::CSmartPtr< edb::DBQuery<edb::DBBatch>::iterator > iterator_;
			Util::CSmartPtr< edb::DBQuery<edb::DBBatch> > allBatchesQuery_, completeBatchesQuery_, incompleteBatchesQuery_, completeNotAppliedBatchesQuery_, pendingSynchronizationBatchesQuery_, pendingDeleteBatchesQuery_, sameSignatureBatchesQuery_;
			Util::CSmartPtr< edb::DBQuery<edb::DBBatch>::iterator > allBatchesIterator_, completeBatchesIterator_, incompleteBatchesIterator_, completeNotAppliedBatchesIterator_, pendingSynchronizationBatchesIterator_, pendingDeleteBatchesIterator_, sameSignatureBatchesIterator_;


			/* --- Class Data Members --- */
			static int _lastBatchNumber_;
			static util_thread::Mutex<> _lastBatchNumberMutex_;

#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			Batch* createBatch(const edb::DBBatch* dbBatchRec);
			int getNextBatchNumber();
			
	};


}



#endif //BATCHMANAGER_H
