#ifndef SQLITECOOLTEXT_H
#define SQLITECOOLTEXT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCOOLTEXTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolText : public DBCoolTextI
	{
		public:

			// Types and subtypes
			typedef DBCoolTextI interface_type;


			// Constructors and destructor
			SqliteCoolText(SqliteRecord* r, DBCoolTextI* d = 0);
			~SqliteCoolText();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolTextI
			virtual DBCoolTextI* clone() const { return new SqliteCoolText(new SqliteRecord(*rec_)); }
			virtual const DBCoolTextI* getDecorated() const { return decorated_; }

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

			virtual std::string getText() const { return rec_->getField(COOLTEXT_FIELD); }
			virtual void setText(const std::string& desc) { rec_->setField(COOLTEXT_FIELD, desc); }
			virtual bool isTextNull() const { return rec_->isFieldNull(COOLTEXT_FIELD); }
			virtual void setTextNull() { rec_->setFieldNull(COOLTEXT_FIELD); }
			virtual bool isTextWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string COOLTEXT_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolTextI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolText(const SqliteCoolText& rec);
			// Do not allow the default assignment operator.
			SqliteCoolText operator=(const SqliteCoolText& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLTEXT_H
