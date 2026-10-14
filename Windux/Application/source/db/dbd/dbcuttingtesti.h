#ifndef DBCUTTINGTESTI_H
#define DBCUTTINGTESTI_H


#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCuttingTestI
	{
		public:

			virtual ~DBCuttingTestI() {}


			virtual DBCuttingTestI* clone() const = 0;
			virtual const DBCuttingTestI* getDecorated() const = 0;


			virtual std::string getField(const std::string& field) const = 0;
			virtual void setField(const std::string& field, const std::string& value) = 0;


			virtual std::string getTestNumber() const = 0;
			virtual void setTestNumber(const std::string& num) = 0;
			virtual bool isTestNumberNull() const { return false; }
			virtual bool isTestNumberWritable() const { return true; }
			
			virtual std::string getOperatorNumber() const = 0;
			virtual void setOperatorNumber(const std::string& on) = 0;
			virtual bool isOperatorNumberNull() const { return false; }
			virtual void setOperatorNumberNull() = 0;
			virtual bool isOperatorNumberWritable() const { return true; }

			virtual std::string getProductNumber() const = 0;
			virtual void setProductNumber(const std::string& num) = 0;
			virtual bool isProductNumberNull() const { return false; }
			virtual void setProductNumberNull() = 0;
			virtual bool isProductNumberWritable() const { return true; }

			virtual std::string getVendorNumber() const = 0;
			virtual void setVendorNumber(const std::string& num) = 0;
			virtual bool isVendorNumberNull() const { return false; }
			virtual void setVendorNumberNull() = 0;
			virtual bool isVendorNumberWritable() const { return true; }

			virtual int getPrimalWeight() const = 0;
			virtual void setPrimalWeight(int w) = 0;
			virtual bool isPrimalWeightNull() const { return false; }
			virtual void setPrimalWeightNull() = 0;
			virtual bool isPrimalWeightWritable() const { return true; }

			virtual int getPrimalCost() const = 0;
			virtual void setPrimalCost(int c) = 0;
			virtual bool isPrimalCostNull() const { return false; }
			virtual void setPrimalCostNull() = 0;
			virtual bool isPrimalCostWritable() const { return true; }

			virtual std::string getStartDate() const = 0;
			virtual void setStartDate(const std::string& date) = 0;
			virtual bool isStartDateNull() const { return false; }
			virtual void setStartDateNull() = 0;
			virtual bool isStartDateWritable() const { return true; }

			virtual std::string getEndDate() const = 0;
			virtual void setEndDate(const std::string& date) = 0;
			virtual bool isEndDateNull() const { return false; }
			virtual void setEndDateNull() = 0;
			virtual bool isEndDateWritable() const { return true; }

			virtual std::string getLotNumber() const = 0;
			virtual void setLotNumber(const std::string& num) = 0;
			virtual bool isLotNumberNull() const { return false; }
			virtual void setLotNumberNull() = 0;
			virtual bool isLotNumberWritable() const { return true; }

			virtual std::string getPackingDate() const = 0;
			virtual void setPackingDate(const std::string& date) = 0;
			virtual bool isPackingDateNull() const { return false; }
			virtual void setPackingDateNull() = 0;
			virtual bool isPackingDateWritable() const { return true; }

			virtual std::string getPackingHours() const = 0;
			virtual void setPackingHours(const std::string& hours) = 0;
			virtual bool isPackingHoursNull() const { return false; }
			virtual void setPackingHoursNull() = 0;
			virtual bool isPackingHoursWritable() const { return true; }

			virtual std::string getPackingMinutes() const = 0;
			virtual void setPackingMinutes(const std::string& minutes) = 0;
			virtual bool isPackingMinutesNull() const { return false; }
			virtual void setPackingMinutesNull() = 0;
			virtual bool isPackingMinutesWritable() const { return true; }

			virtual std::string getComments() const = 0;
			virtual void setComments(const std::string& comments) = 0;
			virtual bool isCommentsNull() const { return false; }
			virtual void setCommentsNull() = 0;
			virtual bool isCommentsWritable() const { return true; }

			virtual std::string getFinalComments() const = 0;
			virtual void setFinalComments(const std::string& finalComments) = 0;
			virtual bool isFinalCommentsNull() const { return false; }
			virtual void setFinalCommentsNull() = 0;
			virtual bool isFinalCommentsWritable() const { return true; }

			virtual int getStatus() const = 0;
			virtual void setStatus(const int stat) = 0;
			virtual bool isStatusNull() const { return false; }
			virtual void setStatusNull() = 0;
			virtual bool isStatusWritable() const { return true; }

			virtual std::string getPercentTotalShrinkage() const = 0;
			virtual void setPercentTotalShrinkage(const std::string& percentTotalShrinkage) = 0;
			virtual bool isPercentTotalShrinkageNull() const { return false; }
			virtual void setPercentTotalShrinkageNull() = 0;
			virtual bool isPercentTotalShrinkageWritable() const { return true; }

			virtual std::string getTotalWeightOfCutProducts() const = 0;
			virtual void setTotalWeightOfCutProducts(const std::string& totalWeightOfCutProducts) = 0;
			virtual bool isTotalWeightOfCutProductsNull() const { return false; }
			virtual void setTotalWeightOfCutProductsNull() = 0;
			virtual bool isTotalWeightOfCutProductsWritable() const { return true; }

			virtual std::string getTotalPriceOfCutProducts() const = 0;
			virtual void setTotalPriceOfCutProducts(const std::string& totalPriceOfCutProducts) = 0;
			virtual bool isTotalPriceOfCutProductsNull() const { return false; }
			virtual void setTotalPriceOfCutProductsNull() = 0;
			virtual bool isTotalPriceOfCutProductsWritable() const { return true; }

			virtual bool isRecordValid() { return true; }
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCUTTINGTESTI_H
