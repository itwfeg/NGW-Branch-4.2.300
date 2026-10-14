#ifndef DBKNIFETIMEHISTORY_H
#define DBKNIFETIMEHISTORY_H


#include <string>


#include "edb.h"
#include DBKNIFETIMEHISTORYI_INC
#if defined( __linux__ )
    #include DBKNIFETIMEHISTORYRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBKnifeTimeHistory
	{
			// DBScalesRM class is a friend of DBScale because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBKnifeTimeHistoryRM;
			friend class DBQuery<DBKnifeTimeHistory>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBKnifeTimeHistoryI interface_type;
			typedef DBKnifeTimeHistoryRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBKnifeTimeHistory(const DBKnifeTimeHistory& rec);  
			~DBKnifeTimeHistory();


			// Methods associated with DBKnifeTimeHistoryI
			std::string getType() const { return decorated_->getType(); }
			void setType(const std::string &type) {decorated_->setType(type);}
			bool isTypeNull() const { return decorated_->isTypeNull(); }
			bool isTypeWritable() const { return decorated_->isTypeWritable(); }

			int getKnifeTime() const { return decorated_->getKnifeTime(); }
			void setKnifeTime(int knifeTime) { decorated_->setKnifeTime(knifeTime); }
			bool isKnifeTimeNull() const { return decorated_->isKnifeTimeNull(); }
			void setKnifeTimeNull() { decorated_->setKnifeTimeNull(); }
			bool isKnifeTimeWritable() const { return decorated_->isKnifeTimeWritable(); }

			std::string getTimeStampDateTime() const { return decorated_->getTimeStampDateTime(); }
			void setTimeStampDateTime(const std::string& timeStamp) { decorated_->setTimeStampDateTime(timeStamp); }
			bool isTimeStampDateTimeNull() const { return decorated_->isTimeStampDateTimeNull(); }
			void setTimeStampDateTimeNull() { decorated_->setTimeStampDateTimeNull(); }
			bool isTimeStampDateTimeWritable() const { return decorated_->isTimeStampDateTimeWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }
						

			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBKnifeTimeHistory& rec);


		private:

			edb_adapter::DBKnifeTimeHistoryI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBKnifeTimeHistory(edb_adapter::DBKnifeTimeHistoryI* d);

			// Do not allow the default assignment operator.
			DBKnifeTimeHistory operator=(const DBKnifeTimeHistory& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBKNIFETIMEHISTORY_H
