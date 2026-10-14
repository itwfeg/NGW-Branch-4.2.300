/* Access Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLLISTTEXT_H
#define COOLLISTTEXT_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
#include "DBCoolText.h"
#include "DBCoolListText.h"
#include "DBCoolList.h"


typedef enum COOLListTextFieldOffsets
{
	fieldOffsetCOOLListTextOrder = 1,
	fieldOffsetCOOLListTextLinkNum,			//	Received from Ultima formatted data
	fieldOffsetCOOLListTextNextLinkNum,		//	Received from Ultima formatted data
	fieldOffsetCOOLListTextTextNum,
	fieldOffsetCOOLListTextProductNum,
	fieldOffsetCOOLListTextDeptNum,
	endCOOLListTextFieldOffsets
} COOLListTextFieldOffsets_t;


class COOLListText : public HfcObject
{
	public:

		COOLListText(const char *data, int len, int formatType);
		COOLListText(edb::DBCoolListText *dbCoolListText = 0, DBAccess *access = 0);
		COOLListText(COOLListText *shortlist);

		~COOLListText();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum deptNum);

		int getOrder() const;
		void setOrder(int theText);

		HobRecordNum getLinkNum() const;
		void setLinkNum(HobRecordNum link);

		HobRecordNum getNextLinkNum() const;
		void setNextLinkNum(HobRecordNum nextLink);

		HobRecordNum getCoolTextNum() const;
		void setCoolTextNum(HobRecordNum recNum);

		HobRecordNum getProductNum() const;
		void setProductNum(HobRecordNum recNum);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const COOLListText *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBCoolListText *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		void show();

		static void initFieldLists();


	private:

		HobRecordNum	deptNum;
		int				order;				//	The position in the shortlist.
		HobRecordNum	linkNum;			//	Received from Ultima formatted data
		HobRecordNum	nextLinkNum;		//	Received from Ultima formatted data
		HobRecordNum	coolTextNum;
		HobRecordNum	productNum;

		HobErrorCode	errorCode;

		DBAccess		*dbAccess;
		edb::DBCoolListText	*myDbRecord;

		static std::list<COOLListTextFieldOffsets> uiFieldList, evolFieldList, ultimaFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //COOLLISTTEXT_H
