/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef DATAMANAGER_H
#define DATAMANAGER_H



#include <string>

#include "evolution.h"
#include "PAL_platformheader.h"
#include "Event.h"

#include "DataTypeEvent.h"
#include "HobString.h"
#include "Debug.h"

class SessionMessage;
class HfcObject;
class DBAccess;


class HfcObject;



class DataManager
{
	public:
		virtual ~DataManager();

		void setDBAccess(DBAccess *access);

		virtual HfcObject* getFirstRecord() { return 0; }
		virtual HfcObject* getNextRecord()  { return 0; }

		virtual EvolDeviceType getDevice() const { return UnknownDevice; }
		virtual UINT8 getWriteEvent() const { return DtWrite; }
		virtual UINT8 getDeleteEvent() const { return DtDelete; }
		virtual UINT8 getDeleteAllEvent() const { return DtDeleteAll; }

		virtual void finishedProcessingChanges() {}

		virtual void destroyQueriesAndIters() {}

		virtual void serializeForSynchronization(std::string &resultString, HfcObject *hfcObject, UINT8 event = InvalidEvent);


		/* This method is just a shell method for this class.  Any class that extends DataManager
		 * is expected to provide its own implementation to override this.  I didn't want to make
		 * this pure virtual because I wanted to include the DBAccess * in this class.
		 */
		virtual void handleMessage(SessionMessage* /*msg*/)
			{ Debug::trace("DataManager", "handleMessage method called in DataManager base class"); }

		virtual void handleDeleteAll()
			{ Debug::trace("DataManager", "handleDeleteAll method called in DataManager base class"); }

		virtual void handleDeleteAll(HobRecordNum /*deptNum*/)
			{ Debug::trace("DataManager", "handleDeleteAll(deptNum) method called in DataManager base class"); }


	protected:
		DBAccess *dbAccess;

		DataManager();


	private:
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif //DATAMANAGER_H
