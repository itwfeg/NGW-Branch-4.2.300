/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CMREADERWRITEREVOLTAG_H
#define CMREADERWRITEREVOLTAG_H

#include <map>

#include "Event.h"
#include "CMReaderWriter.h"
#include "HobString.h"

/**
 * This class implements the interface to format Hobart Hobart Tagged messages to/from a stream.
 */
class CMReaderWriterEvolTag : public CMReaderWriter
{
	public:

		static const int  EVOL_TAGGED_IDENTIFIER_LENGTH = 2;
		static const char EVOL_TAGGED_IDENTIFIER[/*EVOL_TAGGED_IDENTIFIER_LENGTH+1*/]; // = {'R', 'T', '\0'};

		static const int  EVOL_TAGGED_SEPARATOR_LENGTH = 1;
		static const char EVOL_TAGGED_SEPARATOR[/*EVOL_TAGGED_SEPARATOR_LENGTH+1*/]; // = {0x1e, '\0'};

		static const char UNIT_SEPARATOR;// = 0x1F;

		CMReaderWriterEvolTag();
		virtual ~CMReaderWriterEvolTag();

		int getRecordHeaderLength() const;
		int getRecordFooterLength() const;

		int finalizeRecordForSending(char *buff, const char *msg, int length);

		bool finishedReadingRecord(const char *msg, int length);
		bool finishedReadingRecord(const HobString &msg);

		int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer);
		int formatFromReceive(HobString &msg);

		static bool canIdentifyBuffer(const char *buff);

		static void determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event);
		static void determineRecordType(char *recordTypeData, EvolDeviceType device, int event);
		static void determineReaderRecordType(char *recordTypeData, EvolDeviceType device, int event);

		static void getRTData(const char *msg, char *rtData);


	private:

		static const int RT_DATA_MAXLENGTH = 5; // 5 = 2 for 'RT', 2 for data, 1 for unit_separator (0x1F).  ending NULL not included in this */
		char previousRTData[RT_DATA_MAXLENGTH + 1];
		HobString previousRTDataStr;

		static bool containsRTData(const char *msg);
		static bool containsRTData(const HobString &msg);
		static void saveRTData(const char *msg, char *whereToPutIt);
		static void saveRTData(const HobString &msg, HobString &whereToPutIt);

		/* I was originally going to make these static, but decided against it to save memory.
		 * Instead, these will be created each time a CMReaderWriterEvolTag object is
		 * instantiated, but that won't be often enough to justify keeping these maps around.
		 */
		static std::map< UINT16, UINT16 > deviceEventReaderMap;  // first is record type from DG, second is the internal Device|Event pair
		static std::map< UINT16, UINT16 > deviceEventWriterMap;  // first is the internal Device|Event pair, second is the record type going to DG
		static std::map< UINT16, UINT16 > deviceEventReversedReaderMap;
		static void initDeviceEventMaps();
		static void initDeviceEventReaderMap();
		static void initDeviceEventWriterMap();

		static void makeEvolPair(EvolDeviceType dev, UINT8 event, UINT16 *buff);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CMREADERWRITEREVOLTAG_H
