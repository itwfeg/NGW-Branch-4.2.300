#ifndef DBSYNCHRONIZATIONCHANGEI_H
#define DBSYNCHRONIZATIONCHANGEI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSynchronizationChangeI
	{
		public:

			virtual ~DBSynchronizationChangeI() {}


			virtual DBSynchronizationChangeI* clone() const = 0;
			virtual const DBSynchronizationChangeI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getIdentifier() const = 0;
			virtual void setIdentifier(const std::string& changeId) = 0;
			virtual bool isIdentifierNull() const { return false; }
			virtual bool isIdentifierWritable() const { return true; }
			
			virtual std::string getBatchNumber() const = 0;
			virtual void setBatchNumber(const std::string& bn) = 0;
			virtual bool isBatchNumberNull() const { return false; }
			virtual void setBatchNumberNull() = 0;
			virtual bool isBatchNumberWritable() const { return true; }

			virtual int getApplicationDevice() const = 0;
			virtual void setApplicationDevice(int dev) = 0;
			virtual bool isApplicationDeviceNull() const { return false; }
			virtual void setApplicationDeviceNull() = 0;
			virtual bool isApplicationDeviceWritable() const { return true; }

			virtual int getApplicationEvent() const = 0;
			virtual void setApplicationEvent(int evt) = 0;
			virtual bool isApplicationEventNull() const { return false; }
			virtual void setApplicationEventNull() = 0;
			virtual bool isApplicationEventWritable() const { return true; }

			virtual int getApplicationFormat() const = 0;
			virtual void setApplicationFormat(int fmt) = 0;
			virtual bool isApplicationFormatNull() const { return false; }
			virtual void setApplicationFormatNull() = 0;
			virtual bool isApplicationFormatWritable() const { return true; }

			virtual std::string getChange() const = 0;
			virtual void setChange(const std::string& change) = 0;
			virtual bool isChangeNull() const { return false; }
			virtual void setChangeNull() = 0;
			virtual bool isChangeWritable() const { return true; }


			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSYNCHRONIZATIONCHANGEI_H
