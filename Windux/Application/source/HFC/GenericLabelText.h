/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GENERICLABELTEXT_H
#define GENERICLABELTEXT_H

#include "evolution.h"
#include "HfcObject.h"
#include "Session.h"

#include <list>

class HobString;
class DBAccess;
namespace edb 
{
class DBLabelText;
}

typedef enum GenericLabelTextOffsets
{
	fieldOffsetGenericTextNum = 1,
	fieldOffsetGenericTextText,

	endGenericLabelTextOffsets
}   GenericLabelTextOffsets_t;

class GenericLabelText : public HfcObject
{
	public:

		GenericLabelText();
		GenericLabelText(const char *data, int len, int formatType);
		GenericLabelText(edb::DBLabelText *labelTextRec, DBAccess *access = (DBAccess *)NULL);

		~GenericLabelText();


		HobRecordNum getTextNum() const;
		void setTextNum(HobRecordNum textNumber);

		const HobString &getText() const;
		void setText(const HobString &text);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);


		void updateText(const GenericLabelText *newGLT);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		bool isRecordValid();
		
		int deleteRecord();
		void saveRecord();

		static GenericLabelText *findLabelText(HobRecordNum textNum, DBAccess *dbAccess);

		static void initFieldList();

		static int verifyDefaultRecords(DBAccess *dbAccess);


	private:

		long	junkParameter;	//	So I can see all of the private parameters.

		HobRecordNum	textNum;
		HobString		text;
		HobErrorCode	errorCode;
		HobRecordNum	maxLabelTexts;

		DBAccess *dbAccess;
		edb::DBLabelText *myDbRecord;

		// Currently, there is no need to have separate lists for evolution format and for db independent format.
		static std::list<GenericLabelTextOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //GENERICLABELTEXT_H
