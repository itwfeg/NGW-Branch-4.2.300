/* Evolution Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "COOLManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "HfcError.h"

#include "DBAccess.h"

#include "COOLDevEvent.h"

#include "AllCoolTotals.h"
#include "CoolTotal.h"
#include "COOLShortlist.h"

#include "DBCoolTotal.h"

#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"
#include "DBCoolTotalsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCOOLTotalMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	if (Debug::isTracing("COOLManager"))
	{
		HobString myDebug("handleCOOLTotalMessage, handling event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("COOLManager", myDebug);
	}
	switch (msg->getEventType())
	{
		case COOLDevReadAllCOOLTotalsForUltima:
			handleReadAllCOOLTotals();
			break;

		case COOLDevDeleteAllCOOLTotals:
			handleDeleteAllCOOLTotals();
			break;

		default:
			if (Debug::isTracing("COOLManager"))
			{
				HobString myDebug("UNKNOWN EVENT: ");
				myDebug.append(Integer::toString(event)).append(" !!!");
				Debug::trace("COOLManager", myDebug);
			}
			break;
	}
}

COOLTotal *COOLManager::getCurrentCoolTotal()
{
	return currentCoolTotal;
}
void COOLManager::setCurrentCoolTotal(COOLTotal *text)
{
	if (currentCoolTotal)
		delete currentCoolTotal;
	currentCoolTotal = text;
}


COOLTotal *COOLManager::firstCOOLTotal()
{
	if (!dbAccess)
		return (COOLTotal *)NULL;

	DBCoolTotalsRM *dbCoolTotalsRM = dbAccess->getDBCoolTotalsRM();
	if (!dbCoolTotalsRM)
		return (COOLTotal *)NULL;

	// If the user calls firstCOOLTotal() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLTotalsQueryAndIter();

	allCoolTotalsQuery = dbCoolTotalsRM->retrieveAll();

	// Couldn't get the query from which to access the Texts, just return false.
	if (!allCoolTotalsQuery)
		return (COOLTotal *)NULL;

	DBQuery<DBCoolTotal>::const_iterator tempIter = allCoolTotalsQuery->begin();
	if (tempIter == allCoolTotalsQuery->end())
		return (COOLTotal *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLTotal()
	 * method call below).
	 */
	allCoolTotalsIter = new DBQuery<DBCoolTotal>::const_iterator(tempIter);
	DBCoolTotal *c = new DBCoolTotal(*(*allCoolTotalsIter));
	COOLTotal *newText = new COOLTotal(c, dbAccess);
	return newText;
}

COOLTotal *COOLManager::nextCOOLTotal()
{
	if (!dbAccess)
		return (COOLTotal *)NULL;

	if (allCoolTotalsQuery && !allCoolTotalsQuery->isEnabled())
		destroyCOOLTotalsQueryAndIter();

	if (!allCoolTotalsQuery || !allCoolTotalsIter)
		return firstCOOLTotal();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolTotalsIter);
	if (*allCoolTotalsIter == allCoolTotalsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all COOLTotals in the db. */
		destroyCOOLTotalsQueryAndIter();
		return (COOLTotal *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolTotal copy constructor in order for the Hfc COOLTotal to
	 * control the destruction of the DBCoolTotal record that the iterator contains; otherwise, the DBCoolTotal will
	 * be deleted when the iterator goes out of scope.  The DBCoolTotal copy will be deleted by the COOLTotal record
	 * created below.
	 */
	DBCoolTotal *c = new DBCoolTotal(*(*allCoolTotalsIter));
	COOLTotal *newText = new COOLTotal(c, dbAccess);
	return newText;
}


int COOLManager::countAllCOOLTotals()
{
	if (!dbAccess)
		return -1;

	DBCoolTotalsRM *dbCoolTotalsRM = dbAccess->getDBCoolTotalsRM();
	if (!dbCoolTotalsRM)
		return -1;

	return dbCoolTotalsRM->countAll();
}


void COOLManager::handleReadAllCOOLTotals()
{
	if (!dbAccess)
		return;

	AllCoolTotals *myList = new AllCoolTotals();
	if (!myList)
		return;

	myList->findAllCoolTotals(dbAccess);

	if (Debug::isTracing("COOLManager.COOLTotal"))
	{
		HobString myDebug("handling event: ");
		myDebug.append("handleCOOLTotalReadAll(): myList size = ").append(Integer::toString(myList->getRecordNumList()->size()));
		Debug::trace("COOLManager.COOLTotal", myDebug);
	}

	if (mySession)
	{
		// Send the class found back to the Browser.
		// Is this the behavior for ALL COOLCoolTotalRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a COOLTotal, but doesn't want the data sent back?
		SessionMessage ctData(COOLDev, COOLDevWriteAllCOOLTotals);
		ctData.setFormatType(BrowserFormat);
		ctData.setHfcObject(myList);

		// Send the product to the UI (Browser)
		int bytesSent = mySession->send(&ctData, Messenger::DEST_UI);
		if (bytesSent < 0)
		{
			// todo - check correct number of bytes sent?
		}
	}
}

void COOLManager::handleDeleteAllCOOLTotals()
{
	Debug::trace("COOLManager", "handleDeleteAllCOOLTotals - deleting all COOL totals");

	if (!dbAccess)
		return;

	DBCoolTotalsRM *dbCoolTotalsRM = dbAccess->getDBCoolTotalsRM();
	if (!dbCoolTotalsRM)
		return;

	dbCoolTotalsRM->deleteAll();

	destroyCOOLTotalsQueryAndIter();
}
