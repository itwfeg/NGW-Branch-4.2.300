#ifndef CMREADERWRITERGCD_H
#define CMREADERWRITERGCD_H

#include "Event.h"
#include "CMReaderWriter.h"
#include "HobString.h"
#include "GCDSystemStatus.h"
#include "Weigher.h"
#include "GCDEvent.h"

class GCDDevDataMsgProvider;

class CMReaderWriterGCD : public CMReaderWriter
{
	struct MessageHeader 
	{
		int type;
		int length;
	};

public:

	CMReaderWriterGCD();
	~CMReaderWriterGCD();

	virtual int getRecordHeaderLength() const { return 0; }
	virtual int getRecordFooterLength() const { return 0; }

	virtual int finalizeRecordForSending(char *buff, const char *msg, int length);

	virtual bool finishedReadingRecord(const char *msg, int length);
	virtual bool finishedReadingRecord(const HobString &msg);

	virtual int formatFromReceive(char *msg, int length, bool isRecordBeginningBuffer);
	virtual int formatFromReceive(HobString &msg);

	static void getRTData(const char *msg, char *rtData);

	static void determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event);

	static void setDataHeader(std::string& data, UINT8 type, int length = 0)
	{
		data.resize(sizeof(MessageHeader));
		reinterpret_cast< MessageHeader * >(&data[0])->type = type;
		reinterpret_cast< MessageHeader * >(&data[0])->length = length;
	}

	static void resetDataHeaderLength(std::string& data)
	{
		reinterpret_cast< MessageHeader * >(&data[0])->length = static_cast< int >(data.size()) - sizeof(MessageHeader);
	}

	static void concatDelimetersToData(std::string& data, DWORD dwPrependingDelimeters)
	{
		DWORD i;
		for (i = 0; i < dwPrependingDelimeters; ++ i)
			data.push_back(CH_DELIMETER);
	}

	static void concatToData(std::string& data, char ch, DWORD dwPrependingDelimeters = 1)
	{
		concatDelimetersToData(data, dwPrependingDelimeters);
		data.push_back(ch);
	}

	static void concatToData(std::string& data, LPCSTR sz, DWORD dwPrependingDelimeters = 1)
	{
		concatDelimetersToData(data, dwPrependingDelimeters);
		for (const char *p = sz; *p; ++ p)
			data.push_back(*p);
	}

	static void concatToData(std::string& data, LPCTSTR sz, DWORD dwPrependingDelimeters = 1)
	{
		concatDelimetersToData(data, dwPrependingDelimeters);
		for (const TCHAR *p = sz; *p; ++ p)
			data.push_back(static_cast< char >(*p));
	}

	static void concatToData(std::string& data, std::string str, DWORD dwPrependingDelimeters = 1)
	{
		concatDelimetersToData(data, dwPrependingDelimeters);
		for (const char *p = str.c_str(); *p; ++ p)
			data.push_back(*p);
	}

	static const DWORD SIZEOF_RTDATA = sizeof(MessageHeader);

	static const char CH_DELIMETER = 0x19;
};
#endif