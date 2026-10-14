/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYFEATUREMANAGER_H
#define SOFTKEYFEATUREMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "SoftkeyFeature.h"
#include "Softkey.h"
#include "SoftkeyDevEvent.h"

#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyFeature.h"
#include "DBQuery.h"

class SessionMessage;
class DBSession;

namespace hfc
{
	class HFCManagerContainer;
}


class SoftkeyFeatureManager : public DataManager
{
	public:

		SoftkeyFeatureManager();
		SoftkeyFeatureManager(hfc::HFCManagerContainer *container);
		virtual ~SoftkeyFeatureManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		SoftkeyFeature* getFirstRecord() { return firstSoftkeyFeature(); }
		SoftkeyFeature* getNextRecord()  { return nextSoftkeyFeature(); }
		EvolDeviceType getDevice() const { return SoftkeyDev; }
		UINT8 getWriteEvent() const { return WriteSoftkeyFeature; }
		UINT8 getDeleteEvent() const { return DeleteSoftkeyFeature; }
		UINT8 getDeleteAllEvent() const { return DeleteAllSoftkeyFeatures; }

		void handleMessage(SessionMessage *msg);


		SoftkeyFeature *firstSoftkeyFeature();
		SoftkeyFeature *nextSoftkeyFeature();
		int countAllSoftkeyFeatures();

		SoftkeyFeature *getCurrentSoftkeyFeature();
		void setCurrentSoftkeyFeature(SoftkeyFeature *newSoftkeyFeature);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(SoftkeyFeature *recordToSave);

		SoftkeyFeature *createNewSoftkeyFeature(HobRecordNum recordNum);
		void sendCurrentSoftkeyFeature();

		void destroyQueriesAndIters();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		SoftkeyFeature *currentSoftkeyFeature;
		edb::DBQuery<edb::DBSoftKeyFeature> *allSoftkeyFeaturesQuery;
		edb::DBQuery<edb::DBSoftKeyFeature>::const_iterator *allSoftkeyFeaturesIter;

//		bool restoreTrackingFlag;

		void init();

		void handleRead(SoftkeyFeature *msgSoftkeyFeature);
		void handleReadAll();
		void handleWrite(SoftkeyFeature *modifiedSoftkeyFeature);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(SoftkeyFeature *modifiedSoftkeyFeature);
		void handleRestore(SessionMessage *msg);
		void handleCreate(SoftkeyFeature *msgSoftkeyFeature);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYFEATUREMANAGER_H
