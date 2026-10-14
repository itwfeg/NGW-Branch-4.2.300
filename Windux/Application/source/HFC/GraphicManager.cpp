/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "GraphicManager.h"
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

#include "LabelGraphicManager.h"
#include "CategoryGraphicManager.h"
#include "ProductGraphicManager.h"
#include "SoftkeyGraphicManager.h"
#include "SpecialsGraphicManager.h"
#include "StreamingGraphicManager.h"
#include "PlanogramGraphicManager.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Bool.h"
#include "CZip.h"

#include "MessageLogger.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GraphicManager::dataType = 0;
std::string GraphicManager::dataTypeName("GraphicManager"); 
#endif


GraphicManager::GraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

GraphicManager::GraphicManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mgrContainer_ = container;
}

GraphicManager::~GraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// private
void GraphicManager::init()
{
	mySession = 0;

	currentType = Graphic::TYPE_UNKNOWN;
}


DBSession *GraphicManager::getMySession()
{
	return mySession;
}
void GraphicManager::setMySession(DBSession *session)
{
	mySession = session;
}


void GraphicManager::handleMessage(SessionMessage *msg)
{
	handleGraphicMessage(msg);
}

bool GraphicManager::handleGraphicMessage(SessionMessage *msg)
{
	bool rv = false;

	// Valid as long as no process calls this method with a message containing
	// some other device than GraphicDev.
	if (!msg)
		return rv;

	switch (msg->getEventType())
	{
		case DtRead:
		case DtDelete:
		case DtBatchApplyDelete:
		case DtDeleteAll:
		case DtRestore:
		case DtFirst:
		case DtLast:
		case DtNext:
		case DtPrevious:
		case GraphicReadAllNames:
			mgrContainer_->getLabelGraphicManager()->handleMessage(msg);
			break;

		case DeleteProductGraphic:
		case DeleteAllProductGraphics:
		case RestoreProductGraphic:
		case ReadAllProductGraphicNames:
			mgrContainer_->getProductGraphicManager()->handleMessage(msg);
			break;

		case DeleteCategoryGraphic:
		case DeleteAllCategoryGraphics:
		case RestoreCategoryGraphic:
		case ReadAllCategoryGraphicNames:
			mgrContainer_->getCategoryGraphicManager()->handleMessage(msg);
			break;

		case DeleteSpecialsGraphic:
		case DeleteAllSpecialsGraphics:
		case RestoreSpecialsGraphic:
		case ReadAllSpecialsGraphicNames:
			mgrContainer_->getSpecialsGraphicManager()->handleMessage(msg);
			break;

		case DeleteStreamingGraphic:
		case DeleteAllStreamingGraphics:
		case RestoreStreamingGraphic:
		case ReadAllSelfServeGraphicNames:
			mgrContainer_->getStreamingGraphicManager()->handleMessage(msg);
			break;

		case DeleteSoftkeyGraphic:
		case DeleteAllSoftkeyGraphics:
		case RestoreSoftkeyGraphic:
		case ReadAllSoftkeyGraphicNames:
			mgrContainer_->getSoftkeyGraphicManager()->handleMessage(msg);
			break;

		case DeletePlanogramGraphic:
		case DeleteAllPlanogramGraphics:
		case RestorePlanogramGraphic:
		case ReadAllPlanogramGraphicNames:
		case NewPlanogramReceived:
		case ReadMostRecentPlanogramGraphic:
		case AcknowledgeMostRecentPlanogramGraphic:
		case CheckForNewPlanogramReceived:
			mgrContainer_->getPlanogramGraphicManager()->handleMessage(msg);
			break;

		case ReadAllVideoNames:
			handleReadAllGraphicNamesPerType(msg->getEventType(),Graphic::TYPE_VIDEOS);
			break;

		default:
			break;
	}

	return rv;
}



/*****************************************************************************************/
/*this function copies all graphics from a specified directory from a thumb drive.
  The graphics on the thumbdrive must have the exact same full path as that of the corresponding
  directory on the ramdDrive. In other words, directory e:/tmp/webpages/images/categories will
  be copied to c:/???/webpages/images/categories. The destination directory will not be erased first.
  files with same names will be overwritten. new files will be added if they do not exist in
  the destination directory.*/
void GraphicManager::importGraphicType(short graphicType, const std::string& baseSrcDir, bool moveFile)
{
	bool copiedOne = false;

	GraphicFileNames skGraphics(graphicType, Graphic::SUBTYPE_USER, &baseSrcDir);  // The subtype doesn't matter in this case, since we're specifying the directory (but can't be SUBTYPE_SYSTEM)

	HobString graphicName("");
	int found = skGraphics.first(graphicName);
	if (found >= 0)
	{
		copiedOne = true;

		do
		{
			File srcFile = skGraphics.getMyFilename();

			srcFile.setFileName(graphicName);

#ifndef EVOL_NDEBUG
			HobString whichStr;
			if (moveFile)
				whichStr = "Mov";
			else
				whichStr = "Copy";
			Debug::trace("GraphicManager.Import", whichStr + HobString("ing [") + srcFile.getFullPath() + "]");
#endif

			Directory destDir(skGraphics.getDestFilename());
			if (!destDir.exists())
			{
#ifndef EVOL_NDEBUG
				Debug::trace("GraphicManager.Import", HobString("Creating graphics dir on ramdrive"));
#endif
				destDir.create();
			}

			File destFile(destDir.getFullPath(), graphicName);

#ifndef EVOL_NDEBUG
			Debug::trace("GraphicManager.Import", HobString("to [") + destFile.getFullPath() + "]");
#endif

//			destFile << srcFile;
			bool result;
			if (moveFile)
				result = srcFile.moveTo(destFile);
			else
				result = srcFile.copyTo(destFile);
#ifndef EVOL_NDEBUG
			if (moveFile)
				whichStr.append("e");
			whichStr.append(" was ");
			if (result == false)
				whichStr.append("NOT ");
			whichStr.append("successful.");
			Debug::trace("GraphicManager.Import", whichStr);
#endif

			string msg("Imported graphic: ");
			msg.append(destFile.getFullPath());
			MessageLogger::logMessage(MessageLogger::ScaleMaintenanceLog, msg);

			found = skGraphics.next(graphicName);
		}
		while (found >= 0);
	}
}


void GraphicManager::copyGraphicFile(const HobString &fileNamePath, const HobString &destDir, short /* graphicType */)
{
	Directory moveToDir(destDir);
	File srcFile(fileNamePath);
	Debug::trace("GraphicManager", HobString("    copyGraphicFile():  Copy file: ") + destDir + HobString(" << ") + fileNamePath);
	moveToDir << srcFile;
}


void GraphicManager::moveGraphicFile(const HobString &fileNamePath, const HobString &destDir, short /* graphicType */)
{
	Directory moveToDir(destDir);
	File srcFile(fileNamePath);
	Debug::trace("GraphicManager", HobString("    moveGraphicFile():  Copy file: ") + destDir + HobString(" << ") + fileNamePath);
	moveToDir << srcFile;

	if (srcFile.exists())
		srcFile.remove();
}

void GraphicManager::handleReadAllGraphicNamesPerType(UINT8 myEvent, short type)
{
	Debug::trace("GraphicManager", "GraphicManager - handleReadAllGraphicNamesPerType()");
	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames(type);
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
