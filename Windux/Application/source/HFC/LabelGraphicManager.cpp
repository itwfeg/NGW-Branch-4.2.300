/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "LabelGraphicManager.h"
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

int LabelGraphicManager::dataType = 0;
std::string LabelGraphicManager::dataTypeName("LabelGraphicManager"); 
#endif


LabelGraphicManager::LabelGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

LabelGraphicManager::LabelGraphicManager(hfc::HFCManagerContainer *container) : AbstractGraphicManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	mgrContainer_ = container;
}

LabelGraphicManager::~LabelGraphicManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

// private
void LabelGraphicManager::init()
{
	currentType = Graphic::TYPE_LABELGRAPHIC;
	myHFCRecordManagerType = hfc::HFCManagerContainer::LabelGraphics;
}


bool LabelGraphicManager::handleGraphicMessage(SessionMessage *msg)
{
	bool rv = false;

	// Valid as long as no process calls this method with a message containing
	// some other device than GraphicDev.
	if (!msg)
		return rv;

	switch (msg->getEventType())
	{
		case DtRead:
			Debug::trace("GraphicManager", "handling LabelGraphicManager DtRead event");
			handleRead(msg);
			break;

		case DtDelete:
		case DtBatchApplyDelete:
			Debug::trace("GraphicManager", "handling LabelGraphicManager DtDelete event");
			rv = handleDelete(msg);
			break;

		case DtDeleteAll:
			Debug::trace("GraphicManager", "handling LabelGraphicManager DtDeleteAll event");
			handleDeleteAll();
			break;

		case DtRestore:
			rv = handleRestore(msg);
			break;

		case DtFirst:
			Debug::trace("GraphicManager", "handling DtFirst event");
			handleFirst();
			break;

		case DtLast:
			Debug::trace("GraphicManager", "handling DtLast event");
			handleLast();
			break;

		case DtNext:
		{
			Debug::trace("GraphicManager", "handling DtNext event");
			Graphic *referenceGraphic = dynamic_cast<Graphic *> (msg->getHfcObject());
			handleNext(referenceGraphic);
			break;
		}

		case DtPrevious:
		{
			Debug::trace("GraphicManager", "handling DtPrevious event");
			Graphic *referenceGraphic = dynamic_cast<Graphic *> (msg->getHfcObject());
			handlePrevious(referenceGraphic);
			break;
		}

		case GraphicReadAllNames:
			Debug::trace("GraphicManager", "handling GraphicReadAllNames event");
			handleReadAllNames();
			break;

		default:
			break;
	}

	return rv;
}


int LabelGraphicManager::countAllCustomGraphics()
{
	return countAllGraphics(Graphic::TYPE_USERLABELGRAPHIC);
}


void LabelGraphicManager::handleDeleteAll()
{
	bool rv = false;

	if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::LabelGraphics))
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
		Directory dir("");
		dir.setFullPath(Configuration::getInstance()->toStringCfgI(CfgInternal::USERGRAPHICSLOCATION));
		Debug::trace("GraphicManager", HobString("LabelGraphicManager::handleDeleteAll - will delete all in: ") + dir.getFullPath());
		if (dir.exists())
		{
			rv = dir.removeContents(true);  // true is to remove all files recursively
			if (rv)
				Debug::trace("GraphicManager", HobString("LabelGraphicManager::handleDeleteAll - successfully deleted all in: ") + dir.getFullPath());
		}
	}
	//return rv;
}


void LabelGraphicManager::handleFirst()
{
	if (!mySession)
		return;

	Graphic *graphic = firstGraphic();
	if (!graphic)
		graphic = new Graphic();

	// Send the Graphic to the UI (Browser)
	SessionMessage gMsg(GraphicDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(graphic);
	mySession->send(&gMsg, Messenger::DEST_UI);

	if (graphic)
		delete graphic;
}

void LabelGraphicManager::handleLast()
{
	if (!mySession)
		return;

	Graphic *graphic = lastGraphic();
	if (!graphic)
		graphic = new Graphic();

	// Send the Graphic to the UI (Browser)
	SessionMessage gMsg(GraphicDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(graphic);
	mySession->send(&gMsg, Messenger::DEST_UI);

	if (graphic)
		delete graphic;
}

void LabelGraphicManager::handleNext(const Graphic *referenceGraphic)
{
	if (!mySession)
		return;

	Graphic *graphic = nextGraphic(true, (referenceGraphic) ? HobString(referenceGraphic->getGraphicName()) : HobString(""));
	if (!graphic)
		graphic = new Graphic();

	// Send the Graphic to the UI (Browser)
	SessionMessage gMsg(GraphicDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(graphic);
	mySession->send(&gMsg, Messenger::DEST_UI);

	if (graphic)
		delete graphic;
}

void LabelGraphicManager::handlePrevious(const Graphic *referenceGraphic)
{
	if (!mySession)
		return;

	Graphic *graphic = previousGraphic(true, (referenceGraphic) ? HobString(referenceGraphic->getGraphicName()) : HobString(""));
	if (!graphic)
		graphic = new Graphic();

	// Send the Graphic to the UI (Browser)
	SessionMessage gMsg(GraphicDev, DtWrite);
	gMsg.setFormatType(BrowserFormat);
	gMsg.setHfcObject(graphic);
	mySession->send(&gMsg, Messenger::DEST_UI);

	if (graphic)
		delete graphic;
}


void LabelGraphicManager::handleReadAllNames()
{
	if (!mySession)
		return;

	GraphicFileNames *gfn = new GraphicFileNames();
	if (gfn)
	{
		if (gfn->getListCount() > 0)
		{
			// Send the Graphic file names found back to the Browser.
			SessionMessage nameData(GraphicDev, GraphicReadAllNames);
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
