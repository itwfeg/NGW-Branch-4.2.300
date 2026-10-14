/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NUTRIFACTSMANAGER_H
#define NUTRIFACTSMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "NutrifactsEvent.h"
#include "Nutrifacts.h"

class SessionMessage;
class DBSession;

#include "DBNutritionsRM.h"
#include "DBNutrition.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class NutrifactsManager : public DataManager
{
	public:

		NutrifactsManager();
		NutrifactsManager(hfc::HFCManagerContainer *container);
		virtual ~NutrifactsManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Nutrifacts* getFirstRecord() { return firstNutrifacts(); }
		Nutrifacts* getNextRecord()  { return nextNutrifacts(); }
		EvolDeviceType getDevice() const { return NutrifactsDev; }

		Nutrifacts *firstNutrifacts();
		Nutrifacts *nextNutrifacts();
		int countAllNutrifacts();

		Nutrifacts *getCurrentNutrifacts();
		void setCurrentNutrifacts(Nutrifacts *newNutrifacts);
		void setCurrentNutrifactsFromProduct(Nutrifacts *newNutrifacts);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(Nutrifacts *recordToSave);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Nutrifacts *currentNutrifacts;

		edb::DBQuery<edb::DBNutrition> *allNutrifactsQuery;
		edb::DBQuery<edb::DBNutrition>::const_iterator *allNutrifactsIter;


		void init();

		void handleDelete(SessionMessage *msg);
		void handleRestore(SessionMessage *msg);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //NUTRIFACTSMANAGER_H
