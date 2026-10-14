#ifndef SQLITECOOLLISTTEXT_H
#define SQLITECOOLLISTTEXT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBCOOLLISTTEXTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteCoolListText : public DBCoolListTextI
	{
		public:

			// Types and subtypes
			typedef DBCoolListTextI interface_type;


			// Constructors and destructor
			SqliteCoolListText(SqliteRecord* r, DBCoolListTextI* d = 0);
			~SqliteCoolListText();


			// Think about replacing this method with two friend functions from the
			// SqliteCoolListTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBCoolListTextI
			virtual DBCoolListTextI* clone() const { return new SqliteCoolListText(new SqliteRecord(*rec_)); }
			virtual const DBCoolListTextI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getCoolListKey() const { return rec_->getField(COOLLISTKEY_FIELD); }
			virtual void setCoolListKey(const std::string& type) { rec_->setField(COOLLISTKEY_FIELD, type); }
			virtual bool isCoolListKeyNull() const { return rec_->isFieldNull(COOLLISTKEY_FIELD); }
			virtual bool isCoolListKeyWritable() const { return true; }

			virtual std::string getCoolTextNumber() const { return rec_->getField(COOLTEXTNUMBER_FIELD); }
			virtual void setCoolTextNumber(const std::string& category) { rec_->setField(COOLTEXTNUMBER_FIELD, category); }
			virtual bool isCoolTextNumberNull() const { return rec_->isFieldNull(COOLTEXTNUMBER_FIELD); }
			virtual bool isCoolTextNumberWritable() const { return true; }

			virtual std::string getDeptNumber() const { return rec_->getField(DEPTNUM_FIELD); }
			virtual void setDeptNumber(const std::string& number) { rec_->setField(DEPTNUM_FIELD, number); }
			virtual bool isDeptNumberNull() const { return rec_->isFieldNull(DEPTNUM_FIELD); }
			virtual bool isDeptNumberWritable() const { return true; }

			virtual int getOrder() const { return rec_->getField(ORDER_FIELD); }
			virtual void setOrder(int order) { rec_->setField(ORDER_FIELD, order); }
			virtual bool isOrderNull() const { return rec_->isFieldNull(ORDER_FIELD); }
			virtual bool isOrderWritable() const { return true; }

			virtual std::string getLinkNumber() const { return rec_->getField(LINKNUMBER_FIELD); }
			virtual void setLinkNumber(const std::string& number) { rec_->setField(LINKNUMBER_FIELD, number); }
			virtual bool isLinkNumberNull() const { return rec_->isFieldNull(LINKNUMBER_FIELD); }
			virtual void setLinkNumberNull() { rec_->setFieldNull(LINKNUMBER_FIELD); }
			virtual bool isLinkNumberWritable() const { return true; }

			virtual std::string getNextLinkNumber() const { return rec_->getField(NEXTLINKNUMBER_FIELD); }
			virtual void setNextLinkNumber(const std::string& next) { rec_->setField(NEXTLINKNUMBER_FIELD, next); }
			virtual bool isNextLinkNumberNull() const { return rec_->isFieldNull(NEXTLINKNUMBER_FIELD); }
			virtual void setNextLinkNumberNull() { rec_->setFieldNull(NEXTLINKNUMBER_FIELD); }
			virtual bool isNextLinkNumberWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string COOLLISTKEY_FIELD;
			static const std::string COOLTEXTNUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string ORDER_FIELD;
			static const std::string LINKNUMBER_FIELD;
			static const std::string NEXTLINKNUMBER_FIELD;


		private:

			SqliteRecord* rec_;
			DBCoolListTextI* decorated_;

			// Do not allow the default copy constructor.
			SqliteCoolListText(const SqliteCoolListText& rec);
			// Do not allow the default assignment operator.
			SqliteCoolListText operator=(const SqliteCoolListText& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLLISTTEXT_H
