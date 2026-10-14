/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "RecordTreeBranch.h"
#include "DBClass.h"
#include "DBProduct.h"

#include "DBClassesRM.h"
#include "DBProductsRM.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RecordTreeBranch::dataType = 0;
std::string RecordTreeBranch::dataTypeName("RecordTreeBranch"); 
#endif

RecordTreeBranch::RecordTreeBranch()
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

RecordTreeBranch::RecordTreeBranch(HobRecordNum classNum, const std::string& description, edb::DBQuery<edb::DBClass>* classesLinkedTo)
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	recordNum = classNum;
	this->description = description;

	edb::DBQuery<edb::DBClass>::const_iterator tempIter = classesLinkedTo->begin();
	while (tempIter != classesLinkedTo->end())
	{
		myList.push_back(tempIter->getNumber());
		++tempIter;
	}
}

RecordTreeBranch::RecordTreeBranch(HobRecordNum productNum, const std::string& UNUSED(description), edb::DBQuery<edb::DBProduct>* productsLinkedTo)
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	recordNum = productNum;

	edb::DBQuery<edb::DBProduct>::const_iterator tempIter = productsLinkedTo->begin();
	while (tempIter != productsLinkedTo->end())
	{
		myList.push_back(tempIter->getPLU());
		++tempIter;
	}
}


RecordTreeBranch::~RecordTreeBranch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


//	private
void RecordTreeBranch::init()
{
	recordNum = RECORD_NOT_FOUND;
	description = "";
}

bool RecordTreeBranch::hasLinks()
{
	if (myList.size() > 0)
		return true;
	else
		return false;
}

HobRecordNum RecordTreeBranch::getRecordNum() const
{
	return recordNum;
}
