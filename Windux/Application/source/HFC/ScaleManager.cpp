#include "ScaleManager.h"


#include "HFCManagerContainer.h"
#include "Scale.h"
#include "Batch.h"

#include "SessionMessage.h"


#include "DBScalesRM.h"
#include "DBScale.h"
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
		int ScaleManager::_dataType_ = 0;
		std::string ScaleManager::_dataTypeName_("hfc::ScaleManager");
	#endif


	ScaleManager::ScaleManager(hfc::HFCManagerContainer* container)
			: mgrContainer_(container)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	ScaleManager::~ScaleManager()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void ScaleManager::handleMessage(SessionMessage* msg)
	{
		if ( !msg ) return;

		if ( msg->getDeviceType() == ScaleDev )
		{
			switch ( msg->getEventType() )
			{
				case DtRestore:
				{
					Scale* scale = dynamic_cast<Scale*>(msg->getHfcObject());
					if ( !scale ) return;

					if ( msg->getFormatType() == DBIndependentFormatFormat )
					{
						edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();
						if ( !scalesRM ) return;

						// Ensure that the auto increment value is consistent with the new number received
						scalesRM->ensureAutoIncrementValue(scale->getIdentifier());


						saveScale(scale);

						Debug::trace("ScaleManager", "In upgrading the db received a scale record, created it and saved it");
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


	Scale * ScaleManager::findScaleByIdentifier(int id)
	{
		return findScaleByIdentifier(util_string::inttostring(id));
	}


	Scale * ScaleManager::findScaleByIdentifier(const std::string & id)
	{
		Scale * retrievedScale = 0;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();

		if ( id.length() == 0 || !scalesRM )
			return retrievedScale;

		try
		{
			Util::CSmartPtr<DBScale> dbScaleRec(scalesRM->findIdentifier(id));
			if ( dbScaleRec.notNull() )
			{
				Debug::trace("Scale", "findScaleByIdentifier() - found this record in the db");

				retrievedScale = createScale(dbScaleRec.get());
				if ( retrievedScale )
				{
					if ( retrievedScale->getErrorCode() == RECORD_CORRUPT )
					{
						if ( scalesRM->deleteRecord(dbScaleRec.get()) )
						{
							delete retrievedScale;
							retrievedScale = 0;
						}
					}
					else
					{
						if ( Debug::isTracing("Scale") )
							Debug::trace("Scale", "findScale() - created the hfc Scale object - Scale: " +
									util_string::ptrtostring(retrievedScale) + "  Scale identifier:" +
									util_string::inttostring(retrievedScale->getIdentifier()) + ", Scale hostname:" +
									retrievedScale->getHostName() + ",  Scale IPAddress:" + retrievedScale->getIPAddress());
					}
				}
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "Scale::findScaleByIdentifier() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return retrievedScale;
	}


	Scale * ScaleManager::findScaleByHostname(const std::string & hostname)
	{
		Scale * retrievedScale = 0;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();

		if ( hostname.length() == 0 || !scalesRM )
			return retrievedScale;

		try
		{
			Util::CSmartPtr<DBScale> dbScaleRec(scalesRM->findHostName(hostname));
			if ( dbScaleRec.notNull() )
			{
				Debug::trace("Scale", "findScale() - found this record in the db");

				retrievedScale = createScale(dbScaleRec.get());
				if ( retrievedScale )
				{
					if ( retrievedScale->getErrorCode() == RECORD_CORRUPT )
					{
						if ( scalesRM->deleteRecord(dbScaleRec.get()) )
						{
							delete retrievedScale;
							retrievedScale = 0;
						}
					}
					else
					{
						if ( Debug::isTracing("Scale") )
							Debug::trace("Scale", "findScale() - created the hfc Scale object - Scale: " +
									util_string::ptrtostring(retrievedScale) + "  Scale hostname:" +
									retrievedScale->getHostName() + "  Scale IPAddress:" + retrievedScale->getIPAddress());
					}
				}
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "Scale::findScale() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}


		return retrievedScale;
	}


	Scale * ScaleManager::findScaleByIPAddress(const std::string & ipAddress)
	{
		Scale * retrievedScale = 0;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();

		if ( ipAddress.length() == 0 || !scalesRM )
			return retrievedScale;

		try
		{
			Util::CSmartPtr<DBScale> dbScaleRec(scalesRM->findIPAddress(ipAddress));
			if ( dbScaleRec.notNull() )
			{
				Debug::trace("Scale", "findScale() - found this record in the db");

				retrievedScale = createScale(dbScaleRec.get());
				if ( retrievedScale )
				{
					if ( retrievedScale->getErrorCode() == RECORD_CORRUPT )
					{
						if ( scalesRM->deleteRecord(dbScaleRec.get()) )
						{
							delete retrievedScale;
							retrievedScale = 0;
						}
					}
					else
					{
						if ( Debug::isTracing("Scale") )
							Debug::trace("Scale", "findScale() - created the hfc Scale object - Scale: " +
									util_string::ptrtostring(retrievedScale) + "  Scale hostname:" +
									retrievedScale->getHostName() + "  Scale IPAddress:" + retrievedScale->getIPAddress());
					}
				}
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "Scale::findScale() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return retrievedScale;
	}


	Scale * ScaleManager::getThisScale()
	{
		Scale* retrievedScale = 0;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();

		if ( !scalesRM )
			return retrievedScale;

		try
		{
			Util::CSmartPtr<DBScale> dbScaleRec(scalesRM->findType(Scale::RoleThis));
			if ( dbScaleRec.notNull() )
			{
				Debug::trace("Scale", "getThisScale() - found ThisScale record in the db");

				retrievedScale = createScale(dbScaleRec.get());
				if ( retrievedScale )
				{
					if ( retrievedScale->getErrorCode() == RECORD_CORRUPT )
					{
						if ( scalesRM->deleteRecord(dbScaleRec.get()) )
						{
							delete retrievedScale;
							retrievedScale = 0;
						}
					}
					else
					{
						if ( Debug::isTracing("Scale") )
							Debug::trace("Scale", "getThisScale() - created the hfc Scale object - Scale: " +
									util_string::ptrtostring(retrievedScale) + "  Scale hostname:" +
									retrievedScale->getHostName() + "  Scale IPAddress:" + retrievedScale->getIPAddress());
					}
				}
			}
		}
		catch ( DBException & dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "Scale::getThisScale() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return retrievedScale;
	}


	Scale * ScaleManager::getPrimaryScale()
	{
		Scale* retrievedScale = 0;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();

		if ( !scalesRM )
			return retrievedScale;

		try
		{
			Util::CSmartPtr<DBScale> dbScaleRec(scalesRM->findType(Scale::RolePrimary));
			if ( dbScaleRec.notNull() )
			{
				Debug::trace("Scale", "getPrimaryScale() - found PrimaryScale record in the db");

				retrievedScale = createScale(dbScaleRec.get());
				if ( retrievedScale )
				{
					if ( retrievedScale->getErrorCode() == RECORD_CORRUPT )
					{
						if ( scalesRM->deleteRecord(dbScaleRec.get()) )
						{
							delete retrievedScale;
							retrievedScale = 0;
						}
					}
					else
					{
						if ( Debug::isTracing("Scale") )
							Debug::trace("Scale", "getPrimaryScale() - created the hfc Scale object - Scale: " +
									util_string::ptrtostring(retrievedScale) + "  Scale hostname:" +
									retrievedScale->getHostName() + "  Scale IPAddress:" + retrievedScale->getIPAddress());
					}
				}
			}
		}
		catch ( DBException & dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "Scale::getPrimaryScale() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return retrievedScale;
	}


	bool ScaleManager::initializeScalesQuery()
	{
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();
		if ( !scalesRM )
			return false;

		allScalesQuery_ = scalesRM->retrieveAll();

		return allScalesQuery_.notNull() ? allScalesQuery_->begin() != allScalesQuery_->end() : false;
	}


	Scale * ScaleManager::getFirstScale()
	{
		if ( allScalesQuery_.null() && !initializeScalesQuery() )
			return 0;

		allScalesIterator_ = new DBQuery<DBScale>::iterator(allScalesQuery_->begin());

		return *allScalesIterator_.get() != allScalesQuery_->end() ? createScale(&*(*allScalesIterator_.get())) : 0;
	}


	Scale * ScaleManager::getNextScale()
	{
		if ( allScalesQuery_.null() || allScalesIterator_.null() )
			return getFirstScale();

		++(*allScalesIterator_.get());

		return *allScalesIterator_.get() != allScalesQuery_->end() ? createScale(&*(*allScalesIterator_.get())) : 0;
	}


	bool ScaleManager::initializeSecondaryScalesQuery()
	{
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();
		if ( !scalesRM )
			return false;

		query_ = scalesRM->retrieveType(hfc::Scale::RoleSecondary);

		return query_.notNull() ? query_->begin() != query_->end() : false;
	}


	Scale * ScaleManager::getFirstSecondaryScale()
	{
		if ( query_.null() && !initializeSecondaryScalesQuery() )
			return 0;

		iterator_ = new DBQuery<DBScale>::iterator(query_->begin());

		return *iterator_.get() != query_->end() ? createScale(&*(*iterator_.get())) : 0;
	}


	Scale * ScaleManager::getNextSecondaryScale()
	{
		if ( query_.null() || iterator_.null() )
			return getFirstSecondaryScale();

		++(*iterator_.get());

		return *iterator_.get() != query_->end() ? createScale(&*(*iterator_.get())) : 0;
	}


	Scale * ScaleManager::createScale(const edb::DBScale * dbScaleRec)
	{
		if ( dbScaleRec )
		{
			Scale* scale = new Scale((Scale::ScaleState)dbScaleRec->getState());

			if ( dbScaleRec->isRecordValid() )
			{
				scale->setIdentifier(util_string::stringtoint(dbScaleRec->getIdentifier()));
				scale->setRole((Scale::ScaleRole)dbScaleRec->getType());
				scale->setHostName(dbScaleRec->getHostName());
				scale->setIPAddress(dbScaleRec->getIPAddress());
				scale->setCreationDateTime(dbScaleRec->getCreationDateTime());
				scale->setStateChangedDateTime(dbScaleRec->getStateChangedDateTime());
				scale->setRegistering(dbScaleRec->isRegistering());
				scale->setDeregistering(dbScaleRec->isDeregistering());
				scale->setNotifiedBatches(dbScaleRec->isNotifiedBatches());
				if ( !dbScaleRec->isNotifiedBatchesDateTimeNull() )
					scale->setNotifiedBatchesDateTime(dbScaleRec->getNotifiedBatchesDateTime());
				scale->setNotifiedOnline(dbScaleRec->isNotifiedOnline());
				if ( !dbScaleRec->isNotifiedOnlineDateTimeNull() )
					scale->setNotifiedOnlineDateTime(dbScaleRec->getNotifiedOnlineDateTime());
				scale->setRetryCount(dbScaleRec->getRetryCount());
				scale->setVersion(dbScaleRec->getVersion());
				scale->setVersionReceived(dbScaleRec->getVersionReceived());
				scale->setVersionReceivedDateTime(dbScaleRec->getVersionReceivedDateTime());
				if ( !dbScaleRec->isLastSuccessfulCommDateTimeNull() )
				{
					scale->setLastSuccessfulCommDateTime(dbScaleRec->getLastSuccessfulCommDateTime());
					scale->setLastCommSuccessful(true);
				}
				else if ( !dbScaleRec->isFirstFailedCommDateTimeNull() )
				{
					scale->setFirstFailedCommDateTime(dbScaleRec->getFirstFailedCommDateTime());
					scale->setLastCommSuccessful(false);
				}
				scale->setTotalSuccessfulCommunications(dbScaleRec->getTotalSuccessfulCommunications());
				scale->setTotalFailedCommunications(dbScaleRec->getTotalFailedCommunications());
				scale->setTotalBusyCommunications(dbScaleRec->getTotalBusyCommunications());

				scale->setErrorCode(RECORD_OK);

				Debug::trace("Scale", "Read Scale db record OK");
			}
			else
			{
				scale->setErrorCode(RECORD_CORRUPT);

				Debug::trace("Scale", "Read Scale db record CORRUPT!!");
			}

			return scale;
		}
		else
			Debug::trace("Scale", "Constructor passing in a db - dbrec IS NULL");

		return 0;
	}


	void ScaleManager::saveScale(Scale* scale)
	{
		Util::CSmartPtr<DBScale> dbScale;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();
		if ( !scalesRM )
			return;


		if ( scale->isThisScale() )
		{
			dbScale = scalesRM->findType(Scale::RoleThis);
		}
		else
		{
			const std::string & hostname = scale->getHostName();
			const std::string & ipAddress = scale->getIPAddress();

			if ( hostname.size() )
				dbScale = scalesRM->findHostName(hostname);
			if ( dbScale.null() && ipAddress.size() )
				dbScale = scalesRM->findIPAddress(ipAddress);
		}

		if ( dbScale.null() )
		{
			//	This must be a new record for the Database
			Debug::trace("Scale", "saveRecord() - don't have a db record, creating it...");

			dbScale = scalesRM->createRecord();
			scale->setIdentifier(util_string::stringtoint(dbScale->getIdentifier()));
		}
		
		if ( dbScale.notNull() )
		{
//			Debug::trace("Scale", "Going to set data elements");

			dbScale->setType(scale->getRole());
			dbScale->setHostName(scale->getHostName());
			dbScale->setIPAddress(scale->getIPAddress());
			if ( scale->getCreationDateTimeAsString().size() )
				dbScale->setCreationDateTime(scale->getCreationDateTimeAsString());
			else
				dbScale->setCreationDateTimeNull();
			dbScale->setState(scale->getState());
			if ( scale->getStateChangedDateTimeAsString().size() )
				dbScale->setStateChangedDateTime(scale->getStateChangedDateTimeAsString());
			else
				dbScale->setStateChangedDateTimeNull();
			dbScale->setRegistering(scale->isRegistering());
			dbScale->setDeregistering(scale->isDeregistering());
			dbScale->setNotifiedBatches(scale->isNotifiedBatches());
			if ( scale->isNotifiedBatches() && scale->getNotifiedBatchesDateTimeAsString().size() )
				dbScale->setNotifiedBatchesDateTime(scale->getNotifiedBatchesDateTimeAsString());
			else
				dbScale->setNotifiedBatchesDateTimeNull();
			dbScale->setNotifiedOnline(scale->isNotifiedOnline());
			if ( scale->isNotifiedOnline() && scale->getNotifiedOnlineDateTimeAsString().size() )
				dbScale->setNotifiedOnlineDateTime(scale->getNotifiedOnlineDateTimeAsString());
			else
				dbScale->setNotifiedOnlineDateTimeNull();
			dbScale->setRetryCount(scale->getRetryCount());
			dbScale->setVersion(scale->getVersion());
			dbScale->setVersionReceived(scale->getVersionReceived());
			if ( scale->getRole() != Scale::RoleThis )
			{
				if ( scale->getVersionReceived() > Batch::_invalidBatchVersion_ )
					dbScale->setVersionReceivedDateTime(scale->getVersionReceivedDateTimeAsString());
				else
					dbScale->setVersionReceivedDateTimeNull();
				if ( scale->isLastCommSuccessful() )
				{
					if ( scale->getLastSuccessfulCommDateTimeAsString().size() )
						dbScale->setLastSuccessfulCommDateTime(scale->getLastSuccessfulCommDateTimeAsString());
					else
						dbScale->setLastSuccessfulCommDateTimeNull();
					dbScale->setFirstFailedCommDateTimeNull();
				}
				else
				{
					dbScale->setLastSuccessfulCommDateTimeNull();
					if ( scale->getFirstFailedCommDateTimeAsString().size() )
						dbScale->setFirstFailedCommDateTime(scale->getFirstFailedCommDateTimeAsString());
					else
						dbScale->setFirstFailedCommDateTimeNull();
				}
				dbScale->setTotalSuccessfulCommunications(scale->getTotalSuccessfulCommunications());
				dbScale->setTotalFailedCommunications(scale->getTotalFailedCommunications());
				dbScale->setTotalBusyCommunications(scale->getTotalBusyCommunications());
			}

			Debug::trace("Scale", "Saving Scale " + scale->getHostName());

			try
			{
				scalesRM->saveRecord(dbScale.get());
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Scale::saveRecord() ********** database exception **********");
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}

//			Debug::trace("Scale", "Saved in the database");
		}
	}


	int ScaleManager::deleteScale(Scale* scale)
	{
		int result = 0;
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();
		if ( !scalesRM )
			return 0;


//		const std::string & hostname = scale->getHostName();
//		const std::string & ipAddress = scale->getIPAddress();

		Util::CSmartPtr<DBScale> dbScale(scalesRM->findIdentifier(util_string::inttostring(scale->getIdentifier())));
		if ( dbScale.notNull() )   //No need to delete if there is no record
		{
			if ( scale->isThisScale() )
				Debug::trace("Scale", "Deleting THIS Scale");
			else
				Debug::trace("Scale", "Deleting Scale " + (scale->getHostName().size() ? scale->getHostName() : scale->getIPAddress()));

			result = scalesRM->deleteRecord(dbScale.get());
		}

		return result;
	}


	int ScaleManager::deleteAllScales()
	{
		edb::DBScalesRM* scalesRM = mgrContainer_->getDBScalesRM();
		if ( !scalesRM )
			return 0;

		return scalesRM->deleteAll();
	}


	void ScaleManager::destroyQueriesAndIters()
	{
		/* When this function is called, coming out of a db compress, it is safe to remove all
		queries and iterators. */

		/* Destroy first the iterators. */
		allScalesIterator_ = 0; iterator_ = 0;

		/* Destroy now the queries. */
		allScalesQuery_ = 0; query_ = 0;
	}


}
