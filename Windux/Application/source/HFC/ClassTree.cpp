/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Class.h"
#include "ClassTree.h"
#include "ClassTreeBranch.h"

#include "DBAccess.h"
#include "DBClassesRM.h"
#include "DBClass.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ClassTree::dataType = 0;
std::string ClassTree::dataTypeName("ClassTree"); 
#endif

ClassTree::ClassTree(DBAccess *access)
	: HobList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = access;
}

ClassTree::~ClassTree()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

//	private
void ClassTree::init()
{
	classNum = Class::GRAND_TOTALS_CLASS;
	dbAccess = (DBAccess *)NULL;
}

void ClassTree::buildClassTree()
{
	if (dbAccess)
		buildClassBranches(classNum);
}

void ClassTree::buildClassBranches(HobRecordNum classNumber)
{
	DBClassesRM	*dbClassesRM = dbAccess->getDBClassesRM();
	if (!dbClassesRM)
		return;

	DBClass *dbClassRec = dbClassesRM->findNumber( util_string::longtostring(classNumber), util_string::longtostring(deptNum) );
	if (dbClassRec)
	{
		DBQuery<DBClass> *classesLinkedTo = dbClassesRM->retrieveChildClasses(dbClassRec);
		if (classesLinkedTo)
		{
			ClassTreeBranch *branch = new ClassTreeBranch( classNumber, dbClassRec->getDescription(), classesLinkedTo );
			if (branch)
			{
#ifndef ADD_DESCRIPTION
//	When we add the description, we will serialize the ClassTreeBranch whether or not it has links.
//	Until then, only serialize it if it has links.
				if (branch->hasLinks())
				{
#endif
					const char *classList = branch->serialize(BrowserFormat);
					HobString tempStr(classList);
					tempStr.quote();
					myList.push_back(tempStr);
					DBQuery<DBClass>::const_iterator iter = classesLinkedTo->begin();
					while (iter != classesLinkedTo->end())
					{
						buildClassBranches(Long::valueOf(iter->getNumber()));
						++iter;
					}
#ifndef ADD_DESCRIPTION
				}
#endif
				delete branch;
			}
			delete classesLinkedTo;
		}
		delete dbClassRec;
	}
}
