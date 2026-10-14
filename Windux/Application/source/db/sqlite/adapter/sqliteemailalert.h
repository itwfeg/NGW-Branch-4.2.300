#ifndef SQLITEEmailAlert_H
#define SQLITEEmailAlert_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBEMAILALERTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteEmailAlert : public DBEmailAlertI
	{
		public:

			// Types and subtypes
			typedef DBEmailAlertI interface_type;


			// Constructors and destructor
			SqliteEmailAlert(SqliteRecord* r, DBEmailAlertI* d = 0);
			~SqliteEmailAlert();


			// Think about replacing this method with two friend functions from the
			// SqliteEmailAlertsRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBEmailAlertI
			virtual DBEmailAlertI* clone() const { return new SqliteEmailAlert(new SqliteRecord(*rec_)); }
			virtual const DBEmailAlertI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual bool getNotifyOnFailure() const { return util_string::stringtobool(rec_->getField(NOTIFY_ON_FAILURE_FIELD)); }
			virtual void setNotifyOnFailure(bool notify) { rec_->setField(NOTIFY_ON_FAILURE_FIELD, Bool::toString(notify)); }
			virtual bool isNotifyOnFailureNull() const { return rec_->isFieldNull(NOTIFY_ON_FAILURE_FIELD); }
			virtual void setNotifyOnFailureNull() { rec_->setFieldNull(NOTIFY_ON_FAILURE_FIELD); }
			virtual bool isNotifyOnFailureWritable() const { return true; }

			virtual bool getNotifyOnResolution() const { return util_string::stringtobool(rec_->getField(NOTIFY_ON_RESOLUTION_FIELD)); }
			virtual void setNotifyOnResolution(bool notify) { rec_->setField(NOTIFY_ON_RESOLUTION_FIELD, Bool::toString(notify)); }
			virtual bool isNotifyOnResolutionNull() const { return rec_->isFieldNull(NOTIFY_ON_RESOLUTION_FIELD); }
			virtual void setNotifyOnResolutionNull() { rec_->setFieldNull(NOTIFY_ON_RESOLUTION_FIELD); }
			virtual bool isNotifyOnResolutionWritable() const { return true; }

			virtual bool getNotifyHandsFreeDeviceOnFailure() const { return util_string::stringtobool(rec_->getField(NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD)); }
			virtual void setNotifyHandsFreeDeviceOnFailure(bool notify) { rec_->setField(NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD, Bool::toString(notify)); }
			virtual bool isNotifyHandsFreeDeviceOnFailureNull() const { return rec_->isFieldNull(NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD); }
			virtual void setNotifyHandsFreeDeviceOnFailureNull() { rec_->setFieldNull(NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD); }
			virtual bool isNotifyHandsFreeDeviceOnFailureWritable() const { return true; }

			virtual bool getNotifyHandsFreeDeviceOnResolution() const { return util_string::stringtobool(rec_->getField(NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD)); }
			virtual void setNotifyHandsFreeDeviceOnResolution(bool notify) { rec_->setField(NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD, Bool::toString(notify)); }
			virtual bool isNotifyHandsFreeDeviceOnResolutionNull() const { return rec_->isFieldNull(NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD); }
			virtual void setNotifyHandsFreeDeviceOnResolutionNull() { rec_->setFieldNull(NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD); }
			virtual bool isNotifyHandsFreeDeviceOnResolutionWritable() const { return true; }

			virtual int getDelay() const { return rec_->getField(DELAY_FIELD); }
			virtual void setDelay(int delay) { rec_->setField(DELAY_FIELD, delay); }
			virtual bool isDelayNull() const { return rec_->isFieldNull(DELAY_FIELD); }
			virtual void setDelayNull() { rec_->setFieldNull(DELAY_FIELD); }
			virtual bool isDelayWritable() const { return true; }

			virtual bool getFailureSent() const { return util_string::stringtobool(rec_->getField(FAILURESENT_FIELD)); }
			virtual void setFailureSent(bool sent) { rec_->setField(FAILURESENT_FIELD, Bool::toString(sent)); }
			virtual bool isFailureSentNull() const { return rec_->isFieldNull(FAILURESENT_FIELD); }
			virtual void setFailureSentNull() { rec_->setFieldNull(FAILURESENT_FIELD); }
			virtual bool isFailureSentWritable() const { return true; }

			virtual std::string getSubject1() const { return rec_->getField(SUBJECT1_FIELD); }
			virtual void setSubject1(const std::string& subject) { rec_->setField(SUBJECT1_FIELD, subject); }
			virtual bool isSubject1Null() const { return rec_->isFieldNull(SUBJECT1_FIELD); }
			virtual void setSubject1Null() { rec_->setFieldNull(SUBJECT1_FIELD); }
			virtual bool isSubject1Writable() const { return true; }

			virtual std::string getSubject2() const { return rec_->getField(SUBJECT2_FIELD); }
			virtual void setSubject2(const std::string& subject) { rec_->setField(SUBJECT2_FIELD, subject); }
			virtual bool isSubject2Null() const { return rec_->isFieldNull(SUBJECT2_FIELD); }
			virtual void setSubject2Null() { rec_->setFieldNull(SUBJECT2_FIELD); }
			virtual bool isSubject2Writable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string NOTIFY_ON_FAILURE_FIELD;
			static const std::string NOTIFY_ON_RESOLUTION_FIELD;
			static const std::string NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD;
			static const std::string NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD;
			static const std::string DELAY_FIELD;
			static const std::string FAILURESENT_FIELD;
			static const std::string SUBJECT1_FIELD;
			static const std::string SUBJECT2_FIELD;


		private:

			SqliteRecord* rec_;
			DBEmailAlertI* decorated_;

			// Do not allow the default copy constructor.
			SqliteEmailAlert(const SqliteEmailAlert& rec);
			// Do not allow the default assignment operator.
			SqliteEmailAlert operator=(const SqliteEmailAlert& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEEmailAlert_H
