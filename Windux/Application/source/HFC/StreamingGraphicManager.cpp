/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "StreamingGraphicManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "DataTypeEvent.h"
#include "StreamingGraphicEvent.h"
#include "StreamingGraphicsList.h"
#include "GraphicFileNames.h"
#include "Configuration.h"

//#include "SynchronizationManagerProxy.h"
//#include "SynchronizationChange.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int StreamingGraphicManager::dataType = 0;
std::string StreamingGraphicManager::dataTypeName("StreamingGraphicManager"); 
#endif

StreamingGraphicManager::StreamingGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

StreamingGraphicManager::StreamingGraphicManager(hfc::HFCManagerContainer *container)
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

StreamingGraphicManager::~StreamingGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentStreamingGraphic)
		delete currentStreamingGraphic;

	if (streamingGraphicsList)
		delete streamingGraphicsList;

	destroyQueriesAndIters();
}


void StreamingGraphicManager::init()
{
	mySession = 0;

	currentStreamingGraphic = 0;
	streamingGraphicsList = 0;
	lastSlideNumber = RECORD_NOT_FOUND;

	allStreamingGraphicsQuery = 0;
	allStreamingGraphicsIter  = 0;
}


DBSession *StreamingGraphicManager::getMySession()
{
	return mySession;
}
void StreamingGraphicManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
StreamingGraphic *StreamingGraphicManager::firstStreamingGraphic()
{
	if (!dbAccess)
		return (StreamingGraphic *)NULL;

	DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
	if (!dbStreamingGraphicsRM)
		return (StreamingGraphic *)NULL;

	// If the user calls firstStreamingGraphic() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allStreamingGraphicsQuery = dbStreamingGraphicsRM->retrieveAll();

	// Couldn't get the query from which to access the StreamingGraphics, just return false.
	if (!allStreamingGraphicsQuery)
		return (StreamingGraphic *)NULL;

	DBQuery<DBStreamingGraphic>::const_iterator tempIter = allStreamingGraphicsQuery->begin();
	if (tempIter == allStreamingGraphicsQuery->end())
		return (StreamingGraphic *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextStreamingGraphic()
	 * method call below).
	 */
	allStreamingGraphicsIter = new DBQuery<DBStreamingGraphic>::const_iterator(tempIter);
	DBStreamingGraphic *sg = new DBStreamingGraphic(*(*allStreamingGraphicsIter));
	StreamingGraphic *newStreamingGraphic = new StreamingGraphic(sg, dbAccess);
	return newStreamingGraphic;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstStreamingGraphic() method, the first call to this method will
 * result in the first record being returned.
 */
StreamingGraphic *StreamingGraphicManager::nextStreamingGraphic()
{
	if (!dbAccess)
		return (StreamingGraphic *)NULL;

	if (allStreamingGraphicsQuery && !allStreamingGraphicsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allStreamingGraphicsQuery || !allStreamingGraphicsIter)
		return firstStreamingGraphic();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allStreamingGraphicsIter);
	if (*allStreamingGraphicsIter == allStreamingGraphicsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Classes in the db. */
		destroyQueriesAndIters();
		return (StreamingGraphic *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBStreamingGraphic copy constructor in order for the Hfc Class to
	 * control the destruction of the DBStreamingGraphic record that the iterator contains; otherwise, the DBClass will
	 * be deleted when the iterator goes out of scope.  The DBStreamingGraphic copy will be deleted by the Class record
	 * created below.
	 */
	DBStreamingGraphic *sg = new DBStreamingGraphic(*(*allStreamingGraphicsIter));
	StreamingGraphic *newStreamingGraphic = new StreamingGraphic(sg, dbAccess);
	return newStreamingGraphic;
}


/* Provide a countAll method so the caller doesn't need to know about a dbStreamingGraphicsRM.
 */
int StreamingGraphicManager::countAllStreamingGraphics()
{
	if (!dbAccess)
		return -1;

	DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
	if (!dbStreamingGraphicsRM)
		return -1;

	return dbStreamingGraphicsRM->countAll();
}


StreamingGraphic *StreamingGraphicManager::getCurrentStreamingGraphic()
{
	return currentStreamingGraphic;
}
void StreamingGraphicManager::setCurrentStreamingGraphic(StreamingGraphic *newStreamingGraphic)
{
//	Note: Since we are keeping a list of these Streaming Graphics, we do not want
//	to delete the pointer to a slide when we set the currentStreamingGraphic.
	currentStreamingGraphic = newStreamingGraphic;
}


StreamingGraphicsList *StreamingGraphicManager::getCurrentStreamingGraphicsList()
{
	return streamingGraphicsList;
}
void StreamingGraphicManager::setCurrentStreamingGraphicsList(StreamingGraphicsList *newList)
{
	if (streamingGraphicsList)
		delete streamingGraphicsList;
	streamingGraphicsList = newList;
}

void StreamingGraphicManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	StreamingGraphic *msgStreamingGraphic = dynamic_cast<StreamingGraphic *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("StreamingGraphicManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgStreamingGraphic);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgStreamingGraphic);
			break;

		case DtDelete:
			handleDelete(msgStreamingGraphic);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleSave(msgStreamingGraphic);
			break;

		case DtRestore:
			handleRestore(msgStreamingGraphic);
			break;

		case StreamingGraphicReadAllGraphicNames:
			handleReadAllGraphicNames();
			break;

		case StreamingGraphicSelectSlide:
			handleSelectSlide(msgStreamingGraphic);
			break;

		case StreamingGraphicMoveSlideUp:
			handleMoveSlideUp();
			break;

		case StreamingGraphicMoveSlideDown:
			handleMoveSlideDown();
			break;

		case StreamingGraphicInsertNewSlide:
			handleInsertNewSlide();
			break;

		case StreamingGraphicDeleteSelectedSlide:
			handleDeleteSlide();
			break;

		default:
			Debug::trace("StreamingGraphicManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}

#if 0
void StreamingGraphicManager::handleSwapSlideOrder(StreamingGraphic *msgStreamingGraphic)
{
	if (!msgStreamingGraphic || !currentStreamingGraphic || !initData())
		return;

	StreamingGraphic *lookedUpStreamingGraphic = StreamingGraphic::findStreamingGraphic(msgStreamingGraphic->getStreamingGraphicNum(), database, dbStreamingGraphicsRM);

	if (lookedUpStreamingGraphic && lookedUpStreamingGraphic->getErrorCode() == RECORD_OK)
	{
		int currentSlideOrder = currentStreamingGraphic->getStreamingGraphicNum();
		int otherSlideOrder;
	}
}
#endif

void StreamingGraphicManager::handleRead(StreamingGraphic *msgStreamingGraphic)
{
	if (!msgStreamingGraphic || !dbAccess)
		return;

	StreamingGraphic *lookedUpStreamingGraphic = StreamingGraphic::findStreamingGraphic(msgStreamingGraphic->getStreamingGraphicNum(), dbAccess);

	if (!lookedUpStreamingGraphic || lookedUpStreamingGraphic->getErrorCode() != RECORD_OK)
	{
		if (mySession)
		{
			// Send error message to browser indicating StreamingGraphic not found.
			SessionMessage errMsg(Error, /*Err*/StreamingGraphicDev);
			errMsg.setFormatType(BrowserFormat);
			mySession->send(&errMsg, Messenger::DEST_UI);
		}

		if (lookedUpStreamingGraphic)
			delete lookedUpStreamingGraphic;
		return;
	}

	// We have a new current StreamingGraphic.
	setCurrentStreamingGraphic(lookedUpStreamingGraphic);
	Debug::trace("StreamingGraphicManager.StreamingGraphic", HobString("handleRead(): currentStreamingGraphic = ") + HobString(Hex::toString((long long) currentStreamingGraphic)));

	if (mySession)
	{
		// Send the StreamingGraphic found back to the Browser.
		// Is this the behavior for ALL StreamingGraphicRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a StreamingGraphic, but doesn't want the data sent back?
		SessionMessage streamingGraphicData(StreamingGraphicDev, DtWrite);
		streamingGraphicData.setFormatType(BrowserFormat);
		streamingGraphicData.setHfcObject(currentStreamingGraphic);

		// Send the StreamingGraphic to the UI (Browser)
		mySession->send(&streamingGraphicData, Messenger::DEST_UI);
	}
}


void StreamingGraphicManager::handleReadAll()
{
	Debug::trace("StreamingGraphicManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	int slideCount = 0;

	StreamingGraphicsList *newSGList = new StreamingGraphicsList(dbAccess);
	if (newSGList)
	{
		setCurrentStreamingGraphicsList(newSGList);

		HobRecordNum firstStreamingGraphicNum = Configuration::getInstance()->getInitialStreamingGraphicNum();
		HobRecordNum streamingGraphicNum = firstStreamingGraphicNum;
		StreamingGraphic *thisStreamingGraphic = (StreamingGraphic *)NULL;
		do
		{
			thisStreamingGraphic = StreamingGraphic::findStreamingGraphic(streamingGraphicNum, dbAccess);
			if (thisStreamingGraphic)
			{
				Debug::trace("StreamingGraphicManager", HobString("handleReadAll() - streamingGraphicNum = ") + HobString(Long::toString(streamingGraphicNum)));
				slideCount++;
				streamingGraphicsList->add(thisStreamingGraphic);
				streamingGraphicNum = thisStreamingGraphic->getNextStreamingGraphicNum();
			}
		} while (streamingGraphicNum != firstStreamingGraphicNum);

		Debug::trace("StreamingGraphicManager", HobString("handleReadAll() - slideCount = ") + HobString(Integer::toString(slideCount)));

		// Send the StreamingGraphics list to the UI (Browser)
		SessionMessage sgMsg(StreamingGraphicDev, DtWriteAll);
		sgMsg.setFormatType(BrowserFormat);
		sgMsg.setHfcObject(streamingGraphicsList);
		mySession->send(&sgMsg, Messenger::DEST_UI);
	}
}


void StreamingGraphicManager::handleWrite(StreamingGraphic *modifiedStreamingGraphic)
{
	if ((!modifiedStreamingGraphic) || (!currentStreamingGraphic))
		return;

	Debug::trace("StreamingGraphicManager", HobString("handleWrite(): currentStreamingGraphic = ") + HobString(Hex::toString((long long) currentStreamingGraphic)) +
														  HobString("  , modifiedStreamingGraphic = ") + HobString(Hex::toString((long long) modifiedStreamingGraphic)));
  	currentStreamingGraphic->updateStreamingGraphic(modifiedStreamingGraphic);
}


void StreamingGraphicManager::handleDelete(StreamingGraphic *msgStreamingGraphic)
{
	if (!msgStreamingGraphic || !dbAccess)
		return;

	StreamingGraphic *existingStreamingGraphic = StreamingGraphic::findStreamingGraphic(msgStreamingGraphic->getStreamingGraphicNum(), dbAccess);
	if (existingStreamingGraphic)
	{
		existingStreamingGraphic->deleteRecord();
		delete existingStreamingGraphic;
		Debug::trace("StreamingGraphicManager", HobString("handleDelete - StreamingGraphic deleted - streamingGraphicNum:") +
														HobString(Long::toString((long)msgStreamingGraphic->getStreamingGraphicNum())));
	}
	else
		Debug::trace("StreamingGraphicManager", HobString("handleDelete - StreamingGraphic to be deleted was not in the database to delete - streamingGraphicNum:") +
														HobString(Long::toString((long)msgStreamingGraphic->getStreamingGraphicNum())));
}


void StreamingGraphicManager::handleDeleteAll()
{
	Debug::trace("StreamingGraphicManager", "handleDeleteAll - deleting all StreamingGraphics");

	if (!dbAccess)
		return;

	DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
	if (!dbStreamingGraphicsRM)
		return;

	dbStreamingGraphicsRM->deleteAll();

	destroyQueriesAndIters();

	Debug::trace("StreamingGraphicManager", "handleDeleteAll - leaving");
}


void StreamingGraphicManager::handleSave(StreamingGraphic *modifiedStreamingGraphic)
{
	Debug::trace("StreamingGraphicManager", HobString("handleSave(): currentStreamingGraphic = ") + HobString(Hex::toString((long long) currentStreamingGraphic)));
	if (currentStreamingGraphic)
	{
		if (modifiedStreamingGraphic)
		  	currentStreamingGraphic->updateStreamingGraphic(modifiedStreamingGraphic);
		currentStreamingGraphic->saveRecord();
	}
}


void StreamingGraphicManager::handleRestore(StreamingGraphic *restoreStreamingGraphic)
{
	if (!restoreStreamingGraphic || !dbAccess)
		return;

	restoreStreamingGraphic->setDBAccess(dbAccess);

	StreamingGraphic *existingStreamingGraphic = StreamingGraphic::findStreamingGraphic(restoreStreamingGraphic->getStreamingGraphicNum(), dbAccess);
	if (existingStreamingGraphic)
	{
		if (existingStreamingGraphic->isRecordValid())
		{
			existingStreamingGraphic->updateStreamingGraphic(restoreStreamingGraphic);
			existingStreamingGraphic->saveRecord();
		}
		else
		{
			existingStreamingGraphic->deleteRecord();
			restoreStreamingGraphic->saveRecord();
		}

		delete existingStreamingGraphic;
	}
	else
		restoreStreamingGraphic->saveRecord();
}


void StreamingGraphicManager::handleReadAllGraphicNames()
{
	Debug::trace("StreamingGraphicManager", "handling StreamingGraphicReadAllGraphicNames event ...");

	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames(Graphic::TYPE_STREAMINGGRAPHIC);
	if (gfn)
	{
		if (gfn->getListCount() > 0)
		{
			// Send the Graphic file names found back to the Browser.
			SessionMessage nameData(StreamingGraphicDev, StreamingGraphicWriteAllGraphicNames);
			nameData.setFormatType(BrowserFormat);
			nameData.setHfcObject(gfn);

			mySession->send(&nameData, Messenger::DEST_UI);
		}
		else
		{
			//	Send a message indicating no files were found in this directory.
		}
		delete gfn;
	}
}


void StreamingGraphicManager::handleSelectSlide(StreamingGraphic *msgStreamingGraphic)
{
	if (!msgStreamingGraphic || !streamingGraphicsList)
		return;

	HobRecordNum slideNumber = msgStreamingGraphic->getStreamingGraphicNum();
	StreamingGraphic *selectedSlide = streamingGraphicsList->selectSlide(slideNumber);

	if (selectedSlide)
	{
		Debug::trace("StreamingGraphicManager", HobString("handleSelectSlide(): Selected the slide."));
		setCurrentStreamingGraphic(selectedSlide);

		if (mySession)
		{
			// Send the StreamingGraphic back to the Browser.
			SessionMessage streamingGraphicData(StreamingGraphicDev, DtWrite);
			streamingGraphicData.setFormatType(BrowserFormat);
			selectedSlide->setFormatType(EditSlideFormat);
			streamingGraphicData.setHfcObject(selectedSlide);

			// Send the StreamingGraphic to the UI (Browser)
			mySession->send(&streamingGraphicData, Messenger::DEST_UI);
		}
	}
}

void StreamingGraphicManager::handleMoveSlideUp()
{
	if (!streamingGraphicsList)
		return;

	StreamingGraphic *modifiedSlide = streamingGraphicsList->moveSlideUp();
	if (modifiedSlide)
		updateCurrentStreamingGraphic(modifiedSlide, DtWrite);
	else
		Debug::trace("StreamingGraphicManager", HobString("handleMoveSlideUp():  modifiedSlide is NULL"));
}

void StreamingGraphicManager::handleMoveSlideDown()
{
	if (!streamingGraphicsList)
		return;

	StreamingGraphic *modifiedSlide = streamingGraphicsList->moveSlideDown();
	if (modifiedSlide)
		updateCurrentStreamingGraphic(modifiedSlide, DtWrite);
	else
		Debug::trace("StreamingGraphicManager", HobString("handleMoveSlideUp():  modifiedSlide is NULL"));
}

void StreamingGraphicManager::handleInsertNewSlide()
{
	if (!dbAccess)
		return;

	StreamingGraphic *newSlide = new StreamingGraphic();
	if (!newSlide)
		Debug::trace("StreamingGraphicManager", HobString("handleMoveSlideUp():  newSlide is NULL"));
	else
	{
		if (!streamingGraphicsList)
			streamingGraphicsList = new StreamingGraphicsList();

		if (streamingGraphicsList)
		{
			newSlide->setErrorCode(RECORD_OK);
			newSlide->setStreamingGraphicNum(getNextAvailStreamingGraphicNum());
			newSlide->setDBAccess(dbAccess);

			newSlide = streamingGraphicsList->insertSlide(newSlide);
			updateCurrentStreamingGraphic(newSlide, StreamingGraphicInsertNewSlide);
		}
	}
}

void StreamingGraphicManager::handleDeleteSlide()
{
	if (!streamingGraphicsList)
		return;

	StreamingGraphic *nextSlide = streamingGraphicsList->deleteSlide();
	if (nextSlide)
	{
		//	Let's not send the next slide automatically. Keep this here in case someone wants it, though.
//		updateCurrentStreamingGraphic(nextSlide, DtWrite);
	}
}

void StreamingGraphicManager::updateCurrentStreamingGraphic(StreamingGraphic *modifiedSlide, int event)
{
	if (!modifiedSlide)
		return;

	setCurrentStreamingGraphic(modifiedSlide);

	if (mySession)
	{
		// Send the StreamingGraphic back to the Browser.
		SessionMessage streamingGraphicData(StreamingGraphicDev, event);
		streamingGraphicData.setFormatType(BrowserFormat);
		currentStreamingGraphic->setFormatType(EditSlideFormat);
		streamingGraphicData.setHfcObject(currentStreamingGraphic);

		// Send the StreamingGraphic to the UI (Browser)
		mySession->send(&streamingGraphicData, Messenger::DEST_UI);
	}
}


HobRecordNum StreamingGraphicManager::getNextAvailStreamingGraphicNum()
{
	if (!dbAccess)
		return 0;

	DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
	if (!dbStreamingGraphicsRM)
		return 0;

	if (lastSlideNumber != RECORD_NOT_FOUND)
		++lastSlideNumber;
	else
	{
		lastSlideNumber = 1;
		DBQuery<DBStreamingGraphic> *tempStreamingGraphicsQuery = dbStreamingGraphicsRM->retrieveAll();

		// Couldn't get the query from which to access the StreamingGraphics.
		if (!tempStreamingGraphicsQuery)
			return lastSlideNumber;

		DBQuery<DBStreamingGraphic>::const_iterator tempIter = tempStreamingGraphicsQuery->rbegin();
		if (tempIter != tempStreamingGraphicsQuery->rend())
		{
			DBStreamingGraphic *sg = new DBStreamingGraphic(*tempIter);
			StreamingGraphic *newStreamingGraphic = new StreamingGraphic(sg, dbAccess);
			if (newStreamingGraphic)
			{
				lastSlideNumber = newStreamingGraphic->getStreamingGraphicNum() + 1;
				delete newStreamingGraphic;
			}
		}
		delete tempStreamingGraphicsQuery;
	}
	return lastSlideNumber;
}

void StreamingGraphicManager::destroyQueriesAndIters()
{
	if (allStreamingGraphicsIter)
	{
		delete allStreamingGraphicsIter;
		allStreamingGraphicsIter  = (DBQuery<DBStreamingGraphic>::const_iterator *) NULL;
	}

	if (allStreamingGraphicsQuery)
	{
		delete allStreamingGraphicsQuery;
		allStreamingGraphicsQuery = (DBQuery<DBStreamingGraphic> *) NULL;
	}

}
