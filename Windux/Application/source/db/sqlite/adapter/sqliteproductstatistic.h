#ifndef SQLITEPRODUCTSTATISTIC_H
#define SQLITEPRODUCTSTATISTIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRODUCTSTATISTICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductStatistic : public DBProductStatisticI
	{
		public:

			// Types and subtypes
			typedef DBProductStatisticI interface_type;


			// Constructors and destructor
			SqliteProductStatistic(SqliteRecord* r, DBProductStatisticI* d = 0);
			~SqliteProductStatistic();


			// Think about replacing this method with two friend functions from the
			// SqliteProductStatisticsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductStatisticI
			virtual DBProductStatisticI* clone() const { return new SqliteProductStatistic(new SqliteRecord(*rec_)); }
			virtual const DBProductStatisticI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getProductKey() const { return rec_->getField(PRODUCTKEY_FIELD); }
			virtual void setProductKey(const std::string& plu) { rec_->setField(PRODUCTKEY_FIELD, plu); }
			virtual bool isProductKeyNull() const { return rec_->isFieldNull(PRODUCTKEY_FIELD); }
			virtual bool isProductKeyWritable() const { return true; }

			virtual std::string getLanguage() const { return rec_->getField(LANGUAGE_FIELD); }
			virtual void setLanguage(const std::string& language) { rec_->setField(LANGUAGE_FIELD, language); }
			virtual bool isLanguageNull() const { return rec_->isFieldNull(LANGUAGE_FIELD); }
			virtual bool isLanguageWritable() const { return true; }

			virtual std::string getHourNumber() const { return rec_->getField(HOURNUMBER_FIELD); }
			virtual void setHourNumber(const std::string& type) { rec_->setField(HOURNUMBER_FIELD, type); }
			virtual bool isHourNumberNull() const { return rec_->isFieldNull(HOURNUMBER_FIELD); }
			virtual bool isHourNumberWritable() const { return true; }

			virtual int getTotalSelections() const { return rec_->getField(TOTALSELECTIONS_FIELD); }
			virtual void setTotalSelections(int selections) { rec_->setField(TOTALSELECTIONS_FIELD, selections); }
			virtual void incrementTotalSelections() { rec_->setField(TOTALSELECTIONS_FIELD, rec_->getField(TOTALSELECTIONS_FIELD) + 1); }
			virtual bool isTotalSelectionsNull() const { return rec_->isFieldNull(TOTALSELECTIONS_FIELD); }
			virtual void setTotalSelectionsNull() { rec_->setFieldNull(TOTALSELECTIONS_FIELD); }
			virtual bool isTotalSelectionsWritable() const { return true; }

			virtual int getTimeToSelect() const { return rec_->getField(TIMETOSELECT_FIELD); }
			virtual void setTimeToSelect(int time) { rec_->setField(TIMETOSELECT_FIELD, time); }
			virtual bool isTimeToSelectNull() const { return rec_->isFieldNull(TIMETOSELECT_FIELD); }
			virtual void setTimeToSelectNull() { rec_->setFieldNull(TIMETOSELECT_FIELD); }
			virtual bool isTimeToSelectWritable() const { return true; }

			virtual int getTotalLabelsPrinted() const { return rec_->getField(TOTALLABELSPRINTED_FIELD); }
			virtual void setTotalLabelsPrinted(int printed) { rec_->setField(TOTALLABELSPRINTED_FIELD, printed); }
			virtual void incrementTotalLabelsPrinted() { rec_->setField(TOTALLABELSPRINTED_FIELD, rec_->getField(TOTALLABELSPRINTED_FIELD) + 1); }
			virtual bool isTotalLabelsPrintedNull() const { return rec_->isFieldNull(TOTALLABELSPRINTED_FIELD); }
			virtual void setTotalLabelsPrintedNull() { rec_->setFieldNull(TOTALLABELSPRINTED_FIELD); }
			virtual bool isTotalLabelsPrintedWritable() const { return true; }

			virtual std::string getRecordedDateTime() const { return rec_->getField(RECORDEDDATETIME_FIELD); }
			virtual void setRecordedDateTime(const std::string& rdt) { rec_->setField(RECORDEDDATETIME_FIELD, rdt); }
			virtual bool isRecordedDateTimeNull() const { return rec_->isFieldNull(RECORDEDDATETIME_FIELD); }
			virtual void setRecordedDateTimeNull() { rec_->setFieldNull(RECORDEDDATETIME_FIELD); }
			virtual bool isRecordedDateTimeWritable() const { return true; }

			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRODUCTKEY_FIELD;
			static const std::string LANGUAGE_FIELD;
			static const std::string HOURNUMBER_FIELD;
			static const std::string TOTALSELECTIONS_FIELD;
			static const std::string TIMETOSELECT_FIELD;
			static const std::string TOTALLABELSPRINTED_FIELD;
			static const std::string RECORDEDDATETIME_FIELD;


		private:

			SqliteRecord* rec_;
			DBProductStatisticI* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductStatistic(const SqliteProductStatistic& rec);
			// Do not allow the default assignment operator.
			SqliteProductStatistic operator=(const SqliteProductStatistic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTSTATISTIC_H
