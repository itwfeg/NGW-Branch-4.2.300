#ifndef SQLITENETWORKERROR_H
#define SQLITENETWORKERROR_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBNETWORKERRORI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteNetworkError : public DBNetworkErrorI
	{
		public:

			// Types and subtypes
			typedef DBNetworkErrorI interface_type;

			
			// Constructors and destructor
			SqliteNetworkError(SqliteRecord* r, DBNetworkErrorI* d = 0);
			~SqliteNetworkError();


			// Think about replacing this method with two friend functions from the
			// SqliteNetworkErrorsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBNetworkErrorI
			virtual DBNetworkErrorI* clone() const { return new SqliteNetworkError(new SqliteRecord(*rec_)); }
			virtual const DBNetworkErrorI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNetworkErrorKey() const { return rec_->getField(NETWORKERRORID_FIELD); }
			virtual void setNetworkErrorKey(const std::string& errorKey) { rec_->setField(NETWORKERRORID_FIELD, errorKey); }
			virtual bool isNetworkErrorKeyNull() const { return rec_->isFieldNull(NETWORKERRORID_FIELD); }
			virtual bool isNetworkErrorKeyWritable() const { return true; }

			// SCALEID_FIELD

			virtual std::string getErrorCode() const { return rec_->getField(ERRORCODE_FIELD); }
			virtual void setErrorCode(const std::string& code) { rec_->setField(ERRORCODE_FIELD, code); }
			virtual bool isErrorCodeNull() const { return rec_->isFieldNull(ERRORCODE_FIELD); }
			virtual void setErrorCodeNull() { rec_->setFieldNull(ERRORCODE_FIELD); }
			virtual bool isErrorCodeWritable() const { return true; }

			virtual int getRecordType() const { return rec_->getField(RECORDTYPE_FIELD); }
			virtual void setRecordType(int type) { rec_->setField(RECORDTYPE_FIELD, type); }
			virtual bool isRecordTypeNull() const { return rec_->isFieldNull(RECORDTYPE_FIELD); }
			virtual void setRecordTypeNull() { rec_->setFieldNull(RECORDTYPE_FIELD); }
			virtual bool isRecordTypeWritable() const { return true; }

			virtual std::string getDateTime() const { return rec_->getField(DATETIME_FIELD); }
			virtual void setDateTime(const std::string& date) { rec_->setField(DATETIME_FIELD, date); }
			virtual bool isDateTimeNull() const { return rec_->isFieldNull(DATETIME_FIELD); }
			virtual void setDateTimeNull() { rec_->setFieldNull(DATETIME_FIELD); }
			virtual bool isDateTimeWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NETWORKERRORID_FIELD;
			static const std::string SCALEID_FIELD;
			static const std::string ERRORCODE_FIELD;
			static const std::string RECORDTYPE_FIELD;
			static const std::string DATETIME_FIELD;


		private:

			SqliteRecord* rec_;
			DBNetworkErrorI* decorated_;

			// Do not allow the default copy constructor.
			SqliteNetworkError(const SqliteNetworkError& rec);
			// Do not allow the default assignment operator.
			SqliteNetworkError operator=(const SqliteNetworkError& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITENETWORKERROR_H
