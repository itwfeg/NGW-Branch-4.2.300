#ifndef SQLITECOOLCOUNTRY_H
#define SQLITECOOLCOUNTRY_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBCOOLCOUNTRYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolCountry : public DBCoolCountryI
	{
		public:

			// Types and subtypes
			typedef DBCoolCountryI interface_type;

			
			// Constructors and destructor
			SqliteCoolCountry(SqliteRecord* r, DBCoolCountryI* d = 0);
			~SqliteCoolCountry();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolCountriesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolCountryI
			virtual DBCoolCountryI* clone() const { return new SqliteCoolCountry(new SqliteRecord(*rec_)); }
			virtual const DBCoolCountryI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getCountryKey() const { return rec_->getField(COUNTRYKEY_FIELD); }
			virtual void setCountryKey(const std::string& key) { rec_->setField(COUNTRYKEY_FIELD, key); }
			virtual bool isCountryKeyNull() const { return rec_->isFieldNull(COUNTRYKEY_FIELD); }
			virtual bool isCountryKeyWritable() const { return true; }

			virtual std::string getName1() const { return rec_->getField(NAME1_FIELD); }
			virtual void setName1(const std::string& desc) { rec_->setField(NAME1_FIELD, desc); }
			virtual bool isName1Null() const { return rec_->isFieldNull(NAME1_FIELD); }
			virtual void setName1Null() { rec_->setFieldNull(NAME1_FIELD); }
			virtual bool isName1Writable() const { return true; }

			virtual std::string getName2() const { return rec_->getField(NAME2_FIELD); }
			virtual void setName2(const std::string& desc) { rec_->setField(NAME2_FIELD, desc); }
			virtual bool isName2Null() const { return rec_->isFieldNull(NAME2_FIELD); }
			virtual void setName2Null() { rec_->setFieldNull(NAME2_FIELD); }
			virtual bool isName2Writable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual bool getAllCategories() const { return util_string::stringtobool(rec_->getField(ALLCATEGORIES_FIELD)); }
			virtual void setAllCategories(bool all) { rec_->setField(ALLCATEGORIES_FIELD, Bool::toString(all)); }
			virtual bool isAllCategoriesNull() const { return rec_->isFieldNull(ALLCATEGORIES_FIELD); }
			virtual void setAllCategoriesNull() { rec_->setFieldNull(ALLCATEGORIES_FIELD); }
			virtual bool isAllCategoriesWritable() const { return true; }

			virtual std::string getCountryNumber() const { return rec_->getField(COUNTRYNUM_FIELD); }
			virtual void setCountryNumber(const std::string& key) { rec_->setField(COUNTRYNUM_FIELD, key); }
			virtual bool isCountryNumberNull() const { return rec_->isFieldNull(COUNTRYNUM_FIELD); }
			virtual bool isCountryNumberWritable() const { return true; }

			virtual std::string getKey() const { return rec_->getField(COUNTRYKEY_FIELD); }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string COUNTRYKEY_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;
			static const std::string ALLCATEGORIES_FIELD;
			static const std::string COUNTRYNUM_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolCountryI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolCountry(const SqliteCoolCountry& rec);
			// Do not allow the default assignment operator.
			SqliteCoolCountry operator=(const SqliteCoolCountry& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCOUNTRY_H
