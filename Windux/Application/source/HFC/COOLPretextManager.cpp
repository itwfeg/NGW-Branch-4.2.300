/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "COOLPretextManager.h"
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

int COOLPretextManager::dataType = 0;
std::string COOLPretextManager::dataTypeName("COOLPretextManager"); 
#endif


COOLPretextManager::COOLPretextManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

COOLPretextManager::COOLPretextManager(hfc::HFCManagerContainer *container)
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

COOLPretextManager::~COOLPretextManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//destroyQueriesAndIters();
}


void COOLPretextManager::init()
{
	mySession = 0;
}


DBSession *COOLPretextManager::getMySession()
{
	return mySession;
}
void COOLPretextManager::setMySession(DBSession *session)
{
	mySession = session;
}


COOLPretext *COOLPretextManager::getFirstRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->firstCOOLPretext();
	return 0;
}


COOLPretext *COOLPretextManager::getNextRecord()
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->nextCOOLPretext();
	return 0;
}


void COOLPretextManager::handleDeleteAll()
{
	handleDeleteAll(RECORD_NOT_FOUND);
}
void COOLPretextManager::handleDeleteAll(HobRecordNum deptNum)
{
	if (mgrContainer_ && mgrContainer_->getCOOLManager())
		return mgrContainer_->getCOOLManager()->handleDeleteAllCOOLPretexts(deptNum);
}
