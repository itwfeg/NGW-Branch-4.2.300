#include "EvolWrapperKnifeHistoryManager.h"
#include "Session.h"
#include "SessionMessage.h"
#include "HFCManagerContainer.h"
#include "DBKnifeTimeHistoryRM.h"
#include "DBKnifeTimeHistory.h"
#include "HfcEvolWrapperKnifeTimeHistory.h"
#include "HfcEvolWrapperEventCapture.h"
#include "KnifeTimeHistory.h"

#include "DBException.h"
#include "Debug.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

#ifdef USE_ACCOUNTANT
int EvolWrapperKnifeHistoryManager::_dataType_ = 0;
std::string EvolWrapperKnifeHistoryManager::_dataTypeName_("hfc::EvolWrapperKnifeHistoryManager");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperKnifeHistoryManager::EvolWrapperKnifeHistoryManager(hfc::HFCManagerContainer* container)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	dbAccess = container;
	mgrContainer_ = container;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperKnifeHistoryManager::~EvolWrapperKnifeHistoryManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int EvolWrapperKnifeHistoryManager::deleteAllEvolWrapperKnifeHistories()
{
	edb::DBKnifeTimeHistoryRM* knifeTimeHistroyRM = mgrContainer_->getKnifeTimeHistoryRM();
	if ( !knifeTimeHistroyRM )
		return 0;

	return knifeTimeHistroyRM->deleteAll();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
HfcEvolWrapperKnifeTimeHistory *EvolWrapperKnifeHistoryManager::getEvolWrapperKnifeTimeHistory()
{
	Debug::trace("EvolWrapperKnifeHistoryManager", "getEvolWrapperKnifeTimeHistory()");
	return (new HfcEvolWrapperKnifeTimeHistory(dbAccess));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EvolWrapperKnifeHistoryManager::processWrapperKnifeTime(HfcEvolWrapperEventCapture *pKnifeTimeMsg)
{
	const bool traceOn( Debug::isTracing("EvolWrapperKnifeHistoryManager") );

	HobString msgDebug;
	if ( traceOn )
	{
		msgDebug = "processWrapperKnifeTime()";
		Debug::trace("EvolWrapperKnifeHistoryManager", msgDebug);

		msgDebug.append(":  ");
	}

	if ( !pKnifeTimeMsg )
		return false;

	KnifeTimeHistory::KnifeHistoryType installHistory;
	KnifeTimeHistory::KnifeHistoryType minHistory;
	KnifeTimeHistory::KnifeHistoryType maxHistory;

	if ( KnifeUpEvent == pKnifeTimeMsg->getEventCaptureType() )
	{
		installHistory = KnifeTimeHistory::KnifeUpTimeInstall;
		minHistory = KnifeTimeHistory::KnifeUpTimeMin;
		maxHistory = KnifeTimeHistory::KnifeUpTimeMax;
	}
	else if ( KnifeDownEvent == pKnifeTimeMsg->getEventCaptureType() )
	{
		installHistory = KnifeTimeHistory::KnifeDownTimeInstall;
		minHistory = KnifeTimeHistory::KnifeDownTimeMin;
		maxHistory = KnifeTimeHistory::KnifeDownTimeMax;
	}
	else
	{
		return false;
	}

	KnifeTimeHistory *pKnifeHistoryInstall = KnifeTimeHistory::findKnifeTimeHistory(installHistory, dbAccess);
	KnifeTimeHistory *pKnifeHistoryMin = KnifeTimeHistory::findKnifeTimeHistory(minHistory, dbAccess);
	KnifeTimeHistory *pKnifeHistoryMax = KnifeTimeHistory::findKnifeTimeHistory(maxHistory, dbAccess);

	if ( Debug::isTracing("KnifeTimeHistory.show") )
	{
		if ( !pKnifeHistoryInstall )
			Debug::trace("KnifeTimeHistory.show", "**  pKnifeHistoryInstall = 0  **");
		else
		{
			Debug::trace("KnifeTimeHistory.show", "***  pKnifeHistoryInstall:  ***");
			pKnifeHistoryInstall->show();
		}

		if ( !pKnifeHistoryMin )
			Debug::trace("KnifeTimeHistory.show", "**  pKnifeHistoryMin = 0  **");
		else
		{
			Debug::trace("KnifeTimeHistory.show", "***  pKnifeHistoryMin:  ***");
			pKnifeHistoryMin->show();
		}

		if ( !pKnifeHistoryMax )
			Debug::trace("KnifeTimeHistory.show", "**  pKnifeHistoryMax = 0  **");
		else
		{
			Debug::trace("KnifeTimeHistory.show", "***  pKnifeHistoryMax:  ***");
			pKnifeHistoryMax->show();
		}
		Debug::trace("KnifeTimeHistory.show", "***  pKnifeTimeMsg  ***");
		Debug::trace("KnifeTimeHistory.show", "knifeTime = " + util_string::uinttostring(pKnifeTimeMsg->getValue()) + "ms");
	}

	bool newHistory = false;

	//	Have we established an installed history?
	if( pKnifeHistoryInstall )
	{
		if ( traceOn )
			Debug::trace("EvolWrapperKnifeHistoryManager", "Found installed knife history");

		//	Compare the current knife time to the installed knife time,
		//	if the current time is not greater than the installed time.
		if( (pKnifeHistoryInstall->getKnifeTime() + 5) <  pKnifeTimeMsg->getValue() )
		{
			//	Have we established a new max history?
			if ( !pKnifeHistoryMax || ((pKnifeHistoryMax->getKnifeTime() + 5) < pKnifeTimeMsg->getValue()))
			{
				if ( traceOn )
					Debug::trace("EvolWrapperKnifeHistoryManager", "We have a new max knife history.");

				//	Create a new max history
				KnifeTimeHistory *pNewKnifeHistory = new KnifeTimeHistory(dbAccess);
				if ( pNewKnifeHistory )
				{
					pNewKnifeHistory->setKnifeHistoryType(maxHistory);
					pNewKnifeHistory->setKnifeTime(pKnifeTimeMsg->getValue());
					
					//	Save the new history
					pNewKnifeHistory->saveRecord();
					delete pNewKnifeHistory;

					newHistory = true;
				}
			}
		}
		//	Have we established a new min history?
		else if ( (pKnifeHistoryInstall->getKnifeTime() - 5) >  pKnifeTimeMsg->getValue() )
		{
			//	Have we established a max time yet?
			if ( !pKnifeHistoryMin || ((pKnifeHistoryMin->getKnifeTime() - 5) > pKnifeTimeMsg->getValue()) )
			{
				if ( traceOn )
					Debug::trace("EvolWrapperKnifeHistoryManager", "We have a new min knife history.");
				
				//	Create a new max history
				KnifeTimeHistory *pNewKnifeHistory = new KnifeTimeHistory(dbAccess);
				if ( pNewKnifeHistory )
				{
					pNewKnifeHistory->setKnifeHistoryType(minHistory);
					pNewKnifeHistory->setKnifeTime(pKnifeTimeMsg->getValue());
					
					//	Save the new history
					pNewKnifeHistory->saveRecord();
					delete pNewKnifeHistory;

					newHistory = true;
				}
			}
		}
		delete pKnifeHistoryInstall;
	}
	else
	{
		if ( traceOn )
			Debug::trace("EvolWrapperKnifeHistoryManager", "No installed knife history. We will create one.");

		KnifeTimeHistory *pNewKnifeHistory = new KnifeTimeHistory(dbAccess);
		if ( pNewKnifeHistory )
		{
			pNewKnifeHistory->setKnifeHistoryType(installHistory);
			pNewKnifeHistory->setKnifeTime(pKnifeTimeMsg->getValue());
			
			//	Save the new history
			pNewKnifeHistory->saveRecord();
			delete pNewKnifeHistory;

			newHistory = true;
		}
	}
	if ( pKnifeHistoryMax )
		delete pKnifeHistoryMax;
	if ( pKnifeHistoryMin )
		delete pKnifeHistoryMin;

	return newHistory;
}
