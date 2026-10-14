#ifndef DBCATEGORYDETAILEDSTATISTIC_H
#define DBCATEGORYDETAILEDSTATISTIC_H


#include <string>


#include "edb.h"
#include DBCATEGORYDETAILEDSTATISTICI_INC
#if defined( __linux__ )
    #include DBCATEGORYDETAILEDSTATISTICSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCategoryDetailedStatistic
	{
			// DBCategoryDetailedStatisticsRM class is a friend of DBCategoryDetailedStatistic because it needs
			// access to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCategoryDetailedStatisticsRM;
			friend class DBQuery<DBCategoryDetailedStatistic>;
			friend class DBCategorysRM;

		public:

			// Types and subtypes
			typedef edb_adapter::DBCategoryDetailedStatisticI interface_type;
			typedef DBCategoryDetailedStatisticsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCategoryDetailedStatistic(const DBCategoryDetailedStatistic& rec);
			 ~DBCategoryDetailedStatistic();


			// Methods associated with DBCategoryDetailedStatisticI
			std::string getPresentationMethodType() const { return decorated_->getPresentationMethodType(); }
			void setPresentationMethodType(const std::string& type) { decorated_->setPresentationMethodType(type); }
			bool isPresentationMethodTypeNull() const { return decorated_->isPresentationMethodTypeNull(); }
			bool isPresentationMethodTypeWritable() const { return decorated_->isPresentationMethodTypeWritable(); }

			std::string getCategoryStatisticLanguage() const { return decorated_->getCategoryStatisticLanguage(); }
			void setCategoryStatisticLanguage(const std::string& language) { decorated_->setCategoryStatisticLanguage(language); }
			bool isCategoryStatisticLanguageNull() const { return decorated_->isCategoryStatisticLanguageNull(); }
			bool isCategoryStatisticLanguageWritable() const { return decorated_->isCategoryStatisticLanguageWritable(); }

			std::string getCategoryStatisticHourNumber() const { return decorated_->getCategoryStatisticHourNumber(); }
			void setCategoryStatisticHourNumber(const std::string& type) { decorated_->setCategoryStatisticHourNumber(type); }
			bool isCategoryStatisticHourNumberNull() const { return decorated_->isCategoryStatisticHourNumberNull(); }
			bool isCategoryStatisticHourNumberWritable() const { return decorated_->isCategoryStatisticHourNumberWritable(); }

			std::string getCategoryNumber() const { return decorated_->getCategoryNumber(); }
			void setCategoryNumber(const std::string& plu) { decorated_->setCategoryNumber(plu); }
			bool isCategoryNumberNull() const { return decorated_->isCategoryNumberNull(); }
			bool isCategoryNumberWritable() const { return decorated_->isCategoryNumberWritable(); }

			int getSelections() const { return decorated_->getSelections(); }
			void setSelections(int selections) { decorated_->setSelections(selections); }
			void incrementSelections() { decorated_->incrementSelections(); }
			bool isSelectionsNull() const { return decorated_->isSelectionsNull(); }
			void setSelectionsNull() { decorated_->setSelectionsNull(); }
			bool isSelectionsWritable() const { return decorated_->isSelectionsWritable(); }


			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getKey1() const { return decorated_->getPresentationMethodType(); }
			void setKey1(const std::string& type) { decorated_->setPresentationMethodType(type); }
			std::string getKey2() const { return decorated_->getCategoryStatisticLanguage(); }
			void setKey2(const std::string& lng) { decorated_->setCategoryStatisticLanguage(lng); }
			std::string getKey3() const { return decorated_->getCategoryStatisticHourNumber(); }
			void setKey3(const std::string& our) { decorated_->setCategoryStatisticHourNumber(our); }
			std::string getKey4() const { return decorated_->getCategoryNumber(); }
			void setKey4(const std::string& num) { decorated_->setCategoryNumber(num); }

			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCategoryDetailedStatistic& rec);


		private:

			edb_adapter::DBCategoryDetailedStatisticI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCategoryDetailedStatistic(edb_adapter::DBCategoryDetailedStatisticI* d);

			// Do not allow the default assignment operator.
			DBCategoryDetailedStatistic operator=(const DBCategoryDetailedStatistic& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCATEGORYDETAILEDSTATISTIC_H
