#ifndef SQLITEROMLABEL_H
#define SQLITEROMLABEL_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBROMLABELI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteROMLabel : public DBROMLabelI
	{
		public:

			// Types and subtypes
			typedef DBROMLabelI interface_type;


			// Constructors and destructor
			SqliteROMLabel(SqliteRecord* r, DBROMLabelI* d = 0);
			~SqliteROMLabel();


			// Think about replacing this method with two friend functions from the
			// SqliteROMLabelsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBROMLabelI
			virtual DBROMLabelI* clone() const { return new SqliteROMLabel(new SqliteRecord(*rec_)); }
			virtual const DBROMLabelI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

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
			static const std::string BINARY_FIELD;
			static const std::string ROTATION_FIELD;


		private:

			SqliteRecord* rec_;
			DBROMLabelI* decorated_;

			// Do not allow the default copy constructor.
			SqliteROMLabel(const SqliteROMLabel& rec);
			// Do not allow the default assignment operator.
			SqliteROMLabel operator=(const SqliteROMLabel& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEROMLABEL_H
