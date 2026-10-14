/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PresentationMethodManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "DataTypeEvent.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBPresentationMethodsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PresentationMethodManager::dataType = 0;
std::string PresentationMethodManager::dataTypeName("PresentationMethodManager"); 
#endif

PresentationMethodManager::PresentationMethodManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

PresentationMethodManager::PresentationMethodManager(Session *session)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mySession = session;
	dbAccess = session;
}

PresentationMethodManager::~PresentationMethodManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentPresentationMethod)
		delete currentPresentationMethod;

	destroyQueriesAndIters();
}


void PresentationMethodManager::init()
{
	mySession = (Session *) NULL;

	currentPresentationMethod = (PresentationMethod *) NULL;

	allPresentationMethodsQuery = (DBQuery<DBPresentationMethod> *) NULL;
	allPresentationMethodsIter  = (DBQuery<DBPresentationMethod>::const_iterator *) NULL;
}


/* If the first record was not read, this method will return NULL.
 */
PresentationMethod *PresentationMethodManager::firstPresentationMethod()
{
	if (!dbAccess)
		return (PresentationMethod *)NULL;

	DBPresentationMethodsRM	*dbPresentationMethodsRM = dbAccess->getDBPresentationMethodsRM();
	if (dbPresentationMethodsRM)
		return (PresentationMethod *)NULL;

	// If the user calls firstPresentationMethod() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allPresentationMethodsQuery = dbPresentationMethodsRM->retrieveAll();

	// Couldn't get the query from which to access the Classes, just return false.
	if (!allPresentationMethodsQuery)
		return (PresentationMethod *)NULL;

	DBQuery<DBPresentationMethod>::const_iterator tempIter = allPresentationMethodsQuery->begin();
	if (tempIter == allPresentationMethodsQuery->end())
		return (PresentationMethod *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextPresentationMethod()
	 * method call below).
	 */
	allPresentationMethodsIter = new DBQuery<DBPresentationMethod>::const_iterator(tempIter);
	DBPresentationMethod *pm = new DBPresentationMethod(*(*allPresentationMethodsIter));
	PresentationMethod *newPresentationMethod = new PresentationMethod(pm, dbAccess);
	return newPresentationMethod;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstPresentationMethod() method, the first call to this method will
 * result in the first record being returned.
 */
PresentationMethod *PresentationMethodManager::nextPresentationMethod()
{
	if (!dbAccess)
		return (PresentationMethod *)NULL;

	if (allPresentationMethodsQuery && !allPresentationMethodsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allPresentationMethodsQuery || !allPresentationMethodsIter)
		return firstPresentationMethod();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allPresentationMethodsIter);
	if (*allPresentationMethodsIter == allPresentationMethodsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Classes in the db. */
		destroyQueriesAndIters();
		return (PresentationMethod *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBPresentationMethod copy constructor in order for the Hfc Class to
	 * control the destruction of the DBPresentationMethod record that the iterator contains; otherwise, the DBClass will
	 * be deleted when the iterator goes out of scope.  The DBPresentationMethod copy will be deleted by the Class record
	 * created below.
	 */
	DBPresentationMethod *pm = new DBPresentationMethod(*(*allPresentationMethodsIter));
	PresentationMethod *newPresentationMethod = new PresentationMethod(pm, dbAccess);
	return newPresentationMethod;
}


/* Provide a countAll method so the caller doesn't need to know about a dbPresentationMethodsRM.
 */
int PresentationMethodManager::countAllPresentationMethods()
{
	if (!dbAccess)
		return -1;

	DBPresentationMethodsRM	*dbPresentationMethodsRM = dbAccess->getDBPresentationMethodsRM();
	if (dbPresentationMethodsRM)
		return -1;

	return dbPresentationMethodsRM->countAll();
}


PresentationMethod *PresentationMethodManager::getCurrentPresentationMethod()
{
	return currentPresentationMethod;
}
void PresentationMethodManager::setCurrentPresentationMethod(PresentationMethod *newPresentationMethod)
{
	if (currentPresentationMethod)
		delete currentPresentationMethod;
	currentPresentationMethod = newPresentationMethod;
}


void PresentationMethodManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	PresentationMethod *msgPresentationMethod = dynamic_cast<PresentationMethod *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("PresentationMethodManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgPresentationMethod);
			break;

		case DtWrite:
			handleWrite(msgPresentationMethod);
			break;

		case DtDelete:
			handleDelete(msgPresentationMethod);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleSave(msgPresentationMethod);
			break;

		case DtRestore:
			handleRestore(msgPresentationMethod);
			break;

		default:
			Debug::trace("PresentationMethodManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void PresentationMethodManager::handleRead(PresentationMethod *msgPresentationMethod)
{
	if (!msgPresentationMethod || !dbAccess)
		return;

	PresentationMethod *lookedUpPresentationMethod = PresentationMethod::findMethod(msgPresentationMethod->getType(), dbAccess);

	if (!lookedUpPresentationMethod)
	{
		if (mySession)
		{
			// Send error message to browser indicating PresentationMethod not found.
			SessionMessage errMsg(Error, /*Err*/PresentationMethodDev);
			errMsg.setFormatType(BrowserFormat);
			mySession->send(&errMsg, Messenger::DEST_UI);
		}
		return;
	}

	// We have a new current PresentationMethod.
	setCurrentPresentationMethod(lookedUpPresentationMethod);
	Debug::trace("PresentationMethodManager.read", HobString("handleRead(): currentPresentationMethod = ") + HobString(Hex::toString((long long) currentPresentationMethod)));

	if (mySession)
	{
		// Send the PresentationMethod found back to the Browser.
		// Is this the behavior for ALL PresentationMethodRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a PresentationMethod, but doesn't want the data sent back?
		SessionMessage presentationMethodData(PresentationMethodDev, DtWrite);
		presentationMethodData.setFormatType(BrowserFormat);
		presentationMethodData.setHfcObject(currentPresentationMethod);

		// Send the PresentationMethod to the UI (Browser)
		int bytesSent = mySession->send(&presentationMethodData, Messenger::DEST_UI);
		if (bytesSent < 0)
		{
			// todo - check correct number of bytes sent?
		}
	}
}


void PresentationMethodManager::handleWrite(PresentationMethod *modifiedPresentationMethod)
{
	if ((!modifiedPresentationMethod) || (!currentPresentationMethod))
		return;

	Debug::trace("PresentationMethodManager", HobString("handleWrite(): currentPresentationMethod = ") + HobString(Hex::toString((long long) currentPresentationMethod)) +
											  HobString("  , modifiedPresentationMethod = ") + HobString(Hex::toString((long long) modifiedPresentationMethod)));
  	currentPresentationMethod->updatePresentationMethod(modifiedPresentationMethod);
}


void PresentationMethodManager::handleDelete(PresentationMethod *msgPresentationMethod)
{
	if (!msgPresentationMethod || !dbAccess)
		return;

	PresentationMethod *existingPresentationMethod = PresentationMethod::findMethod(msgPresentationMethod->getType(), dbAccess);
	if (existingPresentationMethod)
	{
		existingPresentationMethod->deleteRecord();
		delete existingPresentationMethod;
		Debug::trace("PresentationMethodManager", HobString("handleDelete - PresentationMethod deleted - presentationMethodType:") +
														HobString(Long::toString((long)msgPresentationMethod->getType())));
	}
	else
		Debug::trace("PresentationMethodManager", HobString("handleDelete - PresentationMethod to be deleted was not in the database to delete - presentationType:") +
														HobString(Long::toString((long)msgPresentationMethod->getType())));
}


void PresentationMethodManager::handleDeleteAll()
{
	Debug::trace("PresentationMethodManager", "handleDeleteAll - deleting all presentationMethods");

	if (!dbAccess)
		return;

	DBPresentationMethodsRM	*dbPresentationMethodsRM = dbAccess->getDBPresentationMethodsRM();
	if (dbPresentationMethodsRM)
		return;

	dbPresentationMethodsRM->deleteAll();

	destroyQueriesAndIters();

	Debug::trace("PresentationMethodManager", "handleDeleteAll - will create reserved records next");
	PresentationMethod::verifyReservedRecords(dbAccess);
	Debug::trace("PresentationMethodManager", "handleDeleteAll - leaving");
}


void PresentationMethodManager::handleSave(PresentationMethod *modifiedPresentationMethod)
{
	Debug::trace("PresentationMethodManager", HobString("handleSave(): currentPresentationMethod = ") + HobString(Hex::toString((long long) currentPresentationMethod)));
	if (currentPresentationMethod)
	{
		if (modifiedPresentationMethod)
		  	currentPresentationMethod->updatePresentationMethod(modifiedPresentationMethod);
		currentPresentationMethod->saveRecord();
	}
}


void PresentationMethodManager::handleRestore(PresentationMethod *restorePresentationMethod)
{
	if (!restorePresentationMethod || !dbAccess)
		return;

	restorePresentationMethod->setDBAccess(dbAccess);

	PresentationMethod *existingPresentationMethod = PresentationMethod::findMethod(restorePresentationMethod->getType(), dbAccess);
	if (existingPresentationMethod)
	{
		if (existingPresentationMethod->isRecordValid())
		{
			existingPresentationMethod->updatePresentationMethod(restorePresentationMethod);
			existingPresentationMethod->saveRecord();								
		}
		else
		{
			existingPresentationMethod->deleteRecord();	   
			restorePresentationMethod->saveRecord();
		}

		delete existingPresentationMethod;
	}
	else
		restorePresentationMethod->saveRecord();
}


void PresentationMethodManager::destroyQueriesAndIters()
{
	if (allPresentationMethodsIter)
	{
		delete allPresentationMethodsIter;
		allPresentationMethodsIter = (DBQuery<DBPresentationMethod>::const_iterator *)NULL;
	}

	if (allPresentationMethodsQuery)
	{
		delete allPresentationMethodsQuery;
		allPresentationMethodsQuery = (DBQuery<DBPresentationMethod> *)NULL;
	}
}
