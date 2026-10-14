#ifndef DBSYNCHRONIZATIONCHANGE_H
#define DBSYNCHRONIZATIONCHANGE_H


#include <string>


#include "edb.h"
#include DBSYNCHRONIZATIONCHANGEI_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBSynchronizationChangesRM;
	template <class DBEntity> class DBQuery;


	class DBSynchronizationChange
	{
			// DBSynchronizationChangesRM class is a friend of DBSynchronizationChange because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBSynchronizationChangesRM;
			friend class DBQuery<DBSynchronizationChange>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBSynchronizationChangeI interface_type;
			typedef DBSynchronizationChangesRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBSynchronizationChange(const DBSynchronizationChange& rec);
			~DBSynchronizationChange();


			// Methods from DBSynchronizationChangeI
			std::string getIdentifier() const { return decorated_->getIdentifier(); }
//			void setIdentifier(const std::string& changeId) { decorated_->setIdentifier(changeId); }
			bool isIdentifierNull() const { return decorated_->isIdentifierNull(); }
			bool isIdentifierWritable() const { return decorated_->isIdentifierWritable(); }

			std::string getBatchNumber() const { return decorated_->getBatchNumber(); }
			void setBatchNumber(const std::string& batch) { return decorated_->setBatchNumber(batch); }
			bool isBatchNumberNull() const { return decorated_->isBatchNumberNull(); }
			void setBatchNumberNull() { decorated_->setBatchNumberNull(); }
			bool isBatchNumberWritable() const { return decorated_->isBatchNumberWritable(); }

			int getApplicationDevice() const { return decorated_->getApplicationDevice(); }
			void setApplicationDevice(int c) { decorated_->setApplicationDevice(c); }
			bool isApplicationDeviceNull() const { return decorated_->isApplicationDeviceNull(); }
			void setApplicationDeviceNull() { decorated_->setApplicationDeviceNull(); }
			bool isApplicationDeviceWritable() const { return decorated_->isApplicationDeviceWritable(); }

			int getApplicationEvent() const { return decorated_->getApplicationEvent(); }
			void setApplicationEvent(int old) { decorated_->setApplicationEvent(old); }
			bool isApplicationEventNull() const { return decorated_->isApplicationEventNull(); }
			void setApplicationEventNull() { decorated_->setApplicationEventNull(); }
			bool isApplicationEventWritable() const { return decorated_->isApplicationEventWritable(); }

			int getApplicationFormat() const { return decorated_->getApplicationFormat(); }
			void setApplicationFormat(int fmt) { decorated_->setApplicationFormat(fmt); }
			bool isApplicationFormatNull() const { return decorated_->isApplicationFormatNull(); }
			void setApplicationFormatNull() { decorated_->setApplicationFormatNull(); }
			bool isApplicationFormatWritable() const { return decorated_->isApplicationFormatWritable(); }

			std::string getChange() const { return decorated_->getChange(); }
			void setChange(const std::string& old) { decorated_->setChange(old); }
			bool isChangeNull() const { return decorated_->isChangeNull(); }
			void setChangeNull() { decorated_->setChangeNull(); }
			bool isChangeWritable() const { return decorated_->isChangeWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
//			std::string getKey() const { return decorated_->getDateTime(); }
//			void setKey(const std::string& key) { decorated_->setDateTime(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBSynchronizationChange& rec);


		private:

			edb_adapter::DBSynchronizationChangeI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBSynchronizationChange(edb_adapter::DBSynchronizationChangeI* d);

			// Do not allow the default assignment operator.
			DBSynchronizationChange operator=(const DBSynchronizationChange& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSYNCHRONIZATIONCHANGE_H
