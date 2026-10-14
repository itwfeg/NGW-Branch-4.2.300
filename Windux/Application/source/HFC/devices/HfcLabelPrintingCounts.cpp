/* HTi Scale Project
* COPYRIGHT (c) 2022.  All Rights Reserved.
* ITWFEG - Hobart. Troy, Ohio  USA
*/

#include "HfcLabelPrintingCounts.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelPrintingCounts::dataType = 0;
std::string HfcLabelPrintingCounts::dataTypeName("HfcLabelPrintingCounts");
#endif

HfcLabelPrintingCounts::HfcLabelPrintingCounts(const char *data, int UNUSED(len), int formatType)
	: LabelPrintingCounts(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			setAllReceived(true);
			// Nothing else to do, the LabelPrintingCounts was filled in during initialization of this object.
			break;

		default:
			break;
	}
}

HfcLabelPrintingCounts::~HfcLabelPrintingCounts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcLabelPrintingCounts::serialize(int format) const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(util_string::inttostring(currentPrintedCount));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(util_string::inttostring(totalLabelsCount));
			break;
		}

		default:
			HfcObject::serializedData = HobString("HfcLabelPrintingCounts::serialize - unknown format:") + HobString(Integer::toString(format));
			break;
	}
	return HfcObject::serializedData.data();
}

