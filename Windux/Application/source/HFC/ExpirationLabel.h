/* HLX Scale Project
 * COPYRIGHT (c) 2007.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef EXPIRATIONLABEL_H
#define EXPIRATIONLABEL_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <list>

class DBAccess;
namespace edb
{
class DBExpirationLabel;
}


typedef enum ExpirationLabelFieldOffsets
{
	fieldOffsetExpLabelPluNumber = 1,
	fieldOffsetExpLabelPluDescription,
	fieldOffsetExpLabelExpirationDateTime,
	fieldOffsetExpLabelDeptNum,
	endExpirationLabelFieldOffsets
} ExpirationLabelFieldOffsets_t;


class ExpirationLabel : public HfcObject
{
	public:

		ExpirationLabel(DBAccess *access = 0);
		ExpirationLabel(const char *data, int len, int formatType);
		ExpirationLabel(edb::DBExpirationLabel *dbRec, DBAccess *access = (DBAccess *)NULL);

		~ExpirationLabel();

		HobRecordNum getDeptNum() const;
		void setDeptNum(HobRecordNum num);

		HobRecordNum getPluNumber() const;
		void setPluNumber(const HobRecordNum num);

		const HobString &getPluDescription() const;
		void setPluDescription(const HobString &desc);

		const HobDateTime &getExpirationDateTime() const;
		void setExpirationDateTime(const HobDateTime &dateTime);


		bool updateField(int fieldOffset, const HobString &newValue);
		void updateRecord(ExpirationLabel *newData);

		bool isFieldQuoted(int fieldOffset) const;
		void getFieldAsString(int fieldOffset, HobString &result, int format) const;
		virtual const char *serialize(int format) const;

		void setDBAccess(DBAccess *dbAccess);
		bool isRecordValid() const;
		int deleteRecord();
		void saveRecord();

		static ExpirationLabel *findRecord(HobRecordNum productNum, HobRecordNum deptNum, DBAccess *dbAccess);

		static void initFieldLists();


		void show();


	private:

		HobRecordNum pluNumber;
		HobRecordNum deptNum;
		HobString    pluDescription;
		HobDateTime  expirationDateTime;

		DBAccess *dbAccess;

		edb::DBExpirationLabel *myDbRecord;

		static std::list<ExpirationLabelFieldOffsets> evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //EXPIRATIONLABEL_H
