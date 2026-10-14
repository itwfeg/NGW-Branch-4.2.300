/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "COOLManager.h"
#include "SessionMessage.h"
#include "Session.h"
//#include "ResponseMessageManager.h"
#include "HfcError.h"

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "DBAccess.h"

#include "COOLDevEvent.h"

#include "AllCoolCategories.h"
#include "AllCoolCountries.h"
#include "AllCoolProductionSteps.h"
#include "AllCoolPretexts.h"
#include "AllCoolTexts.h"
#include "COOLCategory.h"
#include "COOLCountry.h"
#include "COOLProductionStep.h"
#include "COOLPretext.h"
#include "COOLText.h"
#include "COOLShortlist.h"
#include "COOLList.h"
#include "COOLListText.h"
#include "ProductManager.h"
#include "Product.h"

#include "Configuration.h"
#include "COOLListTextManager.h"
#include "COOLShortlistManager.h"

#include "DBQuery.h"
#include "DBCoolCategory.h"
#include "DBCoolCountry.h"
#include "DBCoolProductionStep.h"
#include "DBCoolPreText.h"
#include "DBCoolCategoryCountry.h"
#include "DBCoolCategoryProductionStep.h"
#include "DBCoolCategoryPreText.h"
#include "DBCoolText.h"
#include "DBCoolList.h"
#include "DBCoolListText.h"
#include "DBProductStatisticsRM.h"

#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"

#include "DBCoolListsRM.h"
#include "DBCoolListTextsRM.h"
#include "DBProductsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"


void COOLManager::handleCOOLShortlistMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	UINT8 event = msg->getEventType();
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString msgDebug;

	if (traceOn)
	{
		msgDebug = "handleCOOLShortlistMessage:  ";
		msgDebug.append(", handling event: ").append(Integer::toString(event)).append(",  ");
	}
	switch (msg->getEventType())
	{
		case COOLDevShortlistReadAllCoolText:
		{
			if (traceOn)
				msgDebug.append("COOLDevShortlistReadAllCoolText");
			AllCoolTexts *msgAll = dynamic_cast<AllCoolTexts *> (msg->getHfcObject());
			handleCOOLShortlistReadAll(msgAll);
			break;
		}

		case COOLDevDeleteAllShortlists:
			if (traceOn)
				msgDebug.append("COOLDevDeleteAllShortlists");
			handleDeleteAllCOOLLists();
			break;

		case COOLDevDeleteAllShortlistTexts:
			if (traceOn)
				msgDebug.append("COOLDevDeleteAllShortlistTexts");
			handleDeleteAllCOOLListTexts();
			break;

		case COOLDevShortlistAddText:
		{
			if (traceOn)
				msgDebug.append("COOLDevShortlistAddText");
			handleCOOLShortlistAddText(msg);
			break;
		}

		case COOLDevShortlistDeleteText:
		{
			if (traceOn)
				msgDebug.append("COOLDevShortlistDeleteText");
			handleCOOLShortlistDeleteText(msg);
			break;
		}

		case COOLDevRestoreShortlistForUltima:
		{
			if (traceOn)
				msgDebug.append("COOLDevRestoreShortlistForUltima");
			handleRestoreHobDiskShortlist(msg);
			break;
		}

		case COOLDevRestoreShortlistText:
		{
			if (traceOn)
				msgDebug.append("COOLDevRestoreShortlistText");
			COOLListText *msgCoolListText = dynamic_cast<COOLListText *> (msg->getHfcObject());
			handleRestoreCOOLShortlistText(msgCoolListText);
			break;
		}

		case COOLDevRestoreShortlist:
		{
			if (traceOn)
				msgDebug.append("COOLDevRestoreShortlist");
			if (msg->getFormatType() == SynchronizationTaggedFormat)
				handleRestoreCOOLShortlistForSync(msg);
			else
			{
				//	PT 465 - A Von's database has the c1, CO, c9 and ca tags but not always the cs tag.
				//			 So, if we can't process it as a normal Shortlist, process it
				//			 as a HobDiskShortlist instead [CCD APR 2011]
				COOLShortlist *tempShortlist = dynamic_cast<COOLShortlist *> (msg->getHfcObject());
				if (tempShortlist->isUltimaFormat())
					handleRestoreHobDiskShortlist(msg);
				else
					handleRestoreCOOLShortlist(msg);
			}
			break;
		}

		default:
			if (traceOn)
				msgDebug.append("UNKNOWN EVENT: ").append(Integer::toString(event)).append(" !!!");
			break;
	}
	if (traceOn)
		Debug::trace("COOLManager.Shortlist", msgDebug);
}


COOLShortlist *COOLManager::firstCOOLShortlist()
{
	if (!dbAccess)
		return 0;

	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
	if (!dbCoolListTextsRM)
		return 0;

	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return 0;

	// If the user calls firstCOOLShortlist() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLShortlistsQueryAndIter();

	allCoolListTextsQuery = dbCoolListTextsRM->retrieveAll();

	// Couldn't get the query from which to access the Categories, just return false.
	if (!allCoolListTextsQuery)
		return 0;

	DBQuery<DBCoolListText>::const_iterator tempIter = allCoolListTextsQuery->begin();
	if (tempIter == allCoolListTextsQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLShortlist()
	 * method call below).
	 */
	allCoolListTextsIter = new DBQuery<DBCoolListText>::const_iterator(tempIter);
	DBCoolListText *c = new DBCoolListText(*(*allCoolListTextsIter));
	if (theDbCoolList)
	{
		delete theDbCoolList;
		theDbCoolList = 0;
	}
	theDbCoolList = dbCoolListsRM->find(c);
	COOLShortlist *newShortlist = new COOLShortlist(c, theDbCoolList, dbAccess);

#ifndef CHECK_FT
	if (c)			//	PTR 1646: Delete the DBCoolListText since it is not stored in the COOLShortlist.
		delete c;
#endif
	return newShortlist;
}


COOLShortlist *COOLManager::nextCOOLShortlist()
{
	bool isInList = false;

	if (!dbAccess)
		return 0;

	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return 0;

	if (allCoolListTextsQuery && !allCoolListTextsQuery->isEnabled())
		destroyCOOLShortlistsQueryAndIter();

	if (!allCoolListTextsQuery || !allCoolListTextsIter)
		return firstCOOLShortlist();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolListTextsIter);
	if (*allCoolListTextsIter == allCoolListTextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all CoolListTexts in the db. */
		destroyCOOLShortlistsQueryAndIter();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolListText copy constructor in order for the Hfc COOLShortlist to
	 * control the destruction of the DBCoolListText record that the iterator contains; otherwise, the DBCoolListText will
	 * be deleted when the iterator goes out of scope.  The DBCoolListText copy will be deleted by the COOLShortlist record
	 * created below.
	 */
	DBCoolListText *c = new DBCoolListText(*(*allCoolListTextsIter));
	if (theDbCoolList)
		isInList = theDbCoolList->containsCoolListText(const_cast<DBCoolListText *> (c));

	if (!isInList)
	{
		if (theDbCoolList)
		{
			delete theDbCoolList;
			theDbCoolList = 0;
		}
		theDbCoolList = dbCoolListsRM->find(c);
	}
	COOLShortlist *newShortlist = new COOLShortlist(c, theDbCoolList, dbAccess);

#ifndef CHECK_FT
	if (c)			//	PTR 1646: Delete the DBCoolListText since it is not stored in the COOLShortlist.
		delete c;
#endif
	return newShortlist;
}


int COOLManager::countAllCOOLShortlists()
{
	if (!dbAccess)
		return -1;

	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
	if (!dbCoolListTextsRM)
		return -1;

	return dbCoolListTextsRM->countAll();
}


void COOLManager::handleCOOLShortlistReadAll(AllCoolTexts *myList)
{
	if (!mySession || !dbAccess || !myList)
		return;

	myList->findAllLinkedToProduct(dbAccess);

	// Send the COOL text found back to the Browser.
	SessionMessage ctData(COOLDev, COOLDevShortlistWriteAllCoolText);
	ctData.setFormatType(BrowserFormat);
	ctData.setHfcObject(myList);
	mySession->send(&ctData, Messenger::DEST_UI);
}


DBCoolList *COOLManager::createNewCoolShortlistRecord(HobRecordNum startingPointForRecordNum, HobRecordNum deptNum)
{
	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return (DBCoolList *)NULL;

	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString myDebug("createNewCoolShortlistRecord():  ");
	if (traceOn)
	{
		HobString myDebug2 = myDebug;
		myDebug2.append("  Creating a new DBCoolList record.");
		Debug::trace("COOLManager.Shortlist", myDebug2);
	}

	DBCoolList *dbCoolList = dbCoolListsRM->createRecord();
	if (dbCoolList)
	{
		//  We have to assign a number to this list. Use the product number as a starting point.
		HobRecordNum listNum = startingPointForRecordNum;
		bool fullPass = false;
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("  Starting at ").append(Long::toString(listNum)).append(" to find a number for the DBCoolList.");
			Debug::trace("COOLManager.Shortlist", myDebug2);
		}

		DBCoolList *tempDbCoolList = dbCoolListsRM->findNumber(longtostring(listNum), longtostring(deptNum));
		while (tempDbCoolList && (listNum < 999999))
		{
			delete tempDbCoolList;
			tempDbCoolList = dbCoolListsRM->findNumber(Long::toString(++listNum), longtostring(deptNum));
			if (listNum == 999999)
			{
				if (tempDbCoolList)
				{
					listNum = 0;
					if (fullPass == true)
					{
						if (traceOn)
						{
							HobString myDebug2 = myDebug;
							myDebug2.append("  Cannot find a number for the DBCoolList!");
							Debug::trace("COOLManager.Shortlist", myDebug2);
						}
						break;              //  We don't want to stay in an infinite loop.
					}
					else
					{
						if (traceOn)
						{
							HobString myDebug2 = myDebug;
							myDebug2.append("  Start at 0 to find a number for the DBCoolList.");
							Debug::trace("COOLManager.Shortlist", myDebug2);
						}
						fullPass = true;
					}
				}
			}
		}

		if (listNum > 0)
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("  The new list number is ").append(Long::toString(listNum));
				Debug::trace("COOLManager.Shortlist", myDebug2);
			}
			dbCoolList->setNumber(longtostring(listNum));
			dbCoolList->setDeptNumber(longtostring(deptNum));
		}
	}
	return dbCoolList;
}

void COOLManager::handleDeleteAllCOOLShortlists(HobRecordNum deptNum)
{
	Debug::trace("COOLManager.Shortlist", "handleDeleteAllCOOLShortlists - deleting all COOL Shortlists");

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
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

		{
			DBCoolListsRM *dbRM = dbAccess->getDBCoolListsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		// When deleting all CoolShortlists, we also delete all CoolShortlistText bridge table records.
		// We don't delete the Cool Text records here.
		{
			DBCoolListTextsRM *dbRM = dbAccess->getDBCoolListTextsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}

		destroyCOOLShortlistsQueryAndIter();
	}
}

void COOLManager::handleDeleteAllCOOLLists(HobRecordNum deptNum)
{
	Debug::trace("COOLManager.Shortlist", "handleDeleteAllCOOLLists - deleting all CoolLists");

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
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

		{
			DBCoolListsRM *dbRM = dbAccess->getDBCoolListsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}
		{
			DBCoolListTextsRM *dbRM = dbAccess->getDBCoolListTextsRM();
			if (!dbRM)
				return;

			if (deptNum == RECORD_NOT_FOUND)
				dbRM->deleteAll();
			else
				dbRM->deleteAllForDept(longtostring(deptNum));
		}


		destroyCOOLListsQueryAndIter();
		destroyCOOLListTextsQueryAndIter();
	}
}

void COOLManager::handleDeleteAllCOOLListTexts(HobRecordNum deptNum)
{
	Debug::trace("COOLManager.Shortlist", "handleDeleteAllCOOLListTexts - deleting all CoolListTexts");

	DataManager *myManager = mgrContainer_->getCOOLListTextManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolListTexts))
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

		DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
		if (!dbCoolListTextsRM)
			return;

		dbCoolListTextsRM->deleteAll();

		destroyCOOLListTextsQueryAndIter();
	}
}


void COOLManager::handleRestoreHobDiskShortlist(SessionMessage * msg)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString msgDebug, linkStr, nextLinkStr, coolTextNumStr;
	int link;
	if (traceOn)
		msgDebug = "handleRestoreHobDiskShortlist()   ";

	COOLShortlist *restoreShortlist = dynamic_cast<COOLShortlist *> (msg->getHfcObject());

	if (!restoreShortlist || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		// Even though this is UltimaTaggedFormat, things have changed that allow the Synchronization feature to deal with this format,
		// so just write out the record received.
		receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), (MessageFormat)msg->getFormatType(), msg->getOriginalBuff());

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (!dbCoolListsRM)
			return;

		DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
		if (!dbCoolListTextsRM)
			return;

		DBCoolListText *dbCoolListTextRec = 0;
		DBCoolList *dbCoolListRec = 0;
		COOLShortlist *usedShortlist = 0;
		int order = 0;

		// Look for this shortlist link in the CoolListText bridge table, where the link field is the same as the link being restored.
		// For data coming in from UltimaFormat, there will really be at most one of these records found.
		// If not found, then we need to create it.
		link = restoreShortlist->getShortlistLink();
		linkStr = Long::toString(link);
		coolTextNumStr = Long::toString(restoreShortlist->getCoolTextNum());

		DBQuery<DBCoolListText> *linkTextsList = dbCoolListTextsRM->retrieveLink(linkStr);
		if (!linkTextsList)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("linkTextsList is 0. Exiting!");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
			return;
		}

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("link = ").append(linkStr);
			msgDebug2.append("  nextLink = ").append(Long::toString(restoreShortlist->getShortlistNextLink()));
			msgDebug2.append("  coolTextNum = ").append(coolTextNumStr);
			msgDebug2.append("  linkTextsList->size() = ").append(Integer::toString((int)linkTextsList->size()));
			Debug::trace("COOLManager.Shortlist", msgDebug2);
		}
		DBQuery<DBCoolListText>::const_iterator listTextsIter = linkTextsList->begin();
		if (listTextsIter == linkTextsList->end())
		{
			//	There were no CoolListText bridge table records with the link number being the same as the link number being restored.
			
			//	Is this link a next link in another list? To determine this, look for a CoolListText bridge table record
			//	with its nextLink being the link number of the record being restored.  If one exists, add it to that list also.

			bool forceFirstInList = false;
			DBQuery<DBCoolListText> *prevLinkTextsList = dbCoolListTextsRM->retrievePreviousCoolListTexts(linkStr);
			if (prevLinkTextsList)
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  prevLinkTextsList->size() = ").append(Integer::toString((int)prevLinkTextsList->size()));
					msgDebug2.append(". Will append to the end of the List(s).");
					Debug::trace("COOLManager.Shortlist", msgDebug2);
				}
				if (!prevLinkTextsList->size())
					forceFirstInList = true;		//	We'll have to add this link as a firstLink to the CoolList.

				DBQuery<DBCoolListText>::const_iterator prevLinkTextsIter = prevLinkTextsList->begin();
				while (prevLinkTextsIter != prevLinkTextsList->end())
				{
					order = prevLinkTextsIter->getOrder();
					if (traceOn)
						Debug::trace("COOLManager.Shortlist", msgDebug + "  Creating a new dbCoolListTextRec 2.");

					dbCoolListRec = dbCoolListsRM->find(&*prevLinkTextsIter);
					if (dbCoolListRec)
					{
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("  Creating a new dbCoolListTextRec 2.");
							Debug::trace("COOLManager.Shortlist", msgDebug2);
						}
						dbCoolListTextRec = dbCoolListTextsRM->createRecord(coolTextNumStr, dbCoolListRec, restoreShortlist->getDeptNum());
						if (dbCoolListTextRec)
						{
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("createRecord 2: dbCoolListTextRec");
								Debug::trace("COOLManager.Shortlist", msgDebug2);
							}
							appendToCoolListTextRec(dbCoolListTextsRM, dbCoolListTextRec, dbCoolListRec, restoreShortlist, ++order);
							delete dbCoolListTextRec;
							dbCoolListTextRec = 0;
							if (traceOn)
							{
								HobString msgDebug2 = msgDebug;
								msgDebug2.append("order = ").append(Integer::toString(order));
								msgDebug2.append(" :  Appended ").append(coolTextNumStr).append(" to the end of the List.");
								Debug::trace("COOLManager.Shortlist", msgDebug2);
							}
						}
						delete dbCoolListRec;
						dbCoolListRec = 0;
					}
					++prevLinkTextsIter;
				}
				delete prevLinkTextsList;
			}

			//	Is this link a first link?
			dbCoolListRec = dbCoolListsRM->findFirstLink(linkStr);
			if (!dbCoolListRec && forceFirstInList)
			{
				//	No, but we need to force a first link
				dbCoolListRec = dbCoolListsRM->createRecord();
				if (dbCoolListRec)
				{
					dbCoolListRec->setFirstLinkNumber(linkStr);		//	This is all we know right now.
					try
					{
						dbCoolListsRM->saveRecord(dbCoolListRec);
					}
					catch (DBException &dbe)
					{
						Debug::trace("DBAccess.exception", "handleRestoreHobDiskShortlist():  dbCoolListsRM->saveRecord(dbCoolListRec) ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("  New dbCoolListRec, FirstLinkNumber ").append(linkStr).append(" saved.");
						Debug::trace("COOLManager.Shortlist", msgDebug2);
					}
				}
			}
			if (dbCoolListRec)
			{
				order = 0;
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  Creating a new dbCoolListTextRec.");
					Debug::trace("COOLManager.Shortlist", msgDebug2);
				}
				dbCoolListTextRec = dbCoolListTextsRM->createRecord(coolTextNumStr, dbCoolListRec, restoreShortlist->getDeptNum());
				if (dbCoolListTextRec)
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("createRecord 1: dbCoolListTextRec");
						Debug::trace("COOLManager.Shortlist", msgDebug2);
					}
					appendToCoolListTextRec(dbCoolListTextsRM, dbCoolListTextRec, dbCoolListRec, restoreShortlist, order);
					delete dbCoolListTextRec;
					dbCoolListTextRec = 0;
				}
				delete dbCoolListRec;
				dbCoolListRec = 0;
			}
		}

		if (usedShortlist && usedShortlist->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedShortlist->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedShortlist->nextError();
			}
		}
		if (linkTextsList)
			delete linkTextsList;
	}
}


void COOLManager::handleRestoreCOOLShortlistText(COOLListText *restoreCoolListText)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString msgDebug = "";
	HobString prodNumStr, coolTextNumStr, linkNumStr, nextLinkNumStr, orderStr, deptNumStr;
	if (traceOn)
		msgDebug = "handleRestoreCOOLShortlistText()   ";

	if (!restoreCoolListText || !dbAccess)
		return;

	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return;

	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
	if (!dbCoolListTextsRM)
		return;

	DBCoolListText *dbCoolListTextRec = 0;
	DBCoolList *dbCoolListRec = 0;
	COOLListText *usedCoolListText = 0;

	restoreCoolListText->setDBAccess(dbAccess);

	coolTextNumStr = longtostring(restoreCoolListText->getCoolTextNum());
	linkNumStr = longtostring(restoreCoolListText->getLinkNum());
	nextLinkNumStr = longtostring(restoreCoolListText->getNextLinkNum());
	orderStr = inttostring(restoreCoolListText->getOrder());
	prodNumStr = longtostring(restoreCoolListText->getProductNum());
	deptNumStr = longtostring(restoreCoolListText->getDeptNum());

	dbCoolListRec = dbCoolListsRM->findNumber(prodNumStr, deptNumStr);
	if (!dbCoolListRec)
	{
		//	Create this record.
		dbCoolListRec = dbCoolListsRM->createRecord();
		if (dbCoolListRec)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  NEW dbCoolListRec was created.");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
			dbCoolListRec->setNumber(prodNumStr);
			try
			{
				dbCoolListsRM->saveRecord(dbCoolListRec);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "handleRestoreCOOLShortlistText():  dbCoolListsRM->saveRecord(dbCoolListRec) ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  New dbCoolListRec:  prodNum = ").append(prodNumStr).append(" was saved.");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
		}
	}
	if (dbCoolListRec)
	{
		dbCoolListTextRec = dbCoolListTextsRM->find(coolTextNumStr, dbCoolListRec->getDeptNumber(), dbCoolListRec);
		if (!dbCoolListTextRec)
		{
			dbCoolListTextRec = dbCoolListTextsRM->createRecord(coolTextNumStr, dbCoolListRec, restoreCoolListText->getDeptNum());
			if (dbCoolListTextRec && traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  NEW dbCoolListTextRec:  prodNum = ").append(prodNumStr);
				msgDebug2.append("  coolTextNum = ").append(coolTextNumStr).append(" was created.");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
		}

		if (dbCoolListTextRec)
		{
			dbCoolListTextRec->setOrder(restoreCoolListText->getOrder());
			dbCoolListTextRec->setCoolTextNumber(coolTextNumStr);

			if (linkNumStr.empty())
				dbCoolListTextRec->setLinkNumberNull();
			else
				dbCoolListTextRec->setLinkNumber(linkNumStr);

			if (nextLinkNumStr.empty())
				dbCoolListTextRec->setNextLinkNumberNull();
			else
				dbCoolListTextRec->setNextLinkNumber(nextLinkNumStr);

			try
			{
				dbCoolListTextsRM->saveRecord(dbCoolListTextRec);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "handleRestoreCOOLShortlistText():  dbCoolListTextsRM->saveRecord(dbCoolListTextRec) ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}

			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("prodNum = ").append(prodNumStr);
				msgDebug2.append("  coolTextNum = ").append(coolTextNumStr);
				msgDebug2.append("  order = ").append(orderStr);
				msgDebug2.append("  linkNum = ").append(linkNumStr);
				msgDebug2.append("  nextLinkNum = ").append(nextLinkNumStr).append(" was saved.");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
			delete dbCoolListTextRec;
			dbCoolListTextRec = 0;
		}
		delete dbCoolListRec;
		dbCoolListRec = 0;
	}

	if (usedCoolListText && usedCoolListText->hasErrors() &&
		 mySession && mySession->isCommSession())
	{
		SessionMessage errMsg(Error, DtWrite);
		errMsg.setFormatType(EvolutionTaggedFormat);
		HfcError *err = usedCoolListText->firstError();
		while (err)
		{
			errMsg.setHfcObject(err);
			mySession->send(&errMsg, Messenger::DEST_UI);

			err = usedCoolListText->nextError();
		}
	}
}


void COOLManager::handleRestoreCOOLShortlist(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString msgDebug = "";

	if (traceOn)
		msgDebug = "handleRestoreCOOLShortlist()   ";

	COOLShortlist *restoreShortlist = dynamic_cast<COOLShortlist *> (msg->getHfcObject());

	if (!restoreShortlist || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		// Even though this is UltimaTaggedFormat, things have changed that allow the Synchronization feature to deal with this format,
		// so just write out the record received.
		receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), (MessageFormat)msg->getFormatType(), msg->getOriginalBuff());

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (!dbCoolListsRM)
			return;

		DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
		if (!dbCoolListTextsRM)
			return;

		DBCoolListText *dbCoolListTextRec = 0;
		DBCoolList *dbCoolListRec = 0;
		COOLShortlist *usedShortlist = 0;
		HobString coolTextNumStr = "";

		restoreShortlist->setDBAccess(dbAccess);

		string prodNumStr = longtostring(restoreShortlist->getProductNum());
		string deptNumStr = longtostring(restoreShortlist->getDeptNum());

		dbCoolListRec = dbCoolListsRM->findNumber(prodNumStr, deptNumStr);
		if (!dbCoolListRec)
		{
			//	Create this record.
			dbCoolListRec = dbCoolListsRM->createRecord();
			if (dbCoolListRec)
			{
				dbCoolListRec->setDeptNumber(deptNumStr);
				dbCoolListRec->setNumber(prodNumStr);
				try
				{
					dbCoolListsRM->saveRecord(dbCoolListRec);
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "handleRestoreCOOLShortlist():  dbCoolListsRM->saveRecord(dbCoolListRec) ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  New dbCoolListRec ").append(prodNumStr).append(" saved.");
					Debug::trace("COOLManager.Shortlist", msgDebug2);
				}
			}
		}

		if (dbCoolListRec)
		{
			coolTextNumStr = Long::toString(restoreShortlist->getCoolTextNum());
			dbCoolListTextRec = dbCoolListTextsRM->find(coolTextNumStr, deptNumStr, dbCoolListRec);
			if (!dbCoolListTextRec)
				dbCoolListTextRec = dbCoolListTextsRM->createRecord(coolTextNumStr, dbCoolListRec, restoreShortlist->getDeptNum());

			if (dbCoolListTextRec)
			{
				dbCoolListTextRec->setOrder(restoreShortlist->getOrder());
				dbCoolListTextRec->setCoolTextNumber(coolTextNumStr);

				try
				{
					dbCoolListTextsRM->saveRecord(dbCoolListTextRec);
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "handleRestoreCOOLShortlist():  dbCoolListTextsRM->saveRecord(dbCoolListTextRec) ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  New dbCoolListTextRec:  order = ").append(Integer::toString(restoreShortlist->getOrder()));
					msgDebug2.append("  coolTextNum = ").append(coolTextNumStr).append(" saved.");
					Debug::trace("COOLManager.Shortlist", msgDebug2);
				}

				delete dbCoolListTextRec;
				dbCoolListTextRec = 0;
			}
			delete dbCoolListRec;
			dbCoolListRec = 0;
		}

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("prodNum = ").append(prodNumStr);
			msgDebug2.append("  coolTextNum = ").append(coolTextNumStr);
			msgDebug2.append("  order = ").append(Integer::toString(restoreShortlist->getOrder()));
			Debug::trace("COOLManager.Shortlist", msgDebug2);
		}

		if (usedShortlist && usedShortlist->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedShortlist->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedShortlist->nextError();
			}
		}
	}
}

void COOLManager::handleRestoreCOOLShortlistForSync(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString msgDebug = "", prodNumStr, firstLinkNumStr, deptNumStr;
	if (traceOn)
		msgDebug = "handleRestoreCOOLShortlistForSync()   ";

	COOLList *restoreCoolList = dynamic_cast<COOLList *> (msg->getHfcObject());

	if (!restoreCoolList || !dbAccess)
		return;

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		// Even though this is UltimaTaggedFormat, things have changed that allow the Synchronization feature to deal with this format,
		// so just write out the record received.
		receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), (MessageFormat)msg->getFormatType(), msg->getOriginalBuff());

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (!dbCoolListsRM)
			return;

		DBCoolList *dbCoolListRec = 0;
		COOLList *usedCoolList = 0;

		restoreCoolList->setDBAccess(dbAccess);

		prodNumStr = longtostring(restoreCoolList->getProductNum());
		firstLinkNumStr = longtostring(restoreCoolList->getFirstLinkNum());
		deptNumStr = longtostring(restoreCoolList->getDeptNum());

		dbCoolListRec = dbCoolListsRM->findNumber(prodNumStr, deptNumStr);
		if (!dbCoolListRec)
		{
			//	Create this record.
			dbCoolListRec = dbCoolListsRM->createRecord();
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  NEW dbCoolListRec was created.");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
			dbCoolListRec->setNumber(prodNumStr);
		}
		if (dbCoolListRec)
		{
			if (firstLinkNumStr.empty())
				dbCoolListRec->setFirstLinkNumberNull();
			else
				dbCoolListRec->setFirstLinkNumber(firstLinkNumStr);

			try
			{
				dbCoolListsRM->saveRecord(dbCoolListRec);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "handleRestoreCOOLShortlistForSync():  dbCoolListsRM->saveRecord(dbCoolListRec) ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  NEW dbCoolListRec:  prodNum = ").append(prodNumStr);
				msgDebug2.append("  firstLinkNum = ").append(firstLinkNumStr).append(" was saved.");
				Debug::trace("COOLManager.Shortlist", msgDebug2);
			}
			delete dbCoolListRec;
			dbCoolListRec = 0;
		}

		if (usedCoolList && usedCoolList->hasErrors() &&
			 mySession && mySession->isCommSession())
		{
			SessionMessage errMsg(Error, DtWrite);
			errMsg.setFormatType(EvolutionTaggedFormat);
			HfcError *err = usedCoolList->firstError();
			while (err)
			{
				errMsg.setHfcObject(err);
				mySession->send(&errMsg, Messenger::DEST_UI);

				err = usedCoolList->nextError();
			}
		}
	}
}


void COOLManager::handleCOOLShortlistAddText(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString myDebug("handleCOOLShortlistAddText():  ");

	COOLShortlist *msgShortlist = dynamic_cast<COOLShortlist *> (msg->getHfcObject());

	if (!dbAccess || !msgShortlist)
	{
		if (traceOn)
		{
			if (!dbAccess)
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  dbAccess is NULL!");
			else
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  msgShortlist is NULL!");
		}
		return;
	}

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		// Even though this is UltimaTaggedFormat, things have changed that allow the Synchronization feature to deal with this format,
		// so just write out the record received.
		receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), (MessageFormat)msg->getFormatType(), msg->getOriginalBuff());

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("COOLShortlist::show()  *****  ");
			Debug::trace("COOLManager", myDebug2 + "Start");
			if (msgShortlist)
				msgShortlist->show();
			Debug::trace("COOLManager.Shortlist", myDebug2 + " End");
		}

		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (!dbCoolListsRM)
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  dbCoolListsRM is NULL!");
			return;
		}

		DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
		if (!dbCoolListTextsRM)
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  dbCoolListTextsRM is NULL!");
			return;
		}

		Util::CSmartPtr<Product> theProduct(Product::findRecord(msgShortlist->getProductNum(), msgShortlist->getDeptNum(), dbAccess));

		if (theProduct.null())
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("EXITING!!!  Product *theProduct is NULL! Looking for PLU# ").append(longtostring(msgShortlist->getProductNum())).append("  deptNum ").append(longtostring(msgShortlist->getDeptNum()));
				Debug::trace("COOLManager.Shortlist", myDebug2);
			}
			return;
		}

		DBCoolList *dbCoolList = 0;
		COOLList *coolList = 0;

		DBProduct *dbProduct = theProduct->getMyDbRecord();
		if (dbProduct)
			dbCoolList = dbCoolListsRM->find(dbProduct);
		else
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "Exiting!!!  DBProduct *dbProduct is NULL!");
			return;
		}

		if (!dbCoolList)
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "No DBCoolList *dbCoolList. Creating a new Shortlist.");

			//  Create a new list.
			dbCoolList = createNewCoolShortlistRecord(msgShortlist->getProductNum(), msgShortlist->getDeptNum());
			if (dbCoolList)
			{
				try
				{
					dbCoolListsRM->saveRecord(dbCoolList);

					coolList = new COOLList(dbCoolList, dbAccess);
					if (coolList)
					{
						DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
						if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
						{
							std::string synchSer = "";
							serializeForSynchronization(synchSer, coolList, myManager->getWriteEvent());

							synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

							if (proxy)
								proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
									hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), synchSer)));
						}
					}
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "handleCOOLShortlistAddText():  dbCoolListsRM->saveRecord(dbCoolList) ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("  New dbCoolList:  ProductNum = ").append(Integer::toString(msgShortlist->getProductNum())).append(" saved.");
					Debug::trace("COOLManager.Shortlist", myDebug2);
				}

				//  Assign this list to the currentProduct.
				Session *s = dynamic_cast<Session *> (mySession);
				ProductManager *theProductManager = (s) ? s->getProductManager() : 0;
				if (theProductManager)
				{
					Debug::trace("COOLManager.Shortlist", "Saving the dbCoolList to the Product via the ProductManager.");
					theProductManager->assignCoolListToProduct(dbCoolList);
				}
			}
		}
		if (dbCoolList)
		{
			//  Create a new record for the list.
			DBCoolListText *dbCoolListText = dbCoolListTextsRM->createRecord(Long::toString(msgShortlist->getCoolTextNum()), dbCoolList, msgShortlist->getDeptNum());
			COOLListText *coolListText = 0;

			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "  Creating a new DBCoolListText record.");

			if (dbCoolListText)
			{
				int myOrder = 0;

				//  We want to append this to the end of the list.
				DBQuery<DBCoolListText>* dbCoolListTextQuery = dbCoolListTextsRM->retrieveLinkedtoCoolList(dbCoolList);
				if (dbCoolListTextQuery)
				{
					DBQuery<DBCoolListText>::const_iterator tempIter = dbCoolListTextQuery->end();
					--tempIter;
					if (tempIter != dbCoolListTextQuery->end())
						myOrder = tempIter->getOrder() + 1;

					delete dbCoolListTextQuery;
				}
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("  The order is ").append(Long::toString(myOrder));
					Debug::trace("COOLManager.Shortlist", myDebug2);
				}
				dbCoolListText->setOrder(myOrder);
				try
				{
					dbCoolListTextsRM->saveRecord(dbCoolListText);

					coolListText = new COOLListText(dbCoolListText, dbAccess);
					if (coolListText)
					{
						DataManager *myManager = mgrContainer_->getCOOLListTextManager();
						if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolListTexts))
						{
							std::string synchSer = "";
							serializeForSynchronization(synchSer, coolListText, myManager->getWriteEvent());

							synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

							if (proxy)
								proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
									hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), synchSer)));
						}
					}
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", "handleCOOLShortlistAddText():  dbCoolListTextsRM->saveRecord(dbCoolListText) ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("  New dbCoolListText:  order = ").append(Integer::toString(myOrder)).append(" saved.");
					Debug::trace("COOLManager.Shortlist", myDebug2);
				}

				if (coolListText)
					delete coolListText;
				else if (dbCoolListText)
					delete dbCoolListText;
			}

			if (coolList)
				delete coolList;
			else if (dbCoolList)
				delete dbCoolList;
		}
		else
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + " Error! Could not create a dbCoolList!");
		}
	}
}

void COOLManager::appendToCoolListTextRec(DBCoolListTextsRM *dbCoolListTextsRM, DBCoolListText *dbCoolListTextRec, DBCoolList *dbCoolListRec, COOLShortlist *shortlist, int order)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "appendToCoolListTextRec()";

	if (dbCoolListTextRec)
	{
		HobString linkStr = Long::toString(shortlist->getShortlistLink());
		HobString nextLinkStr = Long::toString(shortlist->getShortlistNextLink());
		HobString textNumStr = Long::toString(shortlist->getCoolTextNum());
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  order = ").append(Integer::toString(order));
			msgDebug2.append("  link = ").append(linkStr);
			msgDebug2.append("  nextLink = ").append(nextLinkStr);
			msgDebug2.append("  textNum = ").append(textNumStr);
			Debug::trace("COOLManager.Shortlist", msgDebug2);
		}
		dbCoolListTextRec->setOrder(order);
		dbCoolListTextRec->setLinkNumber(linkStr);
		dbCoolListTextRec->setNextLinkNumber(nextLinkStr);
		dbCoolListTextRec->setCoolTextNumber(textNumStr);

		try
		{
			dbCoolListTextsRM->saveRecord(dbCoolListTextRec);
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "appendToCoolListTextRec():  dbCoolListTextsRM->saveRecord(dbCoolListTextRec) ********** database exception **********");
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  New dbCoolListTextRec saved.");
			Debug::trace("COOLManager.Shortlist", msgDebug2);
		}


		//	If the nextLink list exists already in the database, we need to duplicate the entries from here down.
		HobRecordNum nextLink = shortlist->getShortlistNextLink();
		do
		{
			HobString nextLinkStr = Long::toString(nextLink);
			DBQuery<DBCoolListText> *nextLinkTextsList = dbCoolListTextsRM->retrieveLink(nextLinkStr);
			if (nextLinkTextsList && nextLinkTextsList->size())
			{
				DBQuery<DBCoolListText>::const_iterator nextLinkTextsIter = nextLinkTextsList->begin();
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("nextLink = ").append(nextLinkStr);
					msgDebug2.append("  nextLinkTextsList->size() = ").append(Integer::toString((int)nextLinkTextsList->size()));
					Debug::trace("COOLManager.Shortlist", msgDebug2);
				}
				DBCoolListText *newDbCoolListTextRec = dbCoolListTextsRM->createRecord(nextLinkTextsIter->getCoolTextNumber(), dbCoolListRec, stringtolong(nextLinkTextsIter->getDeptNumber()));
				if (newDbCoolListTextRec)
				{
					//	Copy this information to the new list.
					newDbCoolListTextRec->setOrder(++order);
					newDbCoolListTextRec->setLinkNumber(nextLinkTextsIter->getLinkNumber());
					newDbCoolListTextRec->setNextLinkNumber(nextLinkTextsIter->getNextLinkNumber());
					newDbCoolListTextRec->setCoolTextNumber(nextLinkTextsIter->getCoolTextNumber());
					try
					{
						dbCoolListTextsRM->saveRecord(newDbCoolListTextRec);
					}
					catch (DBException &dbe)
					{
						Debug::trace("DBAccess.exception", "appendToCoolListTextRec():  dbCoolListTextsRM->saveRecord(newDbCoolListTextRec) ********** database exception **********");
						DBException::Type t = dbe.getType();
						Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
					}
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("  newDbCoolListTextRec saved.");
						Debug::trace("COOLManager.Shortlist", msgDebug2);
					}
					delete newDbCoolListTextRec;
				}
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("order = ").append(Integer::toString(order));
					Debug::trace("COOLManager.Shortlist", msgDebug2);
				}
				nextLink = Long::valueOf(nextLinkTextsIter->getNextLinkNumber());

				delete nextLinkTextsList;
				nextLinkTextsList = 0;
			}
			else
			{
				nextLink = 0;
			}
		} while (nextLink > 0);
	}
}


COOLList *COOLManager::firstCOOLList()
{
	if (!dbAccess)
		return 0;

	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return 0;

	bool traceOn = Debug::isTracing("COOLManager.COOLList");

	// If the user calls firstCOOLList() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLListsQueryAndIter();

	allCoolListsQuery = dbCoolListsRM->retrieveAll();

	// Couldn't get the query from which to access the CoolLists, just return false.
	if (!allCoolListsQuery)
		return 0;

	if (traceOn)
	{
		HobString myDebug("firstCOOLList():  query size = ");
		myDebug.append(Integer::toString((int)allCoolListsQuery->size()));
		Debug::trace("COOLManager.COOLList", myDebug);
	}

	DBQuery<DBCoolList>::const_iterator tempIter = allCoolListsQuery->begin();
	if (tempIter == allCoolListsQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLList()
	 * method call below).
	 */
	allCoolListsIter = new DBQuery<DBCoolList>::const_iterator(tempIter);
	DBCoolList *c = new DBCoolList(*(*allCoolListsIter));
	if (traceOn)
	{
		HobString myDebug("firstCOOLList():  DBCoolList = ");
		myDebug.append(Hex::toString((long long) c));
		Debug::trace("COOLManager.COOLList", myDebug);
	}
	COOLList *newList = new COOLList(c, dbAccess);
	if (traceOn)
	{
		HobString myDebug("firstCOOLList():  COOLList = ");
		myDebug.append(Hex::toString((long long) newList));
		Debug::trace("COOLManager.COOLList", myDebug);
	}

	return newList;
}


COOLList *COOLManager::nextCOOLList()
{
	if (!dbAccess)
		return 0;

	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return 0;

	if (allCoolListsQuery && !allCoolListsQuery->isEnabled())
		destroyCOOLListsQueryAndIter();

	if (!allCoolListsQuery || !allCoolListsIter)
		return firstCOOLList();

	bool traceOn = Debug::isTracing("COOLManager.COOLList");

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolListsIter);
	if (*allCoolListsIter == allCoolListsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all CoolLists in the db. */
		destroyCOOLListsQueryAndIter();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolList copy constructor in order for the Hfc COOLList to
	 * control the destruction of the DBCoolList record that the iterator contains; otherwise, the DBCoolList will
	 * be deleted when the iterator goes out of scope.  The DBCoolList copy will be deleted by the COOLList record
	 * created below.
	 */
	DBCoolList *c = new DBCoolList(*(*allCoolListsIter));
	if (traceOn)
	{
		HobString myDebug("nextCOOLList():  DBCoolList = ");
		myDebug.append(Hex::toString((long long) c));
		Debug::trace("COOLManager.COOLList", myDebug);
	}
	COOLList *newList = new COOLList(c, dbAccess);
	if (traceOn)
	{
		HobString myDebug("nextCOOLList():  COOLList = ");
		myDebug.append(Hex::toString((long long) newList));
		Debug::trace("COOLManager.COOLList", myDebug);
	}

	return newList;
}


int COOLManager::countAllCOOLLists()
{
	if (!dbAccess)
		return -1;

	DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListsRM)
		return -1;

	if (Debug::isTracing("COOLManager.COOLList"))
	{
		HobString myDebug("countAllCOOLLists():  count = ");
		myDebug.append(Integer::toString(dbCoolListsRM->countAll()));
		Debug::trace("COOLManager.COOLList", myDebug);
	}
	return dbCoolListsRM->countAll();
}


COOLListText *COOLManager::firstCOOLListText()
{
	if (!dbAccess)
		return 0;

	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
	if (!dbCoolListTextsRM)
		return 0;

	bool traceOn = Debug::isTracing("COOLManager.COOLListText");

	// If the user calls firstCOOLListText() after they have already called it,
	// then delete the old query and create a new one.
	destroyCOOLListTextsQueryAndIter();

   	allCoolListTextsQuery = dbCoolListTextsRM->retrieveAll();

	// Couldn't get the query from which to access the COOLListTexts, just return false.
	if (!allCoolListTextsQuery)
		return 0;

	if (traceOn)
	{
		HobString myDebug("firstCOOLListText():  query size = ");
		myDebug.append(Integer::toString((int)allCoolListTextsQuery->size()));
		Debug::trace("COOLManager.COOLListText", myDebug);
	}

	DBQuery<DBCoolListText>::const_iterator tempIter = allCoolListTextsQuery->begin();
	if (tempIter == allCoolListTextsQuery->end())
		return 0;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextCOOLListText()
	 * method call below).
	 */
	allCoolListTextsIter = new DBQuery<DBCoolListText>::const_iterator(tempIter);
	DBCoolListText *c = new DBCoolListText(*(*allCoolListTextsIter));
	if (traceOn)
	{
		HobString myDebug("firstCOOLListText():  DBCoolListText = ");
		myDebug.append(Hex::toString((long long) c));
		Debug::trace("COOLManager.COOLListText", myDebug);
	}
	COOLListText *newListText = new COOLListText(c, dbAccess);
	if (traceOn)
	{
		HobString myDebug("firstCOOLListText():  COOLListText = ");
		myDebug.append(Hex::toString((long long) newListText));
		Debug::trace("COOLManager.COOLListText", myDebug);
	}

	return newListText;
}


COOLListText *COOLManager::nextCOOLListText()
{
	if (!dbAccess)
		return 0;

//	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
//	if (!dbCoolListTextsRM)
//		return 0;

	if (allCoolListTextsQuery && !allCoolListTextsQuery->isEnabled())
		destroyCOOLListTextsQueryAndIter();

	if (!allCoolListTextsQuery || !allCoolListTextsIter)
		return firstCOOLListText();

	bool traceOn = Debug::isTracing("COOLManager.COOLListText");

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allCoolListTextsIter);
	if (*allCoolListTextsIter == allCoolListTextsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all COOLListTexts in the db. */
		destroyCOOLListTextsQueryAndIter();
		return 0;
	}

	/* Per Santos (9/30/02), we need to use the DBCoolListText copy constructor in order for the Hfc COOLListText to
	 * control the destruction of the DBCoolListText record that the iterator contains; otherwise, the DBCoolListText will
	 * be deleted when the iterator goes out of scope.  The DBCoolListText copy will be deleted by the COOLListText record
	 * created below.
	 */
	DBCoolListText *c = new DBCoolListText(*(*allCoolListTextsIter));
	if (traceOn)
	{
		HobString myDebug("nextCOOLListText():  DBCoolListText = ");
		myDebug.append(Hex::toString((long long) c));
		Debug::trace("COOLManager.COOLListText", myDebug);
	}
	COOLListText *newListText = new COOLListText(c, dbAccess);
	if (traceOn)
	{
		HobString myDebug("nextCOOLListText():  COOLListText = ");
		myDebug.append(Hex::toString((long long) newListText));
		Debug::trace("COOLManager.COOLListText", myDebug);
	}

	return newListText;
}


int COOLManager::countAllCOOLListTexts()
{
	if (!dbAccess)
		return -1;

	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
	if (!dbCoolListTextsRM)
		return -1;

	if (Debug::isTracing("COOLManager.COOLListText"))
	{
		HobString myDebug("countAllCOOLListTexts():  count = ");
		myDebug.append(Integer::toString(dbCoolListTextsRM->countAll()));
		Debug::trace("COOLManager.COOLListText", myDebug);
	}
	return dbCoolListTextsRM->countAll();
}


void COOLManager::handleCOOLShortlistDeleteText(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("COOLManager.Shortlist");
	HobString myDebug("handleCOOLShortlistDeleteText():  ");

	COOLShortlist *msgShortlist = dynamic_cast<COOLShortlist *> (msg->getHfcObject());

	if (!dbAccess || !msgShortlist)
	{
		if (traceOn)
		{
			if (!dbAccess)
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  dbAccess is NULL!");
			else
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  msgShortlist is NULL!");
		}
		return;
	}

	DataManager *myManager = mgrContainer_->getCOOLShortlistManager();
	if (!myManager)
		return;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolLists))
	{
		// batch the record being restored; don't actually write it to the database right now.
		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

		// Even though this is UltimaTaggedFormat, things have changed that allow the Synchronization feature to deal with this format,
		// so just write out the record received.
		receivedChange = new hfc::SynchronizationChange(myManager->getDevice(), myManager->getWriteEvent(), (MessageFormat)msg->getFormatType(), msg->getOriginalBuff());

		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
	}
	else
	{
		if (traceOn)
		{
			HobString myDebug2 = myDebug;
			myDebug2.append("COOLShortlist::show()  *****  ");
			Debug::trace("COOLManager", myDebug2 + "Start");
			if (msgShortlist)
				msgShortlist->show();
			Debug::trace("COOLManager.Shortlist", myDebug2 + " End");
		}

		DBCoolListsRM *dbCoolListsRM = dbAccess->getDBCoolListsRM();
		if (!dbCoolListsRM)
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  dbCoolListsRM is NULL!");
			return;
		}

		DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
		if (!dbCoolListTextsRM)
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "EXITING!!!  dbCoolListTextsRM is NULL!");
			return;
		}

		Util::CSmartPtr<Product> theProduct(Product::findRecord(msgShortlist->getProductNum(), msgShortlist->getDeptNum(), dbAccess));

		if (theProduct.null())
		{
			if (traceOn)
			{
				HobString myDebug2 = myDebug;
				myDebug2.append("EXITING!!!  Product *theProduct is NULL! Looking for PLU# ").append(Integer::toString(msgShortlist->getProductNum()));
				Debug::trace("COOLManager.Shortlist", myDebug2);
			}
			return;
		}

		DBCoolList *dbCoolList = 0;
		COOLListText *coolListText = 0;

		DBProduct *dbProduct = theProduct->getMyDbRecord();
		if (dbProduct)
		{
			dbCoolList = dbCoolListsRM->find(dbProduct);
			if (!dbCoolList)
			{
				// Get the CoolList based on the Product number in the Product record.
				dbCoolList = dbCoolListsRM->find(longtostring(theProduct->getProductNum()), longtostring(theProduct->getDeptNum()));
			}
		}
		else
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "Exiting!!!  DBProduct *dbProduct is NULL!");
			return;
		}

		if (dbCoolList)
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + "Have a dbCoolList.");
			//  Find the record in the ShortList.
			DBCoolListText *dbCoolListText = dbCoolListTextsRM->find(longtostring(msgShortlist->getCoolTextNum()), longtostring(msgShortlist->getDeptNum()), dbCoolList);
			if (dbCoolListText)
			{
				if (traceOn)
					Debug::trace("COOLManager.Shortlist", myDebug + "Have a dbCoolListText.");
				int myOrder = dbCoolListText->getOrder();
				HobRecordNum newDefaultCoolTextNum = RECORD_NOT_FOUND;
				HobRecordNum prevCoolTextNum = RECORD_NOT_FOUND;

				//	Now try to find the Cool text prior to this one before it is deleted.
				DBQuery<DBCoolListText>* dbCoolListTextQuery = dbCoolListTextsRM->retrieveLinkedtoCoolList(dbCoolList);
				if (dbCoolListTextQuery)
				{
					if (traceOn)
						Debug::trace("COOLManager.Shortlist", myDebug + "Have a dbCoolListTextQuery.  myOrder = " + Integer::toString(myOrder));
					DBQuery<DBCoolListText>::const_iterator tempIter = dbCoolListTextQuery->begin();
					while (tempIter != dbCoolListTextQuery->end())
					{
						if (traceOn)
							Debug::trace("COOLManager.Shortlist", myDebug + "tempIter->getOrder() = " + Integer::toString(tempIter->getOrder()));
						if (myOrder != tempIter->getOrder())
						{
							prevCoolTextNum = Long::valueOf(tempIter->getCoolTextNumber());
							++tempIter;
						}
						else
						{
							//	If there is no COOLText after this one, use the one before it.
							if (++tempIter != dbCoolListTextQuery->end())
								newDefaultCoolTextNum = Long::valueOf(tempIter->getCoolTextNumber());
							else
								newDefaultCoolTextNum = prevCoolTextNum;
							break;
						}
						if (traceOn)
							Debug::trace("COOLManager.Shortlist", myDebug + "prevCoolTextNum = " + Long::toString(prevCoolTextNum) + ",  newDefaultCoolTextNum = " + Long::toString(newDefaultCoolTextNum));
					}
					delete dbCoolListTextQuery;

					Session *s = dynamic_cast<Session *> (mySession);
					ProductManager *theProductManager = (s) ? s->getProductManager() : 0;
					if (theProductManager)
						theProductManager->assignCOOLTextToCurrentProduct(msgShortlist->getCoolTextNum(), newDefaultCoolTextNum);
					if (traceOn)
						Debug::trace("COOLManager.Shortlist", myDebug + "After call to theProductManager->assignCOOLTextToCurrentProduct");
				}

				try
				{
					dbCoolListTextsRM->deleteRecord(dbCoolListText);

					coolListText = new COOLListText(dbCoolListText, dbAccess);
					if (coolListText)
					{
						DataManager *myManager = mgrContainer_->getCOOLListTextManager();
						if ( myManager && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::CoolListTexts))
						{
							std::string synchSer = "";
							serializeForSynchronization(synchSer, coolListText, myManager->getDeleteEvent());

							synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

							if (proxy)
								proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
									hfc::SynchronizationChange(myManager->getDevice(), myManager->getDeleteEvent(), synchSer)));
						}
					}
				}
				catch (DBException &dbe)
				{
					Debug::trace("DBAccess.exception", myDebug + "dbCoolListTextsRM->deleteRecord(dbCoolListText) ********** database exception **********");
					DBException::Type t = dbe.getType();
					Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
				}
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("  Deleted dbCoolListText:  order = ").append(Integer::toString(myOrder)).append(" .");
					Debug::trace("COOLManager.Shortlist", myDebug2);
				}

				if (coolListText)
					delete coolListText;
				else
					delete dbCoolListText;
			}
			DBQuery<DBCoolListText>* dbCoolListTextQuery = dbCoolListTextsRM->retrieveLinkedtoCoolList(dbCoolList);
			if (dbCoolListTextQuery)
			{
				int count = 0;
				DBQuery<DBCoolListText>::const_iterator tempIter = dbCoolListTextQuery->begin();
				DBQuery<DBCoolListText>::const_iterator prevIter = dbCoolListTextQuery->begin();
				if (tempIter != dbCoolListTextQuery->end())
					++count;

				if (traceOn)
					Debug::trace("COOLManager.Shortlist", myDebug + " Count linked to CoolList = " + Integer::toString(count));
				delete dbCoolListTextQuery;
				if (count == 0)
				{
					dbCoolListsRM->deleteRecord(dbCoolList);
					if (traceOn)
						Debug::trace("COOLManager.Shortlist", myDebug + "Delete dbCoolList from the database.");
				}
			}
			delete dbCoolList;
		}
		else
		{
			if (traceOn)
				Debug::trace("COOLManager.Shortlist", myDebug + " Error! Could not find the dbCoolList!");
		}
	}
}