/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTEXTCFGMANAGER_H
#define PRODUCTTEXTCFGMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "ProductTextCfgEvent.h"
#include "ProductTextCfg.h"

class SessionMessage;
class DBSession;

#include "DBProductTextCfgsRM.h"
#include "DBProductTextCfg.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class ProductTextCfgManager : public DataManager
{
	public:

		ProductTextCfgManager();
		ProductTextCfgManager(hfc::HFCManagerContainer *container);
		virtual ~ProductTextCfgManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		ProductTextCfg* getFirstRecord() { return firstProductTextCfg(); }
		ProductTextCfg* getNextRecord()  { return nextProductTextCfg(); }
		EvolDeviceType getDevice() const { return ProductTextCfgDev; }

		ProductTextCfg *firstProductTextCfg();
		ProductTextCfg *nextProductTextCfg();
		int countAllProductTextCfgs();

		ProductTextCfg *getCurrentProductTextCfg();
		void setCurrentProductTextCfg(ProductTextCfg *newProductTextCfg);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(ProductTextCfg *recordToSave);

		ProductTextCfg *createNewProductTextCfg(ProdSupportTextType type);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		ProductTextCfg *currentProductTextCfg;

		edb::DBQuery<edb::DBProductTextCfg> *allProductTextCfgsQuery;
		edb::DBQuery<edb::DBProductTextCfg>::const_iterator *allProductTextCfgsIter;

		bool receivedProductTextCfgRestore;


		void init();

		void handleRead(ProductTextCfg *msgRec);
		void handleReadAll();
		void handleWrite(ProductTextCfg *modifiedRec);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(ProductTextCfg *modifiedRec = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(ProductTextCfg *msgRec);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //PRODUCTTEXTCFGMANAGER_H
