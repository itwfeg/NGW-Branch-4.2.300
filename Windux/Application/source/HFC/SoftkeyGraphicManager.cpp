/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyGraphicManager.h"
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

int SoftkeyGraphicManager::dataType = 0;
std::string SoftkeyGraphicManager::dataTypeName("SoftkeyGraphicManager"); 
#endif


SoftkeyGraphicManager::SoftkeyGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SoftkeyGraphicManager::SoftkeyGraphicManager(hfc::HFCManagerContainer *container) : AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mgrContainer_ = container;
}

SoftkeyGraphicManager::~SoftkeyGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// private
void SoftkeyGraphicManager::init()
{
	currentType = Graphic::TYPE_SOFTKEYGRAPHIC;
	myHFCRecordManagerType = hfc::HFCManagerContainer::SoftkeyGraphics;
}


bool SoftkeyGraphicManager::handleGraphicMessage(SessionMessage *msg)
{
	bool rv = false;

	// Valid as long as no process calls this method with a message containing
	// some other device than GraphicDev.
	if (!msg)
		return rv;

	switch (msg->getEventType())
	{
		case DtRead:
			Debug::trace("GraphicManager", "handling SoftkeyGraphicManager DtRead event");
			handleRead(msg);
			break;

		//case DtDelete:
		//case DtBatchApplyDelete:
		case DeleteSoftkeyGraphic:
		case BatchApplyDeleteSoftkeyGraphic:
			Debug::trace("GraphicManager", "handling SoftkeyGraphicManager DtDelete event");
			rv = handleDelete(msg);
			break;

		//case DtDeleteAll:
		case DeleteAllSoftkeyGraphics:
			Debug::trace("GraphicManager", "handling SoftkeyGraphicManager DtDeleteAll event");
			handleDeleteAll();
			break;

		//case DtRestore:
		case RestoreSoftkeyGraphic:
			rv = handleRestore(msg);
			break;

		case ReadAllSoftkeyGraphicNames:
			handleReadAllGraphicNamesPerType(msg->getEventType());
			break;

		default:
			break;
	}

	return rv;
}


void SoftkeyGraphicManager::handleDeleteAll()
{
	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SoftkeyGraphics))
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

		gFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
		Directory targetDir(gFilename);
		Debug::trace("GraphicManager", HobString("DeleteAllSoftkeyGraphics - will delete all in: ") + targetDir.getFullPath());
		if (targetDir.exists())
		{
			rv = targetDir.removeContents(true);  // true is to remove all files recursively
			if (rv)
				Debug::trace("GraphicManager", HobString("DeleteAllSoftkeyGraphics - successfully deleted all in: ") + targetDir.getFullPath());
		}
		else
		{
			targetDir.create();
		}
		Directory srcDir (Configuration::getInstance()->toStringCfgI(CfgInternal::RESERVEDSOFTKEYGRAPHICSDIR));
		Debug::trace("GraphicManager", HobString("copy reserved softkey graphics from ") + srcDir.getFullPath());

		// FT-438: copy contents, not directory
		targetDir << contents << srcDir;
	}
	//return rv;
}
