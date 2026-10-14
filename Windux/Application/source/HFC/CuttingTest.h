/* Access Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CUTTINGTEST_H
#define CUTTINGTEST_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobString.h"
#include "HobDateTime.h"


#include "tags/Tags.h"


class DBAccess;

namespace edb 
{
	class DBCuttingTest;
}//end of namespace


typedef enum CutTestOptionEnabled
{
	CutTestOption_Unknown,
	CutTestOption_Classic,
	CutTestOption_Option1,
	MaxCutTestOptions
} CutTestOptionsEnabled_t;

/*
typedef enum CutTestStatus
{
	UnknownStatus,			// 0
	Status_Initial,
	Status_InProcess,
	Status_Completed,
	Status_Suspended,
	Status_Deleted,
	MaxCutTestStatus
} CutTestStatus_t;
*/

class CuttingTest : public HfcObject
{
	public:

		typedef enum CutTestStatus
		{
			UnknownStatus,			// 0
			Status_Initial,
			Status_InProcess,
			Status_Completed,
			Status_Suspended,
			Status_Deleted,
			MaxCutTestStatus
		} CutTestStatus_t;


		typedef enum
		{
			fieldOffsetCuttingTestNum = 1,
			fieldOffsetCuttingTestOperator,
			fieldOffsetCuttingTestVendorNum,
			fieldOffsetCuttingTestProductNum,
			fieldOffsetCuttingTestPrimalWeight,
			fieldOffsetCuttingTestPrimalCost,
			fieldOffsetCuttingTestStartDateTime,
			fieldOffsetCuttingTestEndDateTime,
			fieldOffsetCuttingTestLotNumber,
			fieldOffsetCuttingTestPackingDate,
			fieldOffsetCuttingTestPackingHours,
			fieldOffsetCuttingTestPackingMinutes,
			fieldOffsetCuttingTestComments,
			fieldOffsetCuttingTestFinalComments,
			fieldOffsetCuttingTestStatus,
			fieldOffsetCuttingTestPercentTotalShrinkage,
			fieldOffsetCuttingTestTotalWeightOfCutProducts,
			fieldOffsetCuttingTestTotalPriceOfCutProducts,
			endCuttingTestFieldOffsets
		} CuttingTestFieldOffsets;


		CuttingTest(int testNum = 0, DBAccess *access = 0);
		CuttingTest(const char *data, int len, int formatType);
		CuttingTest(edb::DBCuttingTest *dbCuttingTestRec, DBAccess *access = 0);

		~CuttingTest();

		static void initFieldList();

		void operator=(const CuttingTest& cutTest);

		HobRecordNum getTestNumber() const;
		void setTestNumber(HobRecordNum totalTypeNum);

		const HobString &getOperatorId() const;
		void setOperatorId(const HobString &id);

		const HobString &getVendorNumber() const;
		void setVendorNumber(const HobString &value);

		const HobString &getProductNumber() const;
		void setProductNumber(const HobString &value);

		HobWeight getPrimalWeight() const;
		void setPrimalWeight(HobWeight weight);

		HobPrice getPrimalCost() const;
		void setPrimalCost(HobPrice price);

		HobDateTime getStartDate() const;
		void setStartDate(HobDateTime date);

		HobDateTime getEndDate() const;
		void setEndDate(HobDateTime date);


		const HobString &getLotNumber() const;
		void setLotNumber(const HobString &num);

		HobDateTime getPackingDate() const;
		void setPackingDate(const HobDateTime date);

		int getPackingHours() const;
		void setPackingHours(int hours);

		int getPackingMinutes() const;
		void setPackingMinutes(int minutes);

		const HobString &getComments() const;
		void setComments(const HobString &comm);

		const HobString &getFinalComments() const;
		void setFinalComments(const HobString &finalComm);

		CuttingTest::CutTestStatus getStatus() const;
		void setStatus(CuttingTest::CutTestStatus stat);

		int getPercentTotalShrinkage() const;
		void setPercentTotalShrinkage(const int pct);

		HobWeight getTotalWeightOfCutProducts() const;
		void setTotalWeightOfCutProducts(const HobWeight weight);

		HobPrice getTotalPriceOfCutProducts() const;
		void setTotalPriceOfCutProducts(const HobPrice price);


		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		void setDBAccess(DBAccess *dbAccess);
		edb::DBCuttingTest *getMyDbRecord();
		
		static CuttingTest *findCuttingTest(HobRecordNum cuttingTestNum, DBAccess *dbAccess);

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		virtual const char *serialize(int format) const;

		void getFieldAsString(int fieldCode, HobString &result, int format = BrowserFormat) const;

		bool isFieldQuoted(int fieldOffset) const;
		
		void updateField(int fieldCode, const HobString &token);

		void update(const CuttingTest *newCuttingTest);

		void show();

		static CuttingTest *find(HobRecordNum testNum, DBAccess *dbAccess);

		void updateCutTestTotalsOption1();


	private:

		HobRecordNum	testNum;
		HobString		operatorId;
		HobString		vendorNum;
		HobString		productNum;
		HobWeight		primalWeight;
		HobPrice		primalCost;
		HobDateTime		startDate;
		HobDateTime		endDate;
		HobString		lotNumber;
		HobDateTime		packingDate;
		int				packingHours;
		int				packingMinutes;
		HobString		comments;
		HobString		finalComments;
		CutTestStatus	status;
		int				percentTotalShrinkage;
		HobWeight		totalWeightOfCutProducts;
		HobPrice		totalPriceOfCutProducts;

		HobErrorCode	errorCode;

		DBAccess			*dbAccess;
		edb::DBCuttingTest	*myDbRecord;

		void init();

		/* --- Class data members --- */
		static FieldOffsetsList _fieldList_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CUTTINGTEST_H
