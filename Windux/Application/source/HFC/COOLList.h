/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLLIST_H
#define COOLLIST_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
#include "DBCoolList.h"


typedef enum COOLListFieldOffsets
{
	fieldOffsetCOOLListProductNum = 1,
	fieldOffsetCOOLFirstLinkNum,
	fieldOffsetCOOLListDeptNum,
	endCOOLListFieldOffsets
} COOLListFieldOffsets_t;


class COOLList : public HfcObject
{
	public:

		COOLList(const char *data, int len, int formatType);
		COOLList(edb::DBCoolList *dbRec = 0, DBAccess *access = 0);
		COOLList(COOLList *dbRec);

		~COOLList();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobRecordNum getProductNum() const;
		void setProductNum(HobRecordNum recNum);

		HobRecordNum getFirstLinkNum() const;
		void setFirstLinkNum(HobRecordNum recNum);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);


		bool updateField(int fieldCode, const HobString &newValue);
		void updateRecord(const COOLList *newData);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBCoolList *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();


		void show();

		static void initFieldLists();


	private:

		HobRecordNum	deptNum;
		HobRecordNum	productNum;
		HobRecordNum	firstLinkNum;

		HobErrorCode	errorCode;

		DBAccess		*dbAccess;
		edb::DBCoolList		*myDbRecord;

		static std::list<COOLListFieldOffsets> evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLLIST_H
