#ifndef DBPRODUCTSTATISTIC_H
#define DBPRODUCTSTATISTIC_H


#include <string>


#include "edb.h"
#include DBPRODUCTSTATISTICI_INC
#if defined( __linux__ )
    #include DBPRODUCTSTATISTICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBProductStatistic
	{
			// DBProductStatisticsRM class is a friend of DBProductStatistic because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBProductStatisticsRM;
			friend class DBQuery<DBProductStatistic>;
			friend class DBProductsRM;
			friend class DBProductDetailedStatisticsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBProductStatisticI interface_type;
			typedef DBProductStatisticsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBProductStatistic(const DBProductStatistic& rec);
			~DBProductStatistic();


			// Methods associated with DBProductStatisticI
			std::string getLanguage() const { return decorated_->getLanguage(); }
			void setLanguage(const std::string& language) { decorated_->setLanguage(language); }
			bool isLanguageNull() const { return decorated_->isLanguageNull(); }
			bool isLanguageWritable() const { return decorated_->isLanguageWritable(); }

			std::string getHourNumber() const { return decorated_->getHourNumber(); }
			void setHourNumber(const std::string& type) { decorated_->setHourNumber(type); }
			bool isHourNumberNull() const { return decorated_->isHourNumberNull(); }
			bool isHourNumberWritable() const { return decorated_->isHourNumberWritable(); }

			int getTotalSelections() const { return decorated_->getTotalSelections(); }
			void setTotalSelections(int selections) { decorated_->setTotalSelections(selections); }
			void incrementTotalSelections() { decorated_->incrementTotalSelections(); }
			bool isTotalSelectionsNull() const { return decorated_->isTotalSelectionsNull(); }
			void setTotalSelectionsNull() { decorated_->setTotalSelectionsNull(); }
			bool isTotalSelectionsWritable() const { return decorated_->isTotalSelectionsWritable(); }

			int getTimeToSelect() const { return decorated_->getTimeToSelect(); }
			void setTimeToSelect(int time) { decorated_->setTimeToSelect(time); }
			bool isTimeToSelectNull() const { return decorated_->isTimeToSelectNull(); }
			void setTimeToSelectNull() { decorated_->setTimeToSelectNull(); }
			bool isTimeToSelectWritable() const { return decorated_->isTimeToSelectWritable(); }

			int getTotalLabelsPrinted() const { return decorated_->getTotalLabelsPrinted(); }
			void setTotalLabelsPrinted(int printed) { decorated_->setTotalLabelsPrinted(printed); }
			void incrementTotalLabelsPrinted() { decorated_->incrementTotalLabelsPrinted(); }
			bool isTotalLabelsPrintedNull() const { return decorated_->isTotalLabelsPrintedNull(); }
			void setTotalLabelsPrintedNull() { decorated_->setTotalLabelsPrintedNull(); }
			bool isTotalLabelsPrintedWritable() const { return decorated_->isTotalLabelsPrintedWritable(); }

			std::string getRecordedDateTime() const { return decorated_->getRecordedDateTime(); }
			void setRecordedDateTime(const std::string& rdt) { decorated_->setRecordedDateTime(rdt); }
			bool isRecordedDateTimeNull() const { return decorated_->isRecordedDateTimeNull(); }
			void setRecordedDateTimeNull() { decorated_->setRecordedDateTimeNull(); }
			bool isRecordedDateTimeWritable() const { return decorated_->isRecordedDateTimeWritable(); }

			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getLanguage(); }
			void setKey1(const std::string& lan) { decorated_->setLanguage(lan); }
			std::string getKey2() const { return decorated_->getHourNumber(); }
			void setKey2(const std::string& hnm) { decorated_->setHourNumber(hnm); }
			std::string getKey3() const { return decorated_->getProductKey(); }
			void setKey3(const std::string& key) { decorated_->setProductKey(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBProductStatistic& rec);


		private:

			edb_adapter::DBProductStatisticI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBProductStatistic(edb_adapter::DBProductStatisticI* d);

			// Do not allow the default assignment operator.
			DBProductStatistic operator=(const DBProductStatistic& rhs);

			std::string getProductKey() const { return decorated_->getProductKey(); }
			void setProductKey(const std::string& plu) { decorated_->setProductKey(plu); }
			bool isProductKeyNull() const { return decorated_->isProductKeyNull(); }
			bool isProductKeyWritable() const { return decorated_->isProductKeyWritable(); }
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTSTATISTIC_H
