/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TOTALSMANAGER_H
#define TOTALSMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "TotalType.h"
#include "TotalsEvent.h"

class SessionMessage;
class DBSession;

class ProductTotal;
class ClassTotal;
class OperatorTotal;
class HourlyTotal;
class RunModeTotal;

#include "DBProductTotalsRM.h"
#include "DBProductTotal.h"
#include "DBClassTotalsRM.h"
#include "DBClassTotal.h"
#include "DBOperatorTotalsRM.h"
#include "DBOperatorTotal.h"
#include "DBHourlyTotalsRM.h"
#include "DBHourlyTotal.h"
#include "DBCoolTotalsRM.h"
#include "DBCoolTotal.h"

#include "DBProductsRM.h"
#include "DBClassesRM.h"
#include "DBProductStatisticsRM.h"

#include "DBTotalTypesRM.h"
#include "DBTotalType.h"
#include "DBProduct.h"
#include "DBClass.h"
#include "DBQuery.h"

namespace hfc
{
	class HFCManagerContainer;
}


class TotalsManager : public DataManager
{
	public:

		TotalsManager();
		TotalsManager(hfc::HFCManagerContainer *container);
		virtual ~TotalsManager();

		DBSession *getMySession();
		void setMySession(DBSession *session);

		/* DataManager virtual methods. */
		//TotalType* getFirstRecord() { return firstTotalType(); }
		//TotalType* getNextRecord()  { return nextTotalType(); }
		//EvolDeviceType getDevice() const { return TotalTypeDev; }

		void handleMessage(SessionMessage *msg);

		void addTotalsToDatabase(RunModeTotal *currentTotal);

		ProductTotal *getCurrentProductTotal();
		void setCurrentProductTotal(ProductTotal *newProductTotal);
		ProductTotal *firstProductTotal();
		ProductTotal *nextProductTotal();
		int countAllProductTotals();
		void saveToProductTotals(RunModeTotal *currentTotal);
		void saveToProductTotalsPerType(RunModeTotal *currentTotal, edb::DBProduct* dbProduct, edb::DBTotalType* dbTotalType);
		void handleDeleteAllProductTotals();

		ClassTotal *getCurrentClassTotal();
		void setCurrentClassTotal(ClassTotal *newClassTotal);
		ClassTotal *firstClassTotal();
		ClassTotal *nextClassTotal();
		int countAllClassTotals();
		void saveToClassTotals(HobRecordNum classNum, RunModeTotal *currentTotal, edb::DBTotalType *theDbTotalType);
		void saveToClassTotalsPerType(RunModeTotal *currentTotal, edb::DBClass* dbClass, edb::DBTotalType* dbTotalType);
		void handleDeleteAllClassTotals();

		OperatorTotal *getCurrentOperatorTotal();
		void setCurrentOperatorTotal(OperatorTotal *newOperatorTotal);
		OperatorTotal *firstOperatorTotal();
		OperatorTotal *nextOperatorTotal();
		int countAllOperatorTotals();
		void saveToOperatorTotals(RunModeTotal *currentTotal);
		void handleDeleteAllOperatorTotals();

		HourlyTotal *getCurrentHourlyTotal();
		void setCurrentHourlyTotal(HourlyTotal *newHourlyTotal);
		HourlyTotal *firstHourlyTotal();
		HourlyTotal *nextHourlyTotal();
		int countAllHourlyTotals();
		void saveToHourlyTotals(RunModeTotal *currentTotal);
		void handleDeleteAllHourlyTotals();

		void destroyQueriesAndIters();

		void saveToCoolTotals(RunModeTotal *currentTotal);

		//	AWL-52
		void tempSwitchToThisTotalType(HobRecordNum totalTypeNum);
		void revertBackToPrevTotalType();
		void setCurrentRunModeTotalType(TotalType *newTotalType);


	private:

		DBSession *mySession;
		hfc::HFCManagerContainer *mgrContainer_;

		void init();

		void handleDeleteAllTotals();


		ProductTotal *currentProductTotal;

		edb::DBQuery<edb::DBProductTotal> *allProductTotalsQuery;
		edb::DBQuery<edb::DBProductTotal>::const_iterator *allProductTotalsIter;

		void handleReadProductTotal(SessionMessage *msg);
		void handleWriteProductTotal(SessionMessage *msg);
		void handleDeleteProductTotal(SessionMessage *msg);
		void handleSaveProductTotal(SessionMessage *msg);
		void handleRestoreProductTotal(SessionMessage *msg);
		void handleVoidProductTotal(SessionMessage *msg);
		void handleReadProductTotalAllTypes(SessionMessage *msg);
		void handleDeleteProductTotalAllTypes(SessionMessage *msg);

		void handleDeleteAllProductAndClassTotals();


		ClassTotal *currentClassTotal;

		edb::DBQuery<edb::DBClassTotal> *allClassTotalsQuery;
		edb::DBQuery<edb::DBClassTotal>::const_iterator *allClassTotalsIter;

		void handleReadClassTotal(SessionMessage *msg);
		void handleWriteClassTotal(SessionMessage *msg);
		void handleDeleteClassTotal(SessionMessage *msg);
		void handleSaveClassTotal(SessionMessage *msg);
		void handleRestoreClassTotal(SessionMessage *msg);
		void handleVoidClassTotal(SessionMessage *msg);
		void handleReadClassTotalAllTypes(SessionMessage *msg);
		void handleDeleteClassTotalAllTypes(SessionMessage *msg);


		OperatorTotal *currentOperatorTotal;

		edb::DBQuery<edb::DBOperatorTotal> *allOperatorTotalsQuery;
		edb::DBQuery<edb::DBOperatorTotal>::const_iterator *allOperatorTotalsIter;

		void handleReadOperatorTotal(SessionMessage *msg);
		void handleReadAllOperatorTotals();
		void handleWriteOperatorTotal(SessionMessage *msg);
		void handleDeleteOperatorTotal(SessionMessage *msg);
		void handleSaveOperatorTotal(SessionMessage *msg);
		void handleRestoreOperatorTotal(SessionMessage *msg);
		void handleVoidOperatorTotal(SessionMessage *msg);


		HourlyTotal *currentHourlyTotal;

		edb::DBQuery<edb::DBHourlyTotal> *allHourlyTotalsQuery;
		edb::DBQuery<edb::DBHourlyTotal>::const_iterator *allHourlyTotalsIter;

		void handleReadHourlyTotal(SessionMessage *msg);
		void handleReadAllHourlyTotals();
		void handleWriteHourlyTotal(SessionMessage *msg);
		void handleDeleteHourlyTotal(SessionMessage *msg);
		void handleSaveHourlyTotal(SessionMessage *msg);
		void handleRestoreHourlyTotal(SessionMessage *msg);
		void handleVoidHourlyTotal(SessionMessage *msg);

		void destroyProductTotalsQueryAndIter();
		void destroyClassTotalsQueryAndIter();
		void destroyOperatorTotalsQueryAndIter();
		void destroyHourlyTotalsQueryAndIter();

		void handleDeleteAllCOOLTotals();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //TOTALSMANAGER_H
