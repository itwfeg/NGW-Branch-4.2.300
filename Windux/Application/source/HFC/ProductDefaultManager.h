/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTDEFAULTMANAGER_H
#define PRODUCTDEFAULTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "ProductDevEvent.h"
#include "ProductDefault.h"

class SessionMessage;
class DBSession;

#include "DBProductDefaultsRM.h"
#include "DBProductDefault.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class ProductDefaultManager : public DataManager
{
	public:

		ProductDefaultManager();
		ProductDefaultManager(hfc::HFCManagerContainer *container);
		virtual ~ProductDefaultManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		ProductDefault* getFirstRecord() { return firstProductDefault(); }
		ProductDefault* getNextRecord()  { return nextProductDefault(); }
		EvolDeviceType getDevice() const { return ProductDefaultsDev; }

		ProductDefault *firstProductDefault();
		ProductDefault *nextProductDefault();
		int countAllProductDefaults();

		ProductDefault *getCurrentProductDefault();
		void setCurrentProductDefault(ProductDefault *newProductDefault);


		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(ProductDefault *recordToSave);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		ProductDefault *currentProductDefault;

		edb::DBQuery<edb::DBProductDefault> *allProductDefaultsQuery;
		edb::DBQuery<edb::DBProductDefault>::const_iterator *allProductDefaultsIter;


		void init();

		void handleRead(ProductDefault *msgProductDefault);
		void handleWrite(ProductDefault *msgProductDefault);
		void handleUpdateAndSave(ProductDefault *msgProductDefault);
		void handleRestore(SessionMessage *msg);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //PRODUCTDEFAULTMANAGER_H
