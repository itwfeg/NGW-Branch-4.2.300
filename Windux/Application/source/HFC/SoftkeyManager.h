/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SOFTKEYMANAGER_H
#define SOFTKEYMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "Softkey.h"
#include "SoftkeyDevEvent.h"
#include "SoftkeyScriptManager.h"

#include "DBQuery.h"
#include "DBSoftKey.h"
#include "DBSoftKeyGroup.h"
#include "DBSoftKeyFeature.h"
#include "DBSoftKeyStyle.h"
#include "DBCategory.h"

#include "DBOperatorsRM.h"

#include "HobList.h"

#include <string>
#include <vector>

class SessionMessage;
class DBSession;

class SoftkeyStyle;
class SoftkeyFeature;
class SoftkeyGroup;

class WriteSingleField;


namespace hfc
{
	class HFCManagerContainer;
	class ListCameraSystemPredictionsMessage;
}


class SoftkeyManager : public DataManager
{
	public:

		SoftkeyManager();
		SoftkeyManager(hfc::HFCManagerContainer *container);
		virtual ~SoftkeyManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		Softkey* getFirstRecord() { return firstSoftkey(); }
		Softkey* getNextRecord()  { return nextSoftkey(); }
		EvolDeviceType getDevice() const { return SoftkeyDev; }
		UINT8 getWriteEvent() const { return WriteSoftkey; }
		UINT8 getDeleteEvent() const { return DeleteSoftkey; }
		UINT8 getDeleteAllEvent() const { return DeleteAllSoftkeys; }

		void handleMessage(SessionMessage *msg);

		Softkey *firstSoftkey();
		Softkey *nextSoftkey();
		int countAllSoftkeys();
		int countAllSoftkeys(bool notDeleted);

		Softkey *getCurrentSoftkey();
		void setCurrentSoftkey(Softkey *newSoftkey);

		void handleExecute(Softkey *msgSoftkey);

		void handleOperatorLogin(Softkey *msgSoftkey);
		void handleProductDiscount(Softkey *msgSoftkey, int msgEvent);
		void handleProductResetPrice(Softkey *msgSoftkey);
		void handleProductRead(Softkey *msgSoftkey);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);
		void handleSaveRecord(Softkey *recordToSave);

		Softkey *createNewSoftkey(HobRecordNum recordNum = 0, int horizPos = 0, int verticalPos = 0);
		void sendCurrentSoftkey();

		void destroyQueriesAndIters();
		void setAccumItemsPrintKeyHit();
		void resetAccumItemsPrintKeyHit();
		bool isAccumItemsPrintKeyHit() const;


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		Softkey *currentSoftkey;
		edb::DBQuery<edb::DBSoftKey> *allSoftkeysQuery;
		edb::DBQuery<edb::DBSoftKey>::const_iterator *allSoftkeysIter;

		edb::DBQuery<edb::DBCategory> *allCategoriesQuery;
		edb::DBQuery<edb::DBCategory>::const_iterator *allCategoriesIter;

		SoftkeyGroup *currentGridData;		//	formerly currentGridData

		WriteSingleField *writeTareValue;
		bool accumItemsPrintKeyHit;  // Added for Jira GCDR-199 to determine if printing individual labels or totals if RFID and forced login

		HobList cameraPredictions;

		void init();

		void handleSoftkeyMessage(SessionMessage *msg);
		void handleRead(Softkey *msgSoftkey);
		void handleReadAll();
		void handleWrite(Softkey *modifiedSoftkey);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(Softkey *modifiedSoftkey);
		void handleRestore(SessionMessage *msg);
		void handleRestoreSoftkey(Softkey *restoreSoftkey);
		void handleRestoreFlashkeyAsSoftkey(Softkey *restoreSoftkey);
		void handleCreate(Softkey *msgSoftkey);

		void handleLoadCategorySoftkeyGroup(Softkey *msgSoftkey);

		void handleLoadProductSearchSoftkeyGroup(Softkey *msgSoftkey);
		void buildSoftkeyGridData(SoftkeyFeature *feature, SoftkeyStyle *style, edb::DBQuery<edb::DBProduct> *productsQuery, HobRecordNum categoryNum = 0);
		void handleLoadSpecialsSoftkeyGroup(Softkey *msgSoftkey);
		
		void handleSelfServeLoadCategorySoftkeyGroup(Softkey *msgSoftkey);
		void buildSelfServeSoftkeyGridData(SoftkeyFeature *feature, SoftkeyStyle *style, edb::DBQuery<edb::DBProduct> *productsQuery, HobRecordNum categoryNum = 0);
		void handleSelfServeLoadSpecialsSoftkeyGroup(Softkey *msgSoftkey);
		void handleLoadAlphabeticSearch(Softkey *msgSoftkey);
		void handleLoadMostCommonSoftkeyGroup(Softkey *msgSoftkey);
		void handleChangeLabelPlacementAndPosition(Softkey *msgSoftkey);
		void handleToggleLabelType(Softkey *msgSoftkey);

		void saveCameraSystemPredictions(hfc::ListCameraSystemPredictionsMessage *msg);
		void buildCameraSystemPredictions();

		//IR #302 -> Old style flashkey conversions
		void detectAndConvertOldStyleBackground(Softkey *msgSoftkey);
		//	PT 441 - Void transaction action from vxWorks
		void detectAndConvertVoidTransactionAction(Softkey *msgSoftkey);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SOFTKEYMANAGER_H
