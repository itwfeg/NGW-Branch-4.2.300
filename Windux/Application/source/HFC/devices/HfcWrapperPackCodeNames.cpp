/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperPackCodeNames.h"
#include "HobList.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperPackCodeNames::dataType = 0;
std::string HfcWrapperPackCodeNames::dataTypeName("HfcWrapperPackCodeNames"); 
#endif

HfcWrapperPackCodeNames::HfcWrapperPackCodeNames(HfcWrapperConfig *wrapConfig)
	: wrapperConfig(wrapConfig)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	WrapperPackCodeInfo packCode;
	int 		packCodeNum;
	UINT8		packCodeName[PackCodeDescriptionLength];

	packCodeNameList = new HobList();
	for (packCodeNum = PackCode0; packCodeNum < NumberPackCodes; packCodeNum++)
	{
		packCode = wrapperConfig->getWrapperPackCodeInfo((WrPackCode) packCodeNum);

		//	Pack code name/description
		packCode.getDescription(packCodeName);
		HobString temp((const char*) packCodeName);
		temp.quoteForCSV();
		packCodeNameList->push_back(temp);
	}
}

HfcWrapperPackCodeNames::~HfcWrapperPackCodeNames()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (packCodeNameList)
		delete packCodeNameList;
}

const char *HfcWrapperPackCodeNames::serialize(int format) const
{
	serializedData = "";
	if (wrapperConfig)
	{
		switch (format)
		{
			case BrowserFormat:
				serializedData.append(packCodeNameList->serialize(format));
				break;

			default:
				// todo - throw exception("invalid format");
				break;
		}
	}
	return serializedData.data();
}
