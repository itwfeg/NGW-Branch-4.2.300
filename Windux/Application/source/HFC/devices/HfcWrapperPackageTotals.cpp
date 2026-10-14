/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperPackageTotals.h"
#include "Configuration.h"
#include "Long.h"
#include "Integer.h"
#include "HobList.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperPackageTotals::dataType = 0;
std::string HfcWrapperPackageTotals::dataTypeName("HfcWrapperPackageTotals"); 
#endif

HfcWrapperPackageTotals::HfcWrapperPackageTotals(HfcWrapperConfig *wrapConfig)
	: wrapperConfig(wrapConfig), dateReset(0), pcNameList(0), pcPackageCounts(0), pcJamCounts(0), totalPackages(0), totalJams(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	WrapperPackCodeInfo packCode;
	int 		packCodeNum;
	UINT8		packCodeName[PackCodeDescriptionLength];
	long		packages, jams;

	dateReset = new HobDateTime(wrapperConfig->getPackCodeTotalsDate(), Configuration::getInstance()->getDateDisplayFormat());
	pcNameList = new HobList();
	pcPackageCounts = new HobList();
	pcJamCounts = new HobList();
	for (packCodeNum = PackCode0; packCodeNum < NumberPackCodes; packCodeNum++)
	{
		packCode = wrapperConfig->getWrapperPackCodeInfo((WrPackCode) packCodeNum);

		//	Pack code name/description
		packCode.getDescription(packCodeName);
		HobString temp((const char*) packCodeName);
		temp.quoteForCSV();
		pcNameList->push_back(temp);

		//	Package counts
		packages = packCode.getPackageTotal();
		pcPackageCounts->push_back(Long::toString(packages));
		totalPackages += packages;

		//	Jam counts
		jams = packCode.getJamTotal();
		pcJamCounts->push_back(Long::toString(jams));
		totalJams += jams;
	}
}

HfcWrapperPackageTotals::~HfcWrapperPackageTotals()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (dateReset)
		delete dateReset;
	if (pcNameList)
		delete pcNameList;
	if (pcPackageCounts)
		delete pcPackageCounts;
	if (pcJamCounts)
		delete pcJamCounts;
}

const char *HfcWrapperPackageTotals::serialize(int format) const
{
	serializedData = "";
	if (wrapperConfig)
	{
		switch (format)
		{
			case BrowserFormat:
			{
				short loopCnt = 1;
				while(loopCnt < endHfcWrapperPackageTotalsFieldOffsets)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch (loopCnt)
					{
						case fieldOffsetWrPkgTotPackages:
							serializedData.append(Long::toString(totalPackages));
							break;
						case fieldOffsetWrPkgTotJams:
							serializedData.append(Integer::toString(totalJams));
							break;
						case fieldOffsetWrPkgTotCycles:
							serializedData.append(Long::toString(wrapperConfig->getWrapperCycleTotal()));
							break;
						case fieldOffsetWrPkgTotResetDate:
							if (dateReset)
							{
								HobString tempStr = dateReset->formatDate(Configuration::getInstance()->getDateDisplayFormat());
								tempStr.quote();
								serializedData.append(tempStr);
							}
							break;
						case fieldOffsetWrPkgTotPCNameList:
							serializedData.append(pcNameList->serialize(format));
							break;
						case fieldOffsetWrPkgTotPCPackagesList:
							serializedData.append(pcPackageCounts->serialize(format));
							break;
						case fieldOffsetWrPkgTotPCJamsList:
							serializedData.append(pcJamCounts->serialize(format));
							break;
						default:
							break;
					}
					loopCnt++;
				}
			}
			break;

			default:
				// todo - throw exception("invalid format");
				break;
		}
	}
	return serializedData.data();
}
