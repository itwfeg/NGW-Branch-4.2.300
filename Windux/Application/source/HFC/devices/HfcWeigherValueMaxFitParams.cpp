/*
 * HfcWeigherValueMaxFitParams.cpp
 *
 *  Created on: January 31, 2020
 *      Author: Doug Antonides
 */

#include "HfcWeigherValueMaxFitParams.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "UInteger.h"
#include "Long.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterHeadDotStatus::dataType = 0;
std::string HfcPrinterHeadDotStatus::dataTypeName("HfcPrinterHeadDotStatus"); 
#endif

HfcWeigherValueMaxFitParams::HfcWeigherValueMaxFitParams(const char *data) : WeigherValueMaxFitParams(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HfcWeigherValueMaxFitParams::HfcWeigherValueMaxFitParams(const char *data, int len, int formatType) : WeigherValueMaxFitParams()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case InternalFormat:
			break;

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
					case fieldOffset_gravity:
						setGravity(Long::valueOf(token));
						break;
					case fieldOffset_m_LbsCountsConvFactor:
						setConvFactor(Long::valueOf(token));
						break;
					case fieldOffset_zeroValue:
						setZeroValue(Long::valueOf(token));
						break;
					case fieldOffset_spanValue:
						setSpanValue(Long::valueOf(token));
						break;
					case fieldOffset_z0ElectronicOffset:
						setZ0ElectronicOffset(Long::valueOf(token));
						break;
					case fieldOffset_deadWeight:
						setDeadWeight(Long::valueOf(token));
						break;
					case fieldOffset_x2:
						setX2(Long::valueOf(token));
						break;
					case fieldOffset_x1:
						setX1(Long::valueOf(token));
						break;
					case fieldOffset_x0:
						setX0(Long::valueOf(token));
						break;
					case fieldOffset_y2:
						setY2(Long::valueOf(token));
						break;
					case fieldOffset_y1:
						setY1(Long::valueOf(token));
						break;
					case fieldOffset_y0:
						setY0(Long::valueOf(token));
						break;
					case fieldOffset_xRot_0:
						setX_Rot_0(Long::valueOf(token));
						break;
					case fieldOffset_xRot_1:
						setX_Rot_1(Long::valueOf(token));
						break;
					case fieldOffset_xRot_2:
						setX_Rot_2(Long::valueOf(token));
						break;
					case fieldOffset_yRot_0:
						setY_Rot_0(Long::valueOf(token));
						break;
					case fieldOffset_yRot_1:
						setY_Rot_1(Long::valueOf(token));
						break;
					case fieldOffset_yRot_2:
						setY_Rot_2(Long::valueOf(token));
						break;
					case fieldOffset_zRot_0:
						setZ_Rot_0(Long::valueOf(token));
						break;
					case fieldOffset_zRot_1:
						setZ_Rot_1(Long::valueOf(token));
						break;
					case fieldOffset_zRot_2:
						setZ_Rot_2(Long::valueOf(token));
						break;
					case fieldOffset_xGain:
						setX_Gain(Long::valueOf(token));
						break;
					case fieldOffset_yGain:
						setY_Gain(Long::valueOf(token));
						break;
					case fieldOffset_gCoeff:
						setG_Coeff(Long::valueOf(token));
						break;
					case fieldOffset_checkSum:
						setCheckSum(Long::valueOf(token));
						break;

					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWeigherValueMaxFitParams::~HfcWeigherValueMaxFitParams()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcWeigherValueMaxFitParams::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case InternalFormat:
		{
			msgdata = WeigherValueMaxFitParams::serialize();
			msglen = WeigherValueMaxFitParams::getSerializedLength();

			HfcObject::serializedData = WeigherValueMaxFitParams::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWeigherValueMaxFitParamsFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffset_gravity:
						HfcObject::serializedData.append(Long::toString(getGravity()));
						break;

					case fieldOffset_m_LbsCountsConvFactor:
						HfcObject::serializedData.append(Long::toString(getConvFactor()));
						break;

 					case fieldOffset_zeroValue:
						HfcObject::serializedData.append(Long::toString(getZeroValue()) );
						break;

					case fieldOffset_spanValue:
						HfcObject::serializedData.append(Long::toString(getSpanValue()));
						break;

					case fieldOffset_z0ElectronicOffset:
						HfcObject::serializedData.append(Long::toString(getZ0ElectronicOffset()));
						break;

					case fieldOffset_deadWeight:
						HfcObject::serializedData.append(Long::toString(getDeadWeight()));
						break;

					case fieldOffset_x2:
						HfcObject::serializedData.append(Long::toString(getX2()));
						break;

					case fieldOffset_x1:
						HfcObject::serializedData.append(Long::toString(getX1()));
						break;

					case fieldOffset_x0:
						HfcObject::serializedData.append(Long::toString(getX0()));
						break;

					case fieldOffset_y2:
						HfcObject::serializedData.append(Long::toString(getY2()));
						break;

					case fieldOffset_y1:
						HfcObject::serializedData.append(Long::toString(getY1()));
						break;

					case fieldOffset_y0:
						HfcObject::serializedData.append(Long::toString(getY0()));
						break;

					case fieldOffset_xRot_0:
						HfcObject::serializedData.append(Long::toString(getX_Rot_0()));
						break;

					case fieldOffset_xRot_1:
						HfcObject::serializedData.append(Long::toString(getX_Rot_1()));
						break;

					case fieldOffset_xRot_2:
						HfcObject::serializedData.append(Long::toString(getX_Rot_2()));
						break;

					case fieldOffset_yRot_0:
						HfcObject::serializedData.append(Long::toString(getY_Rot_0()));
						break;

					case fieldOffset_yRot_1:
						HfcObject::serializedData.append(Long::toString(getY_Rot_1()));
						break;

					case fieldOffset_yRot_2:
						HfcObject::serializedData.append(Long::toString(getY_Rot_2()));
						break;

					case fieldOffset_zRot_0:
						HfcObject::serializedData.append(Long::toString(getZ_Rot_0()));
						break;

					case fieldOffset_zRot_1:
						HfcObject::serializedData.append(Long::toString(getZ_Rot_1()));
						break;

					case fieldOffset_zRot_2:
						HfcObject::serializedData.append(Long::toString(getZ_Rot_2()));
						break;

					case fieldOffset_xGain:
						HfcObject::serializedData.append(Long::toString(getX_Gain()));
						break;

					case fieldOffset_yGain:
						HfcObject::serializedData.append(Long::toString(getY_Gain()));
						break;

					case fieldOffset_gCoeff:
						HfcObject::serializedData.append(Long::toString(getG_Coeff()));
						break;

					case fieldOffset_checkSum:
						HfcObject::serializedData.append(Long::toString(getCheckSum()));
						break;

					default:
						break;
				}
				loopCnt++;
			}

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer HeadDot Status : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
