#ifndef SQLITESCALE_H
#define SQLITESCALE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBSCALEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteScale : public DBScaleI
	{
		public:

			// Types and subtypes
			typedef DBScaleI interface_type;


			// Constructors and destructor
			SqliteScale(SqliteRecord* r, DBScaleI* d = 0);
			~SqliteScale();


			// Think about replacing this method with two friend functions from the
			// SqliteScalesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBScaleI
			virtual DBScaleI* clone() const { return new SqliteScale(new SqliteRecord(*rec_)); }
			virtual const DBScaleI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getIdentifier() const { return rec_->getField(IDENTIFIER_FIELD); }
			virtual bool isIdentifierNull() const { return rec_->isFieldNull(IDENTIFIER_FIELD); }
			virtual bool isIdentifierWritable() const { return true; }

			virtual std::string getHostName() const { return rec_->getField(HOSTNAME_FIELD); }
			virtual void setHostName(const std::string& hostname) { rec_->setField(HOSTNAME_FIELD, hostname); }
			virtual bool isHostNameNull() const { return rec_->isFieldNull(HOSTNAME_FIELD); }
			virtual void setHostNameNull() { rec_->setFieldNull(HOSTNAME_FIELD); }
			virtual bool isHostNameWritable() const { return true; }

			virtual std::string getIPAddress() const { return rec_->getField(IPADDRESS_FIELD); }
			virtual void setIPAddress(const std::string& ipAddress) { rec_->setField(IPADDRESS_FIELD, ipAddress); }
			virtual bool isIPAddressNull() const { return rec_->isFieldNull(IPADDRESS_FIELD); }
			virtual void setIPAddressNull() { rec_->setFieldNull(IPADDRESS_FIELD); }
			virtual bool isIPAddressWritable() const { return true; }

			virtual int getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(int type) { rec_->setField(TYPE_FIELD, type); }
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getCreationDateTime() const { return rec_->getField(CREATIONDATETIME_FIELD); }
			virtual void setCreationDateTime(const std::string& creation) { rec_->setField(CREATIONDATETIME_FIELD, creation); }
			virtual bool isCreationDateTimeNull() const { return rec_->isFieldNull(CREATIONDATETIME_FIELD); }
			virtual void setCreationDateTimeNull() { rec_->setFieldNull(CREATIONDATETIME_FIELD); }
			virtual bool isCreationDateTimeWritable() const { return true; }

			virtual int getState() const { return rec_->getField(STATE_FIELD); }
			virtual void setState(int state) { rec_->setField(STATE_FIELD, state); }
			virtual bool isStateNull() const { return rec_->isFieldNull(STATE_FIELD); }
			virtual void setStateNull() { rec_->setFieldNull(STATE_FIELD); }
			virtual bool isStateWritable() const { return true; }

			virtual std::string getStateChangedDateTime() const { return rec_->getField(STATECHANGEDDATETIME_FIELD); }
			virtual void setStateChangedDateTime(const std::string& dt) { rec_->setField(STATECHANGEDDATETIME_FIELD, dt); }
			virtual bool isStateChangedDateTimeNull() const { return rec_->isFieldNull(STATECHANGEDDATETIME_FIELD); }
			virtual void setStateChangedDateTimeNull() { rec_->setFieldNull(STATECHANGEDDATETIME_FIELD); }
			virtual bool isStateChangedDateTimeWritable() const { return true; }

			virtual bool isRegistering() const { return util_string::stringtobool(rec_->getField(REGISTERING_FIELD)); }
			virtual void setRegistering(bool registering) { rec_->setField(REGISTERING_FIELD, Bool::toString(registering)); }
			virtual bool isRegisteringNull() const { return rec_->isFieldNull(REGISTERING_FIELD); }
			virtual void setRegisteringNull() { rec_->setFieldNull(REGISTERING_FIELD); }
			virtual bool isRegisteringWritable() const { return true; }

			virtual bool isDeregistering() const { return util_string::stringtobool(rec_->getField(DEREGISTERING_FIELD)); }
			virtual void setDeregistering(bool deregistering) { rec_->setField(DEREGISTERING_FIELD, Bool::toString(deregistering)); }
			virtual bool isDeregisteringNull() const { return rec_->isFieldNull(DEREGISTERING_FIELD); }
			virtual void setDeregisteringNull() { rec_->setFieldNull(DEREGISTERING_FIELD); }
			virtual bool isDeregisteringWritable() const { return true; }

			virtual bool isNotifiedBatches() const { return util_string::stringtobool(rec_->getField(NOTIFIEDBATCHES_FIELD)); }
			virtual void setNotifiedBatches(bool notifyBatches) { rec_->setField(NOTIFIEDBATCHES_FIELD, Bool::toString(notifyBatches)); }
			virtual bool isNotifiedBatchesNull() const { return rec_->isFieldNull(NOTIFIEDBATCHES_FIELD); }
			virtual void setNotifiedBatchesNull() { rec_->setFieldNull(NOTIFIEDBATCHES_FIELD); }
			virtual bool isNotifiedBatchesWritable() const { return true; }

			virtual std::string getNotifiedBatchesDateTime() const { return rec_->getField(NOTIFIEDBATCHESDATETIME_FIELD); }
			virtual void setNotifiedBatchesDateTime(const std::string& creation) { rec_->setField(NOTIFIEDBATCHESDATETIME_FIELD, creation); }
			virtual bool isNotifiedBatchesDateTimeNull() const { return rec_->isFieldNull(NOTIFIEDBATCHESDATETIME_FIELD); }
			virtual void setNotifiedBatchesDateTimeNull() { rec_->setFieldNull(NOTIFIEDBATCHESDATETIME_FIELD); }
			virtual bool isNotifiedBatchesDateTimeWritable() const { return true; }

			virtual bool isNotifiedOnline() const { return util_string::stringtobool(rec_->getField(NOTIFIEDONLINE_FIELD)); }
			virtual void setNotifiedOnline(bool notifyOnline) { rec_->setField(NOTIFIEDONLINE_FIELD, Bool::toString(notifyOnline)); }
			virtual bool isNotifiedOnlineNull() const { return rec_->isFieldNull(NOTIFIEDONLINE_FIELD); }
			virtual void setNotifiedOnlineNull() { rec_->setFieldNull(NOTIFIEDONLINE_FIELD); }
			virtual bool isNotifiedOnlineWritable() const { return true; }

			virtual std::string getNotifiedOnlineDateTime() const { return rec_->getField(NOTIFIEDONLINEDATETIME_FIELD); }
			virtual void setNotifiedOnlineDateTime(const std::string& creation) { rec_->setField(NOTIFIEDONLINEDATETIME_FIELD, creation); }
			virtual bool isNotifiedOnlineDateTimeNull() const { return rec_->isFieldNull(NOTIFIEDONLINEDATETIME_FIELD); }
			virtual void setNotifiedOnlineDateTimeNull() { rec_->setFieldNull(NOTIFIEDONLINEDATETIME_FIELD); }
			virtual bool isNotifiedOnlineDateTimeWritable() const { return true; }

			virtual int getRetryCount() const { return rec_->getField(RETRYCOUNT_FIELD); }
			virtual void setRetryCount(int retryCount) { rec_->setField(RETRYCOUNT_FIELD, retryCount); }
			virtual bool isRetryCountNull() const { return rec_->isFieldNull(RETRYCOUNT_FIELD); }
			virtual void setRetryCountNull() { rec_->setFieldNull(RETRYCOUNT_FIELD); }
			virtual bool isRetryCountWritable() const { return true; }

			virtual int getVersion() const { return rec_->getField(VERSION_FIELD); }
			virtual void setVersion(int version) { rec_->setField(VERSION_FIELD, version); }
			virtual bool isVersionNull() const { return rec_->isFieldNull(VERSION_FIELD); }
			virtual void setVersionNull() { rec_->setFieldNull(VERSION_FIELD); }
			virtual bool isVersionWritable() const { return true; }

			virtual int getVersionReceived() const { return rec_->getField(VERSIONRECEIVED_FIELD); }
			virtual void setVersionReceived(int version) { rec_->setField(VERSIONRECEIVED_FIELD, version); }
			virtual bool isVersionReceivedNull() const { return rec_->isFieldNull(VERSIONRECEIVED_FIELD); }
			virtual void setVersionReceivedNull() { rec_->setFieldNull(VERSIONRECEIVED_FIELD); }
			virtual bool isVersionReceivedWritable() const { return true; }

			virtual std::string getVersionReceivedDateTime() const { return rec_->getField(VERSIONRECEIVEDDATETIME_FIELD); }
			virtual void setVersionReceivedDateTime(const std::string& versionReceived) { rec_->setField(VERSIONRECEIVEDDATETIME_FIELD, versionReceived); }
			virtual bool isVersionReceivedDateTimeNull() const { return rec_->isFieldNull(VERSIONRECEIVEDDATETIME_FIELD); }
			virtual void setVersionReceivedDateTimeNull() { rec_->setFieldNull(VERSIONRECEIVEDDATETIME_FIELD); }
			virtual bool isVersionReceivedDateTimeWritable() const { return true; }

			virtual std::string getLastSuccessfulCommDateTime() const { return rec_->getField(LASTSUCCESSFULCOMMDATETIME_FIELD); }
			virtual void setLastSuccessfulCommDateTime(const std::string& dt) { rec_->setField(LASTSUCCESSFULCOMMDATETIME_FIELD, dt); }
			virtual bool isLastSuccessfulCommDateTimeNull() const { return rec_->isFieldNull(LASTSUCCESSFULCOMMDATETIME_FIELD); }
			virtual void setLastSuccessfulCommDateTimeNull() { rec_->setFieldNull(LASTSUCCESSFULCOMMDATETIME_FIELD); }
			virtual bool isLastSuccessfulCommDateTimeWritable() const { return true; }

			virtual std::string getFirstFailedCommDateTime() const { return rec_->getField(FIRSTFAILEDCOMMDATETIME_FIELD); }
			virtual void setFirstFailedCommDateTime(const std::string& dt) { rec_->setField(FIRSTFAILEDCOMMDATETIME_FIELD, dt); }
			virtual bool isFirstFailedCommDateTimeNull() const { return rec_->isFieldNull(FIRSTFAILEDCOMMDATETIME_FIELD); }
			virtual void setFirstFailedCommDateTimeNull() { rec_->setFieldNull(FIRSTFAILEDCOMMDATETIME_FIELD); }
			virtual bool isFirstFailedCommDateTimeWritable() const { return true; }

			virtual int getTotalSuccessfulCommunications() const { return rec_->getField(TOTALSUCCESSFULCOMMUNICATIONS_FIELD); }
			virtual void setTotalSuccessfulCommunications(int total) { rec_->setField(TOTALSUCCESSFULCOMMUNICATIONS_FIELD, total); }
			virtual bool isTotalSuccessfulCommunicationsNull() const { return rec_->isFieldNull(TOTALSUCCESSFULCOMMUNICATIONS_FIELD); }
			virtual void setTotalSuccessfulCommunicationsNull() { rec_->setFieldNull(TOTALSUCCESSFULCOMMUNICATIONS_FIELD); }
			virtual bool isTotalSuccessfulCommunicationsWritable() const { return true; }

			virtual int getTotalFailedCommunications() const { return rec_->getField(TOTALFAILEDCOMMUNICATIONS_FIELD); }
			virtual void setTotalFailedCommunications(int total) { rec_->setField(TOTALFAILEDCOMMUNICATIONS_FIELD, total); }
			virtual bool isTotalFailedCommunicationsNull() const { return rec_->isFieldNull(TOTALFAILEDCOMMUNICATIONS_FIELD); }
			virtual void setTotalFailedCommunicationsNull() { rec_->setFieldNull(TOTALFAILEDCOMMUNICATIONS_FIELD); }
			virtual bool isTotalFailedCommunicationsWritable() const { return true; }

			virtual int getTotalBusyCommunications() const { return rec_->getField(TOTALBUSYCOMMUNICATIONS_FIELD); }
			virtual void setTotalBusyCommunications(int total) { rec_->setField(TOTALBUSYCOMMUNICATIONS_FIELD, total); }
			virtual bool isTotalBusyCommunicationsNull() const { return rec_->isFieldNull(TOTALBUSYCOMMUNICATIONS_FIELD); }
			virtual void setTotalBusyCommunicationsNull() { rec_->setFieldNull(TOTALBUSYCOMMUNICATIONS_FIELD); }
			virtual bool isTotalBusyCommunicationsWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string IDENTIFIER_FIELD;
			static const std::string HOSTNAME_FIELD;
			static const std::string IPADDRESS_FIELD;
			static const std::string TYPE_FIELD;
			static const std::string CREATIONDATETIME_FIELD;
			static const std::string STATE_FIELD;
			static const std::string STATECHANGEDDATETIME_FIELD;
			static const std::string REGISTERING_FIELD;
			static const std::string DEREGISTERING_FIELD;
			static const std::string NOTIFIEDBATCHES_FIELD;
			static const std::string NOTIFIEDBATCHESDATETIME_FIELD;
			static const std::string NOTIFIEDONLINE_FIELD;
			static const std::string NOTIFIEDONLINEDATETIME_FIELD;
			static const std::string RETRYCOUNT_FIELD;
			static const std::string VERSION_FIELD;
			static const std::string VERSIONRECEIVED_FIELD;
			static const std::string VERSIONRECEIVEDDATETIME_FIELD;
			static const std::string LASTSUCCESSFULCOMMDATETIME_FIELD;
			static const std::string FIRSTFAILEDCOMMDATETIME_FIELD;
			static const std::string TOTALSUCCESSFULCOMMUNICATIONS_FIELD;
			static const std::string TOTALFAILEDCOMMUNICATIONS_FIELD;
			static const std::string TOTALBUSYCOMMUNICATIONS_FIELD;


		private:

			SqliteRecord* rec_;
			DBScaleI* decorated_;

			// Do not allow the default copy constructor.
			SqliteScale(const SqliteScale& rec);
			// Do not allow the default assignment operator.
			SqliteScale operator=(const SqliteScale& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESCALE_H
