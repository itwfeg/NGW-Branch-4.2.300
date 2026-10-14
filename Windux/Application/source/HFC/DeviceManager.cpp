/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DeviceManager.h"
#include "DeviceEvent.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"
#include "HfcUpdateStatus.h"

#include "SessionManager.h"
#include "ScaleSynchronizationType.h"
#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "CSmartPtr.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBDevicesRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DeviceManager::_dataType_ = 0;
std::string DeviceManager::_dataTypeName_("DeviceManager"); 
#endif


DeviceManager::DeviceManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

DeviceManager::DeviceManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}


DeviceManager::~DeviceManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	destroyQueriesAndIters();
}


void DeviceManager::init()
{
	mySession = 0;

	currentDevice = 0;

	allDevicesQuery = 0;
	allDevicesIter  = 0;
}


DBSession *DeviceManager::getMySession()
{
	return mySession;
}
void DeviceManager::setMySession(DBSession *session)
{
	mySession = session;
}


/* If the first record was not read, this method will return NULL.
 */
Device *DeviceManager::firstDevice()
{
	if (!dbAccess)
		return (Device *)NULL;

	DBDevicesRM *myRM = dbAccess->getDBDevicesRM();
	if (!myRM)
		return (Device *)NULL;

	// If the user calls firstDevice() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allDevicesQuery = myRM->retrieveAll();

	// Couldn't get the query from which to access the Devices, just return false.
	if (!allDevicesQuery)
		return (Device *)NULL;

	DBQuery<DBDevice>::const_iterator tempIter = allDevicesQuery->begin();
	if (tempIter == allDevicesQuery->end())
		return (Device *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextDevice()
	 * method call below).
	 */
	allDevicesIter = new DBQuery<DBDevice>::const_iterator(tempIter);
	DBDevice *d = new DBDevice(*(*allDevicesIter));
	Device *newRec = new Device(d, dbAccess);
	return newRec;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstDevice() method, the first call to this method will
 * result in the first record being returned.
 */
Device *DeviceManager::nextDevice()
{
	if (!dbAccess)
		return (Device *)NULL;

	if (allDevicesQuery && !allDevicesQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allDevicesQuery || !allDevicesIter)
		return firstDevice();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allDevicesIter);
	if (*allDevicesIter == allDevicesQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all records in the db. */
		destroyQueriesAndIters();
		return (Device *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBDevice copy constructor in order for the Hfc Device to
	 * control the destruction of the DBDevice record that the iterator contains; otherwise, the DBDevice will
	 * be deleted when the iterator goes out of scope.  The DBDevice copy will be deleted by the Device record
	 * created below.
	 */
	DBDevice *d = new DBDevice(*(*allDevicesIter));
	Device *newRec = new Device(d, dbAccess);
	return newRec;
}


/* Provide a countAll method so the caller doesn't need to know about a DBDevicesRM.
 */
int DeviceManager::countAllDevices()
{
	if (!dbAccess)
		return -1;

	DBDevicesRM *myRM = dbAccess->getDBDevicesRM();
	if (!myRM)
		return -1;

	return myRM->countAll();
}


Device *DeviceManager::getCurrentDevice()
{
	return currentDevice;
}
void DeviceManager::setCurrentDevice(Device *newDevice)
{
	if (currentDevice)
		delete currentDevice;
	currentDevice = newDevice;
}


void DeviceManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	Device *msgDevice = dynamic_cast<Device *> (msg->getHfcObject());
	UINT8 event = msg->getEventType();
	Debug::trace("DeviceManager", HobString("handling event: ") + HobString(Integer::toString(event)));
	switch (event)
	{
		case DtRead:
			handleRead(msgDevice);
			break;

		case DtWrite:
			handleWrite(msgDevice);
			break;

		case DtDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgDevice);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgDevice);
			break;

		default:
			Debug::trace("DeviceManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}


void DeviceManager::handleRead(Device *msgRec)
{
	if (!msgRec || !dbAccess)
		return;

	Device *lookedUpRec = Device::findDevice(msgRec->getDeviceType(), msgRec->getDeviceSubType(), dbAccess);

	if (!lookedUpRec || lookedUpRec->getErrorCode() != RECORD_OK)		//	This device does not exist
	{
			// Send error message to browser indicating Device not found.
		if (mySession) 
			mySession->addDialogMessage(ErrDeviceDev, DeviceErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpRec)
		{
			delete lookedUpRec;
			lookedUpRec = 0;
		}
		return;
	}

	// We have a new current Device.
	setCurrentDevice(lookedUpRec);
	Debug::trace("DeviceManager.Device", HobString("handleRead(): currentDevice = ") + HobString(Hex::toString((long long) currentDevice)));

	if (mySession)
	{
		// Send the Device found back to the Browser.
		SessionMessage deviceData(DeviceDev, DtWrite);
		deviceData.setFormatType(BrowserFormat);
		deviceData.setHfcObject(currentDevice);

		// Send the Device to the UI (Browser)
		mySession->send(&deviceData, Messenger::DEST_UI);
	}
}


void DeviceManager::handleWrite(Device *modifiedRec)
{
	if ((!modifiedRec) || (!currentDevice))
		return;

	if (Debug::isTracing("DeviceManager"))
		Debug::trace("DeviceManager", HobString("handleWrite(): currentDevice = ") + HobString(Hex::toString((long long) currentDevice)) +
									  HobString("  , modifiedDevice = ") + HobString(Hex::toString((long long) modifiedRec)));

	currentDevice->clearErrors();
  	currentDevice->updateDevice(modifiedRec);
	if (mySession && Configuration::getInstance()->toBoolCfgI(CfgInternal::NOTIFYUIOFUPDATESTATUS))
	{
		SessionMessage updateStatusMsg(DeviceDev, DtUpdateStatus);
		updateStatusMsg.setFormatType(BrowserFormat);
		HfcUpdateStatus stat(!currentDevice->hasErrors());
		updateStatusMsg.setHfcObject(&stat);
		mySession->send(&updateStatusMsg, Messenger::DEST_UI);
	}
}


void DeviceManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	Device *msgRec = dynamic_cast<Device *> (msg->getHfcObject());
	if (!msgRec || !dbAccess)
		return;

	Device *existingRec = Device::findDevice(msgRec->getDeviceType(), msgRec->getDeviceSubType(), dbAccess);
	if (existingRec)
	{
		existingRec->deleteRecord();
		Debug::trace("DeviceManager", string("handleDelete - Device deleted - deviceType:") + Integer::toString(msgRec->getDeviceType()) + string("  subType:") + Integer::toString(msgRec->getDeviceSubType()));
		delete existingRec;
		existingRec = 0;
	}
	else
		Debug::trace("DeviceManager", HobString("handleDelete - Device to be deleted was not in the database to delete - deviceType:") + Integer::toString(msgRec->getDeviceType()) + string("  subType:") + Integer::toString(msgRec->getDeviceSubType()));
}


void DeviceManager::handleDeleteAll()
{
	Debug::trace("DeviceManager", "handleDeleteAll - deleting all recalls");

	if (!dbAccess)
		return;

	DBDevicesRM *myRM = dbAccess->getDBDevicesRM();
	if (!myRM)
		return;

	myRM->deleteAll();
	destroyQueriesAndIters();

	Debug::trace("DeviceManager", "handleDeleteAll - leaving");
}


void DeviceManager::handleUpdateAndSave(Device *modifiedRec)
{
	Debug::trace("DeviceManager", HobString("handleUpdateAndSave(): currentDevice = ") + HobString(Hex::toString((long long) currentDevice)));
	if (currentDevice)
	{
		handleWrite(modifiedRec);
		handleSaveRecord(currentDevice);
	}
}


void DeviceManager::handleSaveRecord(Device *recordToSave)
{
	Debug::trace("DeviceManager", HobString("handleSaveRecord(): recordToSave = ") + HobString(Hex::toString((long long) recordToSave)));
	if (recordToSave)
		recordToSave->saveRecord();
}


void DeviceManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	Device *restoreRec = dynamic_cast<Device *> (msg->getHfcObject());
	if (!restoreRec || !dbAccess)
		return;

	restoreRec->setDBAccess(dbAccess);

	Device *usedRec = (Device *)NULL;
	Device *existingRec = Device::findDevice(restoreRec->getDeviceType(), restoreRec->getDeviceSubType(), dbAccess);
	if (existingRec)
	{
		if (existingRec->isRecordValid())
		{
			existingRec->updateDevice(restoreRec);
			existingRec->saveRecord();
			usedRec = existingRec;								
		}
		else
		{
			existingRec->deleteRecord();	   
			restoreRec->saveRecord();
			usedRec = restoreRec;								
		}
	}
	else
	{
		restoreRec->saveRecord();
		usedRec = restoreRec;								
	}

	if (usedRec && usedRec->hasErrors() && mySession && mySession->isCommSession())
	{
		SessionMessage errMsg(Error, DtWrite);
		errMsg.setFormatType(EvolutionTaggedFormat);
		HfcError *err = usedRec->firstError();
		while (err)
		{
			errMsg.setHfcObject(err);
			mySession->send(&errMsg, Messenger::DEST_UI);

			err = usedRec->nextError();
		}
	}

	if (existingRec)
		delete existingRec;
}


void DeviceManager::handleCreate(Device *msgRec)
{
	if (!msgRec)
		return;

	Device *newRec = createNewDevice(msgRec->getDeviceType(), msgRec->getDeviceSubType());
	if (newRec)
	{
		//	Set this new device as the current device.
		setCurrentDevice(newRec);

		if (mySession)
		{
			// Send the new device to the Browser.
			SessionMessage msg(DeviceDev, DtWrite);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(currentDevice);

			mySession->send(&msg, Messenger::DEST_UI);
		}
	}
}

Device *DeviceManager::createNewDevice(int type, int subType)
{
	Device *newDevice = (Device *) NULL;
	if (mySession)
	{
		// Send the current new Device to the browser.
		newDevice = new Device();
		if (newDevice)
		{
			newDevice->setDBAccess(dbAccess);
			newDevice->setDeviceType(type);
			newDevice->setDeviceSubType(subType);
		}
	}
	return newDevice;
}

void DeviceManager::sendCurrentDevice()
{
	if (!currentDevice || !mySession)
	{
		Debug::trace("DeviceManager", "sendCurrentDevice(): currentDevice or mySession is NULL!");
		return;
	}
	SessionMessage msg(DeviceDev, DtWrite);
	msg.setFormatType(BrowserFormat);
	msg.setHfcObject(currentDevice);
	mySession->send(&msg, Messenger::DEST_UI);
}


void DeviceManager::destroyQueriesAndIters()
{
	if (allDevicesIter)
	{
		delete allDevicesIter;
		allDevicesIter = (DBQuery<DBDevice>::const_iterator *)NULL;
	}

	if (allDevicesQuery)
	{
		delete allDevicesQuery;
		allDevicesQuery = (DBQuery<DBDevice> *)NULL;
	}
}
