/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ScaleStatus.h"
#include "ScaleStatusTags.h"
#include "Configuration.h"

#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ScaleStatus::dataType = 0;
std::string ScaleStatus::dataTypeName("ScaleStatus"); 
#endif

std::list<ScaleStatusFieldOffsets> ScaleStatus::fieldList;


ScaleStatus::ScaleStatus()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

/* We don't receive any of these messages, we only send them out when requested;
 * therefore, we don't have the constructor that takes the buffer received from the outside world.
 *
 * ScaleStatus::ScaleStatus(const char *data, int len, int formatType)
 *
 */

ScaleStatus::~ScaleStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void ScaleStatus::init()
{
	initFieldList();
}


void ScaleStatus::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetScaleCondition);
		fieldList.push_back(fieldOffsetScaleError);
		fieldList.push_back(fieldOffsetScaleStatus);
		fieldList.push_back(fieldOffsetStatusScaleType);
		fieldList.push_back(fieldOffsetStatusScaleModel);

		ScaleStatusTags scaleStatusTags((FieldOffsetsList *)&fieldList);
	}
}


const char *ScaleStatus::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			if (Configuration::getInstance()->isServiceCounterScale())
				serializedData.append("Condition,0,0,400");
			else if (Configuration::getInstance()->isPrepackScale())
				serializedData.append("Condition,0,0,300");
			else
				serializedData.append("Condition,0,0,0");
			//serializedData.append(BrowserFormatSeparator);
			break;

		case EvolutionTaggedFormat:
		{
			ScaleStatusTags scaleStatusTags((FieldOffsetsList *)&fieldList);
			HobString tag;
			HobString fieldData = "";
									
			for (int loopCnt = 1; loopCnt < endScaleStatusFieldOffsets; loopCnt++)
			{
				tag = "";
				switch (loopCnt)
				{
					// From minMaxFields.xls -
					// This field specifies the "state" of the scale (i.e. Operator Entry, Supervisor Mode, Start Run, PLU entry, etc.)  (Numeric)
					case fieldOffsetScaleCondition:
						tag = scaleStatusTags.getTag(loopCnt);
						fieldData = "Condition";
						break;

					// From minMaxFields.xls -
					// This field reflects any error condition in the last operation.  See error code table.
					case fieldOffsetScaleError:
						tag = scaleStatusTags.getTag(loopCnt);
						fieldData = "0";
						break;

					// From minMaxFields.xls -
					// This field indicates if the scale is busy or idle.  0 - idle, 1 - busy, 2 - error
					case fieldOffsetScaleStatus:
						tag = scaleStatusTags.getTag(loopCnt);
						fieldData = "0";
						break;

					// From minMaxFields.xls -
					// This field identifies the current scale's type. (Numeric)
					case fieldOffsetStatusScaleType:
						tag = scaleStatusTags.getTag(loopCnt);
						if (Configuration::getInstance()->isServiceCounterScale())
							fieldData = "400";
						else if (Configuration::getInstance()->isPrepackScale())
							fieldData = "300";
						else
							fieldData = "0";
						break;

					case fieldOffsetStatusScaleModel:
						tag = scaleStatusTags.getTag(loopCnt);
						fieldData = Configuration::getInstance()->toStringCfg(fieldOffsetCfgScaleModel);
						break;

					default:
						break;
				}

				if (tag.size())
				{
					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
			}
			break;
		}

		default:
			serializedData = HobString("ScaleStatus::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
