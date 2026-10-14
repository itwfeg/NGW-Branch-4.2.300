/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "HfcWrapperErrorTotals.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperErrorTotals::dataType = 0;
std::string HfcWrapperErrorTotals::dataTypeName("HfcWrapperErrorTotals"); 
#endif

HfcWrapperErrorTotals::HfcWrapperErrorTotals(HfcWrapperConfig *wrapConfig)
	: wrapperConfig(wrapConfig), dateReset(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	dateReset = new HobDateTime(wrapperConfig->getErrorTotalsDate(), Configuration::getInstance()->getDateDisplayFormat());
}

HfcWrapperErrorTotals::~HfcWrapperErrorTotals()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (dateReset)
		delete dateReset;
}

const char *HfcWrapperErrorTotals::serialize(int format) const
{
	serializedData = "";
	if (wrapperConfig)
	{
		switch (format)
		{
			case BrowserFormat:
			{
				short loopCnt = 1;
				while(loopCnt < endHfcWrapperErrorTotalsFieldOffsets)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch (loopCnt)
					{
						case fieldOffsetWrErrTotElevatorCollapse:
							serializedData.append(Integer::toString(wrapperConfig->getElevatorCollapseTotal()));
							break;
						case fieldOffsetWrErrTotIntakeJam:
							serializedData.append(Integer::toString(wrapperConfig->getIntakeJamTotal()));
							break;
						case fieldOffsetWrErrTotLengthJam:
							serializedData.append(Integer::toString(wrapperConfig->getLengthJamTotal()));
							break;
						case fieldOffsetWrErrTotWidthJam:
							serializedData.append(Integer::toString(wrapperConfig->getWidthJamTotal()));
							break;
						case fieldOffsetWrErrTotCoverOpen:
							serializedData.append(Integer::toString(wrapperConfig->getCoverOpenTotal()));
							break;
						case fieldOffsetWrErrTotSealerBeltOpen:
							serializedData.append(Integer::toString(wrapperConfig->getSealerBeltOpenTotal()));
							break;
						case fieldOffsetWrErrTotPackageTooTall:
							serializedData.append(Integer::toString(wrapperConfig->getPackageTooTallTotal()));
							break;
						case fieldOffsetWrErrTotPusherHomeError:
							serializedData.append(Integer::toString(wrapperConfig->getPusherHomeErrorTotal()));
							break;
						case fieldOffsetWrErrTotFilmPullError:
							serializedData.append(Integer::toString(wrapperConfig->getFilmPullErrorTotal()));
							break;
						case fieldOffsetWrErrTotResetDate:
							if (dateReset)
							{
								HobString tempStr = dateReset->formatDate(Configuration::getInstance()->getDateDisplayFormat());
								tempStr.quote();
								serializedData.append(tempStr);
							}
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
