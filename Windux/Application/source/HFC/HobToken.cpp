/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobToken.h"
#include "HobTokenTagged.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobToken::dataType = 0;
std::string HobToken::dataTypeName("HobToken"); 
#endif

// static
HobToken *HobToken::create(const HobString &hobString, HobTokenType type)
{
	HobToken *tok = (HobToken *)NULL;
	switch (type)
	{
		case HobStringToken:
		{
			tok = new HobToken(hobString);
			break;
		}
		case HobTaggedToken:
		{
			tok = new HobTokenTagged(hobString);
			break;
		}
		default:
		{
			tok = new HobToken(hobString);
			break;
		}
	}
	return tok;
}

HobToken::HobToken(const HobString &rawData)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->rawData = rawData;
}

HobToken::~HobToken()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	//Debug::trace("HobToken", HobString("Entering destructor: ") + rawData);
}

const HobString &HobToken::getRawData() const
{
	return rawData;
}

void HobToken::operator=(const HobToken &ht)
{
	rawData = ht.getRawData();
}
