/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCERROR_H
#define HFCERROR_H

#include "evolution.h"
#include "HfcResponse.h"
#include <list>


typedef enum HfcErrorFieldOffsets
{
	fieldOffsetHfcErrRecordNum = endHfcResponseOffsets,
	fieldOffsetHfcErrErrorType,	

	fieldOffsetHfcErrErrorCode,
	fieldOffsetHfcErrRecordType,
	fieldOffsetHfcErrFailedTag,
	fieldOffsetHfcErrKeyValue,
	endHfcErrorFieldOffsets
}   HfcErrorFieldOffsets_t;


class HfcError : public HfcResponse
{
	public:
		// These are the errors defined for the errorCode attribute.
		static const int ERRORCODE_RECORDNOTFOUND				= 0;
		static const int ERRORCODE_FILENOTAVAILABLE			= 1;
		static const int ERRORCODE_CORRUPTRECORD				= 2;
		static const int ERRORCODE_RECORDINUSE					= 3;
		static const int ERRORCODE_SCALEMASTERFATALERROR	= 4;
		static const int ERRORCODE_INVALIDFUNCTIONCODE		= 5;
		static const int ERRORCODE_RAMDISKERROR				= 13;
		static const int ERRORCODE_COMMLIBERROR				= 14;
		static const int ERRORCODE_DATABASEERROR				= 15;
		static const int ERRORCODE_DATABASEFULL				= 16;

		static const int ERRORCODE_NOTSYNCHRONIZING         = 50;
		static const int ERRORCODE_NOTAPRIMARYSCALE         = 51;
		static const int ERRORCODE_NOTASECONDARYSCALE       = 52;
		static const int ERRORCODE_NOTYOURSECONDARYSCALE    = 53;
		static const int ERRORCODE_SCALEBUSY                = 54;

		static const int ERRORCODE_INVALIDRANGE				= 101;


		HfcError();
		HfcError(long code, short style, HobRecordNum recordNum, long errorType);
		HfcError(int code, EvolDeviceType dev, const HobString &tag, const HobString &value);
		HfcError(int code);
		HfcError(int code, EvolDeviceType dev, UINT16 evt, const HobString &tag = "", const HobString &value = "");
		HfcError(const char *data, int len, int formatType);
		~HfcError();

		virtual const char *serialize(int format) const;

		virtual void getFieldAsString(int fieldCode, HobString &result, int format) const;


		const HobString &getData() const;
		void setData(const HobString &data);


		int getErrorCode() const;
		void setErrorCode(int code);

		EvolDeviceType getDevice() const;
		void setDevice(EvolDeviceType dev);

		int getEvent() const;
		void setEvent(int event);

		const HobString &getFailedTag() const;
		void setFailedTag(const HobString &tag);

		const HobString &getKeyValue() const;
		void setKeyValue(const HobString &value);

		static void initFieldList();


	private:

		HobRecordNum recordNum;
		int errorType;

		HobString data;

		int errorCode;
		EvolDeviceType device;
		int event;
		HobString failedTag;
		HobString keyValue;

		static std::list<HfcErrorFieldOffsets> browserFieldList, evolFieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCERROR_H




