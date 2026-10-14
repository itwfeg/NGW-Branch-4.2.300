/* HT Series Scale Project
 * COPYRIGHT (c) 2017.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef DEPT_H
#define DEPT_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb
{
class DBDept;
}


typedef enum DeptFieldOffsets
{
	fieldOffsetDeptNum = 1,
	fieldOffsetDeptName1,
	fieldOffsetDeptName2,
	endDeptFieldOffsets
}   DeptFieldOffsets_t;


class Dept : public HfcObject
{
	public:
		Dept();
		Dept(const char *data, int len, int formatType);
		Dept(edb::DBDept *dbDept, DBAccess *access = (DBAccess *)NULL);

		~Dept();


		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		const HobString &getDeptName1() const;
		void setDeptName1(const HobString &name);

		const HobString &getDeptName2() const;
		void setDeptName2(const HobString &name);


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const Dept *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord(); 

		static Dept *findRecord(HobRecordNum deptNum, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);

		static void initFieldLists();


	private:

		HobRecordNum deptNum;
		HobString deptName1;
		HobString deptName2;

		DBAccess *dbAccess;

		edb::DBDept *myDbRecord; 

		static std::list<DeptFieldOffsets> uiFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //DEPT_H




