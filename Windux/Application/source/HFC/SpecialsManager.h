/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SPECIALSMANAGER_H
#define SPECIALSMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "SpecialsEvent.h"
#include "Special.h"

class SessionMessage;
class DBSession;
class HobList;

#include "DBSpecialProgramsRM.h"
#include "DBSpecialProgram.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class SpecialsManager : public DataManager
{
	public:

		SpecialsManager();
		SpecialsManager(hfc::HFCManagerContainer *container);
		virtual ~SpecialsManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Special* getFirstRecord() { return firstSpecial(); }
		Special* getNextRecord()  { return nextSpecial(); }
		EvolDeviceType getDevice() const { return SpecialsDev; }

		Special *firstSpecial();
		Special *nextSpecial();
		int countAllSpecials();

		Special *getCurrentSpecial();
		void setCurrentSpecial(Special *newSpecial);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(Special *recordToSave);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Special *currentSpecial;

		edb::DBQuery<edb::DBSpecialProgram> *allSpecialsQuery;
		edb::DBQuery<edb::DBSpecialProgram>::const_iterator *allSpecialsIter;

		void init();

		void handleRead(Special *op);
		void handleReadAll();
		void handleWrite(Special *op);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Special *modifiedSpecial = 0);
		void handleRestore(SessionMessage *msg);
		void handleCreate(Special *op);
		void handleReadAllGraphicNames();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SPECIALSMANAGER_H
