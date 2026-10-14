/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "DataTypeEvent.h"
#include "Device.h"

class SessionMessage;
class DBSession;

#include "DBDevicesRM.h"
#include "DBDevice.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class DeviceManager : public DataManager
{
	public:

		DeviceManager();
		DeviceManager(hfc::HFCManagerContainer *container);
		virtual ~DeviceManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Device* getFirstRecord() { return firstDevice(); }
		Device* getNextRecord()  { return nextDevice(); }
		EvolDeviceType getDevice() const { return DeviceDev; }

		Device *firstDevice();
		Device *nextDevice();
		int countAllDevices();

		Device *getCurrentDevice();
		void setCurrentDevice(Device *newDevice);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(Device *recordToSave);

		Device *createNewDevice(int type, int subtype);
		void sendCurrentDevice();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Device *currentDevice;

		edb::DBQuery<edb::DBDevice> *allDevicesQuery;
		edb::DBQuery<edb::DBDevice>::const_iterator *allDevicesIter;


		void init();

		void handleRead(Device *msgDevice);
		void handleReadAll();
		void handleWrite(Device *modifiedDevice);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Device *modifiedDevice = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(Device *msgDevice);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //DEVICEMANAGER_H
