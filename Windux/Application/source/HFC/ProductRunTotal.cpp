#include "ProductRunTotal.h"
#include "Configuration.h"
#include "Product.h"
#include "Transaction.h"

#include "Long.h"
#include "Integer.h"


using namespace hfc;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int ProductRunTotal::_dataType_ = 0;
	std::string ProductRunTotal::_dataTypeName_("ProductRunTotal"); 
#endif

ProductRunTotal::ProductRunTotal(HobDateTime &transTime, HobRecordNum deptNum, HobRecordNum prodNum, HobRecordNum transTypeNum)
	: ProductTotal(deptNum, prodNum, transTypeNum), currentProductRunTotalTime_(transTime),
	  productNumber_(prodNum), transactionTypeNumber_(transTypeNum)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


ProductRunTotal::~ProductRunTotal()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}



void ProductRunTotal::update(Transaction *trans, bool isVoid)
{
	if (!trans)
		return;

	if (isVoid)
	{
		HobDateTime voidTotalTime_ = trans->getCommittedDateTime();
		if (voidTotalTime_ >= getCurrentProductRunTotalTime())
		{
			//	Void in the currentProductRunTotal_ only if it is part of this Run.
			incrementWeight(-trans->getFinalNetWeight());
			incrementPrice(-trans->getTotalValue());
			incrementPackages(-trans->getNumberPackages());
			incrementItems(-trans->getByCount());
		}
	}
	else
	{
		incrementWeight(trans->getFinalNetWeight());
		incrementPrice(trans->getTotalValue());
		incrementPackages();
		incrementItems(trans->getByCount());
	}
}
