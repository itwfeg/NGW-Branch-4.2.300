/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CMREADERWRITERULTIMADISK_H
#define CMREADERWRITERULTIMADISK_H

#include "Event.h"
#include "CMReaderWriter.h"
#include <map>


#define ULTIMA_DISK_RECORD_SEPARATOR 0x03		// ETX
#define ULTIMA_DISK_FILE_TERMINATOR  0x17		// ETB


/**
 * This class implements the interface to format Hobart Hobart Tagged messages to/from a stream.
 */
class CMReaderWriterUltimaDisk : public CMReaderWriter
{
	public:

		CMReaderWriterUltimaDisk();
		virtual ~CMReaderWriterUltimaDisk();
		

		int getRecordHeaderLength() const {return 0;};
		int getRecordFooterLength() const {return 1;};

		int finalizeRecordForSending(char *buff, const char *msg, int length);

		bool finishedReadingRecord(const char *msg, int length);
		bool finishedReadingRecord(const HobString &msg);

		int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer);
		int formatFromReceive(HobString &msg);

		static bool canIdentifyBuffer(const char *msg);

		static void determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event);
		static void determineRecordType(char *recordTypeData, EvolDeviceType device, int event);

		static void getRTData(const char *msg, char *rtData);


	private:

		static std::map< UINT16, UINT16 > deviceEventReaderMap;  // first is record type from DG, second is the internal Device|Event pair
		static std::map< UINT16, UINT16 > deviceEventWriterMap;  // first is the internal Device|Event pair, second is the record type going to DG
		static void initDeviceEventMaps();
		static void initDeviceEventReaderMap();
		static void initDeviceEventWriterMap();

		static void makeEvolPair(EvolDeviceType dev, UINT8 event, UINT16 *buff);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //CMREADERWRITERULTIMADISK_H
