#ifndef SQLITECATEGORYSTATISTIC_H
#define SQLITECATEGORYSTATISTIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCATEGORYSTATISTICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCategoryStatistic : public DBCategoryStatisticI
	{
		public:

			// Types and subtypes
			typedef DBCategoryStatisticI interface_type;


			// Constructors and destructor
			SqliteCategoryStatistic(SqliteRecord* r, DBCategoryStatisticI* d = 0);
			~SqliteCategoryStatistic();


			// Think about replacing this method with two friend functions from the
			// SqliteCategoryStatisticsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCategoryStatisticI
			virtual DBCategoryStatisticI* clone() const { return new SqliteCategoryStatistic(new SqliteRecord(*rec_)); }
			virtual const DBCategoryStatisticI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getCategoryNumber() const { return rec_->getField(CATEGORYNUMBER_FIELD); }
			virtual void setCategoryNumber(const std::string& plu) { rec_->setField(CATEGORYNUMBER_FIELD, plu); }
			virtual bool isCategoryNumberNull() const { return rec_->isFieldNull(CATEGORYNUMBER_FIELD); }
			virtual bool isCategoryNumberWritable() const { return true; }

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


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string CATEGORYNUMBER_FIELD;
			static const std::string LANGUAGE_FIELD;
			static const std::string HOURNUMBER_FIELD;
			static const std::string TOTALSELECTIONS_FIELD;


		private:

			SqliteRecord* rec_;
			DBCategoryStatisticI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCategoryStatistic(const SqliteCategoryStatistic& rec);
			// Do not allow the default assignment operator.
			SqliteCategoryStatistic operator=(const SqliteCategoryStatistic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECATEGORYSTATISTIC_H
