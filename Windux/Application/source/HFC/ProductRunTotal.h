#ifndef PRODUCTRUNTOTAL_H
#define PRODUCTRUNTOTAL_H

#include "ProductTotal.h"
#include "HobDateTime.h"

namespace hfc
{
	class Transaction;
}

using namespace hfc;

class ProductRunTotal : public ProductTotal
{
	public:
		ProductRunTotal(HobDateTime &transTime, HobRecordNum deptNum, HobRecordNum prodNum, HobRecordNum transTypeNum);
		~ProductRunTotal();

		HobDateTime getCurrentProductRunTotalTime() const { return currentProductRunTotalTime_; }

		HobRecordNum getProductNumber() { return productNumber_; }
		void setProductNumber(HobRecordNum pNum) { productNumber_ = pNum; }

		HobRecordNum getTransactionTypeNum() { return transactionTypeNumber_; }
		void setTransactionTypeNum(HobRecordNum tNum) { transactionTypeNumber_ = tNum; }

		void update(Transaction *trans, bool isVoid);


	private:
		HobDateTime currentProductRunTotalTime_;			//	Keep track of the start of the run, in case of Voids.
		HobRecordNum productNumber_;
		HobRecordNum transactionTypeNumber_;


#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
};

#endif //PRODUCTRUNTOTAL_H
