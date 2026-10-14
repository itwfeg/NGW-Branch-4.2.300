#ifndef PRODUCTTOTAL_H
#define PRODUCTTOTAL_H



#include "Total.h"


#include "tags/Tags.h"



namespace hfc
{


	class ProductTotal : public Total
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetTotalProductNum = Total::endTotalFieldOffsets,
				fieldOffsetTotalTransactionTypeNum,
				fieldOffsetTotalReportingClassNum,
				fieldOffsetTotalDeptNum,

				endProductTotalFieldOffsets
			} ProductTotalFieldOffsets;


			/* --- Constructors and destructor --- */
			ProductTotal(HobRecordNum deptNum, HobRecordNum pluNumber, HobRecordNum transTypeNumber);
			~ProductTotal();


			/* --- Getter and setter methods --- */
			HobRecordNum getDeptNumber() const { return deptNum_; }
			void setDeptNumber(HobRecordNum num) { deptNum_ = num; }

			HobRecordNum getProductNumber() const { return productNumber_; }
			void setProductNumber(HobRecordNum number) { productNumber_ = number; }

			HobRecordNum getTransactionTypeNumber() const { return transTypeNumber_; }
			void setTransactionTypeNumber(HobRecordNum number) { transTypeNumber_ = number; }

			HobRecordNum getReportingClassNumber() const { return reportingClassNumber_; }
			void setReportingClassNumber(HobRecordNum number) { reportingClassNumber_ = number; }


			/* --- Instance function members --- */
			void getFieldAsString(int fieldCode, HobString &result, int format) const;
			const char *serialize(int format) const;

			bool isRecordValid() const { return true; }


		private:
			/* --- Instance data members --- */
			HobRecordNum deptNum_;
			HobRecordNum productNumber_;
			HobRecordNum transTypeNumber_;
			HobRecordNum reportingClassNumber_;


			/* --- Instance function members --- */
			void init();
			void show();


			/* --- Class data members --- */
			static FieldOffsetsList _uiFieldList_, _evolFieldList_;

	#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
	#endif


			/* --- Class function members -- */
			static void initFieldLists();
	};


}



#endif //PRODUCTTOTAL_H
