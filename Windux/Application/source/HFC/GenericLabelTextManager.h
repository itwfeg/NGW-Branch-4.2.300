/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GENERICLABELTEXTMANAGER_H
#define GENERICLABELTEXTMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "GenericTextEvent.h"
#include "GenericLabelText.h"

class SessionMessage;
class DBSession;

#include "DBLabelTextsRM.h"
#include "DBLabelText.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class GenericLabelTextManager : public DataManager
{
	public:

		GenericLabelTextManager();
		GenericLabelTextManager(hfc::HFCManagerContainer *container);
		virtual ~GenericLabelTextManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		GenericLabelText* getFirstRecord() { return firstGenericLabelText(); }
		GenericLabelText* getNextRecord()  { return nextGenericLabelText(); }
		EvolDeviceType getDevice() const { return GenericTextDev; }

		GenericLabelText *firstGenericLabelText();
		GenericLabelText *nextGenericLabelText();
		int countAllGenericLabelTexts();

		GenericLabelText *getCurrentGenericLabelText();
		void setCurrentGenericLabelText(GenericLabelText *newGenericLabelText);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleSaveRecord(GenericLabelText *recordToSave);

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		GenericLabelText *currentGenericLabelText;
		short maxLabelTexts;

		DBQuery<edb::DBLabelText> *allGenericLabelTextsQuery;
		DBQuery<edb::DBLabelText>::const_iterator *allGenericLabelTextsIter;

		void init();

		void handleRead(GenericLabelText *msgGenericText);
		void handleReadAll(UINT8 event);
		void handleWrite(GenericLabelText *modifiedText);
		void handleUpdateAndSave(GenericLabelText *modifiedText);
		void handleRestore(SessionMessage *msg);
		void handleCreate(GenericLabelText *modifiedText);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //GENERICLABELTEXTMANAGER_H
