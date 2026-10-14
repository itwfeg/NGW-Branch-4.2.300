#ifndef DBCATEGORYSTATISTIC_H
#define DBCATEGORYSTATISTIC_H


#include <string>


#include "edb.h"
#include DBCATEGORYSTATISTICI_INC
#if defined( __linux__ )
    #include DBCATEGORYSTATISTICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCategoryStatistic
	{
			// DBCategoryStatisticsRM class is a friend of DBCategoryStatistic because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCategoryStatisticsRM;
			friend class DBQuery<DBCategoryStatistic>;
			friend class DBCategorysRM;
			friend class DBCategoryDetailedStatisticsRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCategoryStatisticI interface_type;
			typedef DBCategoryStatisticsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCategoryStatistic(const DBCategoryStatistic& rec);
			~DBCategoryStatistic();


			// Methods associated with DBCategoryStatisticI
			std::string getHourNumber() const { return decorated_->getHourNumber(); }
			void setHourNumber(const std::string& type) { decorated_->setHourNumber(type); }
			bool isHourNumberNull() const { return decorated_->isHourNumberNull(); }
			bool isHourNumberWritable() const { return decorated_->isHourNumberWritable(); }

			std::string getLanguage() const { return decorated_->getLanguage(); }
			void setLanguage(const std::string& language) { decorated_->setLanguage(language); }
			bool isLanguageNull() const { return decorated_->isLanguageNull(); }
			bool isLanguageWritable() const { return decorated_->isLanguageWritable(); }

			std::string getCategoryNumber() const { return decorated_->getCategoryNumber(); }
			void setCategoryNumber(const std::string& plu) { decorated_->setCategoryNumber(plu); }
			bool isCategoryNumberNull() const { return decorated_->isCategoryNumberNull(); }
			bool isCategoryNumberWritable() const { return decorated_->isCategoryNumberWritable(); }

			int getTotalSelections() const { return decorated_->getTotalSelections(); }
			void setTotalSelections(int selections) { decorated_->setTotalSelections(selections); }
			void incrementTotalSelections() { decorated_->incrementTotalSelections(); }
			bool isTotalSelectionsNull() const { return decorated_->isTotalSelectionsNull(); }
			void setTotalSelectionsNull() { decorated_->setTotalSelectionsNull(); }
			bool isTotalSelectionsWritable() const { return decorated_->isTotalSelectionsWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getLanguage(); }
			void setKey1(const std::string& lang) { decorated_->setLanguage(lang); }
			std::string getKey2() const { return decorated_->getHourNumber(); }
			void setKey2(const std::string& hour) { decorated_->setHourNumber(hour); }
			std::string getKey3() const { return decorated_->getCategoryNumber(); }
			void setKey3(const std::string& cat) { decorated_->setCategoryNumber(cat); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCategoryStatistic& rec);


		private:

			edb_adapter::DBCategoryStatisticI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCategoryStatistic(edb_adapter::DBCategoryStatisticI* d);

			// Do not allow the default assignment operator.
			DBCategoryStatistic operator=(const DBCategoryStatistic& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCATEGORYSTATISTIC_H
