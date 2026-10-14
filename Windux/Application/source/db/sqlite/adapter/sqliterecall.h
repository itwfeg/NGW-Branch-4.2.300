#ifndef SQLITERECALL_H
#define SQLITERECALL_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBRECALLI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteRecall : public DBRecallI
	{
		public:

			// Types and subtypes
			typedef DBRecallI interface_type;

			
			// Constructors and destructor
			SqliteRecall(SqliteRecord* r, DBRecallI* d = 0);
			~SqliteRecall();


			// Think about replacing this method with two friend functions from the
			// SqliteRecallsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBRecallI
			virtual DBRecallI* clone() const { return new SqliteRecall(new SqliteRecord(*rec_)); }
			virtual const DBRecallI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& oper) { rec_->setField(NUMBER_FIELD, oper); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual std::string getName() const { return rec_->getField(NAME_FIELD); }
			virtual void setName(const std::string& name) { rec_->setField(NAME_FIELD, name); }
			virtual bool isNameNull() const { return rec_->isFieldNull(NAME_FIELD); }
			virtual void setNameNull() { rec_->setFieldNull(NAME_FIELD); }
			virtual bool isNameWritable() const { return true; }

			virtual std::string getText() const { return rec_->getField(TEXT_FIELD); }
			virtual void setText(const std::string& text) { rec_->setField(TEXT_FIELD, text); }
			virtual bool isTextNull() const { return rec_->isFieldNull(TEXT_FIELD); }
			virtual void setTextNull() { rec_->setFieldNull(TEXT_FIELD); }
			virtual bool isTextWritable() const { return true; }

			virtual std::string getReceivedDateTime() const { return rec_->getField(RECEIVEDDATETIME_FIELD); }
			virtual void setReceivedDateTime(const std::string& date) { rec_->setField(RECEIVEDDATETIME_FIELD, date); }
			virtual bool isReceivedDateTimeNull() const { return rec_->isFieldNull(RECEIVEDDATETIME_FIELD); }
			virtual void setReceivedDateTimeNull() { rec_->setFieldNull(RECEIVEDDATETIME_FIELD); }
			virtual bool isReceivedDateTimeWritable() const { return true; }
			
			virtual bool getAcknowledged() const { return util_string::stringtobool(rec_->getField(ACKNOWLEDGED_FIELD)); }
			virtual void setAcknowledged(bool ack) { rec_->setField(ACKNOWLEDGED_FIELD, Bool::toString(ack)); }
			virtual bool isAcknowledgedNull() const { return rec_->isFieldNull(ACKNOWLEDGED_FIELD); }
			virtual void setAcknowledgedNull() { rec_->setFieldNull(ACKNOWLEDGED_FIELD); }
			virtual bool isAcknowledgedWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string NAME_FIELD;
			static const std::string TEXT_FIELD;
			static const std::string RECEIVEDDATETIME_FIELD;
			static const std::string ACKNOWLEDGED_FIELD;


		private:

			SqliteRecord* rec_;
			DBRecallI* decorated_;

			// Do not allow the default copy constructor.
			SqliteRecall(const SqliteRecall& rec);
			// Do not allow the default assignment operator.
			SqliteRecall operator=(const SqliteRecall& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITERECALL_H
