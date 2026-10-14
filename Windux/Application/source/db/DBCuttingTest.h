#ifndef DBCUTTINGTEST_H
#define DBCUTTINGTEST_H


#include <string>


#include "edb.h"
#include DBCUTTINGTESTI_INC
#if defined( __linux__ )
    #include DBCUTTINGTESTSRM_INC
#endif


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	template <class DBEntity> class DBQuery;


	class DBCuttingTest
	{
			// DBCuttingTestsRM class is a friend of DBCuttingTest because it needs access
			// to its "decorated_" member, which is private and we don't want to make
			// it publicly available, neither directly nor through a getter method.
			// It also needs access to the private constructor.
			friend class DBCuttingTestsRM;
			friend class DBQuery<DBCuttingTest>;
			friend class DBOperatorsRM;      // Needs access to decorated_ for cross-referencing.

		public:

			// Types and subtypes
			typedef edb_adapter::DBCuttingTestI interface_type;
			typedef DBCuttingTestsRM manager_type;


			// Constructors and destructor
			// The copy constructor is required for iterator interface. It is also made
			// publicly available so the application may keep copies of a retrived
			// record instead of looking it up too many times.
			DBCuttingTest(const DBCuttingTest& rec);
			~DBCuttingTest();


			// Methods from DBCuttingTestI
			std::string getTestNumber() const { return decorated_->getTestNumber(); }
			void setTestNumber(const std::string& num) { decorated_->setTestNumber(num); }
			bool isTestNumberNull() const { return decorated_->isTestNumberNull(); }
			bool isTestNumberWritable() const { return decorated_->isTestNumberWritable(); }

			std::string getOperatorNumber() const { return decorated_->getOperatorNumber(); }
			void setOperatorNumber(const std::string& num) { decorated_->setOperatorNumber(num); }
			bool isOperatorNumberNull() const { return decorated_->isOperatorNumberNull(); }
			void setOperatorNumberNull() { decorated_->setOperatorNumberNull(); }
			bool isOperatorNumberWritable() const { return decorated_->isOperatorNumberWritable(); }

			std::string getProductNumber() const { return decorated_->getProductNumber(); }
			void setProductNumber(const std::string& num) { decorated_->setProductNumber(num); }
			bool isProductNumberNull() const { return decorated_->isProductNumberNull(); }
			void setProductNumberNull() { decorated_->setProductNumberNull(); }
			bool isProductNumberWritable() const { return decorated_->isProductNumberWritable(); }

			std::string getVendorNumber() const { return decorated_->getVendorNumber(); }
			void setVendorNumber(const std::string& num) { decorated_->setVendorNumber(num); }
			bool isVendorNumberNull() const { return decorated_->isVendorNumberNull(); }
			void setVendorNumberNull() { decorated_->setVendorNumberNull(); }
			bool isVendorNumberWritable() const { return decorated_->isVendorNumberWritable(); }

			int getPrimalWeight() const { return decorated_->getPrimalWeight(); }
			void setPrimalWeight(int w) { decorated_->setPrimalWeight(w); }
			bool isPrimalWeightNull() const { return decorated_->isPrimalWeightNull(); }
			void setPrimalWeightNull() { decorated_->setPrimalWeightNull(); }
			bool isPrimalWeightWritable() const { return decorated_->isPrimalWeightWritable(); }

			int getPrimalCost() const { return decorated_->getPrimalCost(); }
			void setPrimalCost(int c) { decorated_->setPrimalCost(c); }
			bool isPrimalCostNull() const { return decorated_->isPrimalCostNull(); }
			void setPrimalCostNull() { decorated_->setPrimalCostNull(); }
			bool isPrimalCostWritable() const { return decorated_->isPrimalCostWritable(); }

			std::string getStartDate() const { return decorated_->getStartDate(); }
			void setStartDate(const std::string& date) { decorated_->setStartDate(date); }
			bool isStartDateNull() const { return decorated_->isStartDateNull(); }
			void setStartDateNull() { decorated_->isStartDateNull(); }
			bool isStartDateWritable() const { return decorated_->isStartDateWritable(); }

			std::string getEndDate() const { return decorated_->getEndDate(); }
			void setEndDate(const std::string& date) { decorated_->setEndDate(date); }
			bool isEndDateNull() const { return decorated_->isEndDateNull(); }
			void setEndDateNull() { decorated_->isEndDateNull(); }
			bool isEndDateWritable() const { return decorated_->isEndDateWritable(); }

			std::string getLotNumber() const { return decorated_->getLotNumber(); }
			void setLotNumber(const std::string& num) { decorated_->setLotNumber(num); }
			bool isLotNumberNull() const { return decorated_->isLotNumberNull(); }
			void setLotNumberNull() { decorated_->setLotNumberNull(); }
			bool isLotNumberWritable() const { return decorated_->isLotNumberWritable(); }

			std::string getPackingDate() const { return decorated_->getPackingDate(); }
			void setPackingDate(const std::string& date) { decorated_->setPackingDate(date); }
			bool isPackingDateNull() const { return decorated_->isPackingDateNull(); }
			void setPackingDateNull() { decorated_->isPackingDateNull(); }
			bool isPackingDateWritable() const { return decorated_->isPackingDateWritable(); }

			std::string getPackingHours() const { return decorated_->getPackingHours(); }
			void setPackingHours(const std::string& hours) { decorated_->setPackingHours(hours); }
			bool isPackingHoursNull() const { return decorated_->isPackingHoursNull(); }
			void setPackingHoursNull() { decorated_->isPackingHoursNull(); }
			bool isPackingHoursWritable() const { return decorated_->isPackingHoursWritable(); }

			std::string getPackingMinutes() const { return decorated_->getPackingMinutes(); }
			void setPackingMinutes(const std::string& minutes) { decorated_->setPackingMinutes(minutes); }
			bool isPackingMinutesNull() const { return decorated_->isPackingMinutesNull(); }
			void setPackingMinutesNull() { decorated_->isPackingMinutesNull(); }
			bool isPackingMinutesWritable() const { return decorated_->isPackingMinutesWritable(); }

			std::string getComments() const { return decorated_->getComments(); }
			void setComments(const std::string& comments) { decorated_->setComments(comments); }
			bool isCommentsNull() const { return decorated_->isCommentsNull(); }
			void setCommentsNull() { decorated_->setCommentsNull(); }
			bool isCommentsWritable() const { return decorated_->isCommentsWritable(); }

			std::string getFinalComments() const { return decorated_->getFinalComments(); }
			void setFinalComments(const std::string& finalComments) { decorated_->setFinalComments(finalComments); }
			bool isFinalCommentsNull() const { return decorated_->isFinalCommentsNull(); }
			void setFinalCommentsNull() { decorated_->setFinalCommentsNull(); }
			bool isFinalCommentsWritable() const { return decorated_->isFinalCommentsWritable(); }

			int getStatus() const { return decorated_->getStatus(); }
			void setStatus(const int stat) { decorated_->setStatus(stat); }
			bool isStatusNull() const { return decorated_->isStatusNull(); }
			void setStatusNull() { decorated_->setStatusNull(); }
			bool isStatusWritable() const { return decorated_->isStatusWritable(); }

			std::string getPercentTotalShrinkage() const { return decorated_->getPercentTotalShrinkage(); }
			void setPercentTotalShrinkage(const std::string& percentTotalShrinkage) { decorated_->setPercentTotalShrinkage(percentTotalShrinkage); }
			bool isPercentTotalShrinkageNull() const { return decorated_->isPercentTotalShrinkageNull(); }
			void setPercentTotalShrinkageNull() { decorated_->setPercentTotalShrinkageNull(); }
			bool isPercentTotalShrinkageWritable() const { return decorated_->isPercentTotalShrinkageWritable(); }

			std::string getTotalWeightOfCutProducts() const { return decorated_->getTotalWeightOfCutProducts(); }
			void setTotalWeightOfCutProducts(const std::string& totalWeightOfCutProducts) { decorated_->setTotalWeightOfCutProducts(totalWeightOfCutProducts); }
			bool isTotalWeightOfCutProductsNull() const { return decorated_->isTotalWeightOfCutProductsNull(); }
			void setTotalWeightOfCutProductsNull() { decorated_->setTotalWeightOfCutProductsNull(); }
			bool isTotalWeightOfCutProductsWritable() const { return decorated_->isTotalWeightOfCutProductsWritable(); }

			std::string getTotalPriceOfCutProducts() const { return decorated_->getTotalPriceOfCutProducts(); }
			void setTotalPriceOfCutProducts(const std::string& totalPriceOfCutProducts) { decorated_->setTotalPriceOfCutProducts(totalPriceOfCutProducts); }
			bool isTotalPriceOfCutProductsNull() const { return decorated_->isTotalPriceOfCutProductsNull(); }
			void setTotalPriceOfCutProductsNull() { decorated_->setTotalPriceOfCutProductsNull(); }
			bool isTotalPriceOfCutProductsWritable() const { return decorated_->isTotalPriceOfCutProductsWritable(); }

			bool isRecordValid() const { return decorated_->isRecordValid(); }


			// Testing methods
			std::string getField(const std::string& field) const { return decorated_->getField(field); }
			void setField(const std::string& field, const std::string& value) { decorated_->setField(field, value); }


			// Printer.
			friend std::ostream& operator<<(std::ostream& os, const DBCuttingTest& rec);


		private:

			edb_adapter::DBCuttingTestI* decorated_;

			// Constructors are private so that only friend classes can execute them
			DBCuttingTest(edb_adapter::DBCuttingTestI* d);

			// Do not allow the default assignment operator.
			DBCuttingTest operator=(const DBCuttingTest& rhs);
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCUTTINGTEST_H
