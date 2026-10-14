#ifndef DBPRESENTATIONMETHODDETAILEDSTATISTIC_H
#define DBPRESENTATIONMETHODDETAILEDSTATISTIC_H


#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODDETAILEDSTATISTICI_INC
#if defined( __linux__ )
    #include DBPRESENTATIONMETHODDETAILEDSTATISTICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBPresentationMethodDetailedStatistic
	{
			// DBPresentationMethodDetailedStatisticsRM class is a friend of DBPresentationMethodDetailedStatistic because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBPresentationMethodDetailedStatisticsRM;
			friend class DBQuery<DBPresentationMethodDetailedStatistic>;
			friend class DBPresentationMethodsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBPresentationMethodDetailedStatisticI interface_type;
			typedef DBPresentationMethodDetailedStatisticsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBPresentationMethodDetailedStatistic(const DBPresentationMethodDetailedStatistic& rec);
			~DBPresentationMethodDetailedStatistic();


			// Methods associated with DBPresentationMethodDetailedStatisticI
			std::string getPresentationMethodType() const { return decorated_->getPresentationMethodType(); }
			void setPresentationMethodType(const std::string& number) { decorated_->setPresentationMethodType(number); }
			bool isPresentationMethodTypeNull() const { return decorated_->isPresentationMethodTypeNull(); }
			bool isPresentationMethodTypeWritable() const { return decorated_->isPresentationMethodTypeWritable(); }

			std::string getPresentationMethodStatisticHourNumber() const { return decorated_->getPresentationMethodStatisticHourNumber(); }
			void setPresentationMethodStatisticHourNumber(const std::string& hour) { decorated_->setPresentationMethodStatisticHourNumber(hour); }
			bool isPresentationMethodStatisticHourNumberNull() const { return decorated_->isPresentationMethodStatisticHourNumberNull(); }
			bool isPresentationMethodStatisticHourNumberWritable() const { return decorated_->isPresentationMethodStatisticHourNumberWritable(); }

			std::string getPresentationMethodStatisticMethodType() const { return decorated_->getPresentationMethodStatisticMethodType(); }
			void setPresentationMethodStatisticMethodType(const std::string& method) { decorated_->setPresentationMethodStatisticMethodType(method); }
			bool isPresentationMethodStatisticMethodTypeNull() const { return decorated_->isPresentationMethodStatisticMethodTypeNull(); }
			bool isPresentationMethodStatisticMethodTypeWritable() const { return decorated_->isPresentationMethodStatisticMethodTypeWritable(); }

			int getSelections() const { return decorated_->getSelections(); }
			void setSelections(int selections) { decorated_->setSelections(selections); }
			void incrementSelections() { decorated_->incrementSelections(); }
			bool isSelectionsNull() const { return decorated_->isSelectionsNull(); }
			void setSelectionsNull() { decorated_->setSelectionsNull(); }
			bool isSelectionsWritable() const { return decorated_->isSelectionsWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getPresentationMethodType(); }
			void setKey1(const std::string& number) { decorated_->setPresentationMethodType(number); }
			std::string getKey2() const { return decorated_->getPresentationMethodStatisticHourNumber(); }
			void setKey2(const std::string& hour) { decorated_->setPresentationMethodStatisticHourNumber(hour); }
			std::string getKey3() const { return decorated_->getPresentationMethodStatisticMethodType(); }
			void setKey3(const std::string& method) { decorated_->setPresentationMethodStatisticMethodType(method); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBPresentationMethodDetailedStatistic& rec);


		private:

			edb_adapter::DBPresentationMethodDetailedStatisticI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBPresentationMethodDetailedStatistic(edb_adapter::DBPresentationMethodDetailedStatisticI* d);

			// Do not allow the default assignment operator.
			DBPresentationMethodDetailedStatistic operator=(const DBPresentationMethodDetailedStatistic& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRESENTATIONMETHODDETAILEDSTATISTIC_H
