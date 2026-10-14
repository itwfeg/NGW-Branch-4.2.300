/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EMAILALERTSMANAGER_H
#define EMAILALERTSMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "HobEmailAlert.h"

class SessionMessage;
class DBSession;

#include "DBEmailAlertsRM.h"
#include "DBEmailAlert.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class EmailAlertsManager : public DataManager
{
	public:

		EmailAlertsManager();
		EmailAlertsManager(hfc::HFCManagerContainer *container);
		virtual ~EmailAlertsManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		HobEmailAlert* getFirstRecord() { return firstEmailAlert(); }
		HobEmailAlert* getNextRecord()  { return nextEmailAlert(); }
		EvolDeviceType getDevice() const { return EmailAlertsDev; }

		HobEmailAlert *firstEmailAlert();
		HobEmailAlert *nextEmailAlert();
		int countAllEmailAlerts();


		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(HobEmailAlert *recordToSave);

		void showAllEmailAlerts();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		HobEmailAlert	*currentEmailAlert;

		edb::DBQuery<edb::DBEmailAlert> *allEmailAlertsQuery;
		edb::DBQuery<edb::DBEmailAlert>::const_iterator *allEmailAlertsIter;


		void init();

		void setCurrentEmailAlert(HobEmailAlert *emailAlert);

		void handleReadAll();
		void handleRead(HobEmailAlert *msgEmailAlert);
		void handleWrite(HobEmailAlert *msgEmailAlert);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(HobEmailAlert *msgEmailAlert);
		void handleRestore(SessionMessage *msg);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //EMAILALERTSMANAGER_H
