//#include "vxWorks.h"

#include "NumericVirtualImage.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int NumericVirtualImage::dataType = 0;
std::string NumericVirtualImage::dataTypeName("NumericVirtualImage"); 
#endif

NumericVirtualImage::NumericVirtualImage(NmDisplayType nmType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	nmType_ = nmType;
}

NumericVirtualImage::~NumericVirtualImage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
