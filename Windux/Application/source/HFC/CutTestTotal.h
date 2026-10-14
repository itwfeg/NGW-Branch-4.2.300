#ifndef CUTTESTTOTAL_H
#define CUTTESTTOTAL_H

#include "Total.h"

#include "tags/Tags.h"

//class DBAccess;
//namespace edb 
//{
//	class DBCutTestTotal;
//}//end of namespace

class CutTestTotal : public hfc::Total
{
	public:
		/* --- Types and subtypes --- */
		typedef enum
		{
			fieldOffsetTotalCutTestNum = hfc::Total::endTotalFieldOffsets,
			fieldOffsetTotalProductNum,
			fieldOffsetTotalTransactionTypeNum,

			endCutTestTotalFieldOffsets
		} CutTestTotalFieldOffsets;


		/* --- Constructors and destructor --- */
		CutTestTotal(HobRecordNum cutTestNumber, HobRecordNum pluNumber, HobRecordNum deptNum, HobRecordNum transTypeNumber = 0);
//			CutTestTotal(RunModeTotal *runModeTotal, DBAccess *access = 0);
//			CutTestTotal(edb::DBCutTestTotal *dbCutTestTotal, DBAccess *access = 0);
//			CutTestTotal(const char *data, int len, int formatType);
		~CutTestTotal();


		/* --- Getter and setter methods --- */
		HobRecordNum getCutTestNumber() const { return cutTestNumber_; }
		void setCutTestNumber(HobRecordNum number) { cutTestNumber_ = number; }

		HobRecordNum getDeptNum() const { return deptNum_; }
		void setDeptNum(HobRecordNum deptNum) { deptNum_ = deptNum; }

		HobRecordNum getProductNumber() const { return productNumber_; }
		void setProductNumber(HobRecordNum number) { productNumber_ = number; }

		HobRecordNum getTransactionTypeNumber() const { return transTypeNumber_; }
		void setTransactionTypeNumber(HobRecordNum number) { transTypeNumber_ = number; }


		/* --- Instance function members --- */
		void getFieldAsString(int fieldCode, HobString &result, int format) const;
		const char *serialize(int format) const;

		bool isRecordValid() const { return true; }

		void show();


	private:
		/* --- Instance data members --- */
		HobRecordNum cutTestNumber_;
		HobRecordNum deptNum_;
		HobRecordNum productNumber_;
		HobRecordNum transTypeNumber_;

//		DBAccess			*dbAccess;
//		edb::DBCutTestTotal	*myDbRecord;


		/* --- Instance function members --- */
		void init();


		/* --- Class data members --- */
		static FieldOffsetsList _fieldList_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif


		/* --- Class function members -- */
		static void initFieldList();
};



#endif //CUTTESTTOTAL_H
