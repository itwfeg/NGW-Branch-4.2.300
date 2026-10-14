#ifndef SQLITELABEL_H
#define SQLITELABEL_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBLABELI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteLabel : public DBLabelI
	{
		public:

			// Types and subtypes
			typedef DBLabelI interface_type;


			// Constructors and destructor
			SqliteLabel(SqliteRecord* r, DBLabelI* d = 0);
			~SqliteLabel();


			// Think about replacing this method with two friend functions from the
			// SqliteLabelsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBLabelI
			virtual DBLabelI* clone() const { return new SqliteLabel(new SqliteRecord(*rec_)); }
			virtual const DBLabelI* getDecorated() const { return decorated_; }

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

			virtual std::string getBinary() const { return rec_->getField(BINARY_FIELD); }
			virtual void setBinary(const std::string& binary) { rec_->setField(BINARY_FIELD, binary); }
			virtual bool isBinaryNull() const { return rec_->isFieldNull(BINARY_FIELD); }
			virtual void setBinaryNull() { rec_->setFieldNull(BINARY_FIELD); }
			virtual bool isBinaryWritable() const { return true; }

			virtual std::string getRotation() const { return rec_->getField(ROTATION_FIELD); }
			virtual void setRotation(const std::string& binary) { rec_->setField(ROTATION_FIELD, binary); }
			virtual bool isRotationNull() const { return rec_->isFieldNull(ROTATION_FIELD); }
			virtual void setRotationNull() { rec_->setFieldNull(ROTATION_FIELD); }
			virtual bool isRotationWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string DEPTNUM_FIELD;
			static const std::string BINARY_FIELD;
			static const std::string ROTATION_FIELD;


		private:

			SqliteRecord* rec_;
			DBLabelI* decorated_;

			// Do not allow the default copy constructor.
			SqliteLabel(const SqliteLabel& rec);
			// Do not allow the default assignment operator.
			SqliteLabel operator=(const SqliteLabel& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITELABEL_H
