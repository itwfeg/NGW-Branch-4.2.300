#include "HfcEvolWrapperConfigLabelWidthUpdate.h"
#include "Integer.h"




#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperConfigLabelWidthUpdate::_dataType_ = 0;
std::string HfcEvolWrapperConfigLabelWidthUpdate::_dataTypeName_("HfcEvolWrapperConfigLabelWidthUpdate"); 
#endif

HfcEvolWrapperConfigLabelWidthUpdate::HfcEvolWrapperConfigLabelWidthUpdate(const char *data) : WrapperGeneric(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	whichPrinter_ = (PrinterType)Integer::valueOf(data);
	data += 2;
	newSize_ = (LabelWidth)Integer::valueOf(data);
}

HfcEvolWrapperConfigLabelWidthUpdate::~HfcEvolWrapperConfigLabelWidthUpdate()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperConfigLabelWidthUpdate::serialize(int format)const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolWrapperConfigLabelWidthFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetEvolWrPrinterTypeOffset:
					{
						HfcObject::serializedData.append(Integer::toString(whichPrinter_));
						break;
					}
					case fieldOffsetEvolWrPrinterSizeOffset:
					{
						HfcObject::serializedData.append(Integer::toString(newSize_));
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}
		default:
		break;
	}

	return HfcObject::serializedData.data();
}
