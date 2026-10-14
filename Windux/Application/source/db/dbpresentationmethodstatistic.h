#ifndef DBPRESENTATIONMETHODSTATISTIC_H
#define DBPRESENTATIONMETHODSTATISTIC_H


#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODSTATISTICI_INC
#if defined( __linux__ )
    #include DBPRESENTATIONMETHODSTATISTICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBPresentationMethodStatistic
	{
			// DBPresentationMethodStatisticsRM class is a friend of DBPresentationMethodStatistic because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBPresentationMethodStatisticsRM;
			friend class DBQuery<DBPresentationMethodStatistic>;
			friend class DBPresentationMethodsRM;
			friend class DBPresentationMethodDetailedStatisticsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBPresentationMethodStatisticI interface_type;
			typedef DBPresentationMethodStatisticsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBPresentationMethodStatistic(const DBPresentationMethodStatistic& rec);
			~DBPresentationMethodStatistic();


			// Methods associated with DBPresentationMethodStatisticI
			std::string getPresentationMethodType() const { return decorated_->getPresentationMethodType(); }
			void setPresentationMethodType(const std::string& type) { decorated_->setPresentationMethodType(type); }
			bool isPresentationMethodTypeNull() const { return decorated_->isPresentationMethodTypeNull(); }
			bool isPresentationMethodTypeWritable() const { return decorated_->isPresentationMethodTypeWritable(); }

			std::string getLanguage() const { return decorated_->getLanguage(); }
			void setLanguage(const std::string& type) { decorated_->setLanguage(type); }
			bool isLanguageNull() const { return decorated_->isLanguageNull(); }
			bool isLanguageWritable() const { return decorated_->isLanguageWritable(); }

			std::string getHourNumber() const { return decorated_->getHourNumber(); }
			void setHourNumber(const std::string& type) { decorated_->setHourNumber(type); }
			bool isHourNumberNull() const { return decorated_->isHourNumberNull(); }
			bool isHourNumberWritable() const { return decorated_->isHourNumberWritable(); }

			int getTotalMethodSelections() const { return decorated_->getTotalMethodSelections(); }
			void setTotalMethodSelections(int selections) { decorated_->setTotalMethodSelections(selections); }
			void incrementTotalMethodSelections() { decorated_->incrementTotalMethodSelections(); }
			bool isTotalMethodSelectionsNull() const { return decorated_->isTotalMethodSelectionsNull(); }
			void setTotalMethodSelectionsNull() { decorated_->setTotalMethodSelectionsNull(); }
			bool isTotalMethodSelectionsWritable() const { return decorated_->isTotalMethodSelectionsWritable(); }

			int getTotalCategorySelections() const { return decorated_->getTotalCategorySelections(); }
			void setTotalCategorySelections(int selections) { decorated_->setTotalCategorySelections(selections); }
			void incrementTotalCategorySelections() { decorated_->incrementTotalCategorySelections(); }
			bool isTotalCategorySelectionsNull() const { return decorated_->isTotalCategorySelectionsNull(); }
			void setTotalCategorySelectionsNull() { decorated_->setTotalCategorySelectionsNull(); }
			bool isTotalCategorySelectionsWritable() const { return decorated_->isTotalCategorySelectionsWritable(); }

			int getTotalProductSelections() const { return decorated_->getTotalProductSelections(); }
			void setTotalProductSelections(int selections) { decorated_->setTotalProductSelections(selections); }
			void incrementTotalProductSelections() { decorated_->incrementTotalProductSelections(); }
			bool isTotalProductSelectionsNull() const { return decorated_->isTotalProductSelectionsNull(); }
			void setTotalProductSelectionsNull() { decorated_->setTotalProductSelectionsNull(); }
			bool isTotalProductSelectionsWritable() const { return decorated_->isTotalProductSelectionsWritable(); }

			int getTotalLabelsPrinted() const { return decorated_->getTotalLabelsPrinted(); }
			void setTotalLabelsPrinted(int selections) { decorated_->setTotalLabelsPrinted(selections); }
			void incrementTotalLabelsPrinted() { decorated_->incrementTotalLabelsPrinted(); }
			bool isTotalLabelsPrintedNull() const { return decorated_->isTotalLabelsPrintedNull(); }
			void setTotalLabelsPrintedNull() { decorated_->setTotalLabelsPrintedNull(); }
			bool isTotalLabelsPrintedWritable() const { return decorated_->isTotalLabelsPrintedWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getPresentationMethodType(); }
			void setKey1(const std::string& number) { decorated_->setPresentationMethodType(number); }
			std::string getKey2() const { return decorated_->getLanguage(); }
			void setKey2(const std::string& lang) { decorated_->setLanguage(lang); }
			std::string getKey3() const { return decorated_->getHourNumber(); }
			void setKey3(const std::string& hour) { decorated_->setHourNumber(hour); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBPresentationMethodStatistic& rec);


		private:

			edb_adapter::DBPresentationMethodStatisticI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBPresentationMethodStatistic(edb_adapter::DBPresentationMethodStatisticI* d);

			// Do not allow the default assignment operator.
			DBPresentationMethodStatistic operator=(const DBPresentationMethodStatistic& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRESENTATIONMETHODSTATISTIC_H
