/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTree.h"
#include "ClassTreeBranch.h"
#include "ProductTreeBranch.h"
#include "Class.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProduct.h"
#include "DBProductsRM.h"
#include "DBClass.h"
#include "DBClassesRM.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTree::dataType = 0;
std::string ProductTree::dataTypeName("ProductTree"); 
#endif

ProductTree::ProductTree(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = access;
}

ProductTree::~ProductTree()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


//	private
void ProductTree::init()
{
	productNum = Class::GRAND_TOTALS_CLASS;
	dbAccess = (DBAccess *)NULL;
}

void ProductTree::buildProductTree()
{
	buildProductAndClassBranches(productNum);
}

void ProductTree::buildProductAndClassBranches(HobRecordNum classNumber)
{
	HobRecordNum tempClassNum;

	if (!dbAccess)
		return;

	DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
	if (!dbProductsRM)
		return;

	DBClassesRM *dbClassesRM = dbAccess->getDBClassesRM();
	if (!dbClassesRM)
		return;

	DBClass *dbClassRec = dbClassesRM->findNumber( util_string::longtostring(classNumber), Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
	if (dbClassRec)
	{
		DBQuery<DBClass> *classesLinkedTo = dbClassesRM->retrieveChildClasses(dbClassRec);
		if (classesLinkedTo)
		{
			ClassTreeBranch *classBranch = new ClassTreeBranch(classNumber, dbClassRec->getDescription(), classesLinkedTo);
			if (classBranch)
			{
				//	First, get any products that report to this class.
				DBQuery<DBProduct> *productsLinkedTo = dbProductsRM->retrieveLinkedtoClass(dbClassRec);
				if (productsLinkedTo)
				{
					ProductTreeBranch *productBranch = new ProductTreeBranch(classNumber, productsLinkedTo);
					if (productBranch)
					{
						if (productBranch->hasLinks())
						{
							const char *productList = productBranch->serialize(BrowserFormat);
							HobString tempStr(productList);
							tempStr.quote();
							myProductList.push_back(tempStr);
						}
						delete productBranch;
					}
					delete productsLinkedTo;
				}

				if (classBranch->hasLinks())
				{
					const char *classList = classBranch->serialize(BrowserFormat);
					HobString tempStr(classList);
					tempStr.quote();
					myClassList.push_back(tempStr);
					DBQuery<DBClass>::const_iterator iter = classesLinkedTo->begin();
					while (iter != classesLinkedTo->end())
					{
						tempClassNum = Long::valueOf(iter->getNumber());
						buildProductAndClassBranches(tempClassNum);
						++iter;
					}
				}
				delete classBranch;
			}
			delete classesLinkedTo;
		}
		delete dbClassRec;
	}
}

HobList *ProductTree::getClassList()
{
	return &myClassList;
}

HobList *ProductTree::getProductList()
{
	return &myProductList;
}
