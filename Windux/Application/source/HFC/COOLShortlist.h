/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef COOLSHORTLIST_H
#define COOLSHORTLIST_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBCoolText;
class DBCoolListText;
class DBCoolList;
}


typedef enum COOLShortlistFieldOffsets
{
	fieldOffsetCOOLShortlistProductNum = 1,
	fieldOffsetCOOLShortlistTextNum,
	fieldOffsetCOOLShortlistOrder,
	fieldOffsetCOOLShortlistLink,			//	Received from Ultima formatted data
	fieldOffsetCOOLShortlistNextLink,		//	Received from Ultima formatted data
	fieldOffsetCOOLShortlistNum,
	fieldOffsetCOOLShortlistDeptNum,
	endCOOLShortlistFieldOffsets
} COOLShortlistFieldOffsets_t;


class COOLShortlist : public HfcObject
{
	public:

		COOLShortlist();
		COOLShortlist(const char *data, int len, int formatType);
		COOLShortlist(edb::DBCoolListText *dbCoolListText, edb::DBCoolList *dbCoolList, DBAccess *access);
		COOLShortlist(COOLShortlist *shortlist);

		~COOLShortlist();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum recNum);

		HobRecordNum getProductNum() const;
		void setProductNum(HobRecordNum recNum);

		HobRecordNum getCoolTextNum() const;
		void setCoolTextNum(HobRecordNum recNum);

		int getOrder() const;
		void setOrder(int theText);

		HobRecordNum getShortlistLink() const;
		void setShortlistLink(HobRecordNum link);

		HobRecordNum getShortlistNextLink() const;
		void setShortlistNextLink(HobRecordNum nextLink);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);

		bool isUltimaFormat();
		bool isWinduxFormat();


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(const COOLShortlist *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		edb::DBCoolText *getMyDbRecord() const;
		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		void show();

		static void initFieldLists();


	private:

		HobRecordNum	deptNum;
		HobRecordNum	productNum;
		HobRecordNum	coolTextNum;
		int				order;					//	The position in the shortlist.
		HobRecordNum	shortlistLink;			//	Received from Ultima formatted data
		HobRecordNum	shortlistNextLink;		//	Received from Ultima formatted data

		HobErrorCode	errorCode;

		DBAccess		*dbAccess;
		edb::DBCoolText		*myDbRecord;

		static std::list<COOLShortlistFieldOffsets> uiFieldList, evolFieldList, ultimaFieldList, productNumList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif //COOLSHORTLIST_H
