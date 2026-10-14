#ifndef SQLITESYNCHRONIZATIONCHANGE_H
#define SQLITESYNCHRONIZATIONCHANGE_H


#include <string>


#include "edb.h"
#include "sqliterecord.h"

#include DBSYNCHRONIZATIONCHANGEI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteSynchronizationChange : public DBSynchronizationChangeI
	{
		public:

			// Types and subtypes
			typedef DBSynchronizationChangeI interface_type;

			
			// Constructors and destructor
			SqliteSynchronizationChange(SqliteRecord* r, DBSynchronizationChangeI* d = 0);
			~SqliteSynchronizationChange();


			// Think about replacing this method with two friend functions from the
			// SqliteSynchronizationChangesRM class (next and previous).
			const SqliteRecord* getRecord() const { return rec_; }


			// Methods from DBSynchronizationChangeI
			DBSynchronizationChangeI* clone() const { return new SqliteSynchronizationChange(new SqliteRecord(*rec_)); }
			const DBSynchronizationChangeI* getDecorated() const { return decorated_; }

			std::string getField(const std::string& field) const { return rec_->getField(field); }
			void setField(const std::string& field, const std::string& value) { rec_->setField(field, value); }


			std::string getIdentifier() const { return rec_->getField(IDENTIFIER_FIELD); }
			void setIdentifier(const std::string& changeId) { rec_->setField(IDENTIFIER_FIELD, changeId); }
			bool isIdentifierNull() const { return rec_->isFieldNull(IDENTIFIER_FIELD); }
			bool isIdentifierWritable() const { return true; }

			std::string getBatchNumber() const { return rec_->getField(BATCHNUMBER_FIELD); }
			void setBatchNumber(const std::string& number) { rec_->setField(BATCHNUMBER_FIELD, number); }
			bool isBatchNumberNull() const { return rec_->isFieldNull(BATCHNUMBER_FIELD); }
			void setBatchNumberNull() { rec_->setFieldNull(BATCHNUMBER_FIELD); }
			bool isBatchNumberWritable() const { return true; }

			int getApplicationDevice() const { return rec_->getField(APPLICATIONDEVICE_FIELD); }
			void setApplicationDevice(int dev) { rec_->setField(APPLICATIONDEVICE_FIELD, dev); }
			bool isApplicationDeviceNull() const { return rec_->isFieldNull(APPLICATIONDEVICE_FIELD); }
			void setApplicationDeviceNull() { rec_->setFieldNull(APPLICATIONDEVICE_FIELD); }
			bool isApplicationDeviceWritable() const { return true; }

			int getApplicationEvent() const { return rec_->getField(APPLICATIONEVENT_FIELD); }
			void setApplicationEvent(int evt) { rec_->setField(APPLICATIONEVENT_FIELD, evt); }
			bool isApplicationEventNull() const { return rec_->isFieldNull(APPLICATIONEVENT_FIELD); }
			void setApplicationEventNull() { rec_->setFieldNull(APPLICATIONEVENT_FIELD); }
			bool isApplicationEventWritable() const { return true; }

			int getApplicationFormat() const { return rec_->getField(APPLICATIONFORMAT_FIELD); }
			void setApplicationFormat(int evt) { rec_->setField(APPLICATIONFORMAT_FIELD, evt); }
			bool isApplicationFormatNull() const { return rec_->isFieldNull(APPLICATIONFORMAT_FIELD); }
			void setApplicationFormatNull() { rec_->setFieldNull(APPLICATIONFORMAT_FIELD); }
			bool isApplicationFormatWritable() const { return true; }

			std::string getChange() const { return rec_->getField(CHANGE_FIELD); }
			void setChange(const std::string& change) { rec_->setField(CHANGE_FIELD, change); }
			bool isChangeNull() const { return rec_->isFieldNull(CHANGE_FIELD); }
			void setChangeNull() { rec_->setFieldNull(CHANGE_FIELD); }
			bool isChangeWritable() const { return true; }


			virtual bool isRecordValid() { return rec_->isValid(); }


			static const std::string IDENTIFIER_FIELD;
			static const std::string BATCHNUMBER_FIELD;
			static const std::string APPLICATIONDEVICE_FIELD;
			static const std::string APPLICATIONEVENT_FIELD;
			static const std::string APPLICATIONFORMAT_FIELD;
			static const std::string CHANGE_FIELD;


		private:

			SqliteRecord* rec_;
			DBSynchronizationChangeI* decorated_;

			// Do not allow the default copy constructor.
			SqliteSynchronizationChange(const SqliteSynchronizationChange& rec);
			// Do not allow the default assignment batch.
			SqliteSynchronizationChange operator=(const SqliteSynchronizationChange& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESYNCHRONIZATIONCHANGE_H
