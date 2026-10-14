/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RECALL_H
#define RECALL_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobDateTime.h"
#include <list>

class DBAccess;
namespace edb
{
class DBRecall;
}


typedef enum RecallFieldOffsets
{
	fieldOffsetRecallName = 1,
	fieldOffsetRecallText,
	fieldOffsetReceivedDateTime,
	fieldOffsetAcknowledged,
	fieldOffsetRecallNum,
	endRecallFieldOffsets
}   RecallFieldOffsets_t;


class Recall : public HfcObject
{
	public:
		Recall();
		Recall(const char *data, int len, int formatType);
		Recall(edb::DBRecall *dbRecall, DBAccess *access = (DBAccess *)NULL);

		~Recall();


		const HobString &getRecallName() const { return recallName; }
		void setRecallName(const HobString &name) { recallName = name; }

		const HobString &getRecallText() const { return recallText; }
		void setRecallText(const HobString &text) { recallText = text; }

		const HobDateTime &getReceivedDateTime() const { return receivedDateTime; }
		void setReceivedDateTime(const HobDateTime &dateTime) { receivedDateTime = dateTime; }

		bool isAcknowledged() const { return acknowledged; }
		void setAcknowledged(bool ack) { acknowledged = ack; }

		HobRecordNum getRecallNum() const { return recallNum; }
		void setRecallNum(HobRecordNum num) { recallNum = num; }

		HobErrorCode getErrorCode() const { return errorCode; }
		void setErrorCode(HobErrorCode code) { errorCode = code; }


		void updateRecall(const Recall *newRecall);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBRecall *getMyDbRecord() const;

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		// overrides from HfcObject class
		virtual HobString getMyKeyValue() const;
		virtual EvolDeviceType getMyEvolDevice() const;

		static Recall *findRecall(const std::string &name, DBAccess *dbAccess);

		static void initFieldList();


	private:

		HobString recallName;
		HobString recallText;
		HobDateTime receivedDateTime;
		bool acknowledged;
		HobRecordNum recallNum;

		HobErrorCode errorCode;

		DBAccess *dbAccess;

		edb::DBRecall *myDbRecord;

		static std::list<RecallFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //RECALL_H




