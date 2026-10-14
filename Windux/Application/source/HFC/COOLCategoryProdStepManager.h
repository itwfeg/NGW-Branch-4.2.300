/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYPRODSTEPMANAGER_H
#define COOLCATEGORYPRODSTEPMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLCategoryProdStep.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLCategoryProdStepManager : public DataManager
{
	public:

		COOLCategoryProdStepManager();
		COOLCategoryProdStepManager(hfc::HFCManagerContainer *container);
		virtual ~COOLCategoryProdStepManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLCategoryProdStep* getFirstRecord();
		COOLCategoryProdStep* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteCategoryProductionStep; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteCategoryProductionStep; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllCategoryProductionSteps; }

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLCATEGORYPRODSTEPMANAGER_H
