/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcEvolWrapperInquire.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperInquire::dataType = 0;
std::string HfcEvolWrapperInquire::dataTypeName("HfcEvolWrapperInquire"); 
#endif

HfcEvolWrapperInquire::HfcEvolWrapperInquire(const char *data)
	: HfcEvolWrapperConfig(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcEvolWrapperInquire::~HfcEvolWrapperInquire()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
