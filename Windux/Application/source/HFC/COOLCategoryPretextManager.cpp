/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLCategoryPretextManager.h"
#include "COOLManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "DataTypeEvent.h"
#include "Configuration.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int COOLCategoryPretextManager::dataType = 0;
std::string COOLCategoryPretextManager::dataTypeName("COOLCategoryPretextManager"); 
#endif


COOLCategoryPretextManager::COOLCategoryPretextManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLCategoryPretextManager::COOLCategoryPretextManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

COOLCategoryPretextManager::~COOLCategoryPretextManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//destroyQueriesAndIters();
}


void COOLCategoryPretextManager::init()
{
	mySession = 0;
}


DBSession *COOLCategoryPretextManager::getMySession()
{
	return mySession;
}
void COOLCategoryPretextManager::setMySession(DBSession *session)
{
	mySession = session;
}


COOLCategoryPretext *COOLCategoryPretextManager::getFirstRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->firstCOOLCategoryPretext();
	return 0;
}


COOLCategoryPretext *COOLCategoryPretextManager::getNextRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->nextCOOLCategoryPretext();
	return 0;
}


void COOLCategoryPretextManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void COOLCategoryPretextManager::handleDeleteAll(HobRecordNum deptNum)
{
	Debug::trace("COOLManager", "handleDeleteAll - COOLCategoryPretexts");

	if (!mgrContainer_)
		return;

	DataManager *myManager = mgrContainer_->getCOOLCategoryPretextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolCategoryPreTexts))
	{
		// batch the delete all command; don't actually do it right now.
		SessionMessage message(myManager->getDevice(), myManager->getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (!dbAccess)
			return;

		DBCoolCategoryPreTextsRM *dbRM = dbAccess->getDBCoolCategoryPreTextsRM();
		if (!dbRM)
			return;

		if (deptNum == RECORD_NOT_FOUND)
			dbRM->deleteAll();
		else
			dbRM->deleteAllForDept(longtostring(deptNum));
	}
}
