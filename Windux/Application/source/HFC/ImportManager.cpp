/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "evolution.h"

#include "ImportManager.h"
#include "GraphicManager.h"
#include "Session.h"
#include "SessionMessage.h"
#include "ImportSelections.h"
#include "ScaleModel.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "BackupRestoreEvent.h"
#include "Bool.h"
#include "HobTokenizerCSV.h"
#include "ImportEvent.h"
#include "SessionManager.h"
#include "Session.h"

#include "filesys/directory.h"
#include "filesys/filename.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ImportManager::dataType = 0;
std::string ImportManager::dataTypeName("ImportManager"); 
#endif

ImportManager::ImportManager(Session *session, SessionMessage *pMsg, bool popDialogs)
	: graphicManager_(new GraphicManager()), pMySession_(session), directoryPath_(""),
		popDialogs_(popDialogs), moveFile_(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager", HobString("ImportManager(") + util_string::ptrtostring(this) + "): (1)  pMySession_ = " + util_string::ptrtostring(pMySession_));
#endif
	if (pMsg)
	{
		ImportSelections * pIs = dynamic_cast<ImportSelections *>(pMsg->getHfcObject());

		if (pIs)
		{
#ifndef EVOL_NDEBUG
			Debug::trace("ImportManager", HobString("ImportManager(constructor):  Assigning path = ") + pIs->getDirName());
#endif
			directoryPath_ = pIs->getDirName();
			moveFile_ = pIs->getMoveFile();
		}
	}

	if (pMySession_)
		init();
}

ImportManager::ImportManager(const std::string & path, bool popDialogs) :  graphicManager_(new GraphicManager()), pMySession_(0), directoryPath_(""), popDialogs_(popDialogs)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager", HobString("ImportManager(") + util_string::ptrtostring(this) + "): (2)  pMySession_ = " + util_string::ptrtostring(pMySession_));
#endif

	if (path.size() > 0)
	{
		directoryPath_ = path;
	}

	pMySession_ = SessionManager::getInstance()->getPrimaryConsoleSession();
	if (pMySession_)
		init();
}

void ImportManager::init()
{
	if (pMySession_)
	{
		util_filesystem::Directory d(directoryPath_);
		if (d.exists())
		{
			if (popDialogs_)
				pMySession_->getResponseMessageManager()->addMessage(ErrImportDev, ImportStartGraphics, HfcResponse::RESPONSESTYLE_NOINPUT);

#ifndef EVOL_NDEBUG
			Debug::trace("ImportManager", HobString("Directory exists. Attempt to import graphics."));
#endif
			checkingThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "ImportManager Thread" );			
		}
		else
		{
#ifndef EVOL_NDEBUG
			Debug::trace("ImportManager", HobString("Directory does not exist. Dispatch terminate message."));
#endif
			EvolMessage done(ImportDev, ImportTerminate);
			dispatchEvent(&done);

			if (popDialogs_)
				pMySession_->getResponseMessageManager()->addMessage(ErrImportDev, ImportSrcDirDoesNotExist, HfcResponse::RESPONSESTYLE_OK);
		}
	}
}

ImportManager::~ImportManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager", HobString("~ImportManager(") + util_string::ptrtostring(this) + "):  pMySession_ = " + util_string::ptrtostring(pMySession_));
#endif

	if (checkingThread)
	{
		checkingThread->StopThread();
		checkingThread->WaitForCompletion();
	}
	ShutdownNicely();
	
	if (graphicManager_)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("ImportManager", HobString("~ImportManager():  Deleting graphic manager "));
#endif
		delete graphicManager_;
		graphicManager_ = NULL;
	}
#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager", HobString("~ImportManager(") + util_string::ptrtostring(this) + "):  Goodbye");
#endif
}


void ImportManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & UNUSED(msg) )
{
#ifndef EVOL_NDEBUG
		Debug::trace("ImportManager", HobString("ImportManager::processEvent()  - Doing nothing."));
#endif
}

DWORD ImportManager::Run()
{
	/*declare all local objects inside the following brackets.
	  this is to avoid a memory leak when the import manager is
	  deleted.*/
	try
	{
		handleImportAllGraphics();

		if (popDialogs_)
		{
			pMySession_->getResponseMessageManager()->removeMessage(ErrImportDev, ImportStartGraphics, HfcResponse::RESPONSESTYLE_NOINPUT);
			pMySession_->getResponseMessageManager()->addMessage(ErrImportDev, ImportComplete, HfcResponse::RESPONSESTYLE_OK);
		}

		EvolMessage done(ImportDev, ImportTerminate);
		dispatchEvent(&done);
	}
	catch(Util::CTerminateNow)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Exception", std::string("ImportManager::Run():   CTerminateNow"));
#endif
		return 1;
	}
	catch(...)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("Exception", std::string("ImportManager::Run()"));
#endif
		LogCatch;
		return -1;
	}
#ifndef EVOL_NDEBUG
		Debug::trace("ImportManager", HobString("ImportManager::Run()  - Done."));
#endif
	return 1;
}

void ImportManager::handleImportAllGraphics()
{
	ScaleModel sm = Configuration::getInstance()->getScaleModel();

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Importing graphics from: ") + directoryPath_);
	Debug::trace("ImportManager.ImportGraphics", HobString("Import Softkey graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_SOFTKEYGRAPHIC, directoryPath_, moveFile_);

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Import Specials graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_SPECIALSGRAPHIC, directoryPath_, moveFile_);

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Import Product graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_PRODUCTGRAPHIC, directoryPath_, moveFile_);

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Import Category graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_CATEGORYGRAPHIC, directoryPath_, moveFile_);

	if (sm == ScaleModel_ESS)
	{
#ifndef EVOL_NDEBUG
		Debug::trace("ImportManager.ImportGraphics", HobString("Import Streaming graphics."));
#endif
		graphicManager_->importGraphicType(Graphic::TYPE_STREAMINGGRAPHIC, directoryPath_, moveFile_);

#ifndef EVOL_NDEBUG
		Debug::trace("ImportManager.ImportGraphics", HobString("Import Self serve graphics."));
#endif
		graphicManager_->importGraphicType(Graphic::TYPE_SELFSERVEGRAPHIC, directoryPath_, moveFile_);
	}

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Import User graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_USERLABELGRAPHIC, directoryPath_, moveFile_);

#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Import System graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_ROMLABELGRAPHIC, directoryPath_, moveFile_);


#ifndef EVOL_NDEBUG
	Debug::trace("ImportManager.ImportGraphics", HobString("Import Planogram graphics."));
#endif
	graphicManager_->importGraphicType(Graphic::TYPE_PLANOGRAMGRAPHIC, directoryPath_, moveFile_);
}
