/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef LABELTYPEMANAGER_H
#define LABELTYPEMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "LabelTypeEvent.h"
#include "LabelType.h"

class SessionMessage;
class DBSession;
class HfcPrinterStockMetrics;

#include "DBLabelsRM.h"
#include "DBROMLabelsRM.h"
#include "DBLabel.h"
#include "DBROMLabel.h"
#include "DBQuery.h"

#include <list>

namespace hfc
{
	class HFCManagerContainer;
}


class LabelTypeManager : public DataManager
{
	public:

		static const bool BEST_LABELFIT	= false;
		static const bool EXACT_LABELFIT	= true;

		LabelTypeManager();
		LabelTypeManager(hfc::HFCManagerContainer *container);
		virtual ~LabelTypeManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		LabelType* getFirstRecord(HobRecordNum deptNum = RECORD_NOT_FOUND) { return firstLabelType(deptNum); }
		LabelType* getNextRecord(HobRecordNum deptNum = RECORD_NOT_FOUND)  { return nextLabelType(false, 0, deptNum); }
		EvolDeviceType getDevice() const { return LabelTypeDev; }

		HobRecordNum setPositionAtLabelType(HobRecordNum beginFromLabelTypeNum);
		LabelType *firstLabelType(HobRecordNum deptNum = RECORD_NOT_FOUND);
		LabelType *nextLabelType(bool shouldLoop = false, HobRecordNum beginFromLabelTypeNum = 0, HobRecordNum deptNum = RECORD_NOT_FOUND);
		LabelType *previousLabelType(bool shouldLoop = false, HobRecordNum beginFromLabelTypeNum = 0, HobRecordNum deptNum = RECORD_NOT_FOUND);
		LabelType *lastLabelType(HobRecordNum deptNum = RECORD_NOT_FOUND);

		LabelType *firstCustomLabelType();
		LabelType *nextCustomLabelType();

		int countAllLabelTypes();
		int countAllCustomLabelTypes();

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();
		void handleDeleteAll(HobRecordNum deptNum);

		static LabelMediaReturn validateLabelTypeWithStock(LabelStockDescriptor *labelTypeLSD, HfcPrinterStockMetrics *printerStockMetrics, bool exactFit, HobRecordNum *labelTypeNum, HobRecordNum *deptNum, HobRecordNum deptNumOfProduct, DBAccess *dbAccess);
		static LabelMediaReturn findMatchInLabelFamily(HobRecordNum *labelTypeNum, HobRecordNum *deptNum, HobRecordNum deptNumOfProduct, HfcPrinterStockMetrics *printerStockMetrics, DBAccess *dbAccess);

		void destroyQueriesAndIters();
		void destroyListAndIter();
		void destroyQueriesListAndIters();

		bool isRecordIgnored() const { return recordIgnored_; }
		void setRecordIgnored(bool value) { recordIgnored_ = value; }

		void tester();


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		// The following is used when reading label types from an ultima,
		// in which the label type may be received, via comm, in pieces.
		// The pieces need to be "assembled" before saving the label type.
		LabelType *partialLabelType;

		edb::DBQuery<edb::DBLabel> *allLabelTypesQuery;
		edb::DBQuery<edb::DBLabel>::const_iterator *allLabelTypesIter;

		std::list<HobRecordNum> allLabelTypesList;
		std::list<HobRecordNum>::iterator *allLabelTypesListIter;

		bool recordIgnored_;

		void init();

		void handleRead(LabelType *labelType);
		void handleReadAll(LabelType *labelType);
		void handleDelete(SessionMessage *msg);
		void handleUpdateAndSave(LabelType *labelType);
		void handleRestore(SessionMessage *msg);
		void handleFirst(LabelType *labelType);
		void handleLast(LabelType *labelType);
		void handleNext(LabelType *referenceLabel);
		void handlePrevious(LabelType *referenceLabel);

		void buildLabelList(HobRecordNum deptNum = 0);  // Intentionally set the default value to the 'default department'

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //LABELTYPEMANAGER_H
