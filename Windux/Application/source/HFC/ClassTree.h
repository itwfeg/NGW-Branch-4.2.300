/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CLASSTREE_H
#define CLASSTREE_H

#include "evolution.h"
#include "HobList.h"
//#include <list>

class DBAccess;


class ClassTree : public HobList
{
	public:

		ClassTree(DBAccess *access);

		~ClassTree();

		HobRecordNum getDeptNum() const { return deptNum; }
		void setDeptNum(HobRecordNum num) { deptNum = num; }


		void buildClassTree();
		void buildClassBranches(HobRecordNum classNumber);

	private:

		HobRecordNum classNum;
		HobRecordNum deptNum;

		DBAccess *dbAccess;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CLASSTREE_H




