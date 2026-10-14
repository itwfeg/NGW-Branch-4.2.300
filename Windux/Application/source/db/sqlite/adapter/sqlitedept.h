#ifndef SQLITEDEPT_H
#define SQLITEDEPT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBDEPTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDept : public DBDeptI
	{
		public:

			// Types and subtypes
			typedef DBDeptI interface_type;

			
			// Constructors and destructor
			SqliteDept(SqliteRecord* r, DBDeptI* d = 0);
			~SqliteDept();


			// Think about replacing this method with two friend functions from the
			// SqliteDeptsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBDeptI
			virtual DBDeptI* clone() const { return new SqliteDept(new SqliteRecord(*rec_)); }
			virtual const DBDeptI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

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


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string NAME1_FIELD;
			static const std::string NAME2_FIELD;


		private:

			SqliteRecord* rec_;
			DBDeptI* decorated_;

			// Do not allow the default copy constructor.
			SqliteDept(const SqliteDept& rec);
			// Do not allow the default assignment operator.
			SqliteDept operator=(const SqliteDept& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEDEPT_H
