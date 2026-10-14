/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SPECIAL_H
#define SPECIAL_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb
{
class DBSpecialProgram;
}


typedef enum SpecialFieldOffsets
{
	fieldOffsetSpecialNum = 1,
	fieldOffsetSpecialDescription,
	fieldOffsetSpecialImageName1,
	fieldOffsetSpecialImageName2,
	fieldOffsetSpecialDefaultLength,
	fieldOffsetSpecialDeptNum,
	endSpecialFieldOffsets
}   SpecialFieldOffsets_t;


class Special : public HfcObject
{
	public:

		Special();
		Special(const char *data, int len, int formatType);
		Special(edb::DBSpecialProgram *dbRec, DBAccess *access = (DBAccess *)NULL);

		~Special();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getSpecialNum() const;
		void setSpecialNum(HobRecordNum num);

		const HobString &getSpecialDescription() const;
		void setSpecialDescription(const HobString &name);

		const HobString &getImageName1() const;
		void setImageName1(const HobString &name);

		const HobString &getImageName2() const;
		void setImageName2(const HobString &name);

		int getDefaultLength() const;
		void setDefaultLength(int length);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const Special *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord(); 

		static Special *findRecord(HobRecordNum specialNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);

		static void initFieldLists();


	private:

		HobRecordNum deptNum;
		HobRecordNum specialNum;

		HobString specialDescription;

		HobString imageName1;
		HobString imageName2;

		//The default length 
		int defaultLength;

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBSpecialProgram *myDbRecord; 

		static std::list<SpecialFieldOffsets> uiFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SPECIAL_H




