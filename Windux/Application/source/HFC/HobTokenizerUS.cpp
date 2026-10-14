/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerUS.h"
#include "HobToken.h"
#include "HobTokenTagged.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobTokenizerUS::dataType = 0;
std::string HobTokenizerUS::dataTypeName("HobTokenizerUS"); 
#endif

HobTokenizerUS::HobTokenizerUS(const HobString *data, const char separator, int maxTokens) 
					: HobTokenizer(0, separator, maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	tokenType = HobTaggedToken;

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizerUS::HobTokenizerUS(const HobString *data, const char *separators, int maxTokens) 
					: HobTokenizer(0, separators, maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	tokenType = HobTaggedToken;

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizerUS::~HobTokenizerUS()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}
