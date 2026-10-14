/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef OPERATORNOTEMANAGER_H
#define OPERATORNOTEMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "OperatorEvent.h"
#include "OperatorNote.h"

class SessionMessage;
class DBSession;

#include "DBOperatorNotesRM.h"
#include "DBOperatorNote.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class OperatorNoteManager : public DataManager
{
	public:

		OperatorNoteManager();
		OperatorNoteManager(hfc::HFCManagerContainer *container);
		virtual ~OperatorNoteManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		OperatorNote* getFirstRecord() { return firstOperatorNote(); }
		OperatorNote* getNextRecord()  { return 0; }
		EvolDeviceType getDevice() const { return OperatorDev; }
		UINT8 getWriteEvent() const { return OpWriteNote; }
		UINT8 getDeleteAllEvent() const { return OpDeleteNote; }

		OperatorNote *firstOperatorNote();
		int countAllOperatorNotes();

		void handleMessage(SessionMessage *msg);
		void handleDelete();
		void handleDeleteAll();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		void init();

		void handleRead();
		void handleReadForEditing();
		void handleWrite(OperatorNote *note);
		void handleSave(OperatorNote *modifiedOperatorNote, bool shouldBatchIt = true);
		void handleRestore(SessionMessage *msg);

		void setAllOperatorsSawNote(bool sawNote, bool shouldBatchIt);


#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //OPERATORNOTEMANAGER_H
