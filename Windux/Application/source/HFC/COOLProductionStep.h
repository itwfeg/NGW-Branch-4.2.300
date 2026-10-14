/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLPRODUCTIONSTEP_H
#define COOLPRODUCTIONSTEP_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBCoolProductionStep;
}

typedef enum COOLProductionStepFieldOffsets
{
	fieldOffsetCOOLProductionStepName1 = 1,
	fieldOffsetCOOLProductionStepName2,
	fieldOffsetCOOLProductionStepAllCategoriesFlag,
	fieldOffsetCOOLProductionStepCategory,					// received with UltimaFormat data
	fieldOffsetCOOLProductionStepNum,						// received with UltimaFormat data
	fieldOffsetCOOLProductionStepDeptNum,
	endCOOLProductionStepFieldOffsets
}   COOLProductionStepFieldOffsets_t;


class COOLProductionStep : public HfcObject
{
	public:

		COOLProductionStep();
		COOLProductionStep(const COOLProductionStep *productionStepRec);
		COOLProductionStep(const char *data, int len, int formatType);
		COOLProductionStep(edb::DBCoolProductionStep *dbCoolProductionStep, DBAccess *access = (DBAccess *)NULL);

		~COOLProductionStep();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		const HobString &getName1() const;
		void setName1(const HobString &name);

		const HobString &getName2() const;
		void setName2(const HobString &name);

		bool isAllCategoriesFlag() const;
		void setAllCategoriesFlag(bool flag);

		HobRecordNum getCategoryNum() const;
		void setCategoryNum(HobRecordNum recordNum);

		HobRecordNum getProductionStepNum() const;
		void setProductionStepNum(HobRecordNum num);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		int updateField(int fieldCode, const HobString &token);

		void update(const COOLProductionStep *newCoolProductionStep);

		void getFieldAsString(int fieldCode, HobString &result, int format) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBCoolProductionStep *getMyDbRecord();

		static COOLProductionStep *findCOOLProductionStep(HobString stepName, HobString deptNum, DBAccess * dbAccess);

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static void initFieldList();



	private:


		HobString		name1;
		HobString		name2;
		HobRecordNum    deptNum;
		HobRecordNum	categoryNum;
		HobRecordNum	productionStepNum;		// received from Ultima formatted data
		bool			allCategoriesFlag;
		HobErrorCode	errorCode;

		DBAccess				*dbAccess;
		edb::DBCoolProductionStep	*myDbRecord;

		static std::list<COOLProductionStepFieldOffsets> browserFieldList, evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLPRODUCTIONSTEP_H




