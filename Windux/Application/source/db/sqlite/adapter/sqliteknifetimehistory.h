#ifndef SQLITEKNIFETIMEHISTORY_H
#define SQLITEKNIFETIMEHISTORY_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBKNIFETIMEHISTORYI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif

	

	class SqliteKnifeTimeHistory : public DBKnifeTimeHistoryI
	{
		public:

			// Types and subtypes
			typedef DBKnifeTimeHistoryI interface_type;

			
			// Constructors and destructor
			SqliteKnifeTimeHistory(SqliteRecord* r, DBKnifeTimeHistoryI* d = 0);
			~SqliteKnifeTimeHistory();


			// Think about replacing this method with two friend functions from the
			// SqliteKnifeTimeHistoryRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBKnifeTimeHistoryI
			virtual DBKnifeTimeHistoryI* clone() const { return new SqliteKnifeTimeHistory(new SqliteRecord(*rec_)); }
			virtual const DBKnifeTimeHistoryI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(const std::string& type){ rec_->setField(TYPE_FIELD, type); };
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }			

			virtual int getKnifeTime() const { return rec_->getField(KNIFETIME_FIELD); }
			virtual void setKnifeTime(int knifeTime) { rec_->setField(KNIFETIME_FIELD, knifeTime); }
			virtual bool isKnifeTimeNull() const { return rec_->isFieldNull(KNIFETIME_FIELD); }
			virtual void setKnifeTimeNull() { rec_->setFieldNull(KNIFETIME_FIELD); }
			virtual bool isKnifeTimeWritable() const { return true; }

			virtual std::string getTimeStampDateTime() const { return rec_->getField(KNIFETIMESTAMP_FIELD); }
			virtual void setTimeStampDateTime(const std::string& timeStamp) { rec_->setField(KNIFETIMESTAMP_FIELD, timeStamp); };
			virtual bool isTimeStampDateTimeNull() const { return rec_->isFieldNull(KNIFETIMESTAMP_FIELD); }
			virtual void setTimeStampDateTimeNull(){ rec_->setFieldNull(KNIFETIMESTAMP_FIELD); }
			virtual bool isTimeStampDateTimeWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }

			
			static const std::string TYPE_FIELD;
			static const std::string KNIFETIME_FIELD;
			static const std::string KNIFETIMESTAMP_FIELD;


		private:

			SqliteRecord* rec_;
			DBKnifeTimeHistoryI* decorated_;

			// Do not allow the default copy constructor.
			SqliteKnifeTimeHistory(const SqliteKnifeTimeHistory& rec);
			// Do not allow the default assignment operator.
			SqliteKnifeTimeHistory operator=(const SqliteKnifeTimeHistory& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif
