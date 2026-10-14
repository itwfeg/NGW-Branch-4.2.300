#ifndef SQLITECOOLCATEGORYCOUNTRY_H
#define SQLITECOOLCATEGORYCOUNTRY_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCOOLCATEGORYCOUNTRYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolCategoryCountry : public DBCoolCategoryCountryI
	{
		public:

			// Types and subtypes
			typedef DBCoolCategoryCountryI interface_type;


			// Constructors and destructor
			SqliteCoolCategoryCountry(SqliteRecord* r, DBCoolCategoryCountryI* d = 0);
			~SqliteCoolCategoryCountry();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolCategoryCountriesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolCategoryCountryI
			virtual DBCoolCategoryCountryI* clone() const { return new SqliteCoolCategoryCountry(new SqliteRecord(*rec_)); }
			virtual const DBCoolCategoryCountryI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getCountryKey() const { return rec_->getField(COUNTRYKEY_FIELD); }
			virtual void setCountryKey(const std::string& countryKey) { rec_->setField(COUNTRYKEY_FIELD, countryKey); }
			virtual bool isCountryKeyNull() const { return rec_->isFieldNull(COUNTRYKEY_FIELD); }
			virtual bool isCountryKeyWritable() const { return true; }

			virtual std::string getCategoryNumber() const { return rec_->getField(CATEGORYNUMBER_FIELD); }
			virtual void setCategoryNumber(const std::string& category) { rec_->setField(CATEGORYNUMBER_FIELD, category); }
			virtual bool isCategoryNumberNull() const { return rec_->isFieldNull(CATEGORYNUMBER_FIELD); }
			virtual bool isCategoryNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string COUNTRYKEY_FIELD;
			static const std::string CATEGORYNUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolCategoryCountryI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolCategoryCountry(const SqliteCoolCategoryCountry& rec);
			// Do not allow the default assignment operator.
			SqliteCoolCategoryCountry operator=(const SqliteCoolCategoryCountry& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORYCOUNTRY_H
