/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLCATEGORYPRODSTEP_H
#define COOLCATEGORYPRODSTEP_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBCoolCategoryProductionStep;
class DBCoolProductionStep;
}
class COOLProductionStep;


typedef enum COOLCategoryProdStepFieldOffsets
{
	fieldOffsetCOOLCatProdStepCategoryNum = 1,
	fieldOffsetCOOLCatProdStepStepName,
	fieldOffsetCOOLCatProdStepNum,
	fieldOffsetCOOLCatProdDeptNum,
	endCOOLCategoryProdStepFieldOffsets
} COOLCategoryProdStepFieldOffsets_t;


class COOLCategoryProdStep : public HfcObject
{
	public:

		COOLCategoryProdStep(edb::DBCoolCategoryProductionStep *dbCoolCategoryProdStepRec = 0, DBAccess *access = 0);
		COOLCategoryProdStep(const char *data, int len, int formatType);
		COOLCategoryProdStep(COOLProductionStep *coolProductionStepRec, DBAccess *access);

		~COOLCategoryProdStep();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		const HobString &getProductionStep() const;
		void setProductionStep(const HobString &name);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum recordNum);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		int updateField(int fieldCode, const HobString &token);

		void update(const COOLCategoryProdStep *newCOOLProductionStep);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBCoolCategoryProductionStep *getMyDbRecord();

		static COOLCategoryProdStep *findCOOLCategoryProductionStep(HobRecordNum categoryNum, HobRecordNum deptNum, edb::DBCoolProductionStep *dbCoolProductionStep, DBAccess *dbAccess);

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static void initFieldList();

		void show();


	private:

		HobString		productionStep;
		HobRecordNum    deptNum;
		HobRecordNum	categoryNum;
		HobErrorCode	errorCode;
		HobRecordNum	productionStepNum;

		DBAccess						*dbAccess;
		edb::DBCoolCategoryProductionStep	*myDbRecord;

		static std::list<COOLCategoryProdStepFieldOffsets> evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLCATEGORYPRODSTEP_H
