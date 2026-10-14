#include "EvolWrapperErrorHistoryManager.h"
#include "HfcEvolWrapperErrorHistory.h"
#include "HfcEvolWrapperErrorHistoryList.h"
#include "HFCManagerContainer.h"
#include "DBException.h"
#include "Session.h"
#include "SessionMessage.h"
#include "Debug.h"
#include "Integer.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int EvolWrapperErrorHistoryManager::_dataType_ = 0;
	std::string EvolWrapperErrorHistoryManager::_dataTypeName_("hfc::EvolWrapperErrorHistoryManager");
#endif

EvolWrapperErrorHistoryManager::EvolWrapperErrorHistoryManager(hfc::HFCManagerContainer* container) 
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	mgrContainer_ = container;
	dbAccess = container;
	
	query_ = 0;
	allWrapperErrorHistoryIterator_ = 0; 
	allWrapperErrorHistoryQuery_ = 0;
	iterator_ = 0;
	pErrorList = 0;
}

EvolWrapperErrorHistoryManager::~EvolWrapperErrorHistoryManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	destroyQueriesAndIters();
	if (pErrorList)
		delete pErrorList;
}

void EvolWrapperErrorHistoryManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	int event = msg->getEventType();
	Debug::trace("EvolWrapperErrorHistoryManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case WrEvolErrorHistory:
		{
			Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistory");
			HfcEvolWrapperErrorHistory *pHfcErrorHistory = dynamic_cast<HfcEvolWrapperErrorHistory *> (msg->getHfcObject());
			if(pHfcErrorHistory)
			{
				Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistory:  Error = " + Integer::toString(pHfcErrorHistory->getWrapperError()));
				Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistory:   tray = " + pHfcErrorHistory->getTrayName());
				saveEvolWrapperErrorHistory(pHfcErrorHistory);
			}
			break;
		}

		case WrEvolReqErrorHistory:
		{
			Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolReqErrorHistory");
			retrieveWrapperErrorHistory();
			break;
		}

		case WrEvolErrorHistoryListNext:
		{
			Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistoryListNext");
			if (!pErrorList)
				retrieveWrapperErrorHistory();
			else
			{
				pErrorList->gotoNextPage();

				SessionMessage response(Wrapper, WrEvolErrorHistoryList);
				response.setFormatType(BrowserFormat);
				response.setHfcObject(pErrorList);
			
				mySession_->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case WrEvolErrorHistoryListPrevious:
		{
			Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistoryListPrevious");
			if (!pErrorList)
				retrieveWrapperErrorHistory();
			else
			{
				pErrorList->gotoPreviousPage();

				SessionMessage response(Wrapper, WrEvolErrorHistoryList);
				response.setFormatType(BrowserFormat);
				response.setHfcObject(pErrorList);
			
				mySession_->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case WrEvolErrorHistoryListDone:
		{
			Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistoryListDone");
			clearErrorList();
			break;
		}

		case WrEvolErrorHistoryClear:
		{
			Debug::trace("EvolWrapperErrorHistoryManager", "WrEvolErrorHistoryClear");
			int recordsDeleted = deleteAllEvolWrapperErrorHistories();
			Debug::trace("EvolWrapperErrorHistoryManager", "recordsDeleted = " + Integer::toString(recordsDeleted));
			break;
		}
		default:
			Debug::trace("EvolWrapperErrorHistoryManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}

void EvolWrapperErrorHistoryManager::saveEvolWrapperErrorHistory(EvolWrapperErrorHistory* evolWrapperErrorHistory)
{
	Util::CSmartPtr<DBWrapperErrorHistory> dbWrapperErrorHistoryRec;
	edb::DBWrapperErrorHistoryRM* wrapperErrorHistroyRM = mgrContainer_->getWrapperErrorHistoryRM();
	
	if( !wrapperErrorHistroyRM ) 
		return;
	
	//all records should be in decending (dateTime) order (oldest at bottom).
	if( wrapperErrorHistroyRM->countAll() > maxWrapperErrorRecords_ )
	{
		//delete the oldest record.
		allWrapperErrorHistoryQuery_ = wrapperErrorHistroyRM->retrieveAll();
		iterator_ = new DBQuery<DBWrapperErrorHistory>::iterator(allWrapperErrorHistoryQuery_->begin());

		wrapperErrorHistroyRM->deleteRecord(&*(*iterator_.get()));
	}

	dbWrapperErrorHistoryRec = wrapperErrorHistroyRM->createRecord();

	if ( dbWrapperErrorHistoryRec.notNull() )
	{			
		dbWrapperErrorHistoryRec->setErrorCode(evolWrapperErrorHistory->getWrapperError());
		dbWrapperErrorHistoryRec->setErrorDateTime(evolWrapperErrorHistory->getDateTime());
		dbWrapperErrorHistoryRec->setErrorPkgLength(evolWrapperErrorHistory->getPackageLength());
		dbWrapperErrorHistoryRec->setErrorPkgWidth(evolWrapperErrorHistory->getPackageWidth());
		dbWrapperErrorHistoryRec->setErrorPkgHeight(evolWrapperErrorHistory->getPackageHeight());
		dbWrapperErrorHistoryRec->setErrorFilmUsed(evolWrapperErrorHistory->getFilmUsed());
		
		dbWrapperErrorHistoryRec->setErrorBestFit(evolWrapperErrorHistory->getUsedBestFit());
		dbWrapperErrorHistoryRec->setErrorPerNet(evolWrapperErrorHistory->getStretchModified());
		
		dbWrapperErrorHistoryRec->setErrorTrayName(evolWrapperErrorHistory->getTrayName());

		Debug::trace("EvolWrapperErrorHistoryManager", "Saving evolWrapperErrorHistory " + util_string::inttostring(evolWrapperErrorHistory->getWrapperError()));

		try
		{
			wrapperErrorHistroyRM->saveRecord(dbWrapperErrorHistoryRec.get());
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "EvolWrapperKnifeHistoryManager::saveRecord() ********** database exception **********");
			Debug::trace("DBAccess.exception", "     DBException: type:" +
					util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
		}
	}
}

int EvolWrapperErrorHistoryManager::deleteAllEvolWrapperErrorHistories()
{
	edb::DBWrapperErrorHistoryRM* wrapperErrorHistroyRM = mgrContainer_->getWrapperErrorHistoryRM();
	if ( !wrapperErrorHistroyRM )
		return 0;

	return wrapperErrorHistroyRM->deleteAll();

}

bool EvolWrapperErrorHistoryManager::initializeEvolWrapperErrorHistoryQuery()
{
	edb::DBWrapperErrorHistoryRM* wrapperErrorHistroyRM = mgrContainer_->getWrapperErrorHistoryRM();
	if ( !wrapperErrorHistroyRM )
		return false;

	allWrapperErrorHistoryQuery_ = wrapperErrorHistroyRM->retrieveAll();

	return allWrapperErrorHistoryQuery_.notNull() ? allWrapperErrorHistoryQuery_->rbegin() != allWrapperErrorHistoryQuery_->rend() : false;
}

EvolWrapperErrorHistory *EvolWrapperErrorHistoryManager::getFirstEvolWrapperErrorHistory()
{
	if ( allWrapperErrorHistoryQuery_.null() && !initializeEvolWrapperErrorHistoryQuery() )
		return 0;

	allWrapperErrorHistoryIterator_ = new DBQuery<DBWrapperErrorHistory>::iterator(allWrapperErrorHistoryQuery_->rbegin());
	
	return *allWrapperErrorHistoryIterator_.get() != allWrapperErrorHistoryQuery_->rend() ? createEvolWrapperErrorHistory(&*(*allWrapperErrorHistoryIterator_.get())) : 0;

}

EvolWrapperErrorHistory *EvolWrapperErrorHistoryManager::getNextEvolWrapperErrorHistory()
{
	if ( allWrapperErrorHistoryQuery_.null() || allWrapperErrorHistoryIterator_.null() )
		return getFirstEvolWrapperErrorHistory();

	--(*allWrapperErrorHistoryIterator_.get());

	return *allWrapperErrorHistoryIterator_.get() != allWrapperErrorHistoryQuery_->rend() ? createEvolWrapperErrorHistory(&*(*allWrapperErrorHistoryIterator_.get())) : 0;
}

int EvolWrapperErrorHistoryManager::getWrapperErrorRecordCount()
{
	int recordCount = 0;
	edb::DBWrapperErrorHistoryRM* wrapperErrorHistroyRM = mgrContainer_->getWrapperErrorHistoryRM();	
	
	if(!wrapperErrorHistroyRM)
	return recordCount;

	return recordCount = wrapperErrorHistroyRM->countAll();
}

EvolWrapperErrorHistory *EvolWrapperErrorHistoryManager::createEvolWrapperErrorHistory(edb::DBWrapperErrorHistory *dbWrapperErrorHistoryRec)
{
	if ( dbWrapperErrorHistoryRec )
	{
		EvolWrapperErrorHistory *evolWrapperErrorHistory = new EvolWrapperErrorHistory();

		if ( dbWrapperErrorHistoryRec->isRecordValid() )
		{
			evolWrapperErrorHistory->setWrapperError(dbWrapperErrorHistoryRec->getErrorCode());
			evolWrapperErrorHistory->setPackageLength(dbWrapperErrorHistoryRec->getErrorPkgLength());
			evolWrapperErrorHistory->setPackageWidth(dbWrapperErrorHistoryRec->getErrorPkgWidth());
			evolWrapperErrorHistory->setPackageHeight(dbWrapperErrorHistoryRec->getErrorPkgHeight());
			evolWrapperErrorHistory->setFilmUsed((WrFilmWidthSelection)dbWrapperErrorHistoryRec->getErrorFilmUsed());
			evolWrapperErrorHistory->setUsedBestFit(dbWrapperErrorHistoryRec->getErrorBestFit());
			evolWrapperErrorHistory->setStretchModified(dbWrapperErrorHistoryRec->getErrorPerNet());
			
			std::string trayName = dbWrapperErrorHistoryRec->getErrorTrayName();
			evolWrapperErrorHistory->setTrayName(trayName);
			
			std::string dateTime = dbWrapperErrorHistoryRec->getErrorDateTime();
			evolWrapperErrorHistory->setDateTime(dateTime);
			
			//evolWrapperErrorHistory->setErrorCode(RECORD_OK);

			Debug::trace("EvolWrapperErrorHistoryManager", "Read evolWrapperErrorHistory db record OK");
		}
		else
		{
			//evolWrapperErrorHistory->setErrorCode(RECORD_CORRUPT);

			Debug::trace("EvolWrapperErrorHistoryManager", "Read evolWrapperErrorHistory db record CORRUPT!!");
		}

		return evolWrapperErrorHistory;
	}
	else
		Debug::trace("EvolWrapperErrorHistoryManager", "Constructor passing in a db - dbrec IS NULL");

	return 0;
}

void EvolWrapperErrorHistoryManager::destroyQueriesAndIters()
{
	/* When this function is called, coming out of a db compress, it is safe to remove all
	queries and iterators. */

	/* Destroy first the iterators. */
	allWrapperErrorHistoryIterator_ = 0; iterator_ = 0;

	/* Destroy now the queries. */
	allWrapperErrorHistoryQuery_ = 0; query_ = 0;
}


void EvolWrapperErrorHistoryManager::retrieveWrapperErrorHistory()
{
	Debug::trace("EvolWrapperErrorHistoryManager", "retrieveWrapperErrorHistory():   Begin.");
	DBWrapperErrorHistoryRM* wrapperErrorHistoryRM = mgrContainer_->getWrapperErrorHistoryRM();

	if (!wrapperErrorHistoryRM)
		return;

	clearErrorList();

	//int recordCount = countAllEvolWrapperErrors(true);
	int recordCount = getWrapperErrorRecordCount();
	Debug::trace("EvolWrapperErrorHistoryManager", "retrieveWrapperErrorHistory():  recordCount = " + Integer::toString(recordCount));
	
	if (recordCount)
	{
		DBQuery<DBWrapperErrorHistory> *wrErrorHistoryQuery_ = wrapperErrorHistoryRM->retrieveAll();
		if ( !wrErrorHistoryQuery_ )
			return;

		pErrorList = new HfcEvolWrapperErrorHistoryList();
		if ( !pErrorList )
			return;

		DBWrapperErrorHistory *weh = 0;

		DBQuery<DBWrapperErrorHistory>::const_iterator tempIter = wrErrorHistoryQuery_->rbegin();
		while (tempIter != wrErrorHistoryQuery_->rend())
		{
			weh = new DBWrapperErrorHistory(*tempIter);			
			if (weh)
			{
				HfcEvolWrapperErrorHistory *eweh = new HfcEvolWrapperErrorHistory(weh, dbAccess);
				if (eweh)
				{
					pErrorList->addToList(eweh);
					delete eweh;
					eweh = 0;
				}
			}
			--tempIter;
		}
		pErrorList->gotoFirstPage();
		SessionMessage response(Wrapper, WrEvolErrorHistoryList);
		response.setFormatType(BrowserFormat);
		response.setHfcObject(pErrorList);
	
		mySession_->send(&response, Messenger::DEST_UI);

		delete wrErrorHistoryQuery_;
	}
	Debug::trace("EvolWrapperErrorHistoryManager", "retrieveWrapperErrorHistory():   End.");
}

void EvolWrapperErrorHistoryManager::clearErrorList()
{
	if (pErrorList)
	{
		delete pErrorList;
		pErrorList = 0;
	}
}
