#ifndef SYNCHRONIZATIONCHANGE_H
#define SYNCHRONIZATIONCHANGE_H



#include "HfcObject.h"


#include "tags/Tags.h"



class DBAccess;
class SessionMessage;


namespace edb
{
	class DBSynchronizationChange;
}


namespace hfc
{


	class Batch;


	class SynchronizationChange : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
			SynchronizationChange();
			SynchronizationChange(int device, int evt, const std::string& change);
			SynchronizationChange(int device, int evt, MessageFormat format, const std::string& change);
			SynchronizationChange(const SessionMessage& message);
			SynchronizationChange(const char * buffer);
			SynchronizationChange(const char * data, int len, int formatType);
			~SynchronizationChange();


			/* --- Getters and setters --- */
			HobErrorCode getErrorCode() const { return errorCode_; }
			void setErrorCode(HobErrorCode errorCode);
			bool isRecordValid() const { return errorCode_ == RECORD_OK; }

			int getIdentifier() const { return identifier_; }
			void setIdentifier(int id) { identifier_ = id; }

			int getBatchNumber() const { return batchNumber_; }
			void setBatchNumber(int number) { batchNumber_ = number; }

			int getDevice() const { return device_; }
			void setDevice(int device) { device_ = device; }

			const std::string& getDeviceName() const;

			int getEvent() const { return event_; }
			void setEvent(int event) { event_ = event; }

			MessageFormat getFormat() const { return format_; }
			void setFormat(MessageFormat format) { format_ = format; }

			std::string getChange() const { return change_; }
			void setChange(const std::string& change) { change_ = change; }


			/* --- Instance function members --- */
			void updateSynchronizationChange(SynchronizationChange * newChange);
			const char * serialize() const;
			const char * serialize(int format) const;

			bool isConsolidatedOnly();


			/* --- Class function members --- */
			static void initFieldList();

	
		private:
			/* --- Data members --- */
			HobErrorCode errorCode_;

			int identifier_;
			int batchNumber_;
			int device_;
			int event_;
			MessageFormat format_;
			std::string change_;


			DBAccess * dbAccess_;
			edb::DBSynchronizationChange * myDbRecord_;


			/* --- Class data members --- */
			static FieldOffsetsList _fieldList_;

			static const std::string ProductDevName;
			static const std::string OperatorDevName;
			static const std::string GraphicDevName;
			static const std::string ExpandedTextDevName;
			static const std::string SpecialMessageDevName;
			static const std::string ProductNotesDevName;
			static const std::string ClassDevName;
			static const std::string LabelTypeDevName;
			static const std::string ConfigurationDevName;
			static const std::string FontDevName;
			static const std::string NutrifactsDevName;
			static const std::string GenericTextDevName;
			static const std::string CategoryDevName;
			static const std::string SpecialsDevName;
			static const std::string OperatorNotesDevName;
			static const std::string ProductDefaultsDevName;
			static const std::string EmailAlertsDevName;
			static const std::string COOLDevName;
			static const std::string SoftkeyDevName;
			static const std::string MarqueeDevName;
			static const std::string UnknownDeviceName;


			void init();

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif
	};


}



#endif /* SYNCHRONIZATIONCHANGE_H */
