/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLPRODUCTIONSTEPMANAGER_H
#define COOLPRODUCTIONSTEPMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLProductionStep.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLProductionStepManager : public DataManager
{
	public:

		COOLProductionStepManager();
		COOLProductionStepManager(hfc::HFCManagerContainer *container);
		virtual ~COOLProductionStepManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLProductionStep* getFirstRecord();
		COOLProductionStep* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteProductionStep; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteProductionStep; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllProductionSteps; }

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

#endif //COOLPRODUCTIONSTEPMANAGER_H
