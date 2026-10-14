/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLListTextManager.h"
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

int COOLListTextManager::dataType = 0;
std::string COOLListTextManager::dataTypeName("COOLListTextManager"); 
#endif


COOLListTextManager::COOLListTextManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLListTextManager::COOLListTextManager(hfc::HFCManagerContainer *container)
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

COOLListTextManager::~COOLListTextManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//destroyQueriesAndIters();
}


void COOLListTextManager::init()
{
	mySession = 0;
}


DBSession *COOLListTextManager::getMySession()
{
	return mySession;
}
void COOLListTextManager::setMySession(DBSession *session)
{
	mySession = session;
}


COOLListText *COOLListTextManager::getFirstRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->firstCOOLListText();
	return 0;
}


COOLListText *COOLListTextManager::getNextRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->nextCOOLListText();
	return 0;
}


void COOLListTextManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void COOLListTextManager::handleDeleteAll(HobRecordNum deptNum)
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->handleDeleteAllCOOLListTexts(deptNum);
}
