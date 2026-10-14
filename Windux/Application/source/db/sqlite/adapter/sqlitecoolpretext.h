#ifndef SQLITECOOLPRETEXT_H
#define SQLITECOOLPRETEXT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBCOOLPRETEXTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolPreText : public DBCoolPreTextI
	{
		public:

			// Types and subtypes
			typedef DBCoolPreTextI interface_type;

			
			// Constructors and destructor
			SqliteCoolPreText(SqliteRecord* r, DBCoolPreTextI* d = 0);
			~SqliteCoolPreText();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolPreTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolPreTextI
			virtual DBCoolPreTextI* clone() const { return new SqliteCoolPreText(new SqliteRecord(*rec_)); }
			virtual const DBCoolPreTextI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

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

			virtual bool getAllCategories() const { return util_string::stringtobool(rec_->getField(ALLCATEGORIES_FIELD)); }
			virtual void setAllCategories(bool all) { rec_->setField(ALLCATEGORIES_FIELD, Bool::toString(all)); }
			virtual bool isAllCategoriesNull() const { return rec_->isFieldNull(ALLCATEGORIES_FIELD); }
			virtual void setAllCategoriesNull() { rec_->setFieldNull(ALLCATEGORIES_FIELD); }
			virtual bool isAllCategoriesWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;
			static const std::string ALLCATEGORIES_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolPreTextI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolPreText(const SqliteCoolPreText& rec);
			// Do not allow the default assignment operator.
			SqliteCoolPreText operator=(const SqliteCoolPreText& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLPRETEXT_H
