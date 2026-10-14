/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYCOUNTRY_H
#define COOLCATEGORYCOUNTRY_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBCoolCategoryCountry;
class DBCoolCountry;
}
class COOLCountry;


typedef enum COOLCategoryCountryFieldOffsets
{
	fieldOffsetCOOLCatCountryCategoryNum = 1,
	fieldOffsetCOOLCatCountryCountryName,
	fieldOffsetCOOLCatCountryCountryNum,
	fieldOffsetCOOLCatCountryDeptNum,
	endCOOLCategoryCountryFieldOffsets
}   COOLCategoryCountryFieldOffsets_t;


class COOLCategoryCountry : public HfcObject
{
	public:

		COOLCategoryCountry(edb::DBCoolCategoryCountry *dbCoolCategoryCountryRec = 0, DBAccess *access = 0);
		COOLCategoryCountry(const char *data, int len, int formatType);
		COOLCategoryCountry(COOLCountry *coolCountryRec, DBAccess *access);
		~COOLCategoryCountry();

		const HobString &getCountry() const;
		void setCountry(const HobString &name);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum recordNum);

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum number);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		int updateField(int fieldCode, const HobString &token);

		void update(const COOLCategoryCountry *newCOOLCountryText);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBCoolCategoryCountry *getMyDbRecord();

		static COOLCategoryCountry *findCOOLCategoryCountry(HobRecordNum categoryNum, edb::DBCoolCountry *dbCoolCountry, HobRecordNum deptNum, DBAccess *dbAccess);

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static void initFieldList();

		void show();


	private:

		HobString		country;
		HobRecordNum    deptNum;
		HobRecordNum	categoryNum;
		HobErrorCode	errorCode;
		HobRecordNum	countryNum;

		DBAccess				*dbAccess;
		edb::DBCoolCategoryCountry	*myDbRecord;

		static std::list<COOLCategoryCountryFieldOffsets> evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLCATEGORYCOUNTRY_H
