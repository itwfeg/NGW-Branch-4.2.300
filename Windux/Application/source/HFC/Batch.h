#ifndef BATCH_H
#define BATCH_H



#include "HfcObject.h"
#include "HobDateTime.h"


#include "tags/Tags.h"


namespace hfc
{


	class SynchronizationChange;


	class Batch : public HfcObject
	{
		friend class BatchManager;


		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				OriginUnknown,

				OriginLocal,
				OriginRemote,

				OriginLast
			} BatchOrigin;

			typedef enum
			{
				TypeUnknown,

				TypeUser,
				TypeHQ,
				TypeWholeDB,

				TypeLast
			} BatchType;

			class Signature
			{
				public:
					Signature() : crc_(0), numRecords_(0) {}
					Signature(const std::string& buffer);

					unsigned short getCRC() const { return crc_; }
					void setCRC(unsigned short crc) { crc_ = crc; }

					unsigned short getNumberRecords() const { return numRecords_; }
					void addRecord() { ++numRecords_; }

					const std::string& serialize() const;

					void addChange(const std::string& change);

					Signature& operator = (const Signature& rhs)
						{ crc_ = rhs.crc_; numRecords_ = rhs.numRecords_; return *this; }

					bool operator == (const Signature& rhs) const
						{ return crc_ == rhs.crc_ && numRecords_ == rhs.numRecords_; }

				private:
					unsigned short crc_;
					unsigned short numRecords_;
					mutable std::string serializedData_;
			};

			static const int _invalidBatchNumber_  =  0;
			static const int _invalidBatchVersion_ = -1;


			/* --- Constructors and destructor --- */
			Batch();
			Batch(const char * data, int len, int formatType);
			~Batch();


			/* --- Getters and setters --- */
			HobErrorCode getErrorCode() const { return errorCode_; }
			void setErrorCode(HobErrorCode errorCode);
			bool isRecordValid() const { return errorCode_ == RECORD_OK; }

			int getNumber() const { return number_; }
			void setNumber(int number) { number_ = number; }

			int getScaleId() const { return scaleId_; }
			void setScaleId(int id) { scaleId_ = id; }

			int getVersion() const { return version_; }
			void setVersion(int version) { version_ = version; }

			int getRemoteVersion() const { return remoteVersion_; }
			void setRemoteVersion(int version) { remoteVersion_ = version; }

			BatchOrigin getOrigin() const { return origin_; }
			void setOrigin(BatchOrigin origin) { origin_ = origin; }

			BatchType getType() const { return type_; }
			void setType(BatchType type) { type_ = type; }

			HobDateTime getCreatedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(createdDateTime_); }
			const std::string& getCreatedDateTimeAsString() const { return createdDateTime_; }
			void setCreatedDateTime(const std::string & dt) { createdDateTime_ = dt; }
			void setCreatedDateTime(const HobDateTime & dt = HobDateTime()) { createdDateTime_ = dt.convertToDBFormat(); }

			bool isComplete() const { return completed_; }
			void setComplete(bool completed = true) { completed ? setCompletedDateTime() : clearComplete(); }
			void clearComplete() { completed_ = false; completedDateTime_.clear(); }

			HobDateTime getCompletedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(completedDateTime_); }
			const std::string& getCompletedDateTimeAsString() const { return completedDateTime_; }
			void setCompletedDateTime(const std::string & dt) { completedDateTime_ = dt; completed_ = true; }
			void setCompletedDateTime(const HobDateTime & dt = HobDateTime()) { completedDateTime_ = dt.convertToDBFormat(); completed_ = true; }

			bool isSignatureEmtpy() const { return signature_.getNumberRecords() == 0; }
			const std::string& getSignature() const { return signature_.serialize(); }
			const Signature& getSignatureObject() const { return signature_; }
			void setSignature(const std::string& nibblized) { signature_ = Signature(nibblized); }

			bool isSynchronizedSignatureEmtpy() const { return synchronizedSignature_.getNumberRecords() == 0; }
			const std::string& getSynchronizedSignature() const { return synchronizedSignature_.serialize(); }
			const Signature& getSynchronizedSignatureObject() const { return synchronizedSignature_; }
			void setSynchronizedSignature(const std::string& nibblized) { synchronizedSignature_ = Signature(nibblized); }

			bool isPendingApplication() const { return pendingApplication_; }
			void setPendingApplication(bool pending = true) { pendingApplication_ = pending; }
			void clearPendingApplication() { pendingApplication_ = false; }

			bool isApplied() const { return applied_; }
			void setApplied() { setAppliedDateTime(); }
			void clearApplied() { applied_ = false; appliedDateTime_.clear(); }

			HobDateTime getAppliedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(appliedDateTime_); }
			const std::string& getAppliedDateTimeAsString() const { return appliedDateTime_; }
			void setAppliedDateTime(const std::string & dt) { appliedDateTime_ = dt; applied_ = true; }
			void setAppliedDateTime(const HobDateTime & dt = HobDateTime()) { appliedDateTime_ = dt.convertToDBFormat(); applied_ = true; }

			bool isPendingSynchronization() const { return pendingSynchronization_; }
			void setPendingSynchronization(bool pending = true) { pendingSynchronization_ = pending; }
			void clearPendingSynchronization() { pendingSynchronization_ = false; }

			bool isSynchronized() const { return synchronized_; }
			void setSynchronized(bool synchronized = true) { synchronized ? setSynchronizedDateTime() : clearSynchronized(); }
			void clearSynchronized() { synchronized_ = false; synchronizedDateTime_.clear(); }

			HobDateTime getSynchronizedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(synchronizedDateTime_); }
			const std::string& getSynchronizedDateTimeAsString() const { return synchronizedDateTime_; }
			void setSynchronizedDateTime(const std::string & dt) { synchronizedDateTime_ = dt; synchronized_ = true; }
			void setSynchronizedDateTime(const HobDateTime & dt = HobDateTime()) { synchronizedDateTime_ = dt.convertToDBFormat(); synchronized_ = true; }

			bool isPendingDeletion() const { return pendingDeletion_; }
			void setPendingDeletion(bool pending = true) { pendingDeletion_ = pending; }
			void clearPendingDeletion() { pendingDeletion_ = false; }

			bool isDeleted() const { return deleted_; }
			void setDeleted() { setDeletedDateTime(); }
			void clearDeleted() { deleted_ = false; deletedDateTime_.clear(); }

			HobDateTime getDeletedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(deletedDateTime_); }
			const std::string& getDeletedDateTimeAsString() const { return deletedDateTime_; }
			void setDeletedDateTime(const std::string & dt) { deletedDateTime_ = dt; deleted_ = true; }
			void setDeletedDateTime(const HobDateTime & dt = HobDateTime()) { deletedDateTime_ = dt.convertToDBFormat(); deleted_ = true; }


			/* --- Instance function members --- */
			void updateBatch(const Batch * newBatch);
			const char * serialize(int format) const;

			void computeSignature(const std::string& change);
			void computeSynchronizedSignature(const std::string& change);


			/* --- Class function members --- */
			static void initFieldList();

			static const std::string& getTypeName(BatchType type);
			static const std::string& getOriginName(BatchOrigin origin);


		private:
			/* --- Data members --- */
			HobErrorCode errorCode_;
			int number_;
			int scaleId_;
			int version_;
			int remoteVersion_;
			BatchOrigin origin_;
			BatchType type_;
			std::string createdDateTime_;
			bool completed_;
			std::string completedDateTime_;
			Signature signature_;
			Signature synchronizedSignature_;
			bool pendingApplication_;
			bool applied_;
			std::string appliedDateTime_;
			bool pendingSynchronization_;
			bool synchronized_;
			std::string synchronizedDateTime_;
			bool pendingDeletion_;
			bool deleted_;
			std::string deletedDateTime_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
			static FieldOffsetsList _synchronizationFieldList_, _dbindependentFieldList_;

			static const std::string OriginLocalName;
			static const std::string OriginRemoteName;
			static const std::string OriginUnknownName;

			static const std::string TypeUserName;
			static const std::string TypeHQName;
			static const std::string TypeWholeDBName;
			static const std::string TypeUnknownName;


			/* --- Constructors --- */


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* BATCH_H */
