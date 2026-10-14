#include "HfcLabelApplierSetup.h"
#include "LabelApplier.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierSetup::_dataType_ = 0;
std::string HfcLabelApplierSetup::_dataTypeName_("HfcLabelApplierSetup");
#endif

HfcLabelApplierSetup::HfcLabelApplierSetup(const char *data) : LabelApplierSetup(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcLabelApplierSetup::HfcLabelApplierSetup(const char *data, int len, int formatType) : LabelApplierSetup()
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
					case fieldOffsetPrimaryLabelSize:
						Debug::trace("HfcLabelApplierSetup",HobString("fieldOffsetPrimaryLabelSize: ") + token);
						setPrimaryLabelSize( (UINT16)Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetPrimaryLabelRotation:
						Debug::trace("HfcLabelApplierSetup",HobString("setPrimaryLabelRotation: ") + token);
						setPrimaryLabelRotation( (LaRotation)Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetPrimaryLabelPlacement:
						Debug::trace("HfcLabelApplierSetup",HobString("fieldOffsetPrimaryLabelPlacement: ") + token);
						setPrimaryLabelPlacement( (LaPosLoc)Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetSecondaryLabelSize:
						Debug::trace("HfcLabelApplierSetup",HobString("fieldOffsetSecondaryLabelSize: ") + token);
						setSecondaryLabelSize( (UINT16)Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetSecondaryLabelRotation:
						Debug::trace("HfcLabelApplierSetup",HobString("fieldOffsetSecondaryLabelRotation: ") + token);
						setSecondaryLabelRotation( (LaRotation)Integer::valueOf( token.c_str() ) );
						break;
					case fieldOffsetSecondaryLabelPlacement:
						Debug::trace("HfcLabelApplierSetup",HobString("fieldOffsetSecondaryLabelPlacement: ") + token);
						setSecondaryLabelPlacement( (LaPosLoc)Integer::valueOf( token.c_str() ) );
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;
		default:
			break;
	}
}

HfcLabelApplierSetup::~HfcLabelApplierSetup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcLabelApplierSetup::serialize(int format) const
{
    HfcObject::serializedData = "";

	switch (format)
	{
		case InternalFormat: {
			const char *data = LabelApplierSetup::serialize();
			int len = LabelApplierSetup::getSerializedLength();
			HfcObject::serializedData.assign(data, len);
		}
		break;
		default:
			break;
	}

	return HfcObject::serializedData.data();
}



