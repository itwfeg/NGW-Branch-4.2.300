/* Evolution Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CUTTINGTESTREPORT_H
#define CUTTINGTESTREPORT_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include "HobDateTime.h"
#include "DBAccess.h"
#include "CSmartPtr.h"

class CuttingTest;
class CutTestTotal;

typedef enum CuttingTestReportFieldOffsets
{
	fieldOffsetCTReportNum = 1,
	fieldOffsetCTReportBuyItem,
	fieldOffsetCTReportVendorNum,
	fieldOffsetCTReportOperator,
	fieldOffsetCTReportTotalSellWeight,
	fieldOffsetCTReportTotalShrinkWeight,
	fieldOffsetCTReportTotalItemWeight,
	fieldOffsetCTReportTotalSellYield,
	fieldOffsetCTReportTotalShrinkYield,
	fieldOffsetCTReportTotalItemYield,
	fieldOffsetCTReportAvgPricePerUnitWt,
	fieldOffsetCTReportTotalSellPrice,
	fieldOffsetCTReportTotalItemCost,
	fieldOffsetCTReportPercentProfit,
	endCuttingTestReportFieldOffsets
} CuttingTestReportFieldOffsets_t;

class CuttingTestReport : public HfcObject
{
	public:

		CuttingTestReport(CuttingTest *cutTest = 0);
		CuttingTestReport(HobString ctNum, HobString prodNum, HobString venNum, HobString opId, 
									HobPrice prodCost, HobWeight prodWeight, HobDateTime sTime, HobDateTime eTime);
		~CuttingTestReport();

		virtual void getFieldAsString(int fieldCode, HobString &result) const;

		virtual const char *serialize(int format = BrowserFormat) const;

		void add(Util::CSmartPtr<CutTestTotal> cutTestTotal, DBAccess *dbAccess);
		void finalize();


	private:

		HobString cutTestNum;
		HobString buyItem;
		HobString vendorNum;

		HobString operId;
		HobPrice itemCost, totalSellPrice, avgUnitPrice;
		HobWeight itemWeight, totalSellWt, totalShrinkWt;

		long totalSellYield, totalShrinkYield, percentProfit;

		HobDateTime startTime;
		HobDateTime endTime;

		HobList pluList;
		HobList descList;
		HobList weightList;
		HobList yieldList;
		HobList unitPriceList;
		HobList totalPriceList;

		void init();
		void initFieldLists();

		static std::list<CuttingTestReportFieldOffsets> browserFieldList;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CUTTINGTESTREPORT_H
