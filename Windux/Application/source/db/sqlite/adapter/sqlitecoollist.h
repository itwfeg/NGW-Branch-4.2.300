#ifndef SQLITECOOLLIST_H
#define SQLITECOOLLIST_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCOOLLISTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListTextI;


	class SqliteCoolList : public DBCoolListI
	{
		public:

			// Types and subtypes
			typedef DBCoolListI interface_type;


			// Constructors and destructor
			SqliteCoolList(SqliteRecord* r, DBCoolListI* d = 0);
			~SqliteCoolList();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolListsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolListI
			virtual DBCoolListI* clone() const { return new SqliteCoolList(new SqliteRecord(*rec_)); }
			virtual const DBCoolListI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getListKey() const { return rec_->getField(LISTKEY_FIELD); }
			virtual void setListKey(const std::string& key) { rec_->setField(LISTKEY_FIELD, key); }
			virtual bool isListKeyNull() const { return rec_->isFieldNull(LISTKEY_FIELD); }
			virtual bool isListKeyWritable() const { return true; }

			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual void setNumberNull() { rec_->setFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual std::string getFirstLinkNumber() const { return rec_->getField(FIRSTLINKNUMBER_FIELD); }
			virtual void setFirstLinkNumber(const std::string& first) { rec_->setField(FIRSTLINKNUMBER_FIELD, first); }
			virtual bool isFirstLinkNumberNull() const { return rec_->isFieldNull(FIRSTLINKNUMBER_FIELD); }
			virtual void setFirstLinkNumberNull() { rec_->setFieldNull(FIRSTLINKNUMBER_FIELD); }
			virtual bool isFirstLinkNumberWritable() const { return true; }

			virtual bool containsCoolListText(const DBCoolListTextI* text) const;


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string LISTKEY_FIELD;
			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string FIRSTLINKNUMBER_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolListI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolList(const SqliteCoolList& rec);
			// Do not allow the default assignment operator.
			SqliteCoolList operator=(const SqliteCoolList& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLLIST_H
