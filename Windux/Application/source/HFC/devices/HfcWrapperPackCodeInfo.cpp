/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWrapperPackCodeInfo.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWrapperPackCodeInfo::dataType = 0;
std::string HfcWrapperPackCodeInfo::dataTypeName("HfcWrapperPackCodeInfo"); 
#endif

HfcWrapperPackCodeInfo::HfcWrapperPackCodeInfo(WrPackCode packCodeNum, HfcWrapperConfig *wrapConfig)
	: WrapperPackCodeInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	UINT8 thisDescription[PackCodeDescriptionLength]; 
	WrapperPackCodeInfo thisPackCode = wrapConfig->getWrapperPackCodeInfo(packCodeNum);

	thisPackCode.getDescription(thisDescription);
	setDescription(thisDescription);
	setPackCode(packCodeNum);

	setPackWidth(thisPackCode.getPackWidth());
	setPackLength(thisPackCode.getPackLength());

	setPackSizeMinLength(thisPackCode.getPackSizeMinLength());
	setPackSizeMaxLength(thisPackCode.getPackSizeMaxLength());
	setPackSizeMinWidth(thisPackCode.getPackSizeMinWidth());
	setPackSizeMaxWidth(thisPackCode.getPackSizeMaxWidth());

	setSideClampOnMin(thisPackCode.getSideClampOnMin());
	setSideClampOnValue(thisPackCode.getSideClampOnValue());
	setSideClampOnMax(thisPackCode.getSideClampOnMax());
	setSideClampOffMin(thisPackCode.getSideClampOffMin());
	setSideClampOffValue(thisPackCode.getSideClampOffValue());
	setSideClampOffMax(thisPackCode.getSideClampOffMax());

	setGripperOnMin(thisPackCode.getGripperOnMin());
	setGripperOnValue(thisPackCode.getGripperOnValue());
	setGripperOnMax(thisPackCode.getGripperOnMax());
	setGripperOffMin(thisPackCode.getGripperOffMin());
	setGripperOffValue(thisPackCode.getGripperOffValue());
	setGripperOffMax(thisPackCode.getGripperOffMax());

	setKnifeOnMin(thisPackCode.getKnifeOnMin());
	setKnifeOnValue(thisPackCode.getKnifeOnValue());
	setKnifeOnMax(thisPackCode.getKnifeOnMax());
	setKnifeOffMin(thisPackCode.getKnifeOffMin());
	setKnifeOffValue(thisPackCode.getKnifeOffValue());
	setKnifeOffMax(thisPackCode.getKnifeOffMax());

	thisPackCode.isSlideBlock() ? setSlideBlock() : clearSlideBlock();
	thisPackCode.isBooster() ? setBooster() : clearBooster();

	setPackageTotal(thisPackCode.getPackageTotal());
	setJamTotal(thisPackCode.getJamTotal());
}

HfcWrapperPackCodeInfo::HfcWrapperPackCodeInfo(const char *data, int len, int formatType)
	: WrapperPackCodeInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool	test;
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
				setReceived(loopCnt, (token.length() == 0) ? false : true);
				switch (loopCnt)
				{
					case fieldOffsetWrPackCodeValue:
						setPackCode((WrPackCode)Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeDescription:
						token.unQuoteForCSV();
			 			setDescription((UINT8*) token.c_str());
						break;

					case fieldOffsetWrPackCodeWidthMin:
						setPackSizeMinWidth(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeWidthMax:
						setPackSizeMaxWidth(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeLengthMin:
						setPackSizeMinLength(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeLengthMax:
						setPackSizeMaxLength(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeSideClampOnMin:
						setSideClampOnMin(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeSideClampOnValue:
						setSideClampOnValue(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeSideClampOnMax:
						setSideClampOnMax(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeSideClampOffMin:
						setSideClampOffMin(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeSideClampOffValue:
						setSideClampOffValue(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeSideClampOffMax:
						setSideClampOffMax(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeGripperOnMin:
						setGripperOnMin(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeGripperOnValue:
						setGripperOnValue(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeGripperOnMax:
						setGripperOnMax(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeGripperOffMin:
						setGripperOffMin(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeGripperOffValue:
						setGripperOffValue(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeGripperOffMax:
						setGripperOffMax(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeKnifeOffMin:
						setKnifeOnMin(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeKnifeOffValue:
						setKnifeOnValue(Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeKnifeOffMax:
						setKnifeOnMax(Integer::valueOf(token));
						break;

					case fieldOffsetWrPackCodeSlideBlock:
					{
						test = Bool::valueOf(token);
						test ? setSlideBlock() : clearSlideBlock();
					}
						break;
					case fieldOffsetWrPackCodePackageBooster:
					{
						test = Bool::valueOf(token);
						test ? setBooster() : clearBooster();
					}
						break;
					case fieldOffsetWrPackCodeFilmWidth:
						setPackWidth((WrPackWidth)Integer::valueOf(token));
						break;
					case fieldOffsetWrPackCodeFilmLength:
						setPackLength((WrPackLength)Integer::valueOf(token));
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

HfcWrapperPackCodeInfo::~HfcWrapperPackCodeInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *HfcWrapperPackCodeInfo::serialize(int format) const
{
	HfcObject::serializedData = "";
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;
			while(loopCnt < endHfcWrapperPackCodeFieldOffsets)
			{
				if (loopCnt > 1)
					HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetWrPackCodeValue:
						HfcObject::serializedData.append(Integer::toString(getPackCode()));
						break;
					case fieldOffsetWrPackCodeDescription:
					{
						UINT8 packCodeName[PackCodeDescriptionLength];
						//	Pack code name/description
						getDescription(packCodeName);
						HobString temp((const char*) packCodeName);
						temp.quoteForCSV();
						HfcObject::serializedData.append(temp);
					}
						break;

					case fieldOffsetWrPackCodeWidthMin:
						HfcObject::serializedData.append(Integer::toString(getPackSizeMinWidth()));
						break;
					case fieldOffsetWrPackCodeWidthMax:
						HfcObject::serializedData.append(Integer::toString(getPackSizeMaxWidth()));
						break;

					case fieldOffsetWrPackCodeLengthMin:
						HfcObject::serializedData.append(Integer::toString(getPackSizeMinLength()));
						break;
					case fieldOffsetWrPackCodeLengthMax:
						HfcObject::serializedData.append(Integer::toString(getPackSizeMaxLength()));
						break;

					case fieldOffsetWrPackCodeSideClampOnMin:
						HfcObject::serializedData.append(Integer::toString(getSideClampOnMin()));
						break;
					case fieldOffsetWrPackCodeSideClampOnValue:
						HfcObject::serializedData.append(Integer::toString(getSideClampOnValue()));
						break;
					case fieldOffsetWrPackCodeSideClampOnMax:
						HfcObject::serializedData.append(Integer::toString(getSideClampOnMax()));
						break;

					case fieldOffsetWrPackCodeSideClampOffMin:
						HfcObject::serializedData.append(Integer::toString(getSideClampOffMin()));
						break;
					case fieldOffsetWrPackCodeSideClampOffValue:
						HfcObject::serializedData.append(Integer::toString(getSideClampOffValue()));
						break;
					case fieldOffsetWrPackCodeSideClampOffMax:
						HfcObject::serializedData.append(Integer::toString(getSideClampOffMax()));
						break;

					case fieldOffsetWrPackCodeGripperOnMin:
						HfcObject::serializedData.append(Integer::toString(getGripperOnMin()));
						break;
					case fieldOffsetWrPackCodeGripperOnValue:
						HfcObject::serializedData.append(Integer::toString(getGripperOnValue()));
						break;
					case fieldOffsetWrPackCodeGripperOnMax:
						HfcObject::serializedData.append(Integer::toString(getGripperOnMax()));
						break;

					case fieldOffsetWrPackCodeGripperOffMin:
						HfcObject::serializedData.append(Integer::toString(getGripperOffMin()));
						break;
					case fieldOffsetWrPackCodeGripperOffValue:
						HfcObject::serializedData.append(Integer::toString(getGripperOffValue()));
						break;
					case fieldOffsetWrPackCodeGripperOffMax:
						HfcObject::serializedData.append(Integer::toString(getGripperOffMax()));
						break;

					case fieldOffsetWrPackCodeKnifeOffMin:
						HfcObject::serializedData.append(Integer::toString(getKnifeOnMin()));
						break;
					case fieldOffsetWrPackCodeKnifeOffValue:
						HfcObject::serializedData.append(Integer::toString(getKnifeOnValue()));
						break;
					case fieldOffsetWrPackCodeKnifeOffMax:
						HfcObject::serializedData.append(Integer::toString(getKnifeOnMax()));
						break;

					case fieldOffsetWrPackCodeSlideBlock:
						HfcObject::serializedData.append(Bool::toString(isSlideBlock()));
						break;
					case fieldOffsetWrPackCodePackageBooster:
						HfcObject::serializedData.append(Bool::toString(isBooster()));
						break;
					case fieldOffsetWrPackCodeFilmWidth:
						HfcObject::serializedData.append(Integer::toString(getPackWidth()));
						break;
					case fieldOffsetWrPackCodeFilmLength:
						HfcObject::serializedData.append(Integer::toString(getPackLength()));
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
	return HfcObject::serializedData.data();
}
