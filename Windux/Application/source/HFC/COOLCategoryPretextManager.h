/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYPRETEXTMANAGER_H
#define COOLCATEGORYPRETEXTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLCategoryPretext.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLCategoryPretextManager : public DataManager
{
	public:

		COOLCategoryPretextManager();
		COOLCategoryPretextManager(hfc::HFCManagerContainer *container);
		virtual ~COOLCategoryPretextManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLCategoryPretext* getFirstRecord();
		COOLCategoryPretext* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteCategoryPretext; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteCategoryPretext; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllCategoryPretexts; }

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

#endif //COOLCATEGORYPRETEXTMANAGER_H
