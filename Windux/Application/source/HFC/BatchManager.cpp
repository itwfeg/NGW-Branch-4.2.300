#include "PAL_platformheader.h"

#include "DBQuery.h"
#include "BatchManager.h"


#include "HFCManagerContainer.h"
#include "Batch.h"
#include "ScaleManager.h"
#include "Scale.h"
#include "SynchronizationChangeManager.h"
#include "SynchronizationChange.h"
#include "SessionMessage.h"


#include "DBBatchesRM.h"
#include "DBBatch.h"
#include "DBException.h"


#include "Debug.h"


#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



using namespace edb;



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int BatchManager::_dataType_ = 0;
		std::string BatchManager::_dataTypeName_("hfc::BatchManager");
	#endif

	int BatchManager::_lastBatchNumber_ = 0;
	util_thread::Mutex<> BatchManager::_lastBatchNumberMutex_ = util_thread::Mutex<>();


	BatchManager::BatchManager(hfc::HFCManagerContainer* container)
		 : mgrContainer_(container)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	BatchManager::~BatchManager()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void BatchManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if (( msg->getDeviceType() == SynchronizationDev ) || ( msg->getDeviceType() == BatchDev ))
		{
			switch ( msg->getEventType() )
			{
				case DtRestore:
				{
					Batch* batch = dynamic_cast<Batch*>(msg->getHfcObject());
					if ( !batch ) return;

					if ( msg->getFormatType() == DBIndependentFormatFormat )
					{
						edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
						if ( !batchesRM ) return;

						// Ensure that the auto increment value is consistent with the new number received
						//batchesRM->ensureAutoIncrementValue(batch->getNumber());


						Util::CSmartPtr<Scale> thisScale(mgrContainer_->getScaleManager()->getThisScale());
						if ( thisScale.notNull() )
						{
							if ( batch->getNumber() > thisScale->getRetryCount() )
							{
								thisScale->setRetryCount(batch->getNumber());
								mgrContainer_->getScaleManager()->saveScale(thisScale.get());

								Debug::trace("BatchManager",
									"In upgrading the db received a batch with number greater than counter. Updating counter.");
							}
						}
						else
							Debug::trace("BatchManager",
								"******* In upgrading the db received a batch but we have no record for this scale.");
					}

					Batch* existingBatch = findBatchNumber(batch->getNumber());
					if ( existingBatch )
					{
						Debug::trace("BatchManager", "restoreBatch - batch already existed");

						if ( existingBatch->isRecordValid() )
						{
							existingBatch->updateBatch(batch);
							saveBatch(existingBatch);
							Debug::trace("BatchManager", "restoreBatch - saved updated batch");
						}
						else
						{
							deleteBatch(existingBatch);
							saveBatch(batch);
						}

						delete existingBatch;
					}
					else
					{
						saveBatch(batch);
						Debug::trace("BatchManager", "restoreBatch - created and saved batch");
					}

					break;
				}
				default:
				{
					break;
				}
			}
		}

		return;
	}


	Batch * BatchManager::findBatchNumber(int number)
	{
		Batch * readBatch = 0;
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();

		if ( number <= Batch::_invalidBatchNumber_ || !batchesRM )
			return readBatch;

		try
		{
			Util::CSmartPtr<DBBatch> dbBatchRec(batchesRM->findNumber(util_string::inttostring(number)));
			if ( dbBatchRec.notNull() )
			{
				Debug::trace("BatchManager.findBatch", "findBatchNumber() - found this record in the db");

				readBatch = createBatch(dbBatchRec.get());

				if ( Debug::isTracing("BatchManager.findBatch") )
					Debug::trace("BatchManager.findBatch", "findBatchNumber() - created the hfc Batch object - Batch: " +
							util_string::ptrtostring(readBatch) + "  Batch number:" +
							util_string::inttostring(readBatch->getNumber()) + "  Batch version:" +
							util_string::inttostring(readBatch->getVersion()));
			}
			else
			{
				Debug::trace("BatchManager", "Unable to find batch with number " + util_string::inttostring(number));
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("BatchManager") )
			{
				Debug::trace("BatchManager", "findBatchNumber() ********** database exception **********");
				Debug::trace("BatchManager", "DBException type:" + util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return readBatch;
	}


	Batch * BatchManager::findBatchVersion(int version)
	{
		Batch * readBatch = 0;
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();

		if ( version <= 0 || !batchesRM )
			return readBatch;

		try
		{
			Util::CSmartPtr<DBBatch> dbBatchRec(batchesRM->findVersion(util_string::inttostring(version)));
			if ( dbBatchRec.notNull() )
			{
				Debug::trace("BatchManager.findBatch", "findBatchVersion() - found this record in the db");

				readBatch = createBatch(dbBatchRec.get());

				if ( Debug::isTracing("BatchManager.findBatch") )
					Debug::trace("BatchManager.findBatch", "findBatch() - created the hfc Batch object - Batch: " +
							util_string::ptrtostring(readBatch) + "  Batch number:" +
							util_string::inttostring(readBatch->getNumber()) + "  Batch version:" +
							util_string::inttostring(readBatch->getVersion()));
			}
			else
			{
				Debug::trace("BatchManager", "Unable to find batch with version " + util_string::inttostring(version));
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("BatchManager") )
			{
				Debug::trace("BatchManager", "findBatchVersion() ********** database exception **********");
				Debug::trace("BatchManager", "DBException type:" + util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return readBatch;
	}


	bool BatchManager::initializeBatchesQuery()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializeBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(allBatchesQuery_.get()));

		allBatchesQuery_ = batchesRM->retrieveAll();

		return allBatchesQuery_.notNull() ? allBatchesQuery_->begin() != allBatchesQuery_->end() : false;
	}


	Batch * BatchManager::getFirstBatch()
	{
		if ( allBatchesQuery_.null() && !initializeBatchesQuery() )
			return 0;

		allBatchesIterator_ = new DBQuery<DBBatch>::iterator(allBatchesQuery_->begin());

		return *allBatchesIterator_.get() != allBatchesQuery_->end() ? createBatch(&*(*allBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextBatch()
	{
		if ( allBatchesQuery_.null() || allBatchesIterator_.null() )
			return getFirstBatch();

		++(*allBatchesIterator_.get());

		return *allBatchesIterator_.get() != allBatchesQuery_->end() ? createBatch(&*(*allBatchesIterator_.get())) : 0;
	}


	bool BatchManager::initializeCompleteLocalBatchesQuery()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializeCompleteLocalBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(query_.get()));

		query_ = batchesRM->retrieveCompleteBatchesOfOrigin(Batch::OriginLocal);

		return query_.notNull() ? query_->begin() != query_->end() : false;
	}


	Batch * BatchManager::getFirstCompleteLocalBatch()
	{
		if ( query_.null() && !initializeCompleteLocalBatchesQuery() )
			return 0;

		iterator_ = new DBQuery<DBBatch>::iterator(query_->begin());

		return *iterator_.get() != query_->end() ? createBatch(&*(*iterator_.get())) : 0;
	}


	Batch * BatchManager::getNextCompleteLocalBatch()
	{
		if ( query_.null() || iterator_.null() )
			return getFirstCompleteLocalBatch();

		++(*iterator_.get());

		return *iterator_.get() != query_->end() ? createBatch(&*(*iterator_.get())) : 0;
	}


	bool BatchManager::initializeCompleteBatchesQuery(int versionNumber)
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializeCompleteBatchesQuery(" + util_string::inttostring(versionNumber) +
				"): previous query was 0x" + util_string::ptrtostring(completeBatchesQuery_.get()));

		completeBatchesQuery_ = batchesRM->retrieveCompleteBatches(versionNumber);

		return completeBatchesQuery_.notNull() ? completeBatchesQuery_->begin() != completeBatchesQuery_->end() : false;
	}


	Batch * BatchManager::getFirstCompleteBatch(int versionNumber)
	{
		if ( completeBatchesQuery_.null() && !initializeCompleteBatchesQuery(versionNumber) )
			return 0;

		completeBatchesIterator_ = new DBQuery<DBBatch>::iterator(completeBatchesQuery_->begin());

		return *completeBatchesIterator_.get() != completeBatchesQuery_->end() ? createBatch(&*(*completeBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextCompleteBatch(int versionNumber)
	{
		if ( completeBatchesQuery_.null() || completeBatchesIterator_.null() )
			return getFirstCompleteBatch(versionNumber);

		++(*completeBatchesIterator_.get());

		return *completeBatchesIterator_.get() != completeBatchesQuery_->end() ? createBatch(&*(*completeBatchesIterator_.get())) : 0;
	}


	bool BatchManager::initializeIncompleteBatchesQuery()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializeIncompleteBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(incompleteBatchesQuery_.get()));

		incompleteBatchesQuery_ = batchesRM->retrieveIncompleteBatches();

		return incompleteBatchesQuery_.notNull() ? incompleteBatchesQuery_->begin() != incompleteBatchesQuery_->end() : false;
	}


	Batch * BatchManager::getFirstIncompleteBatch()
	{
		if ( incompleteBatchesQuery_.null() && !initializeIncompleteBatchesQuery() )
			return 0;

		incompleteBatchesIterator_ = new DBQuery<DBBatch>::iterator(incompleteBatchesQuery_->begin());

		return *incompleteBatchesIterator_.get() != incompleteBatchesQuery_->end() ? createBatch(&*(*incompleteBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextIncompleteBatch()
	{
		if ( incompleteBatchesQuery_.null() || incompleteBatchesIterator_.null() )
			return getFirstIncompleteBatch();

		++(*incompleteBatchesIterator_.get());

		return *incompleteBatchesIterator_.get() != incompleteBatchesQuery_->end() ? createBatch(&*(*incompleteBatchesIterator_.get())) : 0;
	}


	bool BatchManager::initializeCompleteNotAppliedBatchesQuery()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializeCompleteNotAppliedBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(completeNotAppliedBatchesQuery_.get()));

		completeNotAppliedBatchesQuery_ = batchesRM->retrieveCompleteBatches(Batch::_invalidBatchVersion_);

		return getFirstCompleteNotAppliedBatch() != 0;
	}


	Batch * BatchManager::getFirstCompleteNotAppliedBatch()
	{
		if ( completeNotAppliedBatchesQuery_.null() && !initializeCompleteNotAppliedBatchesQuery() )
			return 0;

		completeNotAppliedBatchesIterator_ = new DBQuery<DBBatch>::iterator(completeNotAppliedBatchesQuery_->begin());

		while ( (*completeNotAppliedBatchesIterator_.get() != completeNotAppliedBatchesQuery_->end()) && !(*completeNotAppliedBatchesIterator_.get())->getAppliedDateTime().empty() )
			++(*completeNotAppliedBatchesIterator_.get());

		return *completeNotAppliedBatchesIterator_.get() != completeNotAppliedBatchesQuery_->end() ? createBatch(&*(*completeNotAppliedBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextCompleteNotAppliedBatch()
	{
		if ( completeNotAppliedBatchesQuery_.null() || completeNotAppliedBatchesIterator_.null() )
			return getFirstCompleteNotAppliedBatch();

		++(*completeNotAppliedBatchesIterator_.get());

		while ( (*completeNotAppliedBatchesIterator_.get() != completeNotAppliedBatchesQuery_->end()) && !(*completeNotAppliedBatchesIterator_.get())->getAppliedDateTime().empty() )
			++(*completeNotAppliedBatchesIterator_.get());

		return *completeNotAppliedBatchesIterator_.get() != completeNotAppliedBatchesQuery_->end() ? createBatch(&*(*completeNotAppliedBatchesIterator_.get())) : 0;
	}


	bool BatchManager::initializePendingSynchronizationBatchesQuery()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializePendingSynchronizationBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(pendingSynchronizationBatchesQuery_.get()));

		pendingSynchronizationBatchesQuery_ = batchesRM->retrievePendingSynchronizationBatches();

		return pendingSynchronizationBatchesQuery_.notNull() ? pendingSynchronizationBatchesQuery_->begin() != pendingSynchronizationBatchesQuery_->end() : false;
	}


	Batch * BatchManager::getFirstPendingSynchronizationBatch()
	{
		if ( pendingSynchronizationBatchesQuery_.null() && !initializePendingSynchronizationBatchesQuery() )
			return 0;

		pendingSynchronizationBatchesIterator_ = new DBQuery<DBBatch>::iterator(pendingSynchronizationBatchesQuery_->begin());

		return *pendingSynchronizationBatchesIterator_.get() != pendingSynchronizationBatchesQuery_->end() ? createBatch(&*(*pendingSynchronizationBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextPendingSynchronizationBatch()
	{
		if ( pendingSynchronizationBatchesQuery_.null() || pendingSynchronizationBatchesIterator_.null() )
			return getFirstPendingSynchronizationBatch();

		++(*pendingSynchronizationBatchesIterator_.get());

		return *pendingSynchronizationBatchesIterator_.get() != pendingSynchronizationBatchesQuery_->end() ? createBatch(&*(*pendingSynchronizationBatchesIterator_.get())) : 0;
	}


	bool BatchManager::initializePendingDeleteBatchesQuery()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializePendingDeleteBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(pendingDeleteBatchesQuery_.get()));

		pendingDeleteBatchesQuery_ = batchesRM->retrievePendingDeleteBatches();

		return pendingDeleteBatchesQuery_.notNull() ? pendingDeleteBatchesQuery_->begin() != pendingDeleteBatchesQuery_->end() : false;
	}


	Batch * BatchManager::getFirstPendingDeleteBatch()
	{
		if ( pendingDeleteBatchesQuery_.null() && !initializePendingDeleteBatchesQuery() )
			return 0;

		pendingDeleteBatchesIterator_ = new DBQuery<DBBatch>::iterator(pendingDeleteBatchesQuery_->begin());

		return *pendingDeleteBatchesIterator_.get() != pendingDeleteBatchesQuery_->end() ? createBatch(&*(*pendingDeleteBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextPendingDeleteBatch()
	{
		if ( pendingDeleteBatchesQuery_.null() || pendingDeleteBatchesIterator_.null() )
			return getFirstPendingDeleteBatch();

		++(*pendingDeleteBatchesIterator_.get());

		return *pendingDeleteBatchesIterator_.get() != pendingDeleteBatchesQuery_->end() ? createBatch(&*(*pendingDeleteBatchesIterator_.get())) : 0;
	}


	bool BatchManager::initializeSameSignatureBatchesQuery(const std::string& signature)
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Debug::trace("BatchManager", "initializeSameSignatureBatchesQuery: previous query was 0x" +
				util_string::ptrtostring(sameSignatureBatchesQuery_.get()));

		sameSignatureBatchesQuery_ = batchesRM->retrieveSameSignatureBatches(signature);

		return sameSignatureBatchesQuery_.notNull() ? sameSignatureBatchesQuery_->begin() != sameSignatureBatchesQuery_->end() : false;
	}


	Batch * BatchManager::getFirstSameSignatureBatch(const std::string& signature)
	{
		if ( sameSignatureBatchesQuery_.null() && !initializeSameSignatureBatchesQuery(signature) )
			return 0;

		sameSignatureBatchesIterator_ = new DBQuery<DBBatch>::iterator(sameSignatureBatchesQuery_->begin());

		return *sameSignatureBatchesIterator_.get() != sameSignatureBatchesQuery_->end() ? createBatch(&*(*sameSignatureBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::getNextSameSignatureBatch(const std::string& signature)
	{
		if ( sameSignatureBatchesQuery_.null() || sameSignatureBatchesIterator_.null() )
			return getFirstSameSignatureBatch(signature);

		++(*sameSignatureBatchesIterator_.get());

		return *sameSignatureBatchesIterator_.get() != sameSignatureBatchesQuery_->end() ? createBatch(&*(*sameSignatureBatchesIterator_.get())) : 0;
	}


	Batch * BatchManager::createBatch(int batchNumber, Batch::BatchType type, Batch::BatchOrigin origin)
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return 0;

		Util::CSmartPtr<DBBatch> dbBatch(batchesRM->createRecord());
		if ( dbBatch.notNull() )
		{
			dbBatch->setNumber(util_string::inttostring(batchNumber));
			dbBatch->setVersion(Batch::_invalidBatchVersion_);
			dbBatch->setRemoteVersion(Batch::_invalidBatchVersion_);
			dbBatch->setOrigin(origin);
			dbBatch->setType(type);
			dbBatch->setCreatedDateTime(HobDateTime().convertToDBFormat());
			dbBatch->setCompleted(false);
			dbBatch->setPendingApplication(false);
			dbBatch->setPendingSynchronization(false);
			dbBatch->setPendingDeletion(false);

			Debug::trace("BatchManager", "Created a new batch " + dbBatch->getNumber() +
				" in the database, signature = " + dbBatch->getSignature());

			try
			{
				batchesRM->saveRecord(dbBatch.get());
			}
			catch (DBException &dbe)
			{
				if ( Debug::isTracing("BatchManager") )
				{
					Debug::trace("BatchManager", "createBatch() ********** database exception **********");
					Debug::trace("BatchManager", "DBException type:" + util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
				}
			}

			return createBatch(dbBatch.get());
		}

		return 0;
	}


	Batch* BatchManager::createBatch(const edb::DBBatch* dbBatchRec)
	{
		Batch* batch = new Batch();

		if ( dbBatchRec )
		{
			if ( dbBatchRec->isRecordValid() )
			{
				batch->setNumber(util_string::stringtoint(dbBatchRec->getNumber()));
				if ( dbBatchRec->isScaleIdNull() )
					batch->setScaleId(Scale::_invalidScaleId_);
				else
					batch->setScaleId(dbBatchRec->getScaleId());
				batch->setVersion(dbBatchRec->getVersion());
				batch->setRemoteVersion(dbBatchRec->getRemoteVersion());
				batch->setOrigin((Batch::BatchOrigin)dbBatchRec->getOrigin());
				batch->setType((Batch::BatchType)dbBatchRec->getType());
				batch->setCreatedDateTime(dbBatchRec->getCreatedDateTime());
				batch->setComplete(dbBatchRec->getCompleted());
				if ( !dbBatchRec->isCompletedDateTimeNull() )
					batch->setCompletedDateTime(dbBatchRec->getCompletedDateTime());
				if ( !dbBatchRec->isSignatureNull() )
					batch->setSignature(dbBatchRec->getSignature());
				else
					batch->setSignature("");
				batch->setPendingApplication(dbBatchRec->getPendingApplication());
				if ( !dbBatchRec->isAppliedDateTimeNull() )
					batch->setAppliedDateTime(dbBatchRec->getAppliedDateTime());
				batch->setPendingSynchronization(dbBatchRec->getPendingSynchronization());
				if ( !dbBatchRec->isSynchronizedDateTimeNull() )
					batch->setSynchronizedDateTime(dbBatchRec->getSynchronizedDateTime());
				batch->setPendingDeletion(dbBatchRec->getPendingDeletion());
				if ( !dbBatchRec->isDeletedDateTimeNull() )
					batch->setDeletedDateTime(dbBatchRec->getDeletedDateTime());

				Debug::trace("BatchManager", "Read Batch " + util_string::inttostring(batch->getNumber()) +
					" from database, signature = " + batch->getSignature());

				batch->setErrorCode(RECORD_OK);
			}
			else
				batch->setErrorCode(RECORD_CORRUPT);
		}
		else
			Debug::trace("BatchManager", "createBatch passing in a db - dbrec IS NULL");

		return batch;
	}


	void BatchManager::saveBatch(Batch * batch)
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return;

		Util::CSmartPtr<DBBatch> dbBatch(batchesRM->findNumber(util_string::inttostring(batch->getNumber())));
		if ( dbBatch.null() )
		{
			//	This must be a new record for the Database
			Debug::trace("BatchManager", "saveBatch() - don't have a db record, creating it...");

			dbBatch = batchesRM->createRecord();
		}
		
		if ( dbBatch )
		{
			dbBatch->setNumber(util_string::inttostring(batch->getNumber()));
			if ( batch->getScaleId() == Scale::_invalidScaleId_ )
				dbBatch->setScaleIdNull();
			else
				dbBatch->setScaleId(batch->getScaleId());
			dbBatch->setVersion(batch->getVersion());
			dbBatch->setRemoteVersion(batch->getRemoteVersion());
			dbBatch->setType(batch->getType());
			dbBatch->setOrigin(batch->getOrigin());
			dbBatch->setCreatedDateTime(batch->getCreatedDateTimeAsString());
			dbBatch->setCompleted(batch->isComplete());
			if ( batch->isComplete() && batch->getCompletedDateTimeAsString().size())
				dbBatch->setCompletedDateTime(batch->getCompletedDateTimeAsString());
			else
				dbBatch->setCompletedDateTimeNull();
			dbBatch->setSignature(batch->getSignature());
			dbBatch->setPendingApplication(batch->isPendingApplication());
			if ( batch->isApplied() && batch->getAppliedDateTimeAsString().size())
				dbBatch->setAppliedDateTime(batch->getAppliedDateTimeAsString());
			else
				dbBatch->setAppliedDateTimeNull();
			dbBatch->setPendingSynchronization(batch->isPendingSynchronization());
			if ( batch->isSynchronized() && batch->getSynchronizedDateTimeAsString().size())
				dbBatch->setSynchronizedDateTime(batch->getSynchronizedDateTimeAsString());
			else
				dbBatch->setSynchronizedDateTimeNull();
			dbBatch->setPendingDeletion(batch->isPendingDeletion());
			if ( batch->isDeleted() && batch->getDeletedDateTimeAsString().size())
				dbBatch->setDeletedDateTime(batch->getDeletedDateTimeAsString());
			else
				dbBatch->setDeletedDateTimeNull();

			Debug::trace("BatchManager", "Saving Batch " + util_string::inttostring(batch->getNumber()) +
				", signature = " + batch->getSignature());

			try
			{
				batchesRM->saveRecord(dbBatch.get());
			}
			catch (DBException &dbe)
			{
				if ( Debug::isTracing("BatchManager") )
				{
					Debug::trace("BatchManager", "saveBatch() ********** database exception **********");
					Debug::trace("BatchManager", "DBException type:" + util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
				}
			}

//			Debug::trace("BatchManager", "Saved in the database");
		}
	}


	int BatchManager::deleteBatch(Batch* batch)
	{
		int result = 0;
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return 0;


		Util::CSmartPtr<DBBatch> dbBatch(batchesRM->findNumber(util_string::inttostring(batch->getNumber())));
		if ( dbBatch.notNull() )   //No need to delete if there is no record
		{
			Debug::trace("BatchManager", "Deleting Batch " + util_string::inttostring(batch->getNumber()));

			result = batchesRM->deleteRecord(dbBatch.get());
		}
		else
			Debug::trace("BatchManager", "Failed to find batch with version " +
				util_string::inttostring(batch->getVersion()) + " when attempting to delete it");

		return result;
	}


	int BatchManager::deleteAllBatches()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return 0;

		return batchesRM->deleteAll();
	}


	void BatchManager::addRecordToBatch(Util::CSmartPtr<hfc::SynchronizationChange> record, Util::CSmartPtr<Batch> batch)
	{
		SynchronizationChangeManager* syncChangeMgr = mgrContainer_->getSynchronizationChangeManager();

		syncChangeMgr->saveSynchronizationChange(record, batch->getNumber());
		batch->computeSignature(record->getChange());
		saveBatch(batch.get());
	}


	bool BatchManager::arePendingSynchronizationBatchesPresent()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Util::CSmartPtr< edb::DBQuery<edb::DBBatch> > query(batchesRM->retrieveCompleteBatches(Batch::_invalidBatchVersion_));
		edb::DBQuery<edb::DBBatch>::iterator iter = query->begin();
		while ( iter != query->end() )
		{
			if ( iter->getPendingSynchronization() )
				return true;

			++iter;
		}

		return false;
	}


	bool BatchManager::arePendingSynchronizationLocalBatchesPresent()
	{
		edb::DBBatchesRM* batchesRM = mgrContainer_->getDBBatchesRM();
		if ( !batchesRM )
			return false;

		Util::CSmartPtr< edb::DBQuery<edb::DBBatch> > query(batchesRM->retrieveCompleteBatchesOfOrigin(Batch::OriginLocal));
		edb::DBQuery<edb::DBBatch>::iterator iter = query->begin();
		while ( iter != query->end() )
		{
			if ( iter->getPendingSynchronization() )
				return true;

			++iter;
		}

		return false;
	}


	void BatchManager::destroyQueriesAndIters()
	{
		/* When this function is called, coming out of a db compress, it is safe to remove all
		queries and iterators. */

		/* Destroy first the iterators. */
		iterator_ = 0; allBatchesIterator_ = 0; completeBatchesIterator_ = 0; incompleteBatchesIterator_ = 0; completeNotAppliedBatchesIterator_ = 0;
		pendingSynchronizationBatchesIterator_ = 0; pendingDeleteBatchesIterator_ = 0; sameSignatureBatchesIterator_ = 0;

		/* Destroy now the queries. */
		query_ = 0; allBatchesQuery_ = 0; completeBatchesQuery_ = 0; incompleteBatchesQuery_ = 0; completeNotAppliedBatchesQuery_ = 0;
		pendingSynchronizationBatchesQuery_ = 0; pendingDeleteBatchesQuery_ = 0; sameSignatureBatchesQuery_ = 0;
	}


	void BatchManager::initializeBatchNumber(DBAccess* dbAccess)
	{
		if ( !dbAccess ) return;

		Util::CSmartPtr< edb::DBBatchesRM > batchesRM(dbAccess->getDBBatchesRM());
		if ( batchesRM.null() ) return;

		Util::CSmartPtr< edb::DBQuery<edb::DBBatch> > query(batchesRM->retrieveAll());
		if ( query.null() ) return;

		int largestBatchNumber = 0;
		edb::DBQuery<edb::DBBatch>::iterator iter = query->begin();
		while ( iter != query->end() )
		{
			if ( util_string::stringtoint(iter->getNumber()) > largestBatchNumber )
				largestBatchNumber = util_string::stringtoint(iter->getNumber());

			++iter;
		}

		{
			util_thread::Lock< util_thread::Mutex<> > lock(_lastBatchNumberMutex_);

			_lastBatchNumber_ = largestBatchNumber;
		}
	}


	int BatchManager::getNextBatchNumber()
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_lastBatchNumberMutex_);

		return ++_lastBatchNumber_;
	}


}
