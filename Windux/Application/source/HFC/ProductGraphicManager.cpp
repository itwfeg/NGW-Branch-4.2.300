/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductGraphicManager.h"
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

int ProductGraphicManager::dataType = 0;
std::string ProductGraphicManager::dataTypeName("ProductGraphicManager"); 
#endif


ProductGraphicManager::ProductGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

ProductGraphicManager::ProductGraphicManager(hfc::HFCManagerContainer *container) : AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mgrContainer_ = container;
}

ProductGraphicManager::~ProductGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// private
void ProductGraphicManager::init()
{
	currentType = Graphic::TYPE_PRODUCTGRAPHIC;
	myHFCRecordManagerType = hfc::HFCManagerContainer::ProductGraphics;
}


bool ProductGraphicManager::handleGraphicMessage(SessionMessage *msg)
{
	bool rv = false;

	// Valid as long as no process calls this method with a message containing
	// some other device than GraphicDev.
	if (!msg)
		return rv;

	switch (msg->getEventType())
	{
		case DtRead:
			Debug::trace("GraphicManager", "handling ProductGraphicManager DtRead event");
			handleRead(msg);
			break;

		//case DtDelete:
		//case DtBatchApplyDelete:
		case DeleteProductGraphic:
		case BatchApplyDeleteProductGraphic:
			Debug::trace("GraphicManager", "handling ProductGraphicManager DtDelete event");
			rv = handleDelete(msg);
			break;

		//case DtDeleteAll:
		case DeleteAllProductGraphics:
			Debug::trace("GraphicManager", "handling ProductGraphicManager DtDeleteAll event");
			handleDeleteAll();
			break;

		//case DtRestore:
		case RestoreProductGraphic:
			rv = handleRestore(msg);
			break;

		case ReadAllProductGraphicNames:
			handleReadAllGraphicNamesPerType(msg->getEventType());
			break;

		default:
			break;
	}

	return rv;
}


void ProductGraphicManager::handleDeleteAll()
{
	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::ProductGraphics))
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
		gFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
		Directory dir(gFilename);
		Debug::trace("GraphicManager", HobString("ProductGraphicManager::handleDeleteAll - will delete all in: ") + dir.getFullPath());
		if (dir.exists())
		{
			rv = dir.removeContents(true);  // true is to remove all files recursively
			if (rv)
				Debug::trace("GraphicManager", HobString("ProductGraphicManager::handleDeleteAll - successfully deleted all in: ") + dir.getFullPath());
		}
	}
	//return rv;
}
