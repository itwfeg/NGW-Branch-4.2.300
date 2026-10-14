#ifndef DBPRODUCTDETAILEDSTATISTIC_H
#define DBPRODUCTDETAILEDSTATISTIC_H


#include <string>


#include "edb.h"
#include DBPRODUCTDETAILEDSTATISTICI_INC
#if defined( __linux__ )
    #include DBPRODUCTDETAILEDSTATISTICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBProductDetailedStatistic
	{
			// DBProductDetailedStatisticsRM class is a friend of DBProductDetailedStatistic because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBProductDetailedStatisticsRM;
			friend class DBQuery<DBProductDetailedStatistic>;

		public:

			// Types and subtypes
			typedef edb_adapter::DBProductDetailedStatisticI interface_type;
			typedef DBProductDetailedStatisticsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBProductDetailedStatistic(const DBProductDetailedStatistic& rec);
			~DBProductDetailedStatistic();


			// Methods associated with DBProductDetailedStatisticI
			std::string getPresentationMethodNumber() const { return decorated_->getPresentationMethodNumber(); }
			void setPresentationMethodNumber(const std::string& number) { decorated_->setPresentationMethodNumber(number); }
			bool isPresentationMethodNumberNull() const { return decorated_->isPresentationMethodNumberNull(); }
			bool isPresentationMethodNumberWritable() const { return decorated_->isPresentationMethodNumberWritable(); }

			std::string getProductStatisticLanguage() const { return decorated_->getProductStatisticLanguage(); }
			void setProductStatisticLanguage(const std::string& language) { decorated_->setProductStatisticLanguage(language); }
			bool isProductStatisticLanguageNull() const { return decorated_->isProductStatisticLanguageNull(); }
			bool isProductStatisticLanguageWritable() const { return decorated_->isProductStatisticLanguageWritable(); }

			std::string getProductStatisticHourNumber() const { return decorated_->getProductStatisticHourNumber(); }
			void setProductStatisticHourNumber(const std::string& hour) { decorated_->setProductStatisticHourNumber(hour); }
			bool isProductStatisticHourNumberNull() const { return decorated_->isProductStatisticHourNumberNull(); }
			bool isProductStatisticHourNumberWritable() const { return decorated_->isProductStatisticHourNumberWritable(); }

			int getSelections() const { return decorated_->getSelections(); }
			void setSelections(int selections) { decorated_->setSelections(selections); }
			void incrementSelections() { decorated_->incrementSelections(); }
			bool isSelectionsNull() const { return decorated_->isSelectionsNull(); }
			void setSelectionsNull() { decorated_->setSelectionsNull(); }
			bool isSelectionsWritable() const { return decorated_->isSelectionsWritable(); }

			int getLabelsPrinted() const { return decorated_->getLabelsPrinted(); }
			void setLabelsPrinted(int printed) { decorated_->setLabelsPrinted(printed); }
			void incrementLabelsPrinted() { decorated_->incrementLabelsPrinted(); }
			bool isLabelsPrintedNull() const { return decorated_->isLabelsPrintedNull(); }
			void setLabelsPrintedNull() { decorated_->setLabelsPrintedNull(); }
			bool isLabelsPrintedWritable() const { return decorated_->isLabelsPrintedWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getPresentationMethodNumber(); }
			void setKey1(const std::string& pmn) { decorated_->setPresentationMethodNumber(pmn); }
			std::string getKey2() const { return decorated_->getProductStatisticLanguage(); }
			void setKey2(const std::string& psl) { decorated_->setProductStatisticLanguage(psl); }
			std::string getKey3() const { return decorated_->getProductStatisticHourNumber(); }
			void setKey3(const std::string& hnm) { decorated_->setProductStatisticHourNumber(hnm); }
			std::string getKey4() const { return decorated_->getProductKey(); }
			void setKey4(const std::string& key) { decorated_->setProductKey(key); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBProductDetailedStatistic& rec);


		private:

			edb_adapter::DBProductDetailedStatisticI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBProductDetailedStatistic(edb_adapter::DBProductDetailedStatisticI* d);

			// Do not allow the default assignment operator.
			DBProductDetailedStatistic operator=(const DBProductDetailedStatistic& rhs);

			std::string getProductKey() const { return decorated_->getProductKey(); }
			void setProductKey(const std::string& plu) { decorated_->setProductKey(plu); }
			bool isProductKeyNull() const { return decorated_->isProductKeyNull(); }
			bool isProductKeyWritable() const { return decorated_->isProductKeyWritable(); }
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTDETAILEDSTATISTIC_H
