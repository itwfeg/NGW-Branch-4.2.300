/* Fastrack Scale Project
 * COPYRIGHT (c) 2013.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PlanogramGraphicManager.h"
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
#include "GenericHfcObject.h"

#include "MultiLanguageText.h"
#include "MessageLogger.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Integer.h"
#include "UInteger.h"
#include "Long.h"
#include "ULong.h"
#include "Bool.h"
#include "CZip.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PlanogramGraphicManager::dataType = 0;
std::string PlanogramGraphicManager::dataTypeName("PlanogramGraphicManager"); 
#endif


PlanogramGraphicManager::PlanogramGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

PlanogramGraphicManager::PlanogramGraphicManager(hfc::HFCManagerContainer *container) : AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mgrContainer_ = container;
}

PlanogramGraphicManager::~PlanogramGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// private
void PlanogramGraphicManager::init()
{
	currentType = Graphic::TYPE_PLANOGRAMGRAPHIC;
	myHFCRecordManagerType = hfc::HFCManagerContainer::PlanogramGraphics;
}

const string PlanogramGraphicManager::getFullPathToImages()
{
	util_filesystem::Filename tempFilename;
	tempFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
	tempFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
	return tempFilename.getFullPath();
}
const string PlanogramGraphicManager::getRelativePathToImages()
{
	util_filesystem::Filename tempFilename;
	tempFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
	return tempFilename.getFullPath();
}

bool PlanogramGraphicManager::handleGraphicMessage(SessionMessage *msg)
{
	bool rv = false;

	// Valid as long as no process calls this method with a message containing
	// some other device than GraphicDev.
	if (!msg)
		return rv;

	switch (msg->getEventType())
	{
		case DtRead:
			Debug::trace("GraphicManager", "handling PlanogramGraphicManager DtRead event");
			handleRead(msg);
			break;

		//case DtDelete:
		//case DtBatchApplyDelete:
		case DeletePlanogramGraphic:
		case BatchApplyDeletePlanogramGraphic:
			Debug::trace("GraphicManager", "handling PlanogramGraphicManager DtDelete event");
			rv = handleDelete(msg);
			break;

		//case DtDeleteAll:
		case DeleteAllPlanogramGraphics:
			Debug::trace("GraphicManager", "handling PlanogramGraphicManager DtDeleteAll event");
			handleDeleteAll();
			break;

		//case DtRestore:
		case RestorePlanogramGraphic:
			rv = handleRestore(msg);
			break;

		case ReadAllPlanogramGraphicNames:
			handleReadAllGraphicNamesPerType(msg->getEventType());
			break;

		case NewPlanogramReceived:
		case ReadMostRecentPlanogramGraphic:
		{
			// The most recent planogram is only the most recent until the user, on the primary console, acknowledges it (clears it).
			// So also check the fieldOffsetCfgMostRecentPlanogramAcknowledged flag in config.
			Configuration *config = Configuration::getInstance();
			if (config && mySession)
			{
				string planoName = "";
				if (!config->toBoolCfg(fieldOffsetCfgMostRecentPlanogramAcknowledged))
				{
					util_filesystem::Filename myFilename;
					// Currently, the UI does not want the path with the filename.
					//myFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
					//myFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
					myFilename.setFileName(config->toStringCfg(fieldOffsetCfgMostRecentPlanogramGraphic));
					planoName = myFilename.getFullPath();
				}

				SessionMessage mostRecentPlanogram(GraphicDev, WriteMostRecentPlanogramGraphic);
				mostRecentPlanogram.setFormatType(BrowserFormat);
				GenericHfcObject gho(planoName);
				mostRecentPlanogram.setHfcObject(&gho);
				mySession->send(&mostRecentPlanogram, Messenger::DEST_UI);
			}
			break;
		}

		case AcknowledgeMostRecentPlanogramGraphic:
		{
			// Set it to be acknowledged, but don't clear the name in config (fieldOffsetCfgMostRecentPlanogramGraphic).
			Configuration *config = Configuration::getInstance();
			if (config)
			{
				if (mySession && mySession->isPrimarySession() && !config->toBoolCfg(fieldOffsetCfgMostRecentPlanogramAcknowledged))
				{
					config->updateField(fieldOffsetCfgMostRecentPlanogramAcknowledged, "1");
					config->saveField(fieldOffsetCfgMostRecentPlanogramAcknowledged, mySession);
				}
			}
			break;
		}

		case CheckForNewPlanogramReceived:
		{
			handleCheckForNewPlanogramReceived();
			break;
		}

		default:
			break;
	}

	return rv;
}


void PlanogramGraphicManager::handleDeleteAll()
{
	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::PlanogramGraphics))
	{
		// batch the delete all command; don't actually do it right now.

		SessionMessage message(getDevice(), getDeleteAllEvent());
		message.setFormatType(SynchronizationTaggedFormat);
		const char * buffer = message.serialize();

		Util::CSmartPtr<hfc::SynchronizationChange> receivedChange(new hfc::SynchronizationChange(getDevice(), getDeleteAllEvent(), std::string(buffer, message.getSerializedLength())));
		synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
		if ( proxy )
			proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);

		rv = true;
	}
	else
	{
		Filename gFilename;
		gFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
		gFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
		Directory dir(gFilename);
		Debug::trace("GraphicManager", HobString("PlanogramGraphicManager::handleDeleteAll - will delete all in: ") + dir.getFullPath());
		if (dir.exists())
		{
			rv = dir.removeContents(true);  // true is to remove all files recursively
			if (rv)
				Debug::trace("GraphicManager", HobString("PlanogramGraphicManager::handleDeleteAll - successfully deleted all in: ") + dir.getFullPath());
		}
	}
	//return rv;
}


void PlanogramGraphicManager::handleCheckForNewPlanogramReceived()
{
	if (mySession)
	{
		Configuration *config = Configuration::getInstance();
		if (config && config->toBoolCfgI(CfgInternal::PLANOGRAMSENABLED))
		{
			Debug::trace("Planogram", "PlanogramGraphicManager::handleCheckForNewPlanogramReceived() start");

			GraphicFileNames graphicFileNames(Graphic::TYPE_PLANOGRAMGRAPHIC);
			graphicFileNames.findFileNames(Graphic::TYPE_PLANOGRAMGRAPHIC, Graphic::SUBTYPE_USERANDSYSTEM, 0, Directory::MODIFIED_SORT);
			HobString graphicName;
			int found = graphicFileNames.last(graphicName);
			if (found >= 0)
			{
				util_filesystem::Filename planoFilename;
				planoFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
				planoFilename.appendPath(config->toStringCfgI(CfgInternal::PLANOGRAMGRAPHICSDIR));
				planoFilename.setFileName(graphicName);

				File plano(planoFilename.getFullPath());
				unsigned int planoCRC = 0;
				if (plano.exists())
					planoCRC = plano.checksum();

				string planoName = plano.getFileName();

				if (config->toStringCfg(fieldOffsetCfgMostRecentPlanogramGraphic) != planoName ||
					config->toIntCfg(fieldOffsetCfgMostRecentPlanogramGraphicCRC) != planoCRC)
				{
					Debug::trace("Planogram", "Updating db config with newly detected planogram: " + planoName);

					config->updateField(fieldOffsetCfgMostRecentPlanogramGraphic, planoName);
					config->saveField(fieldOffsetCfgMostRecentPlanogramGraphic, mySession);

					config->updateField(fieldOffsetCfgMostRecentPlanogramGraphicCRC, UInteger::toString(planoCRC));
					config->saveField(fieldOffsetCfgMostRecentPlanogramGraphicCRC, mySession);

					config->updateField(fieldOffsetCfgMostRecentPlanogramAcknowledged, "0");
					config->saveField(fieldOffsetCfgMostRecentPlanogramAcknowledged, mySession);


					hfc::MultiLanguageText internationalize;
					MessageLogger::logMessage(MessageLogger::InfoLog, internationalize("logMessageNewPlanogram") + planoName);


					// Let all Sessions know that there is a new planogram so they can inform their UI.
					SessionMessage newPlanogramMsg(GraphicDev, NewPlanogramReceived);
					newPlanogramMsg.setFormatType(InternalFormat);
					mySession->send(&newPlanogramMsg, Messenger::DEST_RTC);

					if (mySession->isPrimarySession())
						// If the Session handling this message is the primary session, then the send above won't
						// go to the primary session, since sending internal messages don't go to the message sender,
						// so directly call the method to notify the UI of the new planogram
						handleGraphicMessage(&newPlanogramMsg);
				}
			}
			else if (config->toStringCfg(fieldOffsetCfgMostRecentPlanogramGraphic) != "")
			{
				Debug::trace("Planogram", "No planograms found, clearing database configs");

				config->updateField(fieldOffsetCfgMostRecentPlanogramGraphic, "");
				config->saveField(fieldOffsetCfgMostRecentPlanogramGraphic, mySession);

				config->updateField(fieldOffsetCfgMostRecentPlanogramGraphicCRC, "0");
				config->saveField(fieldOffsetCfgMostRecentPlanogramGraphicCRC, mySession);

				config->updateField(fieldOffsetCfgMostRecentPlanogramAcknowledged, "1");
				config->saveField(fieldOffsetCfgMostRecentPlanogramAcknowledged, mySession);

				// Let all Sessions know that there is a new planogram so they can inform their UI.
				SessionMessage newPlanogramMsg(GraphicDev, NewPlanogramReceived);
				newPlanogramMsg.setFormatType(InternalFormat);
				mySession->send(&newPlanogramMsg, Messenger::DEST_RTC);

				if (mySession->isPrimarySession())
					// If the Session handling this message is the primary session, then the send above won't
					// go to the primary session, since sending internal messages don't go to the message sender,
					// so directly call the method to notify the UI of the new planogram
					handleGraphicMessage(&newPlanogramMsg);
			}

			Debug::trace("Planogram", "PlanogramGraphicManager::handleCheckForNewPlanogramReceived() end");
		}  // PLANOGRAMSENABLED
	}
}