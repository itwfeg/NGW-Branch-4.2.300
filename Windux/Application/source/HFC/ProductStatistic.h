/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTSTATISTIC_H
#define PRODUCTSTATISTIC_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <list>

class DBAccess;
namespace edb
{
class DBProductStatistic;
}

typedef enum ProductStatisticFieldOffsets
{
	fieldOffsetPSProductNum = 1,
	fieldOffsetPSLanguage,
	fieldOffsetPSHourNum,
	fieldOffsetPSSelectedCount,
	fieldOffsetPSPrintedCount,
	fieldOffsetPSSelectedTime,
	fieldOffsetPSRecordedDateTime,
	fieldOffsetPSDeptNum,

	endProductStatisticFieldOffsets
} ProductStatisticFieldOffsets_t;


class ProductStatistic : public HfcObject
{
	public:
		ProductStatistic();
		ProductStatistic(const char *data, int len, int formatType);
		ProductStatistic(edb::DBProductStatistic *dbProductStatistic, DBAccess *access = (DBAccess *)NULL);

		~ProductStatistic();


		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobRecordNum getProductNum() const;
		void setProductNum(HobRecordNum num);

		const HobString &getLanguage() const;
		void setLanguage(const HobString &lang);

		int getHourNum() const;
		void setHourNum(int hour);

		int getSelectedCount() const;
		void setSelectedCount(int count);

		int getSelectedTime() const;
		void setSelectedTime(int selTime);

		int getPrintedCount() const;
		void setPrintedCount(int count);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);

		HobDateTime getRecordedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(recordedDateTime_); }
		const std::string& getRecordedDateTimeAsString() const { return recordedDateTime_; }
		void setRecordedDateTime(const std::string& dt) { recordedDateTime_ = dt; }
		void setRecordedDateTime(const HobDateTime& dt = HobDateTime()) { recordedDateTime_ = dt.convertToDBFormat(); }


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const ProductStatistic *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord(); 

		static ProductStatistic *findRecord(HobRecordNum productNum, HobRecordNum deptNum, const std::string &lang, int hour, DBAccess *dbAccess);

		static void initFieldLists();


	private:

		HobRecordNum deptNum;
		HobRecordNum productNum;

		HobString language;

		int hourNum;

		int selectedCount;

		int selectedTime;

		int printedCount;

		std::string recordedDateTime_;

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBProductStatistic *myDbRecord; 

		static std::list<ProductStatisticFieldOffsets> evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTSTATISTIC_H




