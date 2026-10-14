/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenTagged.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobTokenTagged::dataType = 0;
std::string HobTokenTagged::dataTypeName("HobTokenTagged"); 
#endif

HobTokenTagged::HobTokenTagged(const HobString &hobString, TagLength length): HobToken(hobString)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	tagLength = length;
}

HobTokenTagged::~HobTokenTagged()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const HobString HobTokenTagged::getTag()
{
	if (rawData.size() >= (std::string::size_type)tagLength)
		return HobString(rawData.substr(0, tagLength));
	else
		return HobString("");
}

const HobString HobTokenTagged::getData()
{
	if (rawData.size() > (std::string::size_type)tagLength)
		return HobString(rawData.substr(tagLength)); //get the data after the tag 
	else
		return HobString("");
}
