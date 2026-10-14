/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolLabelApplierPositionInfo.h"
#include "LabelApplier.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolLabelApplierPositionInfo::_dataType_ = 0;
std::string HfcEvolLabelApplierPositionInfo::_dataTypeName_("HfcEvolLabelApplierPositionInfo"); 
#endif

HfcEvolLabelApplierPositionInfo::HfcEvolLabelApplierPositionInfo(const char *data) : LabelApplierPositionInfo(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolLabelApplierPositionInfo::HfcEvolLabelApplierPositionInfo(const char *data, int len, int formatType) : LabelApplierPositionInfo()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				switch (loopCnt)
				{
					case fieldOffsetTestLAPosInfoPriLocation:
						setLabelPosition((LaPosLoc) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetTestLAPosInfoPriRotation:
						setPrimaryRotation(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetTestLAPosInfoSecRotation:
						setSecondaryRotation(Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetTestLAPosInfoPrinterStation:
						setPrinterStation(Integer::valueOf(token.c_str()));
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

HfcEvolLabelApplierPositionInfo::~HfcEvolLabelApplierPositionInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolLabelApplierPositionInfo::serialize(int format)
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = LabelApplierPositionInfo::serialize();
			msglen = LabelApplierPositionInfo::getSerializedLength();

			HfcObject::serializedData = LabelApplierPositionInfo::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}

	return HfcObject::serializedData.data();
}
