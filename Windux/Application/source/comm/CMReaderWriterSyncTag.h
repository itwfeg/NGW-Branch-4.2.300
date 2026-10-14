#ifndef CMREADERWRITERSYNCTAG_H
#define CMREADERWRITERSYNCTAG_H



#include <map>


#include "Event.h"
#include "CMReaderWriter.h"
#include "HobString.h"



class CMReaderWriterSyncTag : public CMReaderWriter
{
	public:
		/* --- Types and subtypes --- */
		typedef std::map< UINT16, UINT16 > functionCodesContainer_type;


		/* --- Constructors and destructor --- */
		CMReaderWriterSyncTag();
		~CMReaderWriterSyncTag();


		/* --- Getters and setters --- */
		static char getUnitSeparator() { return UNIT_SEPARATOR; }

		static const char * getTaggedIdentifier() { return SYNC_TAGGED_IDENTIFIER; }

		static int getTaggedIdentifierLength() { return SYNC_TAGGED_IDENTIFIER_LENGTH; }


		/* --- Instance Function members --- */
		int getRecordHeaderLength() const;
		int getRecordFooterLength() const;

		int finalizeRecordForSending(char *buff, const char *msg, int length);

		bool finishedReadingRecord(const char *msg, int length);
		bool finishedReadingRecord(const HobString &msg);

		int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer);
		int formatFromReceive(HobString &msg);


		/* --- Class Function members --- */
		static void getRTData(const char *msg, char *rtData);

		static bool canIdentifyBuffer(const char *buff);

		static void determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event);
		static void determineRecordType(char *recordTypeData, EvolDeviceType device, int event);
		static void determineReaderRecordType(char *recordTypeData, EvolDeviceType device, int event);


	private:
		/* --- Instance data members --- */
		static const int RT_DATA_MAXLENGTH = 5; // 5 = 2 for 'RT', 2 for data, 1 for unit_separator (0x1F).  ending NULL not included in this */
		char previousRTData[RT_DATA_MAXLENGTH + 1];
		HobString previousRTDataStr;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif

		static const int  SYNC_TAGGED_IDENTIFIER_LENGTH = 2;
		static const char SYNC_TAGGED_IDENTIFIER[/*SYNC_TAGGED_IDENTIFIER_LENGTH+1*/]; // = {'R', 'T', '\0'};

		static const int  SYNC_TAGGED_SEPARATOR_LENGTH = 1;
		static const char SYNC_TAGGED_SEPARATOR[/*SYNC_TAGGED_SEPARATOR_LENGTH+1*/]; // = {0x1e, '\0'};

		static const char UNIT_SEPARATOR;// = 0x1F;

		static functionCodesContainer_type deviceEventReaderMap;  // first is record type from DG, second is the internal Device|Event pair
		static functionCodesContainer_type deviceEventWriterMap;  // first is the internal Device|Event pair, second is the record type going to DG
		static functionCodesContainer_type deviceEventReversedReaderMap;


		/* --- Class Function members --- */
		static bool containsRTData(const char *msg);
		static bool containsRTData(const HobString &msg);
		static void saveRTData(const char *msg, char *whereToPutIt);
		static void saveRTData(const HobString &msg, HobString &whereToPutIt);

		static void initDeviceEventMaps();
		static void initDeviceEventReaderMap();
		static void initDeviceEventWriterMap();

		static void makeSyncPair(EvolDeviceType dev, UINT8 event, UINT16 *buff);
};



#endif /* CMREADERWRITERSYNCTAG_H */
