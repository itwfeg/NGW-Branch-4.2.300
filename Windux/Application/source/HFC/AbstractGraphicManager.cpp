/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AbstractGraphicManager.h"
#include "HobString.h"
#include "Configuration.h"
#include "HfcErrorGraphic.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "DBSession.h"
#include "util/filesys/directory.h"
#include "util/filesys/filename.h"
#include "HobTokenizerCSV.h"
#include "ResponseMessageManager.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Bool.h"
#include "CZip.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AbstractGraphicManager::dataType = 0;
std::string AbstractGraphicManager::dataTypeName("AbstractGraphicManager"); 
#endif


AbstractGraphicManager::AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


AbstractGraphicManager::~AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


// private
void AbstractGraphicManager::init()
{
	mySession = 0;
	graphicFileNames = 0;

	currentType = Graphic::TYPE_UNKNOWN;
	currentSubType = Graphic::SUBTYPE_USERANDSYSTEM;
	myHFCRecordManagerType = hfc::HFCManagerContainer::InvalidType;
}


DBSession *AbstractGraphicManager::getMySession()
{
	return mySession;
}
void AbstractGraphicManager::setMySession(DBSession *session)
{
	mySession = session;
}


Graphic *AbstractGraphicManager::firstGraphic()
{
	if (graphicFileNames)
	{
		delete graphicFileNames;
		graphicFileNames = (GraphicFileNames *)NULL;
	}
	graphicFileNames = new GraphicFileNames(currentType, currentSubType);

	Graphic *graphic = (Graphic *)NULL;
	HobString graphicName;
	int found = graphicFileNames->first(graphicName);
	if (found >= 0)
		graphic = new Graphic(graphicName, currentType);
	return graphic;
}

/* If the last record was not read, this method will return NULL.
 */
Graphic *AbstractGraphicManager::lastGraphic()
{
	if (graphicFileNames)
	{
		delete graphicFileNames;
		graphicFileNames = (GraphicFileNames *)NULL;
	}
	graphicFileNames = new GraphicFileNames(currentType);

	Graphic *graphic = (Graphic *)NULL;
	HobString graphicName;
	//graphicName = graphicName.substr(0, graphicName.size() - 2);
	int found = graphicFileNames->last(graphicName);
	if (found >= 0)
		graphic = new Graphic(graphicName, currentType);
	return graphic;
}


Graphic *AbstractGraphicManager::setPositionAtGraphic(const HobString &beginFromGraphicName)
{
	if (beginFromGraphicName.size())
	{
		Graphic *current = firstGraphic();
		bool foundBeginFrom = false;
		while (true)
		{
			if (!current)
				break;

			// Need to check >= in the following check because the requested one may not exist.
			// If it doesn't, then stop looking when the found graphic name is greater than
			// (since they are sorted alphabetically) the one requested.
			std::string name = current->getGraphicNameBase();
			if (name >= beginFromGraphicName)
			{
				foundBeginFrom = true;
				break;
			}
			if (current)
			{
				delete current;
				current = (Graphic *)NULL;
			}

			/* Don't loop on the following line, because if they request graphic name 'zoro', but the 'highest' name
			 * is 'yoyo', this loop will never end, since we'll keep finding next graphic names, but none will have
			 * a 'higher' name (or equal to) to break out of the loop.
			 */
			current = nextGraphic(false);
		}
		if (foundBeginFrom)
			return current;
	}
	return (Graphic *)NULL;
}


Graphic *AbstractGraphicManager::nextGraphic(bool shouldLoop, const HobString &beginFromGraphicName)
{
	Graphic *fg = (Graphic *)NULL;
	if (!graphicFileNames)
		fg = firstGraphic();

	if (beginFromGraphicName.size())
	{
		Graphic *current = setPositionAtGraphic(beginFromGraphicName);
		if (current)
		{
			/* If the graphic found has a graphic name greater than the one requested,
			 * then this is the next one, go ahead and return it.
			 */
			std::string name = current->getGraphicNameBase();
			if (name > beginFromGraphicName)
				return current;
			else
				// Found the one requested; delete it so we can go to the next one.
				delete current;
		}
		else
		{
			if (shouldLoop)
				return firstGraphic();
			else
				return (Graphic *)NULL;
		}
	}
	else if (fg)
		return fg;

	Graphic *graphic = (Graphic *)NULL;
	HobString graphicName("");
	int found = graphicFileNames->next(graphicName);
	if (found < 0)
	{
		if (shouldLoop)
			return firstGraphic();
		else
		{
			/* reclaim the memory from the query and the iterator used to traverse all LabelTypes in the db. */
			delete graphicFileNames;
			graphicFileNames = (GraphicFileNames *)NULL;

			return (Graphic *)NULL;
		}
	}

	graphic = new Graphic(graphicName, currentType);
	return graphic;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstLabelType() method, the first call to this method will
 * result in the first record being returned.
 */
Graphic *AbstractGraphicManager::previousGraphic(bool shouldLoop, const HobString &beginFromGraphicName)
{
	Graphic *fg = (Graphic *)NULL;
	if (!graphicFileNames)
		fg = firstGraphic();

	if (beginFromGraphicName.size())
	{
		Graphic *current = setPositionAtGraphic(beginFromGraphicName);
		if (current)
				delete current;
		else
			/* If we didn't find one from the setPositionAt...(), then we went past the end before finding
			 * the one requested, so return the last one.
			 */
			return lastGraphic();
	}
	else if (fg)
		return fg;

	Graphic *graphic = (Graphic *)NULL;
	HobString graphicName("");
	int found = graphicFileNames->previous(graphicName);
	if (found < 0)
	{
		if (shouldLoop)
			return lastGraphic();
		else
		{
			/* reclaim the memory from the query and the iterator used to traverse all Graphics. */
			delete graphicFileNames;
			graphicFileNames = (GraphicFileNames *)NULL;

			return (Graphic *)NULL;
		}
	}

	graphic = new Graphic(graphicName, currentType);
	return graphic;
}


int AbstractGraphicManager::countAllGraphics()
{
	return countAllGraphics(currentType);
}

int AbstractGraphicManager::countAllGraphics(short graphicType)
{
	HobString myDebug;
	bool isTracing = false;
	int count = 0;
	int tempCount = 0;
	HobString myExtensions;
	Filename myFilename, myFilename2;

HobDateTime t1, t2;
//struct timespec tp1, tp2;
#if defined(LINUX_FIX)
// trackTiming: unused variable
#else
bool trackTiming = Debug::isTracing("GraphicManager.Timing");
#endif

	Configuration *config = Configuration::getInstance();

	if (Debug::isTracing("GraphicManager"))
	{
		isTracing = true;
		myDebug = "countAllGraphics(";
		myDebug.append(Integer::toString(graphicType)).append("):  ");
	}

	string systemUserRootDirectory = config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR);

	switch (graphicType)
	{
		case Graphic::TYPE_LABELGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
			myFilename.setPath(config->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
			myFilename2.setPath(config->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_USERLABELGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
			myFilename.setPath(config->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_ROMLABELGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::GRAPHICSEXTENSIONS);
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMGRAPHICSLOCATION));
			break;
		case Graphic::TYPE_SPECIALSGRAPHIC:
			myExtensions = config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION);
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Specials graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;

		case Graphic::TYPE_PRODUCTGRAPHIC:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Product graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;
		case Graphic::TYPE_CATEGORYGRAPHIC:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Category graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;
		case Graphic::TYPE_STREAMINGGRAPHIC:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find SlideShow graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;
		case Graphic::TYPE_SELFSERVEGRAPHIC:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Self Service graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;

		case Graphic::TYPE_SOFTKEYGRAPHIC:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Softkey graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;

		case Graphic::TYPE_PLANOGRAMGRAPHIC:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Planogram graphics in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;

		case Graphic::TYPE_VIDEOS:
			myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			myFilename.appendPath(config->toStringCfgI(CfgInternal::VIDEOSDIR));
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("will find Videos in: ").append(myFilename.getPath());
				Debug::trace("GraphicManager", myDebug2);
			}
			break;

		default:
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("Invalid graphicType!!!");
				Debug::trace("GraphicManager", myDebug2);
			}
			return count;
			break;
	}

	Directory userDir(myFilename);

	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("userDir = ").append(userDir.getFullPath());
		Debug::trace("GraphicManager", myDebug2);
	}

	if (myExtensions.empty())
	{

		//	Get the count of all files in the directory (They should be graphics only!)
		count = (int)userDir.selectAllFiles();
	}
	else
	{
		HobString extensions(myExtensions);
		HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);
		HobString token;

		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("extensions = ").append(extensions);
			Debug::trace("GraphicManager", myDebug2);
		}

		//	Get the list of Graphics in the directory
		int result = extensionsTok.first(&token);
		if (result != -1)
			myFilename.setFileNameExtension(token);
		while (result != -1)
		{
			token.insert(0, "*");		//	In order to get all files with this extension (e.g. "*.png").
			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("token = ").append(token);
				Debug::trace("GraphicManager", myDebug2);
			}
			tempCount = (int)userDir.select(token);

			if (isTracing)
			{
				HobString myDebug2(myDebug);
				myDebug2.append("Count of ").append(token).append(" = ").append(Integer::toString(tempCount));
				Debug::trace("GraphicManager", myDebug2);
			}
			count += tempCount;
			result = extensionsTok.next(&token);
		}

		if (graphicType == Graphic::TYPE_LABELGRAPHIC)
		{
			//Reset the list of extensions (i.e. get a new one), otherwise, we wouldn't get the list of graphics below.
			HobTokenizerCSV extensionsTok(&extensions, BrowserFormatSeparator);
			Directory systemDir(myFilename2);

			int result = extensionsTok.first(&token);
			if (result != -1)
				myFilename2.setFileNameExtension(token);
			while (result != -1)
			{
				token.insert(0, "*");		//	In order to get all files with this extension (e.g. "*.png").
				tempCount = (int)systemDir.select(token);

				if (isTracing)
				{
					HobString myDebug2(myDebug);
					myDebug2.append("Count of ").append(token).append(" = ").append(Integer::toString(tempCount));
					Debug::trace("GraphicManager", myDebug2);
				}
				count += tempCount;
				result = extensionsTok.next(&token);
			}
		}
	}
	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("Total count = ").append(Integer::toString(count));
		Debug::trace("GraphicManager", myDebug2);
	}
	return count;
}


void AbstractGraphicManager::handleMessage(SessionMessage *msg)
{
	handleGraphicMessage(msg);
}


void AbstractGraphicManager::handleRead(SessionMessage *msg)
{
	if (!mySession || !msg)
		return;

	const Graphic *graphic = dynamic_cast<const Graphic *> (msg->getHfcObject());
	if (!graphic)
		mySession->addDialogMessage(ErrGraphicDev, GraphicErr, HfcResponse::RESPONSESTYLE_OK);
	else
	{
		Graphic *lookedUpGraphic = Graphic::findGraphic(graphic->getGraphicName());
		if (!lookedUpGraphic)
			mySession->addDialogMessage(ErrGraphicDev, GraphicErr, HfcResponse::RESPONSESTYLE_OK);
		else
		{
			if (lookedUpGraphic->getErrorCode() != RECORD_OK)
			{
				// Send error message to browser
				mySession->addDialogMessage(ErrGraphicDev, GraphicErr, HfcResponse::RESPONSESTYLE_OK);
			}
			else
			{
				// We have a new Graphic.
				// Send the Graphic found back to the Browser.
				SessionMessage graphicData(GraphicDev, DtWrite);
				graphicData.setFormatType(BrowserFormat);
				graphicData.setHfcObject(lookedUpGraphic);

				// Send the Graphic to the UI (Browser)
				mySession->send(&graphicData, Messenger::DEST_UI);
			}
			delete lookedUpGraphic;
		}
	}
}


bool AbstractGraphicManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return false;

	Graphic *deleteGraphic = dynamic_cast<Graphic *> (msg->getHfcObject());
	if (!deleteGraphic)
		return false;

	bool rv = false;
	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(myHFCRecordManagerType))
	{
		// if received via comm, batch the record being deleted; don't actually delete it from the database right now.
		std::string synchSer = "";
		serializeForSynchronization(synchSer, deleteGraphic, getWriteEvent());

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
		rv = true;
	}
	else
	{
		rv = deleteGraphic->deleteRecord();
		if (rv)
			Debug::trace("GraphicManager", HobString("handleDelete - deleted graphic: ") + deleteGraphic->getGraphicFile().getFullPath());
		if (msg->getEventType() != DtBatchApplyDelete)
		{
			// if received via UI, delete it and batch the record being deleted.
			if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(myHFCRecordManagerType))
			{
				std::string synchSer = "";
				serializeForSynchronization(synchSer, deleteGraphic, msg->getEventType());

				synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
				
				if (proxy)
				{
					proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), msg->getEventType(), synchSer)));
				}
			}
		}
	}
	return rv;
}

/*
bool ProductGraphicManager::handleDeleteAll(short graphicType)
{
	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(myHFCRecordManagerType))
	{
		// batch the delete all command; don't actually do it right now.

		UINT8 deleteAllEvent = 0;
		switch (graphicType)
		{
			case Graphic::TYPE_USERLABELGRAPHIC:
				deleteAllEvent = DtDeleteAll;
				break;
			case Graphic::TYPE_PRODUCTGRAPHIC:
				deleteAllEvent = DeleteAllProductGraphics;
				break;
			case Graphic::TYPE_CATEGORYGRAPHIC:
				deleteAllEvent = DeleteAllCategoryGraphics;
				break;
			case Graphic::TYPE_SPECIALSGRAPHIC:
				deleteAllEvent = DeleteAllSpecialsGraphics;
				break;
			case Graphic::TYPE_STREAMINGGRAPHIC:
				deleteAllEvent = DeleteAllStreamingGraphics;
				break;
			case Graphic::TYPE_SOFTKEYGRAPHIC:
				deleteAllEvent = DeleteAllSoftkeyGraphics;
				break;
			default:
				break;
		}
		if (deleteAllEvent)
		{
			SessionMessage message(getDevice(), deleteAllEvent);
			message.setFormatType(SynchronizationTaggedFormat);
			const char * buffer = message.serialize();

			Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), deleteAllEvent, std::string(buffer, message.getSerializedLength())));
			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
			if ( proxy )
				proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);

			rv = true;
		}
	}
	else
	{
		Filename gFilename;
		gFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));

		switch (graphicType)
		{
			case Graphic::TYPE_STREAMINGGRAPHIC:
			{
				gFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR));
				Directory dir(gFilename);
				Debug::trace("GraphicManager", HobString("DeleteAllStreamingGraphics - will delete all in: ") + dir.getFullPath());
				if (dir.exists())
				{
					rv = dir.removeContents(true);  // true is to remove all files recursively
					if (rv)
						Debug::trace("GraphicManager", HobString("DeleteAllStreamingGraphics - successfully deleted all in: ") + dir.getFullPath());
				}
				break;
			}

			default:
				break;
		}
	}
	return rv;
}
*/

bool AbstractGraphicManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return false;

	Graphic *restoreGraphic = dynamic_cast<Graphic *> (msg->getHfcObject());
	if (!restoreGraphic)
		return false;

	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(myHFCRecordManagerType))
	{
		// batch the record being restored; don't actually write it to the database right now.
		// Because graphic images can be greater than 10000 bytes, and 10000 is the maximum number of bytes stored in the originalBuff of SessionMessage,
		// we have to reserialize the graphic itself.
		std::string synchSer = "";
		serializeForSynchronization(synchSer, restoreGraphic, getWriteEvent());

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(msg->getDeviceType(), msg->getEventType(), synchSer));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
		rv = true;
	}
	else
	{
		int type = restoreGraphic->getType();
		rv = restoreGraphic->saveRecord();
		if (rv)
		{
			if (type == Graphic::TYPE_USERLABELGRAPHIC)
			{
				HobString storeGraphicName = Configuration::getInstance()->toStringCfg(fieldOffsetCfgStoreGraphic);
				HobString newGraphicName = restoreGraphic->getGraphicName();
				Debug::trace("GraphicManager.handleRestore", HobString(" storeGraphicName = ") + storeGraphicName + HobString(" newGraphicName = ") + newGraphicName);
				if (newGraphicName == storeGraphicName)
				{
					Debug::trace("GraphicManager.handleRestore", HobString(" Reloading the store graphic."));

					//	Reload the store graphic because the graphic itself may be different.
					Configuration::getInstance()->loadStoreGraphic();
				}
			}
		}
	}

	return rv;
}


void AbstractGraphicManager::handleReadAllGraphicNamesPerType(UINT8 myEvent)
{
	Debug::trace("GraphicManager", "AbstractGraphicManager - handleReadAllGraphicNamesPerType()");
	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames(currentType);
	if (gfn)
	{
		//	Send the Graphic file names found back to the Browser,
		//	whether we have any or not.
		SessionMessage nameData(GraphicDev, myEvent);
		nameData.setFormatType(BrowserFormat);
		nameData.setHfcObject(gfn);

		mySession->send(&nameData, Messenger::DEST_UI);
		delete gfn;
	}
}
