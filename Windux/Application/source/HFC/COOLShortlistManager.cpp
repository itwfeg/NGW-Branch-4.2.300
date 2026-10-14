/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLShortlistManager.h"
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

int COOLShortlistManager::dataType = 0;
std::string COOLShortlistManager::dataTypeName("COOLShortlistManager"); 
#endif


COOLShortlistManager::COOLShortlistManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLShortlistManager::COOLShortlistManager(hfc::HFCManagerContainer *container)
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

COOLShortlistManager::~COOLShortlistManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//destroyQueriesAndIters();
}


void COOLShortlistManager::init()
{
	mySession = 0;
}


DBSession *COOLShortlistManager::getMySession()
{
	return mySession;
}
void COOLShortlistManager::setMySession(DBSession *session)
{
	mySession = session;
}


COOLShortlist *COOLShortlistManager::getFirstRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->firstCOOLShortlist();
	return 0;
}


COOLShortlist *COOLShortlistManager::getNextRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->nextCOOLShortlist();
	return 0;
}


void COOLShortlistManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void COOLShortlistManager::handleDeleteAll(HobRecordNum deptNum)
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->handleDeleteAllCOOLShortlists(deptNum);
}
