/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCOUNTRY_H
#define COOLCOUNTRY_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBCoolCountry;
}

typedef enum COOLCountryFieldOffsets
{
	fieldOffsetCOOLCountryName1 = 1,
	fieldOffsetCOOLCountryName2,
	fieldOffsetCOOLCountryAllCategoriesFlag,
	fieldOffsetCOOLCountryCategory,				// received with UltimaFormat data
	fieldOffsetCOOLCountryNum,					// received with UltimaFormat data
	fieldOffsetCOOLCountryDeptNum,
	endCOOLCountryFieldOffsets
}   COOLCountryFieldOffsets_t;


class COOLCountry : public HfcObject
{
	public:

		COOLCountry();
		COOLCountry(const char *data, int len, int formatType);
		COOLCountry(int recordNum, const char *countryText);
		COOLCountry(const COOLCountry *countryTextRec);
		COOLCountry(edb::DBCoolCountry *dbCOOLCountryText, DBAccess *access = (DBAccess *)NULL);

		~COOLCountry();

		const HobString &getName1() const;
		void setName1(const HobString &name);

		const HobString &getName2() const;
		void setName2(const HobString &name);

		bool isAllCategoriesFlag() const;
		void setAllCategoriesFlag(bool flag);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum recordNum);

		HobRecordNum getCountryNum() const;
		void setCountryNum(HobRecordNum num);

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		int updateField(int fieldCode, const HobString &token);

		void update(const COOLCountry *newCOOLCountryText);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBCoolCountry *getMyDbRecord();

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static COOLCountry *findCOOLCountry(const HobString &countryName, HobRecordNum deptNum, DBAccess *dbAccess);

		static void initFieldList();


	private:

		HobString		name1;
		HobString		name2;

		bool			allCategoriesFlag;

		HobRecordNum	categoryNum;
		HobRecordNum	countryNum;		// received from Ultima formatted data
		HobRecordNum	deptNum;

		HobErrorCode	errorCode;

		DBAccess			*dbAccess;
		edb::DBCoolCountry	*myDbRecord;

		static std::list<COOLCountryFieldOffsets> browserFieldList, evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLCOUNTRY_H
