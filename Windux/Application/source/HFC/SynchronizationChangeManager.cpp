#include "SynchronizationChangeManager.h"


#include "HFCManagerContainer.h"
#include "SynchronizationChange.h"


#include "DBSynchronizationChange.h"
#include "DBException.h"

#include "SessionMessage.h"

#include "CSmartPtr.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



using namespace edb;



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int SynchronizationChangeManager::_dataType_ = 0;
		std::string SynchronizationChangeManager::_dataTypeName_("hfc::SynchronizationChangeManager");
	#endif


	SynchronizationChangeManager::SynchronizationChangeManager(hfc::HFCManagerContainer* container)
		 : mgrContainer_(container), dbQueryAllChangesInBatch_(0), dbIteratorAllChangesInBatch_(0)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	SynchronizationChangeManager::~SynchronizationChangeManager()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void SynchronizationChangeManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == SynchronizationChangeDev )
		{
			switch ( msg->getEventType() )
			{
				case DtRestore:
				{
					SynchronizationChange* change = dynamic_cast<SynchronizationChange*>(msg->getHfcObject());
					if ( !change ) return;

					if ( msg->getFormatType() == DBIndependentFormatFormat )
					{
						saveSynchronizationChange(change, change->getBatchNumber());

						Debug::trace("SynchronizationChangeManager", "In upgrading the db received a change record, created it and saved it");
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


	bool SynchronizationChangeManager::initializeChangesQuery()
	{
		edb::DBSynchronizationChangesRM* changesRM = mgrContainer_->getDBSynchronizationChangesRM();
		if ( !changesRM )
			return false;

		dbQueryAllChanges_ = changesRM->retrieveAll();

		return dbQueryAllChanges_.notNull() ? dbQueryAllChanges_->begin() != dbQueryAllChanges_->end() : false;
	}


	SynchronizationChange* SynchronizationChangeManager::getFirstChange()
	{
		if ( dbQueryAllChanges_.null() && !initializeChangesQuery() )
			return 0;

		dbIteratorAllChanges_ = new edb::DBSynchronizationChangesRM::iterator_type(dbQueryAllChanges_->begin());

		return *(dbIteratorAllChanges_.get()) != dbQueryAllChanges_->end() ? createSynchronizationChange(&*(*dbIteratorAllChanges_.get())) : 0;
	}


	SynchronizationChange* SynchronizationChangeManager::getNextChange()
	{
		if ( dbQueryAllChanges_.null() || dbIteratorAllChanges_.null() )
			return getFirstChange();

		++(*dbIteratorAllChanges_.get());

		return *dbIteratorAllChanges_.get() != dbQueryAllChanges_->end() ? createSynchronizationChange(&*(*dbIteratorAllChanges_.get())) : 0;
	}


	bool SynchronizationChangeManager::initializeBatchChangesQuery(int batchNumber)
	{
		edb::DBSynchronizationChangesRM* changesRM = mgrContainer_->getDBSynchronizationChangesRM();
		if ( !changesRM )
			return false;

		dbQueryAllChangesInBatch_ = changesRM->retrieveLinkedToBatch(util_string::inttostring(batchNumber));

		return dbQueryAllChangesInBatch_.notNull() ? dbQueryAllChangesInBatch_->begin() != dbQueryAllChangesInBatch_->end() : false;
	}


	SynchronizationChange* SynchronizationChangeManager::getFirstChangeFromBatch(int batchNumber)
	{
		if ( dbQueryAllChangesInBatch_.null() && !initializeBatchChangesQuery(batchNumber) )
			return 0;

		dbIteratorAllChangesInBatch_ = new edb::DBSynchronizationChangesRM::iterator_type(dbQueryAllChangesInBatch_->begin());

		return *(dbIteratorAllChangesInBatch_.get()) != dbQueryAllChangesInBatch_->end() ? createSynchronizationChange(&*(*dbIteratorAllChangesInBatch_.get())) : 0;
	}


	SynchronizationChange* SynchronizationChangeManager::getNextChangeFromBatch(int batchNumber)
	{
		if ( dbQueryAllChangesInBatch_.null() || dbIteratorAllChangesInBatch_.null() )
			return getFirstChangeFromBatch(batchNumber);

		++(*dbIteratorAllChangesInBatch_.get());

		return *dbIteratorAllChangesInBatch_.get() != dbQueryAllChangesInBatch_->end() ? createSynchronizationChange(&*(*dbIteratorAllChangesInBatch_.get())) : 0;
	}


	void SynchronizationChangeManager::saveSynchronizationChange(SynchronizationChange* change, int batchNumber)
	{
		edb::DBSynchronizationChangesRM* changesRM = mgrContainer_->getDBSynchronizationChangesRM();
		if ( !changesRM )
			return;

		Util::CSmartPtr<DBSynchronizationChange> dbChange(changesRM->findIdentifier(util_string::inttostring(change->getIdentifier())));
		if ( dbChange.null() )
		{
			//	This must be a new record for the Database
			Debug::trace("SynchronizationChange", "saveRecord() - don't have a db record, creating it...");

			dbChange = changesRM->createRecord(util_string::inttostring(batchNumber));
			change->setIdentifier(util_string::stringtoint(dbChange->getIdentifier()));
		}

		if ( dbChange.notNull() )
		{
			dbChange->setApplicationDevice(change->getDevice());
			dbChange->setApplicationEvent(change->getEvent());
			dbChange->setApplicationFormat(change->getFormat());
			dbChange->setChange(change->getChange());

//			Debug::trace("SynchronizationChange", "Saving SynchronizationChange " + util_string::inttostring(identifier_));

			try
			{
				changesRM->saveRecord(dbChange.get());
			}
			catch ( DBException & dbe )
			{
				Debug::trace("DBAccess.exception", "SynchronizationChange::saveRecord() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}

//			Debug::trace("SynchronizationChange", "Saved in the database");
		}
	}


	int SynchronizationChangeManager::deleteSynchronizationChange(SynchronizationChange* change)
	{
		int result = 0;
		edb::DBSynchronizationChangesRM* changesRM = mgrContainer_->getDBSynchronizationChangesRM();
		if ( !change || !changesRM )
			return result;

		Util::CSmartPtr<DBSynchronizationChange> dbSynchronizationChange(changesRM->findIdentifier(util_string::inttostring(change->getIdentifier())));
		if ( dbSynchronizationChange.notNull() )   //No need to delete if there is no record
		{
			Debug::trace("SynchronizationChange", "Deleting SynchronizationChange " + util_string::inttostring(change->getIdentifier()));

			result = changesRM->deleteRecord(dbSynchronizationChange.get());
		}

		return result;
	}


	int SynchronizationChangeManager::deleteBatchChanges(int batchNumber)
	{
		int result = 0;
		edb::DBSynchronizationChangesRM* changesRM = mgrContainer_->getDBSynchronizationChangesRM();
		if ( !changesRM )
			return result;

		DBSynchronizationChangesRM::query_type* query = changesRM->retrieveLinkedToBatch(util_string::inttostring(batchNumber));
		DBSynchronizationChangesRM::iterator_type iterator = query->begin();

		while ( iterator != query->end() )
		{
			DBSynchronizationChangesRM::iterator_type temp = iterator++;
			result = changesRM->deleteRecord(&*temp);
		}

		return result;
	}


	int SynchronizationChangeManager::deleteAllSynchronizationChanges()
	{
		edb::DBSynchronizationChangesRM* changesRM = mgrContainer_->getDBSynchronizationChangesRM();
		if ( !changesRM )
			return 0;

		return changesRM->deleteAll();
	}


	SynchronizationChange* SynchronizationChangeManager::createSynchronizationChange(const edb::DBSynchronizationChange * dbSynchronizationChangeRec)
	{
		if ( dbSynchronizationChangeRec )
		{
			SynchronizationChange* change = new SynchronizationChange();

			if ( dbSynchronizationChangeRec->isRecordValid() )
			{
				change->setIdentifier(util_string::stringtoint(dbSynchronizationChangeRec->getIdentifier()));
				change->setBatchNumber(util_string::stringtoint(dbSynchronizationChangeRec->getBatchNumber()));
				change->setDevice(dbSynchronizationChangeRec->getApplicationDevice());
				change->setEvent(dbSynchronizationChangeRec->getApplicationEvent());
				change->setFormat((MessageFormat)dbSynchronizationChangeRec->getApplicationFormat());
				change->setChange(dbSynchronizationChangeRec->getChange());
				change->setErrorCode(RECORD_OK);
			}
			else
				change->setErrorCode(RECORD_CORRUPT);

			return change;
		}
		else
			Debug::trace("SynchronizationChange", "Constructor passing in a db - dbrec IS NULL");

		return 0;
	}


	void SynchronizationChangeManager::destroyQueriesAndIters()
	{
		/* When this function is called, coming out of a db compress, it is safe to remove all
		queries and iterators. */

		/* Destroy first the iterators. */
		dbIteratorAllChanges_ = 0; dbIteratorAllChangesInBatch_ = 0;

		/* Destroy now the queries. */
		dbQueryAllChanges_ = 0; dbQueryAllChangesInBatch_ = 0;
	}


}
