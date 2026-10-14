/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLPRETEXTMANAGER_H
#define COOLPRETEXTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLPretext.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLPretextManager : public DataManager
{
	public:

		COOLPretextManager();
		COOLPretextManager(hfc::HFCManagerContainer *container);
		virtual ~COOLPretextManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLPretext* getFirstRecord();
		COOLPretext* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWritePretext; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeletePretext; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllPretexts; }

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

#endif //COOLPRETEXTMANAGER_H
