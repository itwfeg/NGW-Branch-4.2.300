/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLSHORTLISTMANAGER_H
#define COOLSHORTLISTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "COOLDevEvent.h"
#include "COOLShortlist.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class COOLShortlistManager : public DataManager
{
	public:

		COOLShortlistManager();
		COOLShortlistManager(hfc::HFCManagerContainer *container);
		virtual ~COOLShortlistManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		COOLShortlist* getFirstRecord();
		COOLShortlist* getNextRecord();
		EvolDeviceType getDevice() const { return COOLDev; }
		virtual UINT8 getWriteEvent() const { return COOLDevWriteShortlist; }
		virtual UINT8 getDeleteEvent() const { return COOLDevDeleteShortlist; }
		virtual UINT8 getDeleteAllEvent() const { return COOLDevDeleteAllShortlists; }

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

#endif //COOLSHORTLISTMANAGER_H
