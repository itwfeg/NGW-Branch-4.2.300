/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PERIPHERALMANAGER_H
#define PERIPHERALMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "PeripheralEvent.h"
#include "Peripheral.h"
#include "HfcPeripheralList.h"

class SessionMessage;
class DBSession;

#include "DBDevice.h"
#include "DBQuery.h"
#include "DBDevicesRM.h"

namespace hfc
{
	class HFCManagerContainer;
}


class PeripheralManager : public DataManager
{
	public:

		PeripheralManager();
		PeripheralManager(hfc::HFCManagerContainer *container);
		virtual ~PeripheralManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Peripheral* getFirstRecord() { return firstPeripheral(); }
		Peripheral* getNextRecord()  { return nextPeripheral(); }
		EvolDeviceType getDevice() const { return PeripheralDev; }

		bool ischeckForMissingPeripherals() const;
		void setcheckForMissingPeripherals(bool check);

		Peripheral *firstPeripheral();
		Peripheral *nextPeripheral();
		int countAllPeripherals();

		Peripheral *getCurrentPeripheral();
		void setCurrentPeripheral(Peripheral *newPeripheral);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();

		Peripheral *createNewPeripheral(PeripheralType type, int subType);

		void destroyQueriesAndIters();

		HfcPeripheralList &getCurrentList();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		// Flag set by the Session when the Peripheral Manager is to use the next HfcPeripheralList
		// to check for lost peripherals.
		bool checkForMissingPeripherals;

		// Flag used to indicate that the devices db table has been updated, and the
		// PeripheralManager should backup the database to flash when done processing the current message.
		// This is an internal flag only, no API for access is needed.
		bool peripheralDBUpdated;

		Peripheral *currentPeripheral;

		HfcPeripheralList currentList;

		edb::DBQuery<edb::DBDevice> *allPeripheralsQuery;
		edb::DBQuery<edb::DBDevice>::const_iterator *allPeripheralsIter;

		void init();

		void handleRead(Peripheral *msgPeripheral);
		void handleReadAll();
		void handleWrite(Peripheral *modifiedPeripheral);
		void handleDelete(Peripheral *msgPeripheral);
		void handleSave(Peripheral *modifiedPeripheral);
		void handleRestore(Peripheral *restorePeripheral);
		void handleCreate(Peripheral *msgPeripheral);
		void handlePeripheralsList(HfcPeripheralList *peripheralList);
		void handleLostPeripheralsList(HfcPeripheralList *peripheralList);
		void checkForPeripheral(EvolDeviceType type, short subType, short kind);
		void handleRemovePeripheral(Peripheral *msgPeripheral);
		void handleReadCreate(Peripheral *msgPeripheral);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //PERIPHERALMANAGER_H
