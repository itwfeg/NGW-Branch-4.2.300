/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLCategoryManager.h"
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

int COOLCategoryManager::dataType = 0;
std::string COOLCategoryManager::dataTypeName("COOLCategoryManager"); 
#endif


COOLCategoryManager::COOLCategoryManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLCategoryManager::COOLCategoryManager(hfc::HFCManagerContainer *container)
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

COOLCategoryManager::~COOLCategoryManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//destroyQueriesAndIters();
}


void COOLCategoryManager::init()
{
	mySession = 0;
}


DBSession *COOLCategoryManager::getMySession()
{
	return mySession;
}
void COOLCategoryManager::setMySession(DBSession *session)
{
	mySession = session;
}


COOLCategory *COOLCategoryManager::getFirstRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->firstCOOLCategory();
	return 0;
}


COOLCategory *COOLCategoryManager::getNextRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->nextCOOLCategory();
	return 0;
}


void COOLCategoryManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void COOLCategoryManager::handleDeleteAll(HobRecordNum deptNum)
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->handleDeleteAllCOOLCategories(deptNum);
}
