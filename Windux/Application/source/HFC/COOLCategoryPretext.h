/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYPRETEXT_H
#define COOLCATEGORYPRETEXT_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb
{
class DBCoolCategoryPreText;
class DBCoolPreText;
}

typedef enum COOLCategoryPretextFieldOffsets
{
	fieldOffsetCOOLCatPretextCategoryNum = 1,
	fieldOffsetCOOLCatPretextPretextNum,
	fieldOffsetCOOLCatPretextDeptNum,
	endCOOLCategoryPretextFieldOffsets
}   COOLCategoryPretextFieldOffsets_t;


class COOLCategoryPretext : public HfcObject
{
	public:

		COOLCategoryPretext(edb::DBCoolCategoryPreText *dbCoolCategoryPretextRec = 0, DBAccess *access = 0);
		COOLCategoryPretext(const char *data, int len, int formatType);

		~COOLCategoryPretext();

		HobRecordNum getPretextNum() const;
		void setPretextNum(HobRecordNum recordNum);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum recordNum);

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		int updateField(int fieldCode, const HobString &token);

		void update(const COOLCategoryPretext *newCOOLCountryText);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBCoolCategoryPreText *getMyDbRecord();

		static COOLCategoryPretext *findCOOLCategoryPretext(HobRecordNum categoryNum, HobRecordNum pretextNum, HobRecordNum deptNum, DBAccess *dbAccess);

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static void initFieldList();

		void show();


	private:

		HobRecordNum    deptNum;
		HobRecordNum	pretextNum;
		HobRecordNum	categoryNum;
		HobErrorCode	errorCode;

		DBAccess				    *dbAccess;
		edb::DBCoolCategoryPreText	*myDbRecord;
		edb::DBCoolPreText			*myDbCoolPretextRec;

		static std::list<COOLCategoryPretextFieldOffsets> evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLCATEGORYPRETEXT_H
