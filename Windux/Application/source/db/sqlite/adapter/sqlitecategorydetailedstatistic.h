#ifndef SQLITECATEGORYDETAILEDSTATISTIC_H
#define SQLITECATEGORYDETAILEDSTATISTIC_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCATEGORYDETAILEDSTATISTICI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCategoryDetailedStatistic : public DBCategoryDetailedStatisticI
	{
		public:

			// Types and subtypes
			typedef DBCategoryDetailedStatisticI interface_type;


			// Constructors and destructor
			SqliteCategoryDetailedStatistic(SqliteRecord* r, DBCategoryDetailedStatisticI* d = 0);
			~SqliteCategoryDetailedStatistic();


			// Think about replacing this method with two friend functions from the
			// SqliteCategoryDetailedStatisticsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCategoryDetailedStatisticI
			virtual DBCategoryDetailedStatisticI* clone() const { return new SqliteCategoryDetailedStatistic(new SqliteRecord(*rec_)); }
			virtual const DBCategoryDetailedStatisticI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getCategoryNumber() const { return rec_->getField(CATEGORYNUMBER_FIELD); }
			virtual void setCategoryNumber(const std::string& plu) { rec_->setField(CATEGORYNUMBER_FIELD, plu); }
			virtual bool isCategoryNumberNull() const { return rec_->isFieldNull(CATEGORYNUMBER_FIELD); }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual std::string getCategoryStatisticLanguage() const { return rec_->getField(CATEGORYSTATISTICLANGUAGE_FIELD); }
			virtual void setCategoryStatisticLanguage(const std::string& language) { rec_->setField(CATEGORYSTATISTICLANGUAGE_FIELD, language); }
			virtual bool isCategoryStatisticLanguageNull() const { return rec_->isFieldNull(CATEGORYSTATISTICLANGUAGE_FIELD); }
			virtual bool isCategoryStatisticLanguageWritable() const { return true; }

			virtual std::string getCategoryStatisticHourNumber() const { return rec_->getField(CATEGORYSTATISTICHOURNUMBER_FIELD); }
			virtual void setCategoryStatisticHourNumber(const std::string& type) { rec_->setField(CATEGORYSTATISTICHOURNUMBER_FIELD, type); }
			virtual bool isCategoryStatisticHourNumberNull() const { return rec_->isFieldNull(CATEGORYSTATISTICHOURNUMBER_FIELD); }
			virtual bool isCategoryStatisticHourNumberWritable() const { return true; }

			virtual std::string getPresentationMethodType() const { return rec_->getField(PRESENTATIONMETHODTYPE_FIELD); }
			virtual void setPresentationMethodType(const std::string& type) { rec_->setField(PRESENTATIONMETHODTYPE_FIELD, type); }
			virtual bool isPresentationMethodTypeNull() const { return rec_->isFieldNull(PRESENTATIONMETHODTYPE_FIELD); }
			virtual bool isPresentationMethodTypeWritable() const { return true; }

			virtual int getSelections() const { return rec_->getField(SELECTIONS_FIELD); }
			virtual void setSelections(int selections) { rec_->setField(SELECTIONS_FIELD, selections); }
			virtual void incrementSelections() { rec_->setField(SELECTIONS_FIELD, rec_->getField(SELECTIONS_FIELD) + 1); }
			virtual bool isSelectionsNull() const { return rec_->isFieldNull(SELECTIONS_FIELD); }
			virtual void setSelectionsNull() { rec_->setFieldNull(SELECTIONS_FIELD); }
			virtual bool isSelectionsWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string CATEGORYNUMBER_FIELD;
			static const std::string CATEGORYSTATISTICLANGUAGE_FIELD;
			static const std::string CATEGORYSTATISTICHOURNUMBER_FIELD;
			static const std::string PRESENTATIONMETHODTYPE_FIELD;
			static const std::string SELECTIONS_FIELD;


		private:

			SqliteRecord* rec_;
			DBCategoryDetailedStatisticI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCategoryDetailedStatistic(const SqliteCategoryDetailedStatistic& rec);
			// Do not allow the default assignment operator.
			SqliteCategoryDetailedStatistic operator=(const SqliteCategoryDetailedStatistic& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECATEGORYDETAILEDSTATISTIC_H
