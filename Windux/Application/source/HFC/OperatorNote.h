/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

/**
 * This class allows anyone to leave a note for the Operator,
 * Service Technician, or anyone else using the scale.
 *
 * There is only one note for the scale.
 *
 * This note is independent of the ProductNotes feature.
 */

#ifndef OPERATORNOTE_H
#define OPERATORNOTE_H

#include "evolution.h"
#include "HfcObject.h"

class DBAccess;
namespace edb
{
class DBOperatorNote;
}


typedef enum OperatorNoteFieldOffsets
{
	fieldOffsetOpNoteText = 1,
	endOperatorNoteFieldOffsets
}   OperatorNoteFieldOffsets_t;


class OperatorNote : public HfcObject
{
	public:

		OperatorNote();
		OperatorNote(const char *data, int len, int formatType);
		OperatorNote(edb::DBOperatorNote *dbOperatorNote, DBAccess *access = (DBAccess *)NULL);

		~OperatorNote();


		const HobString &getNoteText() const;
		void setNoteText(const HobString &noteText);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);


		void updateNote(OperatorNote *newOperatorNote);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		bool isRecordValid();

		void saveRecord();

		// overrides from HfcObject class
		// Don't need to override the following, the Operator Note has no key field.
		//virtual HobString getMyKeyValue() const;
		virtual EvolDeviceType getMyEvolDevice() const;

		static OperatorNote *findOperatorNote(DBAccess *dbAccess);


	private:

		HobString noteText;

		HobErrorCode errorCode;

		DBAccess *dbAccess;

		edb::DBOperatorNote *myDbRecord;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //OPERATORNOTE_H
