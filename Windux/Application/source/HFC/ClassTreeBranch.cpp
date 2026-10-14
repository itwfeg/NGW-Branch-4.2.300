/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ClassTreeBranch.h"

#include "Integer.h"
#include "Long.h"

#include "DBClassesRM.h"
#include "DBProductsRM.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ClassTreeBranch::dataType = 0;
std::string ClassTreeBranch::dataTypeName("ClassTreeBranch"); 
#endif

ClassTreeBranch::ClassTreeBranch()
	: RecordTreeBranch()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

ClassTreeBranch::ClassTreeBranch(HobRecordNum classNum, const std::string& description, edb::DBQuery<edb::DBClass>* classesLinkedTo)
	: RecordTreeBranch(classNum, description, classesLinkedTo)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}


ClassTreeBranch::~ClassTreeBranch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *ClassTreeBranch::serialize(int format) const
{
	serializedData = "";
	HobString tempString = "";
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData.append(Long::toString((long) getRecordNum()));
			serializedData.append(BrowserFormatSeparator);
#ifdef ADD_DESCRIPTION
			HobString descStr = description;
			descStr.quote();
			tempString.append(descStr);
			tempString.append(BrowserFormatSeparator);
#endif
			serializedData.append(HobList::serialize(format));
			break;
		}
		default:
			serializedData = "Serialized ClassTreeBranch : unknown format";
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
