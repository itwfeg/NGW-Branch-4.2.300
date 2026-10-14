/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CLASS_H
#define CLASS_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBClass;
}

class ClassTotal;

typedef enum ClassFieldOffsets
{
	fieldOffsetClassNum = 1,
	fieldOffsetReportingClassNum,
	fieldOffsetClassName,
	fieldOffsetClassDeptNum,
	endClassFieldOffsets
}   ClassFieldOffsets_t;

typedef enum ClassErrorOffsets
{
	fieldOffsetErrClassNum = 1,
	fieldOffsetErrClassErrorCode,
	fieldOffsetErrClassLinkStatus,
	fieldOffsetErrBadClass,
	fieldOffsetErrBadReportingClass,
	endClassErrorOffsets
}   ClassErrorOffsets_t;

typedef enum ClassLinkStatus
{
	CLASS_LINK_OK,
	CLASS_REPORTS_TO_SELF,
	LOOP_IN_CLASS_LINK,				//	A class reports to a class lower in the tree, causing a loop.
	MISSING_CLASS_IN_LINK,			//	A reporting class can not be found to continue branching to the top of the tree.
	DATABASE_ERROR_FOR_CLASS,		//	A Database error occurred when trying to read a class.
	TOO_MANY_CLASS_LINKS,			//	The maximum # of links in the tree has been exceeded.
	INSUFFICIENT_CLASS_LIST_MEMORY,	//	Cannot allocate memory to keep track of the links in the class tree.
	CLASS_DOES_NOT_EXIST			//	The original reporting class does not exist.
}   ClassLinkStatus_t;


class Class : public HfcObject
{
	public:

		static const int GRAND_TOTALS_CLASS = 999999;
		static const int VOID_TOTALS_CLASS  = 999998;
		static const int PRICE_CHANGE_CLASS = 999997;
		static const int LOST_TOTALS_CLASS  = 999995;

		static const short MAX_CLASS_LINKS = 100;

		Class();
		Class(const char *data, int len, int formatType);
		Class(edb::DBClass *dbRec, DBAccess *access = (DBAccess *)NULL);

		~Class();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getClassNum() const;
		void setClassNum(HobRecordNum num);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		HobString getClassName() const;
		void setClassName(HobString className);

		HobRecordNum getReportingClassNum() const;
		void setReportingClassNum(HobRecordNum reportingClassNum);

		ClassLinkStatus getClassLinkStatus() const;
		void setClassLinkStatus(ClassLinkStatus classLinkStatus);

		HobRecordNum getBadClass() const;
		void setBadClass(HobRecordNum badClass);

		HobRecordNum getBadReportingClass() const;
		void setBadReportingClass(HobRecordNum badReportingClass);

		void associateItem(HobRecordNum itemNum);
//		void associateItem(HobRecordNum[] itemNums);

		void checkClassTree(DBAccess *dbAccess);

		bool isReservedClass();


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const Class *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord(); 

		static Class *findRecord(HobRecordNum classNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static Class *findClassValidateLinks(HobRecordNum classNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);
		static void recreateReservedRecords(DBAccess *dbAccess);

		static void initFieldLists();



	private:

		static const short maxClassList = 100;

		HobRecordNum deptNum;
		HobRecordNum classNum;

		HobErrorCode errorCode;

		HobString className;

		/**
		 * @supplierCardinality 1
		 * @supplierRole parent
		 * @clientRole child
		 * @clientCardinality 0..*
		 * @label reportingClass
		 */
//		Class * reportingClass;
		HobRecordNum reportingClassNum;

		//	Check all the class links up the tree
		ClassLinkStatus classLinkStatus;

		//	The class with the bad reporting class.
		HobRecordNum badClass;

		//	The reporting class that breaks or loops the link.
		HobRecordNum badReportingClass;


		DBAccess *dbAccess;

		edb::DBClass *myDbRecord; 

		static std::list<ClassFieldOffsets> uiFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CLASS_H
