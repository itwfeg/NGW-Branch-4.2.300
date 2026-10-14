/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLTEXT_H
#define COOLTEXT_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb
{
class DBCoolText;
}

typedef enum COOLTextFieldOffsets
{
	fieldOffsetCOOLTextRecordNum = 1,
	fieldOffsetCOOLTextText,
	fieldOffsetCOOLTextTextType,		// received from Ultima formatted data
	fieldOffsetCOOLTextDeptNum,
	endCOOLTextFieldOffsets
} COOLTextFieldOffsets_t;


class COOLText : public HfcObject
{
	public:

		typedef enum COOLTextType
		{
			COOLTextType_Text = 0,
			COOLTextType_Pretext
		} COOLTextType_t;

		COOLText();
		COOLText(const char *data, int len, int formatType);
		COOLText(edb::DBCoolText *dbRec, DBAccess *access = (DBAccess *)NULL);
		COOLText(const COOLText *textRec);
		//COOLText(int recordNum, const char *countryText);

		~COOLText();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		HobRecordNum getRecordNum() const;
		void setRecordNum(HobRecordNum recNum);

		const HobString &getText() const;
		void setText(const HobString &theText);

		COOLTextType getTextType() const;
		void setTextType(COOLTextType	type);
		void determineTextType(const HobString &type);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const COOLText *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBCoolText *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static COOLText *findRecord(HobRecordNum textNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void initFieldLists();



	private:

		HobRecordNum deptNum;
		HobRecordNum recordNum;
		HobString    text;

		COOLTextType textType;		// received from Ultima formatted data

		HobErrorCode errorCode;

		DBAccess *dbAccess;
		edb::DBCoolText *myDbRecord;

		// Currently, there is no need to have separate lists for evolution format and for db independent format.
		static std::list<COOLTextFieldOffsets> uiFieldList, evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLTEXT_H




