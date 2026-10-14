/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBEMAILALERT_H
#define HOBEMAILALERT_H

#include "evolution.h"
#include "HfcObject.h"
#include "OpModeEvent.h"

#include <list>

class DBAccess;
namespace edb 
{
class DBEmailAlert;
}

typedef enum HobEmailAlertFieldOffsets
{
	fieldOffsetEANum = 1,
	fieldOffsetEASubject,
	fieldOffsetEANotifyOnFailure,
	fieldOffsetEANotifyOnResolution,
	fieldOffsetEANotifyHandsFreeDeviceOnFailure,
	fieldOffsetEANotifyHandsFreeDeviceOnResolution,
	fieldOffsetEADelay,
	endHobEmailAlertFieldOffsets
}   HobEmailAlertFieldOffsets_t;

typedef enum
{
	EMAILALERT_OUT_OF_LABELS,
	EMAILALERT_CASSETTE_OPEN,
	EMAILALERT_GCD_PLAYLIST_ERROR,
	EMAILALERT_GCD_FTP_ERROR,
	EMAILALERT_SCALE_OFFLINE,
	EMAILALERT_LABEL_PRESENT,

	EMAILALERT_INVALID
} EmailAlertTypes;


class HobEmailAlert : public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HobEmailAlert();
		HobEmailAlert(const char *data, int len, int formatType);
		HobEmailAlert(edb::DBEmailAlert *dbEmailAlert, DBAccess *access = (DBAccess *)NULL);

		~HobEmailAlert();


		HobRecordNum getNum() const;
		void setNum(HobRecordNum number);

		HobString getSubject() const;
		void setSubject(HobString subject);

		bool getNotifyOnFailure() const;
		void setNotifyOnFailure(bool notify);

		bool getNotifyOnResolution() const;
		void setNotifyOnResolution(bool notify);

		bool getNotifyHandsFreeDeviceOnFailure() const;
		void setNotifyHandsFreeDeviceOnFailure(bool notify);

		bool getNotifyHandsFreeDeviceOnResolution() const;
		void setNotifyHandsFreeDeviceOnResolution(bool notify);

		int getDelay() const;
		void setDelay(int delay);

		bool getFailureSent() const;
		void setFailureSent(bool sent);

		HobErrorCode getErrorCode() const {return errorCode;}


		int updateField(int fieldCode, const HobString &token);

		void update(const HobEmailAlert *newHobEmailAlert);

		void getFieldAsString(int fieldCode, HobString &result) const;

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		edb::DBEmailAlert *getMyDbRecord() {return myDbRecord;}

		bool isRecordValid();

		int deleteRecord();
		void saveRecord();

		static HobEmailAlert *findEmailAlert(HobRecordNum searchNumber, DBAccess *dbAccess);

		static void verifyReservedRecords(DBAccess *dbAccess);

		static void initFieldLists();

		void show();


	private:

		HobRecordNum recordNum;
		HobString alertSubject;
		bool notifyOnFailure;
		bool notifyOnResolution;
		bool notifyHandsFreeDeviceOnFailure;
		bool notifyHandsFreeDeviceOnResolution;
		int delayTime;
		bool sentFailure;
		bool saveNeeded;

		HobErrorCode errorCode;

		DBAccess *dbAccess;

		edb::DBEmailAlert *myDbRecord;

		static std::list<HobEmailAlertFieldOffsets> evolFieldList, browserFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBEMAILALERT_H
