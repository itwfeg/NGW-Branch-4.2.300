/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "PeripheralManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"
#include "Device.h"

#include "Weigher.h"
#include "Printer.h"
#include "LabelApplier.h"
#include "Wrapper.h"
#include "SystemEvent.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBDevicesRM.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

#include "HFCManagerContainer.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int PeripheralManager::_dataType_ = 0;
	std::string PeripheralManager::_dataTypeName_("PeripheralManager"); 
#endif


PeripheralManager::PeripheralManager()
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
}

PeripheralManager::PeripheralManager(hfc::HFCManagerContainer *container)
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

PeripheralManager::~PeripheralManager()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

	destroyQueriesAndIters();
}


void PeripheralManager::init()
{
	mySession = 0;

	checkForMissingPeripherals = false;

	peripheralDBUpdated = false;

	currentPeripheral = 0;

	allPeripheralsQuery = 0;
	allPeripheralsIter  = 0;
}


DBSession *PeripheralManager::getMySession()
{
	return mySession;
}
void PeripheralManager::setMySession(DBSession *session)
{
	mySession = session;
}


bool PeripheralManager::ischeckForMissingPeripherals() const
{
	return checkForMissingPeripherals;
}
void PeripheralManager::setcheckForMissingPeripherals(bool check)
{
	checkForMissingPeripherals = check;
}


/* If the first record was not read, this method will return NULL.
 */
Peripheral *PeripheralManager::firstPeripheral()
{
	if (!dbAccess)
		return (Peripheral *)NULL;

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return (Peripheral *)NULL;

	// If the user calls firstPeripheral() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allPeripheralsQuery = dbDevicesRM->retrieveAll();

	// Couldn't get the query from which to access the Peripherals, just return false.
	if (!allPeripheralsQuery)
		return (Peripheral *)NULL;

	DBQuery<DBDevice>::const_iterator tempIter = allPeripheralsQuery->begin();
	if (tempIter == allPeripheralsQuery->end())
		return (Peripheral *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextPeripheral()
	 * method call below).
	 */
	allPeripheralsIter = new DBQuery<DBDevice>::const_iterator(tempIter);
	DBDevice *d = new DBDevice(*(*allPeripheralsIter));
	Peripheral *newPeripheral = new Peripheral(d, dbAccess);
	return newPeripheral;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstPeripheral() method, the first call to this method will
 * result in the first record being returned.
 */
Peripheral *PeripheralManager::nextPeripheral()
{
	if (!dbAccess)
		return (Peripheral *)NULL;

	if (allPeripheralsQuery && !allPeripheralsQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allPeripheralsQuery || !allPeripheralsIter)
		return firstPeripheral();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allPeripheralsIter);
	if (*allPeripheralsIter == allPeripheralsQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all Peripherals in the db. */
		destroyQueriesAndIters();
		return (Peripheral *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBDevice copy constructor in order for the Hfc Peripheral to
	 * control the destruction of the DBDevice record that the iterator contains; otherwise, the DBDevice will
	 * be deleted when the iterator goes out of scope.  The DBDevice copy will be deleted by the Peripheral record
	 * created below.
	 */
	DBDevice *d = new DBDevice(*(*allPeripheralsIter));
	Peripheral *newPeripheral = new Peripheral(d, dbAccess);
	return newPeripheral;
}


/* Provide a countAll method so the caller doesn't need to know about a dbDevicesRM.
 */
int PeripheralManager::countAllPeripherals()
{
	if (!dbAccess)
		return -1;

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return -1;

	return dbDevicesRM->countAll();
}


Peripheral *PeripheralManager::getCurrentPeripheral()
{
	return currentPeripheral;
}

void PeripheralManager::setCurrentPeripheral(Peripheral *newPeripheral)
{
	if (currentPeripheral)
		delete currentPeripheral;
	currentPeripheral = newPeripheral;
}


void PeripheralManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	EvolDeviceType dev = msg->getDeviceType();
	switch (dev)
	{
		case SessionDev:
		{
			break;
		}

		case System:
		{
			if (msg->getEventType() == SysPeripheralList)
			{
				HfcPeripheralList *peripheralList = dynamic_cast<HfcPeripheralList *>(msg->getHfcObject());

				if (checkForMissingPeripherals)
				{
					handleLostPeripheralsList(peripheralList);
					checkForMissingPeripherals = false;
				}
				else
					handlePeripheralsList(peripheralList);

				if (peripheralList)
					currentList = (*peripheralList);	//	Save the current state of the list.
			}
			break;
		}

		case PeripheralDev:
		{
			Peripheral *msgPeripheral = dynamic_cast<Peripheral *> (msg->getHfcObject());
			UINT8 event = msg->getEventType();
			Debug::trace("PeripheralManager", HobString("handling event: ") + HobString(Integer::toString(event)));
			switch (event)
			{
				case DtRead:
					handleRead(msgPeripheral);
					break;

				case DtReadAll:
					handleReadAll();
					break;

				case DtWrite:
					handleWrite(msgPeripheral);
					break;

				case DtDelete:
					handleDelete(msgPeripheral);
					break;

				case DtDeleteAll:
					handleDeleteAll();
					break;

				case DtSave:
					handleSave(msgPeripheral);
					break;

				case DtRestore:
					handleRestore(msgPeripheral);
					break;

				case DtCreateNew:
					handleCreate(msgPeripheral);
					break;

				case RemoveCheckingLostPeripheral:
					handleRemovePeripheral(msgPeripheral);
					break;

				case ReadCreatePeripheral:
					handleReadCreate(msgPeripheral);
					break;

				default:
					Debug::trace("PeripheralManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
					break;
			}
			break;
		}

		default:
			Debug::trace("PeripheralManager", HobString("UNKNOWN DEVICE: ") + HobString(Integer::toString((int)dev)));
			break;
	}


	Session *s = dynamic_cast<Session *> (mySession);
	if (peripheralDBUpdated && s)
	{
		peripheralDBUpdated = false;
		s->backupDatabase("DevicesSaved");
	}
}


void PeripheralManager::handleRead(Peripheral *msgPeripheral)
{
	if (!msgPeripheral || !dbAccess)
		return;

	Peripheral *lookedUpPeripheral = Peripheral::findPeripheral(msgPeripheral->getType(), msgPeripheral->getSubType(), dbAccess);

	if (!lookedUpPeripheral || lookedUpPeripheral->getErrorCode() != RECORD_OK)		//	This Peripheral does not exist
	{
		// Send error message to browser indicating Peripheral not found.
		if (mySession) 
			mySession->addDialogMessage(ErrPeripheralDev, PeripheralErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpPeripheral)
			delete lookedUpPeripheral;
		return;
	}

	// We have a new current Peripheral.
	setCurrentPeripheral(lookedUpPeripheral);
	Debug::trace("PeripheralManager", HobString("handleRead(): currentPeripheral = ") + HobString(Hex::toString((long long) currentPeripheral)));

	if (mySession)
	{
		// Send the Peripheral found back to the Browser.
		SessionMessage peripheralData(PeripheralDev, DtWrite);
		peripheralData.setFormatType(BrowserFormat);
		peripheralData.setHfcObject(currentPeripheral);

		// Send the Peripheral to the UI (Browser)
		mySession->send(&peripheralData, Messenger::DEST_UI);
	}
}


void PeripheralManager::handleReadAll()
{
	Debug::trace("PeripheralManager", "handling DtReadAll event ...");
	if (!mySession || !dbAccess)
		return;

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return;

	int i = 0;

	//	Now query for all Peripherals.
	DBQuery<DBDevice> *devicesQuery = dbDevicesRM->retrieveAll();
	if (devicesQuery)
	{
		Peripheral *cellPeripheral = (Peripheral *)NULL;
		DBQuery<DBDevice>::const_iterator devicesQueryIter = devicesQuery->begin();
		while (devicesQueryIter != devicesQuery->end())
		{
			DBDevice *d = new DBDevice(*devicesQueryIter);
			if (d)
			{
				cellPeripheral = new Peripheral(d, dbAccess);
				if (cellPeripheral)
				{
					i++;
					delete cellPeripheral;
				}
				else
					// only delete the db record if we don't successfully create the HFC Peripheral object.
					// otherwise, the Peripheral object will delete it.
					delete d;
			}
			++devicesQueryIter;
		}
	}
	else
	{
		Debug::trace("PeripheralManager", HobString("handleReadAll() - received back a null pointer for the query retrieveAll"));
		return;
	}
	Debug::trace("PeripheralManager", HobString("handleReadAll() - read this number of Peripherals:") + HobString(Integer::toString(i)));

	// Send the Peripherals list to the UI (Browser)
	SessionMessage pMsg(PeripheralDev, DtWriteAll);
	pMsg.setFormatType(BrowserFormat);
	mySession->send(&pMsg, Messenger::DEST_UI);

	if (devicesQuery)
		delete devicesQuery;
}


void PeripheralManager::handleWrite(Peripheral *modifiedPeripheral)
{
	if ((!modifiedPeripheral) || (!currentPeripheral))
		return;

	Debug::trace("PeripheralManager", HobString("handleWrite(): currentPeripheral = ") + HobString(Hex::toString((long long) currentPeripheral)) +
											  HobString("  , modifiedPeripheral = ") + HobString(Hex::toString((long long) modifiedPeripheral)));
  	currentPeripheral->updatePeripheral(modifiedPeripheral);
}


void PeripheralManager::handleDelete(Peripheral *msgPeripheral)
{
	if (!msgPeripheral || !dbAccess)
		return;

	Peripheral *existingPeripheral = Peripheral::findPeripheral(msgPeripheral->getType(), msgPeripheral->getSubType(), dbAccess);
	if (existingPeripheral)
	{
		existingPeripheral->deleteRecord();
		peripheralDBUpdated = true;
		delete existingPeripheral;
		Debug::trace("PeripheralManager", HobString("handleDelete - Peripheral deleted - type:") + HobString(Integer::toString((int)msgPeripheral->getType())) +
														HobString("  subType:") + HobString(Integer::toString(msgPeripheral->getSubType())));
	}
	else
		Debug::trace("PeripheralManager", HobString("handleDelete - Peripheral to be deleted was not in the database to delete - type:") + HobString(Integer::toString((int)msgPeripheral->getType())) +
														HobString("  subType:") + HobString(Integer::toString(msgPeripheral->getSubType())));
}


void PeripheralManager::handleDeleteAll()
{
	Debug::trace("PeripheralManager", "handleDeleteAll - deleting all Peripherals");

	if (!dbAccess)
		return;

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return;

	dbDevicesRM->deleteAll();

	destroyQueriesAndIters();

	peripheralDBUpdated = true;
	Debug::trace("PeripheralManager", "handleDeleteAll - leaving");
}


void PeripheralManager::handleSave(Peripheral *modifiedPeripheral)
{
	Debug::trace("PeripheralManager", HobString("handleSave(): currentPeripheral = ") + HobString(Hex::toString((long long) currentPeripheral)));
	if (currentPeripheral)
	{
		if (modifiedPeripheral)
		  	currentPeripheral->updatePeripheral(modifiedPeripheral);
		currentPeripheral->saveRecord();
		peripheralDBUpdated = true;
	}
}


void PeripheralManager::handleRestore(Peripheral *restorePeripheral)
{
	if (!restorePeripheral || !dbAccess)
		return;

	restorePeripheral->setDBAccess(dbAccess);

	Peripheral *usedPeripheral = (Peripheral *)NULL;
	Peripheral *existingPeripheral = Peripheral::findPeripheral(restorePeripheral->getType(), restorePeripheral->getSubType(), dbAccess);
	if (existingPeripheral)
	{
		if (existingPeripheral->isRecordValid())
		{
			existingPeripheral->updatePeripheral(restorePeripheral);
			existingPeripheral->saveRecord();
			usedPeripheral = existingPeripheral;								
		}
		else
		{
			existingPeripheral->deleteRecord();	   
			restorePeripheral->saveRecord();
			usedPeripheral = restorePeripheral;								
		}
	}
	else
	{
		restorePeripheral->saveRecord();
		usedPeripheral = restorePeripheral;								
	}

	if (usedPeripheral && usedPeripheral->hasErrors() &&
		 mySession && mySession->isCommSession())
	{
		SessionMessage errMsg(Error, DtWrite);
		errMsg.setFormatType(EvolutionTaggedFormat);
		HfcError *err = usedPeripheral->firstError();
		while (err)
		{
			errMsg.setHfcObject(err);
			mySession->send(&errMsg, Messenger::DEST_UI);

			err = usedPeripheral->nextError();
		}
	}
	if (existingPeripheral)
		delete existingPeripheral;
}


void PeripheralManager::handleCreate(Peripheral *msgPeripheral)
{
	if (!msgPeripheral)
		return;

	Peripheral *newPeripheral = new Peripheral();
	if (newPeripheral)
	{
		//	Set this new Peripheral as the current Peripheral.
		newPeripheral->setDBAccess(dbAccess);
		newPeripheral->setErrorCode(RECORD_OK);

		newPeripheral->setType(msgPeripheral->getType());
		newPeripheral->setSubType(msgPeripheral->getSubType());
		newPeripheral->setKind(msgPeripheral->getKind());
		// Note:  checkingPresence is initialized to false, and leave it that way!!

		setCurrentPeripheral(newPeripheral);

		// Send the new Peripheral to the Browser.
		if (mySession)
		{
			SessionMessage peripheralData(PeripheralDev, DtWrite);
			peripheralData.setFormatType(BrowserFormat);
			peripheralData.setHfcObject(currentPeripheral);
			mySession->send(&peripheralData, Messenger::DEST_UI);
		}
	}
}


void PeripheralManager::handlePeripheralsList(HfcPeripheralList *peripheralList)
{
	if (!peripheralList)
		return;

    HobString debugHeader = HobString("handlePeripheralsList(): ");
    Debug::trace("PeripheralManager", debugHeader+"Entry");

	WeigherKind wk = peripheralList->getFirstWeigher();
	if (wk != WgNoWeigher) {
		checkForPeripheral(Weigher, (short)FirstWeigher, (short)wk);
    }
    
	wk = peripheralList->getSecondWeigher();
	if (wk != WgNoWeigher) {
		checkForPeripheral(Weigher, (short)SecondWeigher, (short)wk);
    }

	PrinterKind pk = peripheralList->getFirstPrinter();
	if (pk != NoPrinter) {
		checkForPeripheral(Printer, (short)FirstPrinter, (short)pk);
    } else {
        Debug::trace("PeripheralManager", debugHeader+"ERROR: No First Printer found!");
    }

	pk = peripheralList->getSecondPrinter();
	if (pk != NoPrinter) {
		checkForPeripheral(Printer, (short)SecondPrinter, (short)pk);
    }

	pk = peripheralList->getThirdPrinter();
	if (pk != NoPrinter) {
		checkForPeripheral(Printer, (short)ThirdPrinter, (short)pk);
    }

	LabelApplierKind lak = peripheralList->getLabelApplier();
	if (lak != NoLabelApplier) {
		checkForPeripheral(LabelApplier, 0, (short)lak);
    }

	WrapperKind wrk = peripheralList->getWrapper();
	if (wrk != WrNoWrapper) {
		checkForPeripheral(Wrapper, 0, (short)wrk);
    }

	NumericKind nk = peripheralList->getNumeric();
	if (nk != NmNoNumeric) {
		checkForPeripheral(Numeric, 0, (short)nk);
    }

	MarqueeKind mk = peripheralList->getMarquee();
	if (mk != MqNoMarquee) {
		checkForPeripheral(Marquee, 0, (short)mk);
    }
	
	// jhc gcd to WINDUX merge: Where is the BakeryLabelApplier that was added for WINDUX
	
	GCDKind gcd = peripheralList->getGCD();
	if (gcd != GCDNoGCD) {
		checkForPeripheral(GCDSessionDev, 0, (short)gcd);
    }

	Device *dev( Device::findDevice(Printer, ThirdPrinter, dbAccess) );
	if (( dev ) && ( !dev->isCheckPresence() ))
	{
		short kind( dev->getKind() );

		HobString systemName( dev->getSystemName() );
		util_string::tolower(systemName);

		if ( kind == NetworkPrinter && systemName.find("zebra") != std::string::npos )
			kind = USBPrinter;

		peripheralList->setThirdPrinter( (PrinterKind)kind );
	}


	if (mySession) {
        Debug::trace("PeripheralManager", debugHeader+"Sending the peripheralList!");
		peripheralList->setFormatType(BrowserFormat);
		mySession->send(peripheralList, Messenger::DEST_UI);
	} else {
        Debug::trace("PeripheralManager.Error", debugHeader+"not sending peripheralList because mySession is NULL!");
    }
}

void PeripheralManager::checkForPeripheral(EvolDeviceType type, short subType, short kind)
{
	if ( kind == NoPrinter )
		return;

	HobDateTime now;
    HobString debugHeader = HobString("checkForPeripheral(): ");
	Debug::trace("PeripheralManager", debugHeader+HobString(" type:") + HobString(Integer::toString((int)type)) +
							"  subType:" + HobString(Integer::toString((int)subType)) +
							"  kind:" + HobString(Integer::toString((int)kind)) );

	Device *dev = Device::findDevice(type, subType, dbAccess);

	// If the record exists in the db, and it's set up to not be checking for it, then just ignore it.
	//if (periph && !periph->isCheckingPresence())
	//	return;

	if (!dev)
	{
		Debug::trace("PeripheralManager", debugHeader+"device not in db, creating new Device");

		dev = new Device();
		if (dev)
		{
			dev->setDBAccess(dbAccess);
			dev->setDeviceType(type);
			dev->setDeviceSubType(subType);
			dev->setInstalledDate(now);
			dev->setUserName( ( type == Printer && subType == (short)SecondPrinter ) ? "EXT1" : ( ( type == Printer && subType == (short)ThirdPrinter ) ? "EXT2" : "" ) );
		}
	}
	if (dev)
	{
		bool chkPres = ( kind == USBPrinter || kind == NetworkPrinter || kind == FakePrinter ) ? false : true;
		dev->setCheckPresence( chkPres );
		dev->setKind(kind);
		// Set these if we can get them from the device.
		//dev->setHardwareVersion("");
		//dev->setSoftwareVersion("");

		Debug::trace("PeripheralManager", debugHeader+"checkPresence:" + HobString(Bool::toString(chkPres)) + "  kind:" + HobString(Integer::toString((int)kind)) );


		Debug::trace("PeripheralManager", debugHeader+"saving Device");

		dev->saveRecord();
		peripheralDBUpdated = true;

		delete dev;
		dev = 0;
	}
	else
		Debug::trace("PeripheralManager", debugHeader+"device was already in db, do nothing");

}


void PeripheralManager::handleLostPeripheralsList(HfcPeripheralList *peripheralList)
{
	if (!peripheralList || !dbAccess)
		return;

	Debug::trace("PeripheralManager", "handleLostPeripheralsList()");

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return;

	int i = 0;
	bool atLeastOneIsLost = false;
	HfcPeripheralList lostPeripheralsList;

	//	Now query for all Peripherals that need checked.
	//DBQuery<DBDevice> *devicesQuery = dbDevicesRM->retrieveCheckPresence(true);
	DBQuery<DBDevice> *devicesQuery = dbDevicesRM->retrieveAll();
	if (devicesQuery)
	{
		DBQuery<DBDevice>::const_iterator devicesQueryIter = devicesQuery->begin();
		while (devicesQueryIter != devicesQuery->end())
		{
			DBDevice *d = new DBDevice(*devicesQueryIter);
			Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - dbDevice: ") +
													HobString("  type:") + HobString(d->getType()) + 
													HobString("  subType:") + HobString(d->getSubtype()) +
													HobString("  check:") + HobString(Bool::toString(d->getCheckPresence())));
			if (d && (d->isCheckPresenceNull() || d->getCheckPresence()))
			{
				i++;

				EvolDeviceType type = (EvolDeviceType)Integer::valueOf(d->getType());
				short subType = Integer::valueOf(d->getSubtype());
				short kind = d->getKind();
				switch (type)
				{
					case Weigher:
					{
						Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - FirstWeigher kind: ") + HobString(Integer::toString((int)peripheralList->getFirstWeigher())));
						if (subType == (short)FirstWeigher)
						{
							if (peripheralList->getFirstWeigher() == WgNoWeigher)
							{
								// The first weigher was connected, but is no longer; add it to the lost list.
								Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - lost FirstWeigher "));
								lostPeripheralsList.setFirstWeigher((WeigherKind)kind);
								atLeastOneIsLost = true;
							}
						}
						else if (subType == (short)SecondWeigher)
						{
							if (peripheralList->getSecondWeigher() == WgNoWeigher)
							{
								// The second weigher was connected, but is no longer; add it to the lost list.
								Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - lost SecondWeigher "));
								lostPeripheralsList.setSecondWeigher((WeigherKind)kind);
								atLeastOneIsLost = true;
							}
						}
						else
							Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - Weigher not a valid subtype: ") + HobString(Integer::toString(subType)));
						break;
					}

					case Printer:
					{
						if (subType == (short)FirstPrinter)
						{
							if (peripheralList->getFirstPrinter() == NoPrinter)
							{
								// The first printer was connected, but is no longer; add it to the lost list.
								lostPeripheralsList.setFirstPrinter((PrinterKind)kind);
								atLeastOneIsLost = true;
							}
						}
						else if (subType == (short)SecondPrinter)
						{
							if (peripheralList->getSecondPrinter() == NoPrinter)
							{
								// The second printer was connected, but is no longer; add it to the lost list.
								lostPeripheralsList.setSecondPrinter((PrinterKind)kind);
								atLeastOneIsLost = true;
							}
						}
						else if (subType == (short)ThirdPrinter)
						{
							if (peripheralList->getThirdPrinter() == NoPrinter)
							{
								// The second printer was connected, but is no longer; add it to the lost list.
								lostPeripheralsList.setThirdPrinter((PrinterKind)kind);
								atLeastOneIsLost = true;
							}
						}
						else
							Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - Printer not a valid subtype: ") + HobString(Integer::toString(subType)));
						break;
					}

					case LabelApplier:
					{
						if (peripheralList->getLabelApplier() == NoLabelApplier)
						{
							lostPeripheralsList.setLabelApplier((LabelApplierKind)kind);
							atLeastOneIsLost = true;
						}
						break;
					}

					case Wrapper:
					{
						if (peripheralList->getWrapper() == WrNoWrapper)
						{
							lostPeripheralsList.setWrapper((WrapperKind)kind);
							atLeastOneIsLost = true;
						}
						break;
					}

					case Numeric:
					{
						if (peripheralList->getNumeric() == 0)
						{
							lostPeripheralsList.setNumeric((NumericKind)kind);
							atLeastOneIsLost = true;
						}
						break;
					}

					case Marquee:
					{
						if (peripheralList->getMarquee() == 0)
						{
							lostPeripheralsList.setMarquee((MarqueeKind)kind);
							atLeastOneIsLost = true;
						}
						break;
					}

					case GCDSessionDev:
					{
						// jhc gcd to WINDUX merge: again, where is the bakeryLabelApplier?
						if (peripheralList->getGCD() == 0)
						{
							lostPeripheralsList.setGCD((GCDKind)GCDUsb);
							atLeastOneIsLost = true;
						}
						break;
					}

					default:
						break;
				}

				delete d;
			}
			++devicesQueryIter;
		}
	}
	else
	{
		Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - received back a null pointer for the query retrieveAll"));
		return;
	}
	Debug::trace("PeripheralManager", HobString("handleLostPeripheralsList() - read this number of Peripherals needing checked:") + HobString(Integer::toString(i)));

	if (atLeastOneIsLost)
	{
		// Send the Lost Peripherals list to the UI (Browser)
		lostPeripheralsList.setDeviceType(PeripheralDev);
		lostPeripheralsList.setEventType(WriteLostPeripheralsList);
		lostPeripheralsList.setFormatType(BrowserFormat);
		mySession->send(&lostPeripheralsList, Messenger::DEST_UI);
	}

	if (devicesQuery)
		delete devicesQuery;
}


void PeripheralManager::handleRemovePeripheral(Peripheral *msgPeripheral)
{
	if (!msgPeripheral || !dbAccess)
		return;

	Debug::trace("PeripheralManager", "handleRemovePeripheral()");

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return;

	Peripheral *lookedUpPeripheral = Peripheral::findPeripheral(msgPeripheral->getType(), msgPeripheral->getSubType(), dbAccess);
	if (lookedUpPeripheral)
	{
		lookedUpPeripheral->setCheckingPresence(false);
		lookedUpPeripheral->saveRecord();
		peripheralDBUpdated = true;

		Debug::trace("PeripheralManager", HobString("handleRemovePeripheral() - 'removed'  type:") + HobString(Integer::toString((int)msgPeripheral->getType())) +
														HobString("  subType:") + HobString(Integer::toString(msgPeripheral->getSubType())));

		delete lookedUpPeripheral;
	}
	else
		Debug::trace("PeripheralManager", HobString("handleRemovePeripheral() - could not find the Peripheral to 'remove'  type:") + HobString(Integer::toString((int)msgPeripheral->getType())) +
														HobString("  subType:") + HobString(Integer::toString(msgPeripheral->getSubType())));
}


void PeripheralManager::handleReadCreate(Peripheral *msgPeripheral)
{
	if (!msgPeripheral || !dbAccess)
		return;

	DBDevicesRM *dbDevicesRM = dbAccess->getDBDevicesRM();
	if (!dbDevicesRM)
		return;

	Peripheral *lookedUpPeripheral = 0;
	bool createdNew = false;

	if (msgPeripheral->isReceived(fieldOffsetPeripheralType) && msgPeripheral->isReceived(fieldOffsetPeripheralSubType))
	{
		// Received both the peripheral type and subtype, look directly in the database for the corresponding device.
		// This would be used for inquiries of Weighers, Printers, Console, and Wireless Cards.

		lookedUpPeripheral = Peripheral::findPeripheral(msgPeripheral->getType(), msgPeripheral->getSubType(), dbAccess);
	}
	else if (msgPeripheral->isReceived(fieldOffsetPeripheralType) && msgPeripheral->isReceived(fieldOffsetPeripheralKind))
	{
		// Received the peripheral type and kind, look database for any device that has that type and kind.
		// This would be used for inquiries of Label Appliers and Wrappers.

		//	Now query for all Peripherals.
		DBQuery<DBDevice> *devicesQuery = dbDevicesRM->retrieveAll();
		if (devicesQuery)
		{
			DBQuery<DBDevice>::const_iterator devicesQueryIter = devicesQuery->begin();
			while (devicesQueryIter != devicesQuery->end())
			{
				DBDevice *d = new DBDevice(*devicesQueryIter);
				if (d)
				{
					if ((EvolDeviceType)Integer::valueOf(d->getType()) == msgPeripheral->getType() && !d->isKindNull() && d->getKind() == msgPeripheral->getKind())
					{
						lookedUpPeripheral = new Peripheral(d, dbAccess);
						if (!lookedUpPeripheral)
						{
							delete d;
							break;
						}
					}
					else
						// only delete the db record if we don't successfully create the HFC Peripheral object.
						// otherwise, the Peripheral object will delete it.
						delete d;
				}
				++devicesQueryIter;
			}

			delete devicesQuery;
		}
		else
			Debug::trace("PeripheralManager", "handleReadCreate() - No devices exist in the database");
	}
	else
		return;

	if (lookedUpPeripheral)
		setCurrentPeripheral(lookedUpPeripheral);
	else		//	This Peripheral does not exist, so create it.
	{
		handleCreate(msgPeripheral);		// Note:  the handleCreate method assigns the currentPeripheral
		createdNew = true;
		lookedUpPeripheral = currentPeripheral;

		// todo - Do we want to send a message to the user letting them know we created the db record for their request???

			// Send error message to browser indicating Peripheral not found.
//			if (mySession) 
//				mySession->addDialogMessage(ErrPeripheralDev, PeripheralErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);
//			if (lookedUpPeripheral)
//				delete lookedUpPeripheral;
	}

	Debug::trace("PeripheralManager", HobString("handleReadCreate(): currentPeripheral = ") + HobString(Hex::toString((long long) currentPeripheral)));

	if (mySession && !createdNew && currentPeripheral)
	{
		// Send the Peripheral found back to the Browser.
		SessionMessage peripheralData(PeripheralDev, DtWrite);
		peripheralData.setFormatType(BrowserFormat);
		peripheralData.setHfcObject(currentPeripheral);

		// Send the Peripheral to the UI (Browser)
		mySession->send(&peripheralData, Messenger::DEST_UI);
	}
}


void PeripheralManager::destroyQueriesAndIters()
{
	if (allPeripheralsIter)
	{
		delete allPeripheralsIter;
		allPeripheralsIter = (DBQuery<DBDevice>::const_iterator *)NULL;
	}

	if (allPeripheralsQuery)
	{
		delete allPeripheralsQuery;
		allPeripheralsQuery = (DBQuery<DBDevice> *)NULL;
	}
}


HfcPeripheralList &PeripheralManager::getCurrentList()
{
	return currentList;
}
