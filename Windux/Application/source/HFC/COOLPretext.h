/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLPRETEXT_H
#define COOLPRETEXT_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBCoolPreText;
}

typedef enum COOLPretextFieldOffsets
{
	fieldOffsetCOOLPretextNumber = 1,
	fieldOffsetCOOLPretextName1,
	fieldOffsetCOOLPretextName2,
	fieldOffsetCOOLPretextAllCategoriesFlag,
	fieldOffsetCOOLPretextCategory,
	// The following 3 field offsets are for when we are creating a
	// CoolPretext object from Ultima formatted 'COOL Text Record' (Function Code 65)
	fieldOffsetCOOLPretextTextNum,
	fieldOffsetCOOLPretextTextText,
	fieldOffsetCOOLPretextTextTextType,
	fieldOffsetCOOLPretextDeptNum,
	endCOOLPretextFieldOffsets
}   COOLPretextFieldOffsets_t;


class COOLPretext : public HfcObject
{
	public:

		COOLPretext();
		COOLPretext(const COOLPretext *pretextRec);
		COOLPretext(const char *data, int len, int formatType);
		COOLPretext(edb::DBCoolPreText *dbCoolPretext, DBAccess *access = (DBAccess *)NULL);

		~COOLPretext();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobRecordNum getRecordNum() const;
		void setRecordNum(HobRecordNum number);

		const HobString &getName1() const;
		void setName1(const HobString &name);

		const HobString &getName2() const;
		void setName2(const HobString &name);

		bool isAllCategoriesFlag() const;
		void setAllCategoriesFlag(bool flag);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum recordNum);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		int updateField(int fieldCode, const HobString &token);

		void update(const COOLPretext *newCOOLPretextText);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBCoolPreText *getMyDbRecord();

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static COOLPretext *findCOOLPretext(HobRecordNum recNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void initFieldList();


	private:

		HobRecordNum	recordNum;
		HobRecordNum    deptNum;

		HobString		name1;
		HobString		name2;
		bool				allCategoriesFlag;
		HobRecordNum	categoryNum;

		HobErrorCode	errorCode;

		DBAccess		*dbAccess;
		edb::DBCoolPreText	*myDbRecord;

		static std::list<COOLPretextFieldOffsets> browserFieldList, evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLPRETEXT_H




