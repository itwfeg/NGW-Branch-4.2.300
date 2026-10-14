/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLTEXTMANAGER_H
#define COOLTEXTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLText.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLTextManager : public DataManager
{
	public:

		COOLTextManager();
		COOLTextManager(hfc::HFCManagerContainer *container);
		virtual ~COOLTextManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLText* getFirstRecord();
		COOLText* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteCoolText; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteCoolText; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllCoolText; }

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

#endif //COOLTEXTMANAGER_H
