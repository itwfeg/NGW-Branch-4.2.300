/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CATEGORY_H
#define CATEGORY_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb
{
class DBCategory;
}


typedef enum CategoryFieldOffsets
{
	fieldOffsetCategoryNum = 1,
	fieldOffsetParentCategoryNum,
	fieldOffsetCategoryName1,
	fieldOffsetCategoryName2,
	fieldOffsetCategoryGraphicName,
	fieldOffsetCategoryBgColor,
	fieldOffsetCategoryTextColor,
	fieldOffsetCategoryPlaylist,
	fieldOffsetCategoryDeptNum,
	endCategoryFieldOffsets
}   CategoryFieldOffsets_t;


class Category : public HfcObject
{
	public:
		static const HobRecordNum RootCategoryNum = 999999;

		Category();
		Category(const char *data, int len, int formatType);
		Category(edb::DBCategory *dbRec, DBAccess *access = (DBAccess *)NULL);

		~Category();


		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum num);

		HobRecordNum getParentCategoryNum() const;
		void setParentCategoryNum(HobRecordNum parentNum);

		const HobString &getCategoryName1() const;
		void setCategoryName1(const HobString &name);

		const HobString &getCategoryName2() const;
		void setCategoryName2(const HobString &name);

		const HobString &getGraphicName() const;
		void setGraphicName(const HobString &name);

		const HobString &getBackgroundColor() const;
		void setBackgroundColor(const HobString &color);

		const HobString &getTextColor() const;
		void setTextColor(const HobString &color);

		const HobString &getPlaylist() const;
		void setPlaylist(const HobString &list);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		// overrides from HfcObject class
		virtual HobString getMyKeyValue() const;
		virtual EvolDeviceType getMyEvolDevice() const;


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const Category *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBCategory *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static Category *findRecord(HobRecordNum categoryNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);

		static void initFieldLists();


	private:

		HobRecordNum deptNum;
		HobRecordNum categoryNum;

		HobRecordNum parentCategoryNum;

		HobString categoryName1;
		HobString categoryName2;

		HobString graphicName;

		HobString backgroundColor;
		HobString textColor;

		HobString playlist;

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBCategory *myDbRecord;

		static std::list<CategoryFieldOffsets> uiFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CATEGORY_H




