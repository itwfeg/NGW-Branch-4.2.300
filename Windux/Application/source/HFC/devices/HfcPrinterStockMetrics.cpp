/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterStockMetrics.h"
#include "LabelStockDescriptor.h"
#include "Configuration.h"


#include "Double.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterStockMetrics::_dataType_ = 0;
std::string HfcPrinterStockMetrics::_dataTypeName_("HfcPrinterStockMetrics"); 
#endif

HfcPrinterStockMetrics::HfcPrinterStockMetrics()
	: PrinterStockMetrics()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	show();
}

HfcPrinterStockMetrics::HfcPrinterStockMetrics(const char *data)
	: PrinterStockMetrics(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	show();
}

HfcPrinterStockMetrics::~HfcPrinterStockMetrics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcPrinterStockMetrics::serialize(int format) const
{
	HobString stringForQuoting;
	double tempValue;

	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrinterStockMetricsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetPSMPrinterType:
						HfcObject::serializedData.append(Integer::toString(getPrinterType()));
						break;
					case fieldOffsetPSMWidthInDots:
						HfcObject::serializedData.append(Integer::toString(getLabelWidthInDots()));
						break;
					case fieldOffsetPSMLengthInLines:
						if (isContinuousStock())
							HfcObject::serializedData.append("continuous");
						else
							HfcObject::serializedData.append(Integer::toString(getLabelLengthInLines()));
						break;
					case fieldOffsetPSMWidth:
					{
						tempValue = LabelStockDescriptor::getWidthInUnits(getLabelWidth(), MetSig::isScaleMetric());
						stringForQuoting = Double::toString(tempValue, 1, 3);
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
					}
						break;
					case fieldOffsetPSMLength:
					{
						if (isContinuousStock())
							HfcObject::serializedData.append("continuous");
						else
						{
							tempValue = LabelStockDescriptor::getLengthInUnits(getLabelLengthInLines(), MetSig::isScaleMetric());
							stringForQuoting = Double::toString(tempValue, 1, 3);
							stringForQuoting.quoteForCSV();
							HfcObject::serializedData.append(stringForQuoting);
						}
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
	return HfcObject::serializedData.c_str();
}

bool HfcPrinterStockMetrics::isContinuousLabelStock() const
{
	return (0 == getLabelLengthInLines());
}

std::string HfcPrinterStockMetrics::getHumanReadableLabelDimensions() const
{
	if ( isContinuousStock() )
		return "Continuous";

	const LabelWidth width = getLabelWidth();
	const LabelLength length = getLabelLength( getLabelLengthInLines() );

	std::string widthStr( "0.00" );

	switch ( width )
	{
	case WideLabel: widthStr = "2.25"; break;
    case NarrowLabel: widthStr = "1.50"; break;
	case UFWLabel: widthStr = "3.00"; break;
	default: break;
	}

	std::string lengthStr( "0.00" );

	switch ( length )
	{
	case Length1_000: lengthStr = "1.00"; break;
	case Length1_500: lengthStr = "1.50"; break;
	case Length1_750: lengthStr = "1.75"; break;
	case Length2_000: lengthStr = "2.00"; break;
	case Length2_375: lengthStr = "2.375"; break;
	case Length2_500: lengthStr = "2.50"; break;
	case Length3_000: lengthStr = "3.00"; break;
	case Length3_500: lengthStr = "3.50"; break;
	case Length4_000: lengthStr = "4.00"; break;
	case Length4_500: lengthStr = "4.50"; break;
	case Length5_000: lengthStr = "5.00"; break;
	case Length5_500: lengthStr = "5.50"; break;
	case Length6_000: lengthStr = "6.00"; break;
	case Length6_500: lengthStr = "6.50"; break;
	case Length7_000: lengthStr = "7.00"; break;
	case Length7_500: lengthStr = "7.50"; break;
	case Length8_000: lengthStr = "8.00"; break;
	case Length8_500: lengthStr = "8.50"; break;
	case Length9_000: lengthStr = "9.00"; break;
	case Length9_500: lengthStr = "9.50"; break;
	case Length10_000: lengthStr = "10.0"; break;
	default: break;
	}

	return widthStr + "in x " + lengthStr + "in";
}
