/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "Cat3AuditTrailRecord.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#include <time.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Cat3AuditTrailRecord::dataType = 0;
std::string Cat3AuditTrailRecord::dataTypeName("Cat3AuditTrailRecord"); 
#endif

std::list<Cat3AuditRecordFieldOffsets> Cat3AuditTrailRecord::fieldList;


Cat3AuditTrailRecord::Cat3AuditTrailRecord()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Cat3AuditTrailRecord::Cat3AuditTrailRecord(const Cat3AuditRecord *data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("cat3");

	if (traceOn)
		Debug::trace("cat3", "Constructor passing in Cat3AuditRecord struct");

	init();

	auditevent = (AuditEvent) data->event;
	parameterId = (SealableParamID) data->parameterId;
	auditType = (AuditType) data->eventType;
	epochTime = data->epochTime;
	newValue = data->newParamValue;
	previousValue = data->oldParamValue;
}

Cat3AuditTrailRecord::~Cat3AuditTrailRecord()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("cat3.Delete", string("    Cat3AuditTrailRecord: ") + ptrtostring(this) + string("  time:") + longtostring(epochTime));
}


//	private
void Cat3AuditTrailRecord::init()
{
	auditevent = CONFIGURATION;
	parameterId = SERVICE_SWITCH_PRESSED;
	auditType = LOCAL_EVENT;
	time_t t;
	epochTime = (unsigned long) time(&t);
	newValue = 0;
	previousValue = 0;

	initFieldList();
}

// static
void Cat3AuditTrailRecord::initFieldList()
{
	if ( fieldList.empty() )
	{
		fieldList.push_back(fieldOffsetCat3Auditevent);
		fieldList.push_back(fieldOffsetCat3ParameterId);
		fieldList.push_back(fieldOffsetCat3AuditType);
		fieldList.push_back(fieldOffsetCat3EpochTime);
		fieldList.push_back(fieldOffsetCat3NewValue);
		fieldList.push_back(fieldOffsetCat3PreviousValue);

		//Cat3AuditTrailRecordTags cat3AuditRecordTags((FieldOffsetsList *)&fieldList);
	}
}


const char *Cat3AuditTrailRecord::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
		{
			for (int loopCnt = 1; loopCnt < endCat3AuditRecordFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetCat3Auditevent:
						serializedData.append(Integer::toString((int)auditevent));
						break;
					case fieldOffsetCat3ParameterId:
						serializedData.append(Integer::toString((int)parameterId));
						break;
					case fieldOffsetCat3AuditType:
						serializedData.append(Integer::toString((int)auditType));
						break;
					case fieldOffsetCat3EpochTime:
					{
						time_t rawtime = epochTime;
						struct tm * timeinfo = localtime (&rawtime);
						char buffer [80];
						strftime (buffer,80,"%c",timeinfo);
						serializedData.append(buffer);
						break;
					}
					case fieldOffsetCat3NewValue:
						serializedData.append(Long::toString(newValue));
						break;
					case fieldOffsetCat3PreviousValue:
						serializedData.append(Long::toString(previousValue));
						break;
					default:
						break;
				}
			}
			break;
		}

		default:
			serializedData = HobString("Cat3AuditTrailRecord::serialize - unknown format:") + HobString(Integer::toString(format));
			break;
	}
	return serializedData.data();
}

void Cat3AuditTrailRecord::show()
{
	if ( Debug::isTracing("cat3") )
		Debug::trace("cat3", string(" Cat3AuditTrailRecord constructor: auditevent: ") + inttostring((int)auditevent) +
						string("  parameterId: ") + inttostring((int)parameterId) +
						string("  auditType: ") + inttostring((int)auditType) +
						string("  epochTime: ") + longtostring((long)epochTime) +
						string("  newValue: ") + longtostring((int)newValue) +
						string("  previousValue: ") + longtostring((int)previousValue)
					);
}
