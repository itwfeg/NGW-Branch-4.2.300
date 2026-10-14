/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRODUCTTREE_H
#define PRODUCTTREE_H

#include "evolution.h"
#include "HobList.h"
#include "HfcObject.h"

class RecordTreeBranch;
class DBAccess;


class ProductTree : public HfcObject
{
	public:

		ProductTree(DBAccess *access);

		~ProductTree();

		void buildProductTree();
		void buildProductAndClassBranches(HobRecordNum classNumber);

		HobList *getClassList();
		HobList *getProductList();


	private:

		long	junkParameter;	//	So I can see all of the private parameters.

		HobRecordNum	productNum;

		DBAccess *dbAccess;

		HobList	myClassList;
		HobList	myProductList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRODUCTTREE_H




