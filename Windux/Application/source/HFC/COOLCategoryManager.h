/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYMANAGER_H
#define COOLCATEGORYMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLCategory.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLCategoryManager : public DataManager
{
	public:

		COOLCategoryManager();
		COOLCategoryManager(hfc::HFCManagerContainer *container);
		virtual ~COOLCategoryManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLCategory* getFirstRecord();
		COOLCategory* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteCategory; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteCategory; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllCategories; }

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

#endif //COOLCATEGORYMANAGER_H
