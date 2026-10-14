#ifndef DBBATCHI_H
#define DBBATCHI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBBatchI
	{
		public:

			virtual ~DBBatchI() {}


			virtual DBBatchI* clone() const = 0;
			virtual const DBBatchI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getNumber() const = 0;
			virtual void setNumber(const std::string& number) = 0;
			virtual bool isNumberNull() const { return false; }
			virtual bool isNumberWritable() const { return true; }

			virtual int getScaleId() const = 0;
			virtual void setScaleId(int id) = 0;
			virtual bool isScaleIdNull() const { return false; }
			virtual void setScaleIdNull() = 0;
			virtual bool isScaleIdWritable() const { return true; }

			virtual int getVersion() const = 0;
			virtual void setVersion(int version) = 0;
			virtual bool isVersionNull() const { return false; }
			virtual void setVersionNull() = 0;
			virtual bool isVersionWritable() const { return true; }

			virtual int getRemoteVersion() const = 0;
			virtual void setRemoteVersion(int version) = 0;
			virtual bool isRemoteVersionNull() const { return false; }
			virtual void setRemoteVersionNull() = 0;
			virtual bool isRemoteVersionWritable() const { return true; }

			virtual int getOrigin() const = 0;
			virtual void setOrigin(int origin) = 0;
			virtual bool isOriginNull() const { return false; }
			virtual void setOriginNull() = 0;
			virtual bool isOriginWritable() const { return true; }

			virtual int getType() const = 0;
			virtual void setType(int type) = 0;
			virtual bool isTypeNull() const { return false; }
			virtual void setTypeNull() = 0;
			virtual bool isTypeWritable() const { return true; }

			virtual std::string getCreatedDateTime() const = 0;
			virtual void setCreatedDateTime(const std::string& created) = 0;
			virtual bool isCreatedDateTimeNull() const { return false; }
			virtual void setCreatedDateTimeNull() = 0;
			virtual bool isCreatedDateTimeWritable() const { return true; }

			virtual bool getCompleted() const = 0;
			virtual void setCompleted(bool completed) = 0;
			virtual bool isCompletedNull() const { return false; }
			virtual void setCompletedNull() = 0;
			virtual bool isCompletedWritable() const { return true; }

			virtual std::string getCompletedDateTime() const = 0;
			virtual void setCompletedDateTime(const std::string& completed) = 0;
			virtual bool isCompletedDateTimeNull() const { return false; }
			virtual void setCompletedDateTimeNull() = 0;
			virtual bool isCompletedDateTimeWritable() const { return true; }

			virtual std::string getSignature() const = 0;
			virtual void setSignature(const std::string& signature) = 0;
			virtual bool isSignatureNull() const { return false; }
			virtual void setSignatureNull() = 0;
			virtual bool isSignatureWritable() const { return true; }

			virtual bool getPendingApplication() const = 0;
			virtual void setPendingApplication(bool applied) = 0;
			virtual bool isPendingApplicationNull() const { return false; }
			virtual void setPendingApplicationNull() = 0;
			virtual bool isPendingApplicationWritable() const { return true; }

			virtual std::string getAppliedDateTime() const = 0;
			virtual void setAppliedDateTime(const std::string& applied) = 0;
			virtual bool isAppliedDateTimeNull() const { return false; }
			virtual void setAppliedDateTimeNull() = 0;
			virtual bool isAppliedDateTimeWritable() const { return true; }

			virtual bool getPendingSynchronization() const = 0;
			virtual void setPendingSynchronization(bool synchronized) = 0;
			virtual bool isPendingSynchronizationNull() const { return false; }
			virtual void setPendingSynchronizationNull() = 0;
			virtual bool isPendingSynchronizationWritable() const { return true; }

			virtual std::string getSynchronizedDateTime() const = 0;
			virtual void setSynchronizedDateTime(const std::string& synchronized) = 0;
			virtual bool isSynchronizedDateTimeNull() const { return false; }
			virtual void setSynchronizedDateTimeNull() = 0;
			virtual bool isSynchronizedDateTimeWritable() const { return true; }

			virtual bool getPendingDeletion() const = 0;
			virtual void setPendingDeletion(bool deleted) = 0;
			virtual bool isPendingDeletionNull() const { return false; }
			virtual void setPendingDeletionNull() = 0;
			virtual bool isPendingDeletionWritable() const { return true; }

			virtual std::string getDeletedDateTime() const = 0;
			virtual void setDeletedDateTime(const std::string& deleted) = 0;
			virtual bool isDeletedDateTimeNull() const { return false; }
			virtual void setDeletedDateTimeNull() = 0;
			virtual bool isDeletedDateTimeWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBBATCHI_H
