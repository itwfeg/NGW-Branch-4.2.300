/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "FlashFileManager.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Configuration.h"
#include "SessionEvent.h"
#include "VersionManager.h"
#include "CMFile.h"
#include "CMReaderWriterEvolTag.h"
#include "filesys/directory.h"
#include "filesys/filename.h"
#include "ResponseMessageManager.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"

#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FlashFileManager::dataType = 0;
std::string FlashFileManager::dataTypeName("FlashFileManager"); 
#endif

FlashFileManager::FlashFileManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

FlashFileManager::FlashFileManager(hfc::HFCManagerContainer *container)
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

FlashFileManager::~FlashFileManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (currentFlashFile)
		delete currentFlashFile;

	destroyQueriesAndIters();

	if (fileDestination)
		delete fileDestination;
}


void FlashFileManager::init()
{
	mySession = 0;

	currentFlashFile = 0;

	allFlashFilesQuery = 0;
	allFlashFilesIter  = 0;

	completed = true;
	loadFile = false;
	fileIsGraphic = false;
	graphicFileType = Graphic::TYPE_UNKNOWN;
	fileDestination = 0;
}


DBSession *FlashFileManager::getMySession()
{
	return mySession;
}
void FlashFileManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
FlashFile *FlashFileManager::firstFlashFile()
{
	if (!dbAccess)
		return (FlashFile *)NULL;

	DBFlashFilesRM *dbFlashFilesRM = dbAccess->getDBFlashFilesRM();
	if (!dbFlashFilesRM)
		return (FlashFile *)NULL;

	// If the user calls firstFlashFile() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allFlashFilesQuery = dbFlashFilesRM->retrieveAll();

	// Couldn't get the query from which to access the FlashFile, just return false.
	if (!allFlashFilesQuery)
		return (FlashFile *)NULL;

	DBQuery<DBFlashFile>::const_iterator tempIter = allFlashFilesQuery->begin();
	if (tempIter == allFlashFilesQuery->end())
		return (FlashFile *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextFlashFile()
	 * method call below).
	 */
	allFlashFilesIter = new DBQuery<DBFlashFile>::const_iterator(tempIter);
	DBFlashFile *ff = new DBFlashFile(*(*allFlashFilesIter));
	FlashFile *newFlashFile = new FlashFile(ff, dbAccess);
	return newFlashFile;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstFlashFile() method, the first call to this method will
 * result in the first record being returned.
 */
FlashFile *FlashFileManager::nextFlashFile()
{
	if (!dbAccess)
		return (FlashFile *)NULL;

	if (allFlashFilesQuery && !allFlashFilesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allFlashFilesQuery || !allFlashFilesIter)
		return firstFlashFile();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allFlashFilesIter);
	if (*allFlashFilesIter == allFlashFilesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all FlashFile in the db. */
		destroyQueriesAndIters();
		return (FlashFile *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBFlashFile copy constructor in order for the Hfc FlashFile to
	 * control the destruction of the DBFlashFile record that the iterator contains; otherwise, the DBFlashFile will
	 * be deleted when the iterator goes out of scope.  The DBFlashFile copy will be deleted by the FlashFile record
	 * created below.
	 */
	DBFlashFile *ff = new DBFlashFile(*(*allFlashFilesIter));
	FlashFile *newFlashFile = new FlashFile(ff, dbAccess);
	return newFlashFile;
}


/* Provide a countAll method so the caller doesn't need to know about a dbFlashFilesRM.
 */
int FlashFileManager::countAllFlashFiles()
{
	if (!dbAccess)
		return -1;

	DBFlashFilesRM *dbFlashFilesRM = dbAccess->getDBFlashFilesRM();
	if (!dbFlashFilesRM)
		return -1;

	return dbFlashFilesRM->countAll();
}


FlashFile *FlashFileManager::getCurrentFlashFile()
{
	return currentFlashFile;
}
void FlashFileManager::setCurrentFlashFile(FlashFile *newFlashFile)
{
	if (currentFlashFile)
		delete currentFlashFile;
	currentFlashFile = newFlashFile;
}


void FlashFileManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	FlashFile *msgFlashFile = dynamic_cast<FlashFile *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	if (Debug::isTracing("FlashFileManager"))
	{
		HobString myDebug("handleMessage()  event: ");
		myDebug.append(Integer::toString(event));
		Debug::trace("FlashFileManager", myDebug);
	}
	switch (event)
	{
#if 0
		case DtRead:
			handleRead(msgFlashFile);
			break;

		case DtWrite:
			handleWrite(msgFlashFile);
			break;

		case DtDelete:
			handleDelete(msgFlashFile);
			break;
#endif

		case DtDeleteAll:
			handleDeleteAll();
			break;

#if 0
		case DtSave:
			handleSave(msgFlashFile);
			break;
#endif

		case DtRestore:
			handleRestore(msgFlashFile);
			break;

		default:
			if (Debug::isTracing("FlashFileManager"))
			{
				HobString myDebug("handleMessage()  UNKNOWN EVENT: ");
				myDebug.append(Integer::toString(event));
				Debug::trace("FlashFileManager", myDebug);
			}
			break;
	}
}


#if 0
void FlashFileManager::handleRead(FlashFile *msgFlashFile)
{
	if (!msgFlashFile || !dbAccess)
		return;

	HobString flashFileName = msgFlashFile->getFileName();
	if (flashFileName.empty())
	{
		setCurrentFlashFile((FlashFile *)NULL);
		return;
	}

	FlashFile *lookedUpFlashFile = FlashFile::findFlashFile(flashFileName, dbAccess);

	if (!lookedUpFlashFile || lookedUpFlashFile->getErrorCode() != RECORD_OK)
	{
		if (lookedUpFlashFile)
			delete lookedUpFlashFile;
		return;
	}

	// We have a new current class.
	setCurrentFlashFile(lookedUpFlashFile);
	Debug::trace("FlashFileManager.FlashFile", "handleFlashFileReadMessage(): currentFlashFile = " + Hex::toString((long int) currentFlashFile));

	if (mySession)
	{
		// Send the FlashFile found back to the Browser.
		// Is this the behavior for ALL FlashFileRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a FlashFile, but doesn't want the data sent back?
		SessionMessage flashFileData(FlashFilesDev, DtWrite);
		flashFileData.setFormatType(BrowserFormat);
		flashFileData.setHfcObject(currentFlashFile);

		// Send the product to the UI (Browser)
		int bytesSent = mySession->send(&flashFileData, Messenger::DEST_UI);
		if (bytesSent < 0)
		{
			// todo - check correct number of bytes sent?
		}
	}
}


void FlashFileManager::handleWrite(FlashFile *modifiedFlashFile)
{
	if (Debug::isTracing("FlashFileManager"))
	{
		HobString myDebug("handleWrite(): currentFlashFile = ");
		myDebug.append(Hex::toString((long int) currentFlashFile));
		myDebug.append("  , modifiedFlashFile = ").append(Hex::toString((long int) modifiedFlashFile));
		Debug::trace("FlashFileManager", myDebug);
	}
	if ((!modifiedFlashFile) || (!currentFlashFile))
		return;

  	currentFlashFile->updateFlashFile(modifiedFlashFile);
}


void FlashFileManager::handleDelete(FlashFile *msgFlashFile)
{
	if (!msgFlashFile || !dbAccess)
		return;

	HobString myDebug("handleDelete():  ");
	HobString flashFileName = msgFlashFile->getFileName();
	FlashFile *existingFlashFile = FlashFile::findFlashFile(flashFileName, dbAccess);
	if (existingFlashFile)
	{
		existingFlashFile->deleteRecord();
		delete existingFlashFile;
		if (Debug::isTracing("FlashFileManager"))
			myDebug.append(" FlashFile deleted: ").append(flashFileName);
	}
	else
	{
		if (Debug::isTracing("FlashFileManager"))
			myDebug.append(" FlashFile to be deleted was not in the database to delete: ").append(flashFileName);
	}
	Debug::trace("FlashFileManager", myDebug);
}
#endif


void FlashFileManager::handleDeleteAll()
{
	Debug::trace("FlashFileManager", "handleDeleteAll - deleting all FlashFile");

	if (!dbAccess)
		return;

	DBFlashFilesRM *dbFlashFilesRM = dbAccess->getDBFlashFilesRM();
	if (!dbFlashFilesRM)
		return;

	dbFlashFilesRM->deleteAll();

	destroyQueriesAndIters();
	Debug::trace("FlashFileManager", "handleDeleteAll - leaving");
}


#if 0
void FlashFileManager::handleSave(FlashFile *modifiedFlashFile)
{
	Debug::trace("FlashFileManager", "handleSave(): currentFlashFile = " + Hex::toString((long int) currentFlashFile));
	if (currentFlashFile)
	{
		if (modifiedFlashFile)
		  	currentFlashFile->updateFlashFile(modifiedFlashFile);
		currentFlashFile->saveRecord();
	}
}
#endif


void FlashFileManager::handleRestore(FlashFile *restoreFlashFile)
{
	if (!restoreFlashFile || !dbAccess)
		return;

	Debug::trace("FlashFileManager", "handleRestore - attempting to restore: " + restoreFlashFile->getFileName());

	restoreFlashFile->setDBAccess(dbAccess);

	HobString flashFileName = restoreFlashFile->getFileName();
	FlashFile *existingFlashFile = FlashFile::findFlashFile(flashFileName, dbAccess);
	if (existingFlashFile)
	{
		Debug::trace("FlashFileManager", "handleRestore - FlashFile already existed");
		if (existingFlashFile->isRecordValid())
		{
			existingFlashFile->updateFlashFile(restoreFlashFile);
			existingFlashFile->saveRecord();
			Debug::trace("FlashFileManager", "handleRestore - saved updated FlashFile");
		}
		else
		{
			existingFlashFile->deleteRecord();
			restoreFlashFile->saveRecord();
		}

		delete existingFlashFile;
	}
	else
	{
		Debug::trace("FlashFileManager", "handleRestore - did not find FlashFile, going to save");
		restoreFlashFile->saveRecord();
		Debug::trace("FlashFileManager", "handleRestore - created and saved FlashFile");
	}
}


const HobString &FlashFileManager::getFileName() const
{
	return fileName;
}
void FlashFileManager::setFileName(const HobString &name)
{
	fileName = name;
}


const HobString &FlashFileManager::getLocalFlashFileName() const
{
	return localFlashFileName;
}


bool FlashFileManager::doLoadTheFile() const
{
	return loadFile;
}


bool FlashFileManager::isCompleted() const
{
	return completed;
}


bool FlashFileManager::testIsGraphicFile(util_filesystem::File &srcFile)
{
	HobString myDebug("testIsGraphicFile():  ");
	bool isTracing = Debug::isTracing("FlashFileManager");
	bool isUserGraphic = false;
	bool isSelfServeGraphic = false;
	std::string::size_type pos;

	Configuration *myConfig = Configuration::getInstance();
	HobString sourcePath = srcFile.getPath();

	if ((sourcePath.rfind('/') != (sourcePath.length() - 1)) && (sourcePath.rfind('\\') != (sourcePath.length() - 1)))
		sourcePath.append("/");

	HobString userGraphicsPath(myConfig->getPermanentRamDriveName());
	if ((userGraphicsPath.rfind('/') != (userGraphicsPath.length() - 1)) && (userGraphicsPath.rfind('\\') != (userGraphicsPath.length() - 1)))
		userGraphicsPath.append("/");
	userGraphicsPath.append(sourcePath);

	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("userGraphicsPath = ").append(userGraphicsPath).append("    UserGraphicsLocation = ").append(myConfig->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
		Debug::trace("FlashFileManager", myDebug2);
	}
	pos = userGraphicsPath.rfind(myConfig->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
	if (pos != std::string::npos)
	{
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append(srcFile.getFileName()).append(" - It's a User Graphic.");
			Debug::trace("FlashFileManager", myDebug2);
		}
		graphicFileType = Graphic::TYPE_USERLABELGRAPHIC;
		isUserGraphic = true;
	}
	else
	{
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("sourcePath = ").append(sourcePath).append("    ProductGraphicsDirectoryName = ").append(myConfig->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
			Debug::trace("FlashFileManager", myDebug2);
		}
		pos = sourcePath.rfind(myConfig->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
		if (pos != std::string::npos)
		{
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append(srcFile.getFileName()).append(" - It's a Product Graphic.");
				Debug::trace("FlashFileManager", myDebug2);
			}
			graphicFileType = Graphic::TYPE_PRODUCTGRAPHIC;
			isSelfServeGraphic = true;
		}
		else
		{
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("sourcePath = ").append(sourcePath).append("    CategoryGraphicsDirectoryName = ").append(myConfig->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
				Debug::trace("FlashFileManager", myDebug2);
			}
			pos = sourcePath.rfind(myConfig->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
			if (pos != std::string::npos)
			{
				if (isTracing)
				{
					HobString myDebug2(myDebug);
					myDebug2.append(srcFile.getFileName()).append(" - It's a Category Graphic.");
					Debug::trace("FlashFileManager", myDebug2);
				}
				graphicFileType = Graphic::TYPE_CATEGORYGRAPHIC;
				isSelfServeGraphic = true;
			}
			else
			{
				if (isTracing)
				{
					HobString myDebug2(myDebug);
					myDebug2.append("sourcePath = ").append(sourcePath).append("    SpecialsGraphicsDirectoryName = ").append(myConfig->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
					Debug::trace("FlashFileManager", myDebug2);
				}
				pos = sourcePath.rfind(myConfig->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
				if (pos != std::string::npos)
				{
					if (isTracing)
					{
						HobString myDebug2(myDebug);
						myDebug2.append(srcFile.getFileName()).append(" - It's a Specials Graphic.");
						Debug::trace("FlashFileManager", myDebug2);
					}
					graphicFileType = Graphic::TYPE_SPECIALSGRAPHIC;
					isSelfServeGraphic = true;
				}
				else
				{
					if (isTracing)
					{
						HobString myDebug2(myDebug);
						myDebug2.append("sourcePath = ").append(sourcePath).append("    SlideShowGraphicsDirectoryName = ").append(myConfig->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
						Debug::trace("FlashFileManager", myDebug2);
					}
					pos = sourcePath.rfind(myConfig->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
					if (pos != std::string::npos)
					{
						if (isTracing)
						{
							HobString myDebug2(myDebug);
							myDebug2.append(srcFile.getFileName()).append(" - It's a SlideShow Graphic.");
							Debug::trace("FlashFileManager", myDebug2);
						}
						graphicFileType = Graphic::TYPE_STREAMINGGRAPHIC;
						isSelfServeGraphic = true;
					}
					else
					{
						if (isTracing)
						{
							HobString myDebug2(myDebug);
							myDebug2.append("sourcePath = ").append(sourcePath).append("    SoftkeyGraphicsDirectoryName = ").append(myConfig->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
							Debug::trace("FlashFileManager", myDebug2);
						}
						pos = sourcePath.rfind(myConfig->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
						if (pos != std::string::npos)
						{
							if (isTracing)
							{
								HobString myDebug2(myDebug);
								myDebug2.append(srcFile.getFileName()).append(" - It's a Softkey Graphic.");
								Debug::trace("FlashFileManager", myDebug2);
							}
							graphicFileType = Graphic::TYPE_SOFTKEYGRAPHIC;
							isSelfServeGraphic = true;
						}
						else
						{
							if (isTracing)
							{
								HobString myDebug2(myDebug);
								myDebug2.append("sourcePath = ").append(sourcePath).append("    PlanogramGraphicsDirectoryName = ").append(myConfig->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
								Debug::trace("FlashFileManager", myDebug2);
							}
							pos = sourcePath.rfind(myConfig->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
							if (pos != std::string::npos)
							{
								if (isTracing)
								{
									HobString myDebug2(myDebug);
									myDebug2.append(srcFile.getFileName()).append(" - It's a Planogram Graphic.");
									Debug::trace("FlashFileManager", myDebug2);
								}
								graphicFileType = Graphic::TYPE_PLANOGRAMGRAPHIC;
								isSelfServeGraphic = true;
							}
						}
					}
				}
			}
		}
	}
	if (fileDestination)
	{
		delete fileDestination;
		fileDestination = (Filename *)NULL;
	}

	if (isUserGraphic)
	{
		fileDestination = new Filename(userGraphicsPath, srcFile.getFileName());
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("isUserGraphic:   fileDestination->getFullPath() = ").append(fileDestination->getFullPath());
			Debug::trace("FlashFileManager", myDebug2);
		}
	}
	else if (isSelfServeGraphic)
	{
		HobString selfServeGraphicsPath(myConfig->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
		if ((selfServeGraphicsPath.rfind('/') != (selfServeGraphicsPath.length() - 1)) && (selfServeGraphicsPath.rfind('\\') != (selfServeGraphicsPath.length() - 1)))
			selfServeGraphicsPath.append("/");
		selfServeGraphicsPath.append(sourcePath);

		fileDestination = new Filename(selfServeGraphicsPath, srcFile.getFileName());
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("isSelfServeGraphic:   fileDestination->getFullPath() = ").append(fileDestination->getFullPath());
			Debug::trace("FlashFileManager", myDebug2);
		}
	}

	if (fileDestination)
	{
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("testIsGraphicFile():   fileDestination->getFullPath() = ").append(fileDestination->getFullPath());
			Debug::trace("FlashFileManager", myDebug2);
		}
	}
	fileIsGraphic = (isSelfServeGraphic | isUserGraphic);
	return fileIsGraphic;
}

bool FlashFileManager::isGraphicFile()
{
	return fileIsGraphic;
}

Filename *FlashFileManager::getFileDestination()
{
	return fileDestination;
}

short FlashFileManager::getGraphicFileType()
{
	return graphicFileType;
}


bool FlashFileManager::startRemoteFlashFileLoad()
{
	completed = false;
	loadFile = false;
	fileIsGraphic = false;
	graphicFileType = Graphic::TYPE_UNKNOWN;

	Debug::trace("FlashFileManager", "  startRemoteFlashFileLoad()");
	if (dbAccess)
	{
		fileName = "";

		VersionManager::getInstance()->getRemoteDBFile(fileName);
		Debug::trace("FlashFileManager", "  startRemoteFlashFileLoad():  fileName = " + fileName);
		if (!fileName.empty())
			checkRemoteFlashFile();
	}
	return completed;
}


void FlashFileManager::checkRemoteFlashFile()
{
	HobString myDebug("checkRemoteFlashFile()");
	bool isTracing = Debug::isTracing("FlashFileManager");

	Debug::trace("FlashFileManager", myDebug);
	loadFile = false;
	if (!fileName.empty())
	{
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("  fileName = ").append(fileName);
			Debug::trace("FlashFileManager", myDebug2);
		}

    #if defined(LINUX_FIX)
        // myConfig: unused variable
    #else
		Configuration *myConfig = Configuration::getInstance();
    #endif

		completed = false;

		util_filesystem::File destFile(fileName);

		//	Set localFlashFileName to the destFile full path name.
		localFlashFileName = destFile.getFullPath();

		if (isTracing)
		{
			HobString myDebug2("");
			myDebug2.append("  destFile = ").append(destFile.getFullPath());
			Debug::trace("FlashFileManager", myDebug2);
		}


		Debug::trace("FlashFileManager", HobString("    checkRemoteFlashFile()  Database file.\n\n\n\n"));

		HobString data = "";
		HobString dateString = "";

		//	Test to see if if we should load this file.
		CMFile *fp;
		CMReaderWriter *frw;

		fp = new CMFile(destFile.getFullPath().c_str(), CommMechanism::MODE_INPUT);
		if (!fp)
		{
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append(":   Cannot read CMFile ").append(localFlashFileName);
				Debug::trace("FlashFileManager", myDebug2);
			}
		}
		else
		{
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append(":   Opening CMFile ").append(localFlashFileName);
				Debug::trace("FlashFileManager", myDebug2);
			}
			frw = new CMReaderWriterEvolTag();
			fp->setMyReaderWriter(frw);
			int len = fp->cmReceive(data);
			while (len > 0)
			{
				if (isTracing)
				{
					HobString myDebug2(myDebug);
					myDebug2.append(":   data = ").append(data);
					Debug::trace("FlashFileManager", myDebug2);
				}

				EvolDeviceType device = UnknownDevice;
				UINT8 event = 0;
				char rtData[5];
				memset(rtData, 0, 5);

				HobTokenizerUS tok(&data, UnitSeparator);
				HobString token;
				if (tok.next(&token) != -1)
				{
					CMReaderWriterEvolTag::getRTData(token.c_str(), rtData);
					CMReaderWriterEvolTag::determineDeviceAndEvent(rtData, &device, &event);

					if ((device == SessionDev) && (event == SnTFTPdbDate))
					{
						//	The next token should contain the TFTP_Db_Date_TAG and date string
						if (tok.next(&token) != -1)
						{
							if (token.substr(0, 2) == "tb")		//	TFTP_Db_Date_TAG (Not in any tag category.)
							{
								dateString = token.substr(2);
								if (isTracing)
								{
									HobString myDebug2(myDebug);
									myDebug2.append(":   Got the date! dateString = ").append(dateString);
									Debug::trace("FlashFileManager", myDebug2);
								}
							}
						}
						break;
					}
				}
				len = fp->cmReceive(data);
			}
			fp->cmClose();
			delete fp;		//	frw will be deleted by the fp destructor.
		}
		if (!dateString.empty())
		{
			HobDateTime dbFileDate;

			currentDate.setToCurrent();
			currentDate.setTimeParts(0, 0, 0);

			HobDateTime	theFileDate;
			theFileDate.setTimeParts(0, 0, 0);

			int theYear = Integer::valueOf(dateString.substr(4, 2));
			theFileDate.setDateParts(Integer::valueOf(dateString.substr(0, 2)), Integer::valueOf(dateString.substr(2, 2)), (theYear < 100 ? (theYear + 100) : theYear));

			long currentCalTime = (long) currentDate.getCalendarTime();
			long fileCalTime = (long) theFileDate.getCalendarTime();

			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append(":   currentCalTime is ").append(Long::toString(currentCalTime));
				Debug::trace("FlashFileManager", myDebug2);
			}
			currentDate.show();
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append(":   fileCalTime is ").append(Long::toString(fileCalTime));
				Debug::trace("FlashFileManager", myDebug2);
			}
			theFileDate.show();
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append(":   currentDate >= theFileDate : ").append(Bool::toString(currentCalTime >= fileCalTime));
				Debug::trace("FlashFileManager", myDebug2);
			}
			if (currentCalTime >= fileCalTime)
			{
				//	Have we already loaded this file? Check it in the FlashFile list in the db.
				FlashFile *readFlashFile = FlashFile::findFlashFile(fileName, dbAccess);
				if (readFlashFile)
				{
					long dbCalTime = 0;

					if (isTracing)
					{
						HobString myDebug2(myDebug);
						myDebug2.append(fileName).append(" DB record exists!");
						Debug::trace("FlashFileManager", myDebug2);
					}

					//	Get the date and compare it with currentCalTime
					HobString dateTime = readFlashFile->getFileDate();
					dbFileDate.convertFromDBFormat(&dateTime);
					dbFileDate.setTimeParts(0, 0, 0);
					dbCalTime = (long) dbFileDate.getCalendarTime();
					if (isTracing)
					{
						HobString myDebug2(myDebug);
						myDebug2.append(":  dbFileDate is     ").append(Long::toString(dbCalTime));
						Debug::trace("FlashFileManager", myDebug2);
					}
					dbFileDate.show();
					if (isTracing)
					{
						HobString myDebug2(myDebug);
						myDebug2.append(":  fileCalTime > dbCalTime : ").append(Bool::toString(fileCalTime > dbCalTime));
						Debug::trace("FlashFileManager", myDebug2);
					}
					if (fileCalTime > dbCalTime)
					{
						if (isTracing)
							Debug::trace("FlashFileManager", HobString ("fileCalTime > dbCalTime:: the files date is new than last time the file was processed"));

						loadFile = true;

					}
				}
				else
				{
					if (isTracing)
					{
						HobString myDebug2(myDebug);
						myDebug2.append(":  DB record does not exist. Creating a new FlashFile record");
						Debug::trace("FlashFileManager", myDebug2);
					}
					readFlashFile = new FlashFile(dbAccess);
					if (readFlashFile)
					{
						readFlashFile->setFileName(fileName);
						loadFile = true;
					}
					else
					{
						if (isTracing)
						{
							HobString myDebug2(myDebug);
							myDebug2.append(":  ERROR:  Could not create a new FlashFile!");
							Debug::trace("FlashFileManager", myDebug2);
						}
						loadFile = false;
					}
				}
				if (readFlashFile)
					setCurrentFlashFile(readFlashFile);
			}
			else
			{
				HobString myDebug2(myDebug);
				myDebug2.append(":  file not ready to be processed yet!");
				Debug::trace("FlashFileManager", myDebug2);

			}

		}
		else
		{
			HobString myDebug2(myDebug);
			myDebug2.append(":  ERROR:  No Date String!");
			Debug::trace("FlashFileManager", myDebug2);

		}

	}
}

bool FlashFileManager::getNextRemoteFlashFile()
{
	HobString myDebug("getNextRemoteFlashFile():  ");
	bool isTracing = Debug::isTracing("FlashFileManager");
	Debug::trace("FlashFileManager", myDebug);
	if (loadFile)
	{
		//	First, update the db record with the current date, then save it.
		try
		{
			Debug::trace("FlashFileManager", myDebug + "  Updating/saving the FlashFile record.");
			currentFlashFile->setFileDate(currentDate.convertToDBFormat());
			currentFlashFile->saveRecord();
		}
		catch (DBException &dbe)
		{
			if (Debug::isTracing("DBAccess.exception"))
			{
				Debug::trace("DBAccess.exception", "FlashFileManager::getNextRemoteFlashFile() ********** database exception **********");
				DBException::Type t = dbe.getType();
				HobString myDebug2("     DBException: type: ");
				myDebug2.append(Integer::toString(t)).append("  info: ").append(dbe.info());
				Debug::trace("DBAccess.exception", myDebug2);
			}
		}
	}
	//	Remove the file locally.
	util_filesystem::File theFile(localFlashFileName);
	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("Before removing file: ").append(localFlashFileName).append("  exists ? = ").append(Bool::toString(theFile.exists()));
		Debug::trace("FlashFileManager", myDebug2);
	}
	if (theFile.exists())
		theFile.remove();
	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("After removing file: ").append(localFlashFileName).append("  exists ? = ").append(Bool::toString(theFile.exists()));
		Debug::trace("FlashFileManager", myDebug2);
	}

	//	Then get the next file.
	VersionManager::getInstance()->getRemoteDBFile(fileName);

	if (!fileName.empty())
		checkRemoteFlashFile();
	else
		completed = true;

	return completed;
}


void FlashFileManager::destroyQueriesAndIters()
{
	if (allFlashFilesIter)
	{
		delete allFlashFilesIter;
		allFlashFilesIter = (DBQuery<DBFlashFile>::const_iterator *)NULL;
	}

	if (allFlashFilesQuery)
	{
		delete allFlashFilesQuery;
		allFlashFilesQuery = (DBQuery<DBFlashFile> *)NULL;
	}
}
