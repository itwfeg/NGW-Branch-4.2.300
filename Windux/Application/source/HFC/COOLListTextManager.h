/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLLISTTEXTMANAGER_H
#define COOLLISTTEXTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLListText.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLListTextManager : public DataManager
{
	public:

		COOLListTextManager();
		COOLListTextManager(hfc::HFCManagerContainer *container);
		virtual ~COOLListTextManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLListText* getFirstRecord();
		COOLListText* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteShortlistText; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteShortlistText; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllShortlistTexts; }

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

#endif //COOLLISTTEXTMANAGER_H
