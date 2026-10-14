#ifndef DBBATCH_H
#define DBBATCH_H


#include <string>


#include "edb.h"
#include DBBATCHI_INC
#if defined( __linux__ )
    #include DBBATCHESRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBBatch
	{
			// DBBatchesRM class is a friend of DBBatch because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBBatchesRM;
			friend class DBQuery<DBBatch>;
			friend class DBSynchronizationChangesRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBBatchI interface_type;
			typedef DBBatchesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBBatch(const DBBatch& rec);  
			~DBBatch();


			// Methods associated with DBBatchI
			std::string getNumber() const { return decorated_->getNumber(); }
			void setNumber(const std::string& number) { decorated_->setNumber(number); }
			bool isNumberNull() const { return decorated_->isNumberNull(); }
			bool isNumberWritable() const { return decorated_->isNumberWritable(); }

			int getScaleId() const { return decorated_->getScaleId(); }
			void setScaleId(int id) { decorated_->setScaleId(id); }
			bool isScaleIdNull() const { return decorated_->isScaleIdNull(); }
			void setScaleIdNull() { decorated_->setScaleIdNull(); }
			bool isScaleIdWritable() const { return decorated_->isScaleIdWritable(); }

			int getVersion() const { return decorated_->getVersion(); }
			void setVersion(int version) { decorated_->setVersion(version); }
			bool isVersionNull() const { return decorated_->isVersionNull(); }
			void setVersionNull() { decorated_->setVersionNull(); }
			bool isVersionWritable() const { return decorated_->isVersionWritable(); }

			int getRemoteVersion() const { return decorated_->getRemoteVersion(); }
			void setRemoteVersion(int version) { decorated_->setRemoteVersion(version); }
			bool isRemoteVersionNull() const { return decorated_->isRemoteVersionNull(); }
			void setRemoteVersionNull() { decorated_->setRemoteVersionNull(); }
			bool isRemoteVersionWritable() const { return decorated_->isRemoteVersionWritable(); }

			int getOrigin() const { return decorated_->getOrigin(); }
			void setOrigin(int origin) { decorated_->setOrigin(origin); }
			bool isOriginNull() const { return decorated_->isOriginNull(); }
			void setOriginNull() { decorated_->setOriginNull(); }
			bool isOriginWritable() const { return decorated_->isOriginWritable(); }

			int getType() const { return decorated_->getType(); }
			void setType(int type) { decorated_->setType(type); }
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			void setTypeNull() { decorated_->setTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			std::string getCreatedDateTime() const { return decorated_->getCreatedDateTime(); }
			void setCreatedDateTime(const std::string& created) { decorated_->setCreatedDateTime(created); }
			bool isCreatedDateTimeNull() const { return decorated_->isCreatedDateTimeNull(); }
			void setCreatedDateTimeNull() { decorated_->setCreatedDateTimeNull(); }
			bool isCreatedDateTimeWritable() const { return decorated_->isCreatedDateTimeWritable(); }

			bool getCompleted() const { return decorated_->getCompleted(); }
			void setCompleted(bool completed) { decorated_->setCompleted(completed); }
			bool isCompletedNull() const { return decorated_->isCompletedNull(); }
			void setCompletedNull() { decorated_->setCompletedNull(); }
			bool isCompletedWritable() const { return decorated_->isCompletedWritable(); }

			std::string getCompletedDateTime() const { return decorated_->getCompletedDateTime(); }
			void setCompletedDateTime(const std::string& completed) { decorated_->setCompletedDateTime(completed); }
			bool isCompletedDateTimeNull() const { return decorated_->isCompletedDateTimeNull(); }
			void setCompletedDateTimeNull() { decorated_->setCompletedDateTimeNull(); }
			bool isCompletedDateTimeWritable() const { return decorated_->isCompletedDateTimeWritable(); }

			std::string getSignature() const { return decorated_->getSignature(); }
			void setSignature(const std::string& signature) { decorated_->setSignature(signature); }
			bool isSignatureNull() const { return decorated_->isSignatureNull(); }
			void setSignatureNull() { decorated_->setSignatureNull(); }
			bool isSignatureWritable() const { return decorated_->isSignatureWritable(); }

			bool getPendingApplication() const { return decorated_->getPendingApplication(); }
			void setPendingApplication(bool applied) { decorated_->setPendingApplication(applied); }
			bool isPendingApplicationNull() const { return decorated_->isPendingApplicationNull(); }
			void setPendingApplicationNull() { decorated_->setPendingApplicationNull(); }
			bool isPendingApplicationWritable() const { return decorated_->isPendingApplicationWritable(); }

			std::string getAppliedDateTime() const { return decorated_->getAppliedDateTime(); }
			void setAppliedDateTime(const std::string& applied) { decorated_->setAppliedDateTime(applied); }
			bool isAppliedDateTimeNull() const { return decorated_->isAppliedDateTimeNull(); }
			void setAppliedDateTimeNull() { decorated_->setAppliedDateTimeNull(); }
			bool isAppliedDateTimeWritable() const { return decorated_->isAppliedDateTimeWritable(); }

			bool getPendingSynchronization() const { return decorated_->getPendingSynchronization(); }
			void setPendingSynchronization(bool synchronized) { decorated_->setPendingSynchronization(synchronized); }
			bool isPendingSynchronizationNull() const { return decorated_->isPendingSynchronizationNull(); }
			void setPendingSynchronizationNull() { decorated_->setPendingSynchronizationNull(); }
			bool isPendingSynchronizationWritable() const { return decorated_->isPendingSynchronizationWritable(); }

			std::string getSynchronizedDateTime() const { return decorated_->getSynchronizedDateTime(); }
			void setSynchronizedDateTime(const std::string& synchronized) { decorated_->setSynchronizedDateTime(synchronized); }
			bool isSynchronizedDateTimeNull() const { return decorated_->isSynchronizedDateTimeNull(); }
			void setSynchronizedDateTimeNull() { decorated_->setSynchronizedDateTimeNull(); }
			bool isSynchronizedDateTimeWritable() const { return decorated_->isSynchronizedDateTimeWritable(); }

			bool getPendingDeletion() const { return decorated_->getPendingDeletion(); }
			void setPendingDeletion(bool deleted) { decorated_->setPendingDeletion(deleted); }
			bool isPendingDeletionNull() const { return decorated_->isPendingDeletionNull(); }
			void setPendingDeletionNull() { decorated_->setPendingDeletionNull(); }
			bool isPendingDeletionWritable() const { return decorated_->isPendingDeletionWritable(); }

			std::string getDeletedDateTime() const { return decorated_->getDeletedDateTime(); }
			void setDeletedDateTime(const std::string& deleted) { decorated_->setDeletedDateTime(deleted); }
			bool isDeletedDateTimeNull() const { return decorated_->isDeletedDateTimeNull(); }
			void setDeletedDateTimeNull() { decorated_->setDeletedDateTimeNull(); }
			bool isDeletedDateTimeWritable() const { return decorated_->isDeletedDateTimeWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey() const { return decorated_->getNumber(); }
			void setKey(const std::string& /*key*/) { /*decorated_->setNumber(key);*/ }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBBatch& rec);


		private:

			edb_adapter::DBBatchI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBBatch(edb_adapter::DBBatchI* d);

			// Do not allow the default assignment operator.
			DBBatch operator=(const DBBatch& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBBATCH_H
