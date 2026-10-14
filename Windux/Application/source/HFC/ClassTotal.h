#ifndef CLASSTOTAL_H
#define CLASSTOTAL_H



#include "Total.h"


#include "tags/Tags.h"



namespace hfc
{


	class ClassTotal : public Total
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetTotalClassNum = Total::endTotalFieldOffsets,
				fieldOffsetTotalTransactionTypeNum,
				fieldOffsetTotalReportingClassNum,
				fieldOffsetTotalDeptNum,

				endClassTotalFieldOffsets
			} ClassTotalFieldOffsets;


			/* --- Constructors and destructor --- */
			ClassTotal(HobRecordNum deptNum, HobRecordNum classNumber, HobRecordNum transTypeNumber);
			~ClassTotal();


			/* --- Getter and setter methods --- */
			HobRecordNum getDeptNumber() const { return deptNum_; }
			void setDeptNumber(HobRecordNum num) { deptNum_ = num; }

			HobRecordNum getClassNumber() const { return classNumber_; }
			void setClassNumber(HobRecordNum number) { classNumber_ = number; }

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
			HobRecordNum classNumber_;
			HobRecordNum transTypeNumber_;
			HobRecordNum reportingClassNumber_;


			/* --- Instance function members --- */
			void init();


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



#endif //CLASSTOTAL_H
