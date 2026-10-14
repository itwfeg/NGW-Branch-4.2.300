/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTreeBranch.h"

#include "Integer.h"
#include "Long.h"

#include "DBClassesRM.h"
#include "DBProductsRM.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTreeBranch::dataType = 0;
std::string ProductTreeBranch::dataTypeName("ProductTreeBranch"); 
#endif

ProductTreeBranch::ProductTreeBranch()
	: RecordTreeBranch()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

ProductTreeBranch::ProductTreeBranch(HobRecordNum productNum, edb::DBQuery<edb::DBProduct>* productsLinkedTo)
	: RecordTreeBranch(productNum, "", productsLinkedTo)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

ProductTreeBranch::ProductTreeBranch(HobRecordNum productNum, const std::string& description, edb::DBQuery<edb::DBProduct>* productsLinkedTo)
	: RecordTreeBranch(productNum, description, productsLinkedTo)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}


ProductTreeBranch::~ProductTreeBranch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *ProductTreeBranch::serialize(int format) const
{
	HobString tempString = "";
	switch (format)
	{
		case BrowserFormat:
		{
			tempString.append(Long::toString((long) getRecordNum()));
			tempString.append(BrowserFormatSeparator);
			tempString.append(HobList::serialize(format));
		}
			break;

		default:
			serializedData = "Serialized ProductTreeBranch : unknown format";
			// todo - throw exception("invalid format");
			break;
	}
	return tempString.c_str();
}
