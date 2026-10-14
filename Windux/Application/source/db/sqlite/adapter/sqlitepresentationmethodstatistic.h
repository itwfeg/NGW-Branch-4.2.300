#ifndef SQLITEPRESENTATIONMETHODSTATISTIC_H
#define SQLITEPRESENTATIONMETHODSTATISTIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRESENTATIONMETHODSTATISTICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqlitePresentationMethodStatistic : public DBPresentationMethodStatisticI
	{
		public:

			// Types and subtypes
			typedef DBPresentationMethodStatisticI interface_type;


			// Constructors and destructor
			SqlitePresentationMethodStatistic(SqliteRecord* r, DBPresentationMethodStatisticI* d = 0);
			~SqlitePresentationMethodStatistic();


			// Think about replacing this method with two friend functions from the
			// SqlitePresentationMethodStatisticsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBPresentationMethodStatisticI
			virtual DBPresentationMethodStatisticI* clone() const { return new SqlitePresentationMethodStatistic(new SqliteRecord(*rec_)); }
			virtual const DBPresentationMethodStatisticI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getPresentationMethodType() const { return rec_->getField(PRESENTATIONMETHODTYPE_FIELD); }
			virtual void setPresentationMethodType(const std::string& type) { rec_->setField(PRESENTATIONMETHODTYPE_FIELD, type); }
			virtual bool isPresentationMethodTypeNull() const { return rec_->isFieldNull(PRESENTATIONMETHODTYPE_FIELD); }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual std::string getLanguage() const { return rec_->getField(LANGUAGE_FIELD); }
			virtual void setLanguage(const std::string& lang) { rec_->setField(LANGUAGE_FIELD, lang); }
			virtual bool isLanguageNull() const { return rec_->isFieldNull(LANGUAGE_FIELD); }
			virtual bool isLanguageWritable() const { return true; }

			virtual std::string getHourNumber() const { return rec_->getField(HOURNUMBER_FIELD); }
			virtual void setHourNumber(const std::string& number) { rec_->setField(HOURNUMBER_FIELD, number); }
			virtual bool isHourNumberNull() const { return rec_->isFieldNull(HOURNUMBER_FIELD); }
			virtual bool isHourNumberWritable() const { return true; }

			virtual int getTotalMethodSelections() const { return rec_->getField(TOTALMETHODSELECTIONS_FIELD); }
			virtual void setTotalMethodSelections(int selections) { rec_->setField(TOTALMETHODSELECTIONS_FIELD, selections); }
			virtual void incrementTotalMethodSelections() { rec_->setField(TOTALMETHODSELECTIONS_FIELD, rec_->getField(TOTALMETHODSELECTIONS_FIELD) + 1); }
			virtual bool isTotalMethodSelectionsNull() const { return rec_->isFieldNull(TOTALMETHODSELECTIONS_FIELD); }
			virtual void setTotalMethodSelectionsNull() { rec_->setFieldNull(TOTALMETHODSELECTIONS_FIELD); }
			virtual bool isTotalMethodSelectionsWritable() const { return true; }

			virtual int getTotalCategorySelections() const { return rec_->getField(TOTALCATEGORYSELECTIONS_FIELD); }
			virtual void setTotalCategorySelections(int selections) { rec_->setField(TOTALCATEGORYSELECTIONS_FIELD, selections); }
			virtual void incrementTotalCategorySelections() { rec_->setField(TOTALCATEGORYSELECTIONS_FIELD, rec_->getField(TOTALCATEGORYSELECTIONS_FIELD) + 1); }
			virtual bool isTotalCategorySelectionsNull() const { return rec_->isFieldNull(TOTALCATEGORYSELECTIONS_FIELD); }
			virtual void setTotalCategorySelectionsNull() { rec_->setFieldNull(TOTALCATEGORYSELECTIONS_FIELD); }
			virtual bool isTotalCategorySelectionsWritable() const { return true; }

			virtual int getTotalProductSelections() const { return rec_->getField(TOTALPRODUCTSELECTIONS_FIELD); }
			virtual void setTotalProductSelections(int selections) { rec_->setField(TOTALPRODUCTSELECTIONS_FIELD, selections); }
			virtual void incrementTotalProductSelections() { rec_->setField(TOTALPRODUCTSELECTIONS_FIELD, rec_->getField(TOTALPRODUCTSELECTIONS_FIELD) + 1); }
			virtual bool isTotalProductSelectionsNull() const { return rec_->isFieldNull(TOTALPRODUCTSELECTIONS_FIELD); }
			virtual void setTotalProductSelectionsNull() { rec_->setFieldNull(TOTALPRODUCTSELECTIONS_FIELD); }
			virtual bool isTotalProductSelectionsWritable() const { return true; }

			virtual int getTotalLabelsPrinted() const { return rec_->getField(TOTALLABELSPRINTED_FIELD); }
			virtual void setTotalLabelsPrinted(int labels) { rec_->setField(TOTALLABELSPRINTED_FIELD, labels); }
			virtual void incrementTotalLabelsPrinted() { rec_->setField(TOTALLABELSPRINTED_FIELD, rec_->getField(TOTALLABELSPRINTED_FIELD) + 1); }
			virtual bool isTotalLabelsPrintedNull() const { return rec_->isFieldNull(TOTALLABELSPRINTED_FIELD); }
			virtual void setTotalLabelsPrintedNull() { rec_->setFieldNull(TOTALLABELSPRINTED_FIELD); }
			virtual bool isTotalLabelsPrintedWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string PRESENTATIONMETHODTYPE_FIELD;
			static const std::string LANGUAGE_FIELD;
			static const std::string HOURNUMBER_FIELD;
			static const std::string TOTALMETHODSELECTIONS_FIELD;
			static const std::string TOTALCATEGORYSELECTIONS_FIELD;
			static const std::string TOTALPRODUCTSELECTIONS_FIELD;
			static const std::string TOTALLABELSPRINTED_FIELD;


		private:

			SqliteRecord* rec_;
			DBPresentationMethodStatisticI* decorated_;

			// Do not allow the default copy constructor.
			SqlitePresentationMethodStatistic(const SqlitePresentationMethodStatistic& rec);
			// Do not allow the default assignment operator.
			SqlitePresentationMethodStatistic operator=(const SqlitePresentationMethodStatistic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRESENTATIONMETHODSTATISTIC_H
