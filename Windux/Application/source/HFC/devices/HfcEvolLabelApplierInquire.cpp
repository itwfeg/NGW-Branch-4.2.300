/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcEvolLabelApplierInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolLabelApplierInquire::_dataType_ = 0;
std::string HfcEvolLabelApplierInquire::_dataTypeName_("HfcEvolLabelApplierInquire"); 
#endif

HfcEvolLabelApplierInquire::HfcEvolLabelApplierInquire(bool hasASecondPrinter, LaSideDischarge sideDischarge)
	: HfcEvolLabelApplierConfig(hasASecondPrinter, sideDischarge)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolLabelApplierInquire::~HfcEvolLabelApplierInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
