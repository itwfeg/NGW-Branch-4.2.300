/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NUTRIFACTS_H
#define NUTRIFACTS_H

#include "evolution.h"
#include "ProductText.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb
{
class DBNutrition;
}

typedef enum NutrifactsFieldOffsets
{
	fieldOffsetNFTextNum = 1,
	fieldOffsetNFColumn1Data,
	fieldOffsetNFColumn2Data,
	fieldOffsetNFColumn3Data,
	fieldOffsetNFColumn4Data,
	fieldOffsetNFDeptNum,
	endNutrifactsFieldOffsets
}   NutrifactsFieldOffsets_t;


class Nutrifacts : public HfcObject
{
	public:
	
		static const short MAX_VALUE_CHARS = 50;	//	Should we consolidate?

		Nutrifacts();
		Nutrifacts(const char *data, int len, int formatType);
		Nutrifacts(edb::DBNutrition *dbNutrifactsRec, DBAccess *access = (DBAccess *)NULL);

		~Nutrifacts();


		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getTextNum() const;
		void setTextNum(HobRecordNum num);

		HobString getColumn1Data() const;
		void setColumn1Data(const HobString &data);
		HobString getColumn2Data() const;
		void setColumn2Data(const HobString &data);
		HobString getColumn3Data() const;
		void setColumn3Data(const HobString &data);
		HobString getColumn4Data() const;
		void setColumn4Data(const HobString &data);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		bool isUsedByMany() const;
		void setUsedByMany(bool usedByMany)	;
		void testIfUsedByMany();

		static Nutrifacts *convertOldToNewData(DBAccess *dbAccess, const char *data, int len);
		static bool isDataTagV1V2V3(HobString& dataTag);
		static bool isFieldNutriColumn(int fieldType);

		void buildNutrifactsData(HobString& dbNutrifactsRec);
		void parseNutridataBlock(HobString dataBlock);


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const Nutrifacts *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result /*, int format*/) const;
		virtual const char *serialize(int format) const;

		edb::DBNutrition *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static Nutrifacts *findRecord(HobRecordNum textNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void initFieldLists();


	private:

		HobRecordNum textNum;
		HobRecordNum deptNum;
		HobString column1Data;
		HobString column2Data;
		HobString column3Data;
		HobString column4Data;

//		HobErrorCode errorCode;

		bool usedByMany;

		HobErrorCode errorCode;

		DBAccess *dbAccess;

		edb::DBNutrition *myDbRecord;

		// Currently, there is no need to have separate lists for evolution format and for db independent format.
		static std::list<NutrifactsFieldOffsets> uiFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //NUTRIFACTS_H
