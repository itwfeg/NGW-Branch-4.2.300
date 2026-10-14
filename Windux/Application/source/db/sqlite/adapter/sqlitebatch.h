#ifndef SQLITEBATCH_H
#define SQLITEBATCH_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"
#include "Bool.h"

#include DBBATCHI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteBatch : public DBBatchI
	{
		public:

			// Types and subtypes
			typedef DBBatchI interface_type;


			// Constructors and destructor
			SqliteBatch(SqliteRecord* r, DBBatchI* d = 0);
			~SqliteBatch();


			// Think about replacing this method with two friend functions from the
			// SqliteBatchesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBBatchI
			virtual DBBatchI* clone() const { return new SqliteBatch(new SqliteRecord(*rec_)); }
			virtual const DBBatchI* getDecorated() const { return decorated_; }

			virtual std::string getField(const std::string& field) const { return rec_->getField(field); }
			virtual void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			virtual std::string getNumber() const { return rec_->getField(NUMBER_FIELD); }
			virtual void setNumber(const std::string& number) { rec_->setField(NUMBER_FIELD, number); }
			virtual bool isNumberNull() const { return rec_->isFieldNull(NUMBER_FIELD); }
			virtual bool isNumberWritable() const { return true; }

			virtual int getScaleId() const { return rec_->getField(SCALEID_FIELD); }
			virtual void setScaleId(int id) { rec_->setField(SCALEID_FIELD, id); }
			virtual bool isScaleIdNull() const { return rec_->isFieldNull(SCALEID_FIELD); }
			virtual void setScaleIdNull() { rec_->setFieldNull(SCALEID_FIELD); }
			virtual bool isScaleIdWritable() const { return true; }

			virtual int getVersion() const { return rec_->getField(VERSION_FIELD); }
			virtual void setVersion(int version) { rec_->setField(VERSION_FIELD, version); }
			virtual bool isVersionNull() const { return rec_->isFieldNull(VERSION_FIELD); }
			virtual void setVersionNull() { rec_->setFieldNull(VERSION_FIELD); }
			virtual bool isVersionWritable() const { return true; }

			virtual int getRemoteVersion() const { return rec_->getField(REMOTEVERSION_FIELD); }
			virtual void setRemoteVersion(int version) { rec_->setField(REMOTEVERSION_FIELD, version); }
			virtual bool isRemoteVersionNull() const { return rec_->isFieldNull(REMOTEVERSION_FIELD); }
			virtual void setRemoteVersionNull() { rec_->setFieldNull(REMOTEVERSION_FIELD); }
			virtual bool isRemoteVersionWritable() const { return true; }

			virtual int getOrigin() const { return rec_->getField(ORIGIN_FIELD); }
			virtual void setOrigin(int origin) { rec_->setField(ORIGIN_FIELD, origin); }
			virtual bool isOriginNull() const { return rec_->isFieldNull(ORIGIN_FIELD); }
			virtual void setOriginNull() { rec_->setFieldNull(ORIGIN_FIELD); }
			virtual bool isOriginWritable() const { return true; }

			virtual int getType() const { return rec_->getField(TYPE_FIELD); }
			virtual void setType(int type) { rec_->setField(TYPE_FIELD, type); }
			virtual bool isTypeNull() const { return rec_->isFieldNull(TYPE_FIELD); }
			virtual void setTypeNull() { rec_->setFieldNull(TYPE_FIELD); }
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getCreatedDateTime() const { return rec_->getField(CREATEDDATETIME_FIELD); }
			virtual void setCreatedDateTime(const std::string& created) { rec_->setField(CREATEDDATETIME_FIELD, created); }
			virtual bool isCreatedDateTimeNull() const { return rec_->isFieldNull(CREATEDDATETIME_FIELD); }
			virtual void setCreatedDateTimeNull() { rec_->setFieldNull(CREATEDDATETIME_FIELD); }
			virtual bool isCreatedDateTimeWritable() const { return true; }

			virtual bool getCompleted() const { return util_string::stringtobool(rec_->getField(COMPLETED_FIELD)); }
			virtual void setCompleted(bool completed) { rec_->setField(COMPLETED_FIELD, Bool::toString(completed)); }
			virtual bool isCompletedNull() const { return rec_->isFieldNull(COMPLETED_FIELD); }
			virtual void setCompletedNull() { rec_->setFieldNull(COMPLETED_FIELD); }
			virtual bool isCompletedWritable() const { return true; }

			virtual std::string getCompletedDateTime() const { return rec_->getField(COMPLETEDDATETIME_FIELD); }
			virtual void setCompletedDateTime(const std::string& completed) { rec_->setField(COMPLETEDDATETIME_FIELD, completed); }
			virtual bool isCompletedDateTimeNull() const { return rec_->isFieldNull(COMPLETEDDATETIME_FIELD); }
			virtual void setCompletedDateTimeNull() { rec_->setFieldNull(COMPLETEDDATETIME_FIELD); }
			virtual bool isCompletedDateTimeWritable() const { return true; }

			virtual std::string getSignature() const { return rec_->getField(SIGNATURE_FIELD); }
			virtual void setSignature(const std::string& signature) { rec_->setField(SIGNATURE_FIELD, signature); }
			virtual bool isSignatureNull() const { return rec_->isFieldNull(SIGNATURE_FIELD); }
			virtual void setSignatureNull() { rec_->setFieldNull(SIGNATURE_FIELD); }
			virtual bool isSignatureWritable() const { return true; }

			virtual bool getPendingApplication() const { return util_string::stringtobool(rec_->getField(PENDINGAPPLICATION_FIELD)); }
			virtual void setPendingApplication(bool applied) { rec_->setField(PENDINGAPPLICATION_FIELD, Bool::toString(applied)); }
			virtual bool isPendingApplicationNull() const { return rec_->isFieldNull(PENDINGAPPLICATION_FIELD); }
			virtual void setPendingApplicationNull() { rec_->setFieldNull(PENDINGAPPLICATION_FIELD); }
			virtual bool isPendingApplicationWritable() const { return true; }

			virtual std::string getAppliedDateTime() const { return rec_->getField(APPLIEDDATETIME_FIELD); }
			virtual void setAppliedDateTime(const std::string& applied) { rec_->setField(APPLIEDDATETIME_FIELD, applied); }
			virtual bool isAppliedDateTimeNull() const { return rec_->isFieldNull(APPLIEDDATETIME_FIELD); }
			virtual void setAppliedDateTimeNull() { rec_->setFieldNull(APPLIEDDATETIME_FIELD); }
			virtual bool isAppliedDateTimeWritable() const { return true; }

			virtual bool getPendingSynchronization() const { return util_string::stringtobool(rec_->getField(PENDINGSYNCHRONIZATION_FIELD)); }
			virtual void setPendingSynchronization(bool synchronized) { rec_->setField(PENDINGSYNCHRONIZATION_FIELD, Bool::toString(synchronized)); }
			virtual bool isPendingSynchronizationNull() const { return rec_->isFieldNull(PENDINGSYNCHRONIZATION_FIELD); }
			virtual void setPendingSynchronizationNull() { rec_->setFieldNull(PENDINGSYNCHRONIZATION_FIELD); }
			virtual bool isPendingSynchronizationWritable() const { return true; }

			virtual std::string getSynchronizedDateTime() const { return rec_->getField(SYNCHRONIZEDDATETIME_FIELD); }
			virtual void setSynchronizedDateTime(const std::string& synchronized) { rec_->setField(SYNCHRONIZEDDATETIME_FIELD, synchronized); }
			virtual bool isSynchronizedDateTimeNull() const { return rec_->isFieldNull(SYNCHRONIZEDDATETIME_FIELD); }
			virtual void setSynchronizedDateTimeNull() { rec_->setFieldNull(SYNCHRONIZEDDATETIME_FIELD); }
			virtual bool isSynchronizedDateTimeWritable() const { return true; }

			virtual bool getPendingDeletion() const { return util_string::stringtobool(rec_->getField(PENDINGDELETION_FIELD)); }
			virtual void setPendingDeletion(bool deleted) { rec_->setField(PENDINGDELETION_FIELD, Bool::toString(deleted)); }
			virtual bool isPendingDeletionNull() const { return rec_->isFieldNull(PENDINGDELETION_FIELD); }
			virtual void setPendingDeletionNull() { rec_->setFieldNull(PENDINGDELETION_FIELD); }
			virtual bool isPendingDeletionWritable() const { return true; }

			virtual std::string getDeletedDateTime() const { return rec_->getField(DELETEDDATETIME_FIELD); }
			virtual void setDeletedDateTime(const std::string& deleted) { rec_->setField(DELETEDDATETIME_FIELD, deleted); }
			virtual bool isDeletedDateTimeNull() const { return rec_->isFieldNull(DELETEDDATETIME_FIELD); }
			virtual void setDeletedDateTimeNull() { rec_->setFieldNull(DELETEDDATETIME_FIELD); }
			virtual bool isDeletedDateTimeWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string NUMBER_FIELD;
			static const std::string SCALEID_FIELD;
			static const std::string VERSION_FIELD;
			static const std::string REMOTEVERSION_FIELD;
			static const std::string ORIGIN_FIELD;
			static const std::string TYPE_FIELD;
			static const std::string CREATEDDATETIME_FIELD;
			static const std::string COMPLETED_FIELD;
			static const std::string COMPLETEDDATETIME_FIELD;
			static const std::string SIGNATURE_FIELD;
			static const std::string PENDINGAPPLICATION_FIELD;
			static const std::string APPLIEDDATETIME_FIELD;
			static const std::string PENDINGSYNCHRONIZATION_FIELD;
			static const std::string SYNCHRONIZEDDATETIME_FIELD;
			static const std::string PENDINGDELETION_FIELD;
			static const std::string DELETEDDATETIME_FIELD;


		private:

			SqliteRecord* rec_;
			DBBatchI* decorated_;

			// Do not allow the default copy constructor.
			SqliteBatch(const SqliteBatch& rec);
			// Do not allow the default assignment operator.
			SqliteBatch operator=(const SqliteBatch& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEBATCH_H
