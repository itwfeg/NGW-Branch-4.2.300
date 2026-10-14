/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SpecialsGraphicManager.h"
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

int SpecialsGraphicManager::dataType = 0;
std::string SpecialsGraphicManager::dataTypeName("SpecialsGraphicManager"); 
#endif


SpecialsGraphicManager::SpecialsGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

SpecialsGraphicManager::SpecialsGraphicManager(hfc::HFCManagerContainer *container) : AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mgrContainer_ = container;
}

SpecialsGraphicManager::~SpecialsGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// private
void SpecialsGraphicManager::init()
{
	currentType = Graphic::TYPE_SPECIALSGRAPHIC;
	myHFCRecordManagerType = hfc::HFCManagerContainer::SpecialsGraphics;
}


bool SpecialsGraphicManager::handleGraphicMessage(SessionMessage *msg)
{
	bool rv = false;

	// Valid as long as no process calls this method with a message containing
	// some other device than GraphicDev.
	if (!msg)
		return rv;

	switch (msg->getEventType())
	{
		case DtRead:
			Debug::trace("GraphicManager", "handling SpecialsGraphicManager DtRead event");
			handleRead(msg);
			break;

		//case DtDelete:
		//case DtBatchApplyDelete:
		case DeleteSpecialsGraphic:
		case BatchApplyDeleteSpecialsGraphic:
			Debug::trace("GraphicManager", "handling SpecialsGraphicManager DtDelete event");
			rv = handleDelete(msg);
			break;

		//case DtDeleteAll:
		case DeleteAllSpecialsGraphics:
			Debug::trace("GraphicManager", "handling SpecialsGraphicManager DtDeleteAll event");
			handleDeleteAll();
			break;

		//case DtRestore:
		case RestoreSpecialsGraphic:
			rv = handleRestore(msg);
			break;

		case ReadAllSpecialsGraphicNames:
			handleReadAllGraphicNamesPerType(msg->getEventType());
			break;

		default:
			break;
	}

	return rv;
}


void SpecialsGraphicManager::handleDeleteAll()
{
	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::SpecialsGraphics))
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

		gFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
		Directory dir(gFilename);
		Debug::trace("GraphicManager", HobString("SpecialsGraphicManager::handleDeleteAll - will delete all in: ") + dir.getFullPath());
		if (dir.exists())
		{
			rv = dir.removeContents(true);  // true is to remove all files recursively
			if (rv)
				Debug::trace("GraphicManager", HobString("SpecialsGraphicManager::handleDeleteAll - successfully deleted all in: ") + dir.getFullPath());
		}
	}
	//return rv;
}
