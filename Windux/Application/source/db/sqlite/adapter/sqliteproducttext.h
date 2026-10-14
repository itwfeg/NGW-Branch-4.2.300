#ifndef SQLITEPRODUCTTEXT_H
#define SQLITEPRODUCTTEXT_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBPRODUCTTEXTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteProductText : public DBProductTextI
	{
		public:

			// Types and subtypes
			typedef DBProductTextI interface_type;


			// Constructors and destructor
			SqliteProductText(SqliteRecord* r, DBProductTextI* d = 0);
			~SqliteProductText();


			// Think about replacing this method with two friend functions from the
			// SqliteProductTextsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBProductTextI
			virtual DBProductTextI* clone() const { return new SqliteProductText(new SqliteRecord(*rec_)); }
			virtual const DBProductTextI* getDecorated() const { return decorated_; }

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

			virtual std::string getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(const std::string& type) { rec_->setField(TYPE_FIELD, type); }
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getText() const { return rec_->getField(TEXT_FIELD); }
			virtual void setText(const std::string& desc) { rec_->setField(TEXT_FIELD, desc); }
			virtual bool isTextNull() const { return rec_->isFieldNull(TEXT_FIELD); }
			virtual void setTextNull() { rec_->setFieldNull(TEXT_FIELD); }
			virtual bool isTextWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string TYPE_FIELD;
			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string TEXT_FIELD;


		private:

			SqliteRecord* rec_;
			DBProductTextI* decorated_;

			// Do not allow the default copy constructor.
			SqliteProductText(const SqliteProductText& rec);
			// Do not allow the default assignment operator.
			SqliteProductText operator=(const SqliteProductText& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTTEXT_H
