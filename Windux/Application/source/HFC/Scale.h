#ifndef SCALE_H
#define SCALE_H



#include <string>


#include "HfcObject.h"
#include "HobDateTime.h"


#include "tags/Tags.h"



namespace hfc
{


	class Scale : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				RoleInvalid = -1,

				RoleThis,
				RolePrimary,
				RoleSecondary,

				RoleLast
			} ScaleRole;

			typedef enum
			{
				StateInvalid,

				StateActive,
				StateRetrying,
				StatePassive,

				StateLast
			} ScaleState;

			static const int _invalidScaleId_ = 0;


			/* --- Constructors and destructor --- */
			Scale(ScaleState state = StateActive);
			Scale(const char * data, int len, int formatType);
			~Scale();


			/* --- Getters and setters --- */
			HobErrorCode getErrorCode() const { return errorCode_; }
			void setErrorCode(HobErrorCode errorCode);
			bool isRecordValid() const { return errorCode_ == RECORD_OK; }


			int getIdentifier() const { return identifier_; }
			void setIdentifier(int id) { identifier_ = id; }

			ScaleRole getRole() const { return role_; }
			bool isThisScale() const { return role_ == RoleThis; }
			bool isPrimaryScale() const { return role_ == RolePrimary; }
			bool isSecondaryScale() const { return role_ == RoleSecondary; }
			void setRole(ScaleRole role) { role_ = role; }
			void setThisScale() { role_ = RoleThis; }
			void setPrimaryScale() { role_ = RolePrimary; }
			void setSecondaryScaleRole() { role_ = RoleSecondary; }

			const std::string & getHostName() const { return hostname_; }
			void setHostName(const std::string & hostname);

			const std::string & getIPAddress() const { return ipAddress_; }
			void setIPAddress(const std::string & ip) { ipAddress_ = ip; }

			HobDateTime getCreationDateTime() const { return HobDateTime::createDateTimeFromDBFormat(creationDateTime_); }
			const std::string& getCreationDateTimeAsString() const { return creationDateTime_; }
			void setCreationDateTime(const std::string& dt) { creationDateTime_ = dt; }
			void setCreationDateTime(const HobDateTime& dt = HobDateTime()) { creationDateTime_ = dt.convertToDBFormat(); }

			ScaleState getState() const { return state_; }
			bool isStateActive() const { return state_ == StateActive; }
			bool isStateRetrying() const { return state_ == StateRetrying; }
			bool isStatePassive() const { return state_ == StatePassive; }
			void setState(ScaleState state);
			void setStateActive() { setState(StateActive); }
			void setStateRetrying() { setState(StateRetrying); }
			void setStatePassive() { setState(StatePassive); }

			HobDateTime getStateChangedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(stateChangedDateTime_); }
			const std::string& getStateChangedDateTimeAsString() const { return stateChangedDateTime_; }
			void setStateChangedDateTime(const std::string& dt) { stateChangedDateTime_ = dt; }
			void setStateChangedDateTime(const HobDateTime& dt = HobDateTime()) { stateChangedDateTime_ = dt.convertToDBFormat(); }

			bool isRegistering() const { return registering_; }
			void setRegistering(bool registering = true) { registering_ = registering; }
			void clearRegistering() { registering_ = false; }

			bool isDeregistering() const { return deregistering_; }
			void setDeregistering(bool deregistering = true) { deregistering_ = deregistering; }
			void clearDeregistering() { deregistering_ = false; }

			bool isNotifiedBatches() const { return notifiedBatches_; }
			void setNotifiedBatches(bool notifiedBatches = true) { notifiedBatches ? setNotifiedBatchesDateTime() : clearNotifiedBatches(); }
			void clearNotifiedBatches() { notifiedBatches_ = false; notifiedBatchesDateTime_.clear();  }

			HobDateTime getNotifiedBatchesDateTime() const { return HobDateTime::createDateTimeFromDBFormat(notifiedBatchesDateTime_); }
			const std::string& getNotifiedBatchesDateTimeAsString() const { return notifiedBatchesDateTime_; }
			void setNotifiedBatchesDateTime(const std::string& dt) { notifiedBatchesDateTime_ = dt; notifiedBatches_ = true; }
			void setNotifiedBatchesDateTime(const HobDateTime& dt = HobDateTime()) { notifiedBatchesDateTime_ = dt.convertToDBFormat(); notifiedBatches_ = true; }

			bool isNotifiedOnline() const { return notifiedOnline_; }
			void setNotifiedOnline(bool notifiedOnline = true) { notifiedOnline ? setNotifiedOnlineDateTime() : clearNotifiedOnline(); }
			void clearNotifiedOnline() { notifiedOnline_ = false; notifiedOnlineDateTime_.clear(); }

			HobDateTime getNotifiedOnlineDateTime() const { return HobDateTime::createDateTimeFromDBFormat(notifiedOnlineDateTime_); }
			const std::string& getNotifiedOnlineDateTimeAsString() const { return notifiedOnlineDateTime_; }
			void setNotifiedOnlineDateTime(const std::string& dt) { notifiedOnlineDateTime_ = dt; notifiedOnline_ = true; }
			void setNotifiedOnlineDateTime(const HobDateTime& dt = HobDateTime()) { notifiedOnlineDateTime_ = dt.convertToDBFormat(); notifiedOnline_ = true; }

			int getRetryCount() const { return retryCount_; }
			void setRetryCount(int count) { retryCount_ = count; }

			int getVersion() const { return version_; }
			void setVersion(int ver) { version_ = ver; }

			int getVersionReceived() const { return versionReceived_; }
			void setVersionReceived(int ver) { versionReceived_ = ver; setVersionReceivedDateTime(); }

			HobDateTime getVersionReceivedDateTime() const { return HobDateTime::createDateTimeFromDBFormat(versionReceivedDateTime_); }
			const std::string& getVersionReceivedDateTimeAsString() const { return versionReceivedDateTime_; }
			void setVersionReceivedDateTime(const std::string& dt) { versionReceivedDateTime_ = dt; }
			void setVersionReceivedDateTime(const HobDateTime& dt = HobDateTime()) { versionReceivedDateTime_ = dt.convertToDBFormat(); }

			bool isLastCommSuccessful() const { return lastCommSuccessful_; }
			void setLastCommSuccessful(bool successful = true);

			HobDateTime getLastSuccessfulCommDateTime() const { return HobDateTime::createDateTimeFromDBFormat(lastSuccessfulCommDateTime_); }
			const std::string& getLastSuccessfulCommDateTimeAsString() const { return lastSuccessfulCommDateTime_; }
			void setLastSuccessfulCommDateTime(const std::string& dt) { lastSuccessfulCommDateTime_ = dt; }
			void setLastSuccessfulCommDateTime(const HobDateTime& dt = HobDateTime()) { lastSuccessfulCommDateTime_ = dt.convertToDBFormat(); }

			HobDateTime getFirstFailedCommDateTime() const { return HobDateTime::createDateTimeFromDBFormat(firstFailedCommDateTime_); }
			const std::string& getFirstFailedCommDateTimeAsString() const { return firstFailedCommDateTime_; }
			void setFirstFailedCommDateTime(const std::string& dt) { firstFailedCommDateTime_ = dt; }
			void setFirstFailedCommDateTime(const HobDateTime& dt = HobDateTime()) { firstFailedCommDateTime_ = dt.convertToDBFormat(); }

			int getTotalSuccessfulCommunications() const { return totalSuccessfulCommunications_; }
			void incrementTotalSuccessfulCommunications() { ++totalSuccessfulCommunications_; }
			void setTotalSuccessfulCommunications(int total) { totalSuccessfulCommunications_ = total; }

			int getTotalFailedCommunications() const { return totalFailedCommunications_; }
			void incrementTotalFailedCommunications() { ++totalFailedCommunications_; }
			void setTotalFailedCommunications(int total) { totalFailedCommunications_ = total; }

			int getTotalBusyCommunications() const { return totalBusyCommunications_; }
			void incrementTotalBusyCommunications() { ++totalBusyCommunications_; }
			void setTotalBusyCommunications(int total) { totalBusyCommunications_ = total; }


			/* --- Instance function members --- */
			void updateScale(Scale * newScale);
			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();


		private:
			/* --- Data members --- */
			HobErrorCode errorCode_;
			int identifier_;
			ScaleRole role_;
			std::string hostname_;
			std::string ipAddress_;
			std::string creationDateTime_;
			ScaleState state_;
			std::string stateChangedDateTime_;
			bool registering_;
			bool deregistering_;
			bool notifiedBatches_;
			std::string notifiedBatchesDateTime_;
			bool notifiedOnline_;
			std::string notifiedOnlineDateTime_;
			int retryCount_;
			int version_;
			int versionReceived_;
			std::string versionReceivedDateTime_;
			bool lastCommSuccessful_;
			std::string lastSuccessfulCommDateTime_;
			std::string firstFailedCommDateTime_;
			int totalSuccessfulCommunications_;
			int totalFailedCommunications_;
			int totalBusyCommunications_;


			static FieldOffsetsList _fieldList_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			void init();
	};


}



#endif //SCALE_H
