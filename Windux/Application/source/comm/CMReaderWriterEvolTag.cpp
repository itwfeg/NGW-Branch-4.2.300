/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "ProductDevEvent.h"
#include "OperatorEvent.h"
#include "SessionEvent.h"
#include "ConfigurationEvent.h"
#include "messages/COOLTotalEvent.h"
#include "messages/TransactionDevEvent.h"
#include "BackupRestoreEvent.h"
#include "GraphicEvent.h"
#include "COOLDevEvent.h"
#include "SoftkeyDevEvent.h"
#include "Messages/SynchronizationDevEvent.h"
#include "VersionManager/include/VersionManagerEvent.h"
#include "WrapperEvent.h"
#include "PrinterEvent.h"
#include "WeigherEvent.h"

#include "CMReaderWriterEvolTag.h"
#include "Integer.h"
#include "Hex.h"

#include "scalemaster.h"
#include "scalemasterEvol.h"

#include "util/e_string/utilstring.h"

using namespace std;


// statics
std::map< UINT16, UINT16 > CMReaderWriterEvolTag::deviceEventReaderMap;
std::map< UINT16, UINT16 > CMReaderWriterEvolTag::deviceEventWriterMap;
std::map< UINT16, UINT16 > CMReaderWriterEvolTag::deviceEventReversedReaderMap;

const char CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER[CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER_LENGTH+1] = {'R', 'T', '\0'};
const char CMReaderWriterEvolTag::EVOL_TAGGED_SEPARATOR[CMReaderWriterEvolTag::EVOL_TAGGED_SEPARATOR_LENGTH+1] = {0x1e, '\0'};
const char CMReaderWriterEvolTag::UNIT_SEPARATOR = 0x1F;

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int CMReaderWriterEvolTag::_dataType_ = 0;
	std::string CMReaderWriterEvolTag::_dataTypeName_("CMReaderWriterEvolTag"); 
#endif

// This is the reader/writer when receiving/sending records to the Evolution formatted backup/restore file,
// as well as the Comm format written to the socket when communicating with Data Gateweigh (ScaleComm).

CMReaderWriterEvolTag::CMReaderWriterEvolTag()
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	readerWriterId = CMReaderWriter::EVOL_TAGGED_ID;
	recordIdentifierLength = 5;
	memset(previousRTData, 0, sizeof(previousRTData));
	previousRTDataStr = "";

	CMReaderWriterEvolTag::initDeviceEventMaps();
}

CMReaderWriterEvolTag::~CMReaderWriterEvolTag()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}

int CMReaderWriterEvolTag::getRecordHeaderLength() const
{
	return 0;
}

int CMReaderWriterEvolTag::getRecordFooterLength() const
{
	return EVOL_TAGGED_SEPARATOR_LENGTH;
}


int CMReaderWriterEvolTag::finalizeRecordForSending(char *buff, const char *msg, int length)
{
	memcpy(buff, msg, length);
	if (*(buff + length - 1) == UNIT_SEPARATOR)
		length -= 1;
	memcpy(buff + length, EVOL_TAGGED_SEPARATOR, EVOL_TAGGED_SEPARATOR_LENGTH);
	*(buff + length + EVOL_TAGGED_SEPARATOR_LENGTH) = (char)NULL;
		
	if (Debug::isTracing("CMReaderWriter.EvolTag"))
		Debug::trace("CMReaderWriter.EvolTag", HobString("finalizeRecordForSending - ") + HobString(Hex::toString(buff, length + EVOL_TAGGED_SEPARATOR_LENGTH)));
	return length + EVOL_TAGGED_SEPARATOR_LENGTH;
}


bool CMReaderWriterEvolTag::finishedReadingRecord(const char *msg, int length)
{
	return (*(msg + length - 1) == *EVOL_TAGGED_SEPARATOR) ? true : false;

/* If the separator ever becomes more than 1 byte, do the following; otherwise, just do above.
	for (int i = 0; i < EVOL_TAGGED_SEPARATOR_LENGTH; i++)
	{
		if (*(msg + length - EVOL_TAGGED_SEPARATOR_LENGTH +i) != *(HOB_RECORD_SEPARATOR + i))
			return false;
	}
	return true;
*/
}

bool CMReaderWriterEvolTag::finishedReadingRecord(const HobString &msg)
{
	return ((INT32) msg.size() >= EVOL_TAGGED_SEPARATOR_LENGTH && msg.substr(msg.size() - EVOL_TAGGED_SEPARATOR_LENGTH, EVOL_TAGGED_SEPARATOR_LENGTH) == EVOL_TAGGED_SEPARATOR) ? true : false;
}


/*
 * Note: The msg buffer being passed in needs to be large enough to allow for this method
 * to prepend the previous RT tag and data onto the msg, if it doesn't contain it.
 */
int CMReaderWriterEvolTag::formatFromReceive(char *msg, int length, bool /*isRecordBeginningBuffer*/)
{
	if (!msg || length <= 0)
		return 0;

	int finalLength = length;

	// First, remove the record separator from the received msg.
	// The following assumes the EVOL_TAGGED_SEPARATOR is only 1 byte!!!!!!!!!!!
	if (*(msg + length - 1) == *EVOL_TAGGED_SEPARATOR)
	{
		*(msg + length - 1) = (char)NULL;
		finalLength -= EVOL_TAGGED_SEPARATOR_LENGTH;
	}

	/* If this record contains the RT tag, save it in the 'previousRTData' buffer.
	 * This would be used in any subsequent records that don't contain the RT tag,
	 * until another RT tag is encountered.
	 * Note: The RT tag will be the first tag in the record, if it exists in the record.
	 */
	if (containsRTData(msg))
		saveRTData(msg, previousRTData);
	else if (strlen(previousRTData) > 0)
	{
		// If the record does not contain the RT tag, prepend the previous one onto the msg.
		int len = finalLength + (int)strlen(previousRTData);
		char *tempBuff = new char[len + 1];
		strcpy(tempBuff, previousRTData);
		memcpy(tempBuff + strlen(previousRTData), msg, finalLength);

		memcpy(msg, tempBuff, len);
		*(msg + len) = (char)NULL;

		Debug::trace("CMReaderWriter.EvolTag", "prepended the saved RT data onto the msg.");
		delete[] tempBuff;
		finalLength = len;
	}

	return finalLength;
}

int CMReaderWriterEvolTag::formatFromReceive(HobString &msg)
{
	if (!msg.size())
		return 0;

	int finalLength = (int)msg.size();

	// First, remove the record separator from the received msg, if it exists.
	if (msg.substr(msg.size() - EVOL_TAGGED_SEPARATOR_LENGTH, EVOL_TAGGED_SEPARATOR_LENGTH) == EVOL_TAGGED_SEPARATOR)
	{
		msg.erase(msg.size() - EVOL_TAGGED_SEPARATOR_LENGTH);
		finalLength -= EVOL_TAGGED_SEPARATOR_LENGTH;
	}

	/* If this record contains the RT tag, save it in the 'previousRTData' buffer.
	 * This would be used in any subsequent records that don't contain the RT tag,
	 * until another RT tag is encountered.
	 * Note: The RT tag will be the first tag in the record, if it exists in the record.
	 */
	if (containsRTData(msg))
		saveRTData(msg, previousRTDataStr);
	else if (previousRTDataStr.size())
	{
		// If the record does not contain the RT tag, prepend the previous one onto the msg.
		msg.insert(0, previousRTDataStr);
		finalLength += (int)previousRTDataStr.size();

		Debug::trace("CMReaderWriter.EvolTag", "prepended the saved RT data onto the msg.");
	}

	return finalLength;
}


//private, static
bool CMReaderWriterEvolTag::containsRTData(const char *msg)
{
	if (!msg)
		return false;

	for (int i = 0; i < EVOL_TAGGED_IDENTIFIER_LENGTH; i++)
	{
		if (*(msg + i) != EVOL_TAGGED_IDENTIFIER[i])
			return false;
	}
	return true;
}

//private, static
bool CMReaderWriterEvolTag::containsRTData(const HobString &msg)
{
	return (!msg.size() || msg.substr(0, EVOL_TAGGED_IDENTIFIER_LENGTH) != EVOL_TAGGED_IDENTIFIER) ? false : true;
}


//private
void CMReaderWriterEvolTag::saveRTData(const char *msg, char *whereToPutIt)
{
	if (!msg || !whereToPutIt)
		return;

	int i = 0;
	while (i < RT_DATA_MAXLENGTH)
	{
		*(whereToPutIt + i) = *(msg + i);
		if (*(msg + i) == UNIT_SEPARATOR || *(msg + i) == *EVOL_TAGGED_SEPARATOR)
			break;
		i++;
	}
	*(whereToPutIt + RT_DATA_MAXLENGTH) = (char)0;
}

void CMReaderWriterEvolTag::saveRTData(const HobString &msg, HobString &whereToPutIt)
{
	whereToPutIt = msg.substr(0, RT_DATA_MAXLENGTH);
}


void CMReaderWriterEvolTag::getRTData(const char *msg, char *rtData)
{
	if (!msg || !rtData)
		return;

	if (CMReaderWriterEvolTag::containsRTData(msg))
	{
		char data[RT_DATA_MAXLENGTH + 1];  // more than enough
		CMReaderWriterEvolTag::saveRTData(msg, data);
		unsigned int i = EVOL_TAGGED_IDENTIFIER_LENGTH;
		int j = 0;
		while (i < sizeof(data))
		{
			if (data[i] == UNIT_SEPARATOR || data[i] == *EVOL_TAGGED_SEPARATOR || data[i] == '\0')
				break;
			*(rtData + j++) = data[i++];
		}
		*(rtData + j) = (char)NULL;
	}
}


// static
bool CMReaderWriterEvolTag::canIdentifyBuffer(const char *msg)
{
	if (!msg)
		return false;

	for (int i = 0; i < EVOL_TAGGED_IDENTIFIER_LENGTH; i++)
	{
		if (*(msg+i) != EVOL_TAGGED_IDENTIFIER[i])
			return false;
	}
	return true;
}


/* By passing in the data associated with the RT tag, identify the representative
 * device and event (as set up in Event.h) by looking up tagData (the key) in the deviceEventReaderMap.
 */
// static
void CMReaderWriterEvolTag::determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event)
{
	*device = UnknownDevice;
	*event = 0;

	if (!recordTypeData || strlen(recordTypeData) < 2)
		return;

	if (strlen(recordTypeData) != 2)
		Debug::trace("CMReaderWriter.EvolTag", "recordTypeData length is " + util_string::inttostring(strlen(recordTypeData)) +
				"   data is *" + util_string::ptrtostring(recordTypeData) + "*  ");

	UINT16 rtui16 = 0;
	UINT8 *pU8 = (UINT8 *) &rtui16;
	// because Intel is little endian, have to byte-reverse the two bytes in the UINT16
	*pU8 = *(recordTypeData+1);
	*(pU8+1) = *recordTypeData;

	std::map< UINT16, UINT16 >::iterator readerMapIter = deviceEventReaderMap.find(rtui16);
	if (readerMapIter != deviceEventReaderMap.end())
	{
		UINT16 data = readerMapIter->second;
		char *pdata = (char *) &data;

		int j = 0;

		UINT8 ev = 0;
		char * pChar = (char *) &ev;
		for (unsigned int i = 0; i < sizeof(ev); i++)
		{
			* pChar++ = pdata[j++];
		}

		UINT8 dev = 0;
		pChar = (char *) &dev;
		for (unsigned int i = 0; i < sizeof(dev); i++)
		{
			* pChar++ = pdata[j++];
		}

		*device = (EvolDeviceType) dev;
		*event = ev;
	}
}

/* By passing in the application's device and event (as set up in Event.h), identify the representative
 * RT tag value by looking up the device/event data (the key) in the deviceEventWriterMap.
 */
// static
void CMReaderWriterEvolTag::determineRecordType(char *recordTypeData, EvolDeviceType device, int event)
{
	UINT16 pair;
	UINT16 *devEventPair = &pair;

	if (!recordTypeData)
		return;
	*recordTypeData = '\0';

	makeEvolPair(device, (UINT8)event, devEventPair);

	UINT16 data = pair;

	std::map< UINT16, UINT16 >::iterator writerMapIter = deviceEventWriterMap.find(data);
	if (writerMapIter != deviceEventWriterMap.end())
	{
		UINT16 rtData = writerMapIter->second;
		UINT8 *pU8 = (UINT8 *) &rtData;
		// because Intel is little endian, have to byte-reverse the two bytes in the UINT16
		*recordTypeData = *(pU8+1);
		*(recordTypeData+1) = *pU8;
		*(recordTypeData+2) = '\0';
	}
}


/* By passing in the application's device and event (as set up in Event.h), identify the representative
 * RT tag value by looking up the device/event data (the key) in the deviceEventReaderMap.
 */
// static
void CMReaderWriterEvolTag::determineReaderRecordType(char *recordTypeData, EvolDeviceType device, int event)
{
	UINT16 pair;
	UINT16 *devEventPair = &pair;

	if (!recordTypeData)
		return;
	*recordTypeData = '\0';

	makeEvolPair(device, (UINT8)event, devEventPair);

	UINT16 data = pair;

	std::map< UINT16, UINT16 >::iterator readerMapIter = deviceEventReversedReaderMap.find(data);
	if (readerMapIter != deviceEventReversedReaderMap.end())
	{
		UINT16 rtData = readerMapIter->second;
		UINT8 *pU8 = (UINT8 *) &rtData;
		// because Intel is little endian, have to byte-reverse the two bytes in the UINT16
		*recordTypeData = *(pU8+1);
		*(recordTypeData+1) = *pU8;
		*(recordTypeData+2) = '\0';
	}
}


void CMReaderWriterEvolTag::makeEvolPair(EvolDeviceType dev, UINT8 event, UINT16 *buff)
{
	if (buff)
	{
		UINT8 *pU8 = (UINT8 *) buff;

		pU8[0] = event;
		pU8[1] = (UINT8)dev;
	}
}


void CMReaderWriterEvolTag::initDeviceEventMaps()
{
	if (deviceEventReaderMap.empty())
	{
		initDeviceEventReaderMap();
		initDeviceEventWriterMap();
	}
}

void CMReaderWriterEvolTag::initDeviceEventWriterMap()
{
/*
	std::map< UINT16, UINT16 >::iterator readerMapIter;
	for (readerMapIter = deviceEventReaderMap.begin(); readerMapIter != deviceEventReaderMap.end(); readerMapIter++)
	{
		deviceEventWriterMap.insert(make_pair(readerMapIter->second, readerMapIter->first));
	}
*/
	UINT16 pair;

	makeEvolPair(DeptDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_DEPT_NAME));

	makeEvolPair(ProductDev, ProductWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COMMODITIES));

	makeEvolPair(ProductDefaultsDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_DEFAULTS));

	makeEvolPair(ClassDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CLASSES));

	makeEvolPair(NutrifactsDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_NU_DATA));

	makeEvolPair(ExpandedTextDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_EXTEXT));

	makeEvolPair(SpecialMessageDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SPECIAL_MSGS));

	makeEvolPair(ProductNotesDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_NOTES));

	makeEvolPair(MarqueeTextDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_MARQUEES));

	makeEvolPair(ProductText5Dev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODTEXT5));

	makeEvolPair(ProductText6Dev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODTEXT6));

	makeEvolPair(ProductText7Dev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODTEXT7));

	makeEvolPair(ProductText8Dev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODTEXT8));

	makeEvolPair(ProductText9Dev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODTEXT9));

	makeEvolPair(TransactionTypeDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_TOT_NAME));

	makeEvolPair(OperatorDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_OP_IDS));

	makeEvolPair(OperatorDev, OpWriteNote, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_OPERATOR_NOTES));

	makeEvolPair(OperatorChangesDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_OP_CHANGES));

    // I think I want to make them all Operator Changes with the only difference the modechange flag.
	makeEvolPair(SupervisorChangesDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SUPER_OP_CHANGES));

	makeEvolPair(ConfigurationDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CONFIG_ITEMS));

	makeEvolPair(ConfigurationDev, CfgStoreInfoWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_STORE_NAME));

	makeEvolPair(ConfigurationDev, CfgSynchronizationWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CONFIG_SYNCHRONIZATION));

	// In the document \\Troygen1\home\Groups\Weigh Wrap Engineering\Projects\Evolution - Ultima Replacement\Firmware\Font Graphics Label Compatibility.doc
	// it says, on page 4, that the 'write one graphic' function code will be '9A' which is different than it is today
	// but '9A' is what it is today.
	makeEvolPair(GraphicDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_USER_GRAPHICS));

	makeEvolPair(FontDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_USER_FONTS));

	makeEvolPair(LabelTypeDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_LAB_FORM));

	makeEvolPair(GenericTextDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_GEN_TEXT));

	makeEvolPair(ProductTotalDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PTOTALS));

	makeEvolPair(ClassTotalDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CTOTALS));

	makeEvolPair(OperatorTotalDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_OTOTALS));

	makeEvolPair(HourlyTotalDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_HTOTALS));

	makeEvolPair(TransactionDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_ITEM_TRANS));


#ifndef NOTSELFSERVICE
	makeEvolPair(CategoryDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CATEGORIES));

	makeEvolPair(PresentationMethodDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRESENTATION_METHODS));

	makeEvolPair(SpecialsDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SPECIALS));

	makeEvolPair(StreamingGraphicDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_STREAMING_GRAPHIC_INFO));

	makeEvolPair(ProductStatisticsDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_STATISTICS));

	makeEvolPair(GraphicDev, WriteProductGraphic, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_GRAPHICS));

	makeEvolPair(GraphicDev, WriteCategoryGraphic, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CATEGORY_GRAPHICS));

	makeEvolPair(GraphicDev, WriteSpecialsGraphic, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SPECIALS_GRAPHICS));

	makeEvolPair(GraphicDev, WriteStreamingGraphic, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_STREAMING_GRAPHICS));
#endif

	makeEvolPair(GraphicDev, WriteSoftkeyGraphic, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_GRAPHICS));

	makeEvolPair(EmailAlertsDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_EMAIL_ALERTS));

	makeEvolPair(Error, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_ERROR));


	makeEvolPair(COOLDev, COOLDevWriteCategory, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORIES));

	makeEvolPair(COOLDev, COOLDevWriteCountry, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_COUNTRIES));

	makeEvolPair(COOLDev, COOLDevWriteCategoryCountry, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORY_COUNTRIES));

	makeEvolPair(COOLDev, COOLDevWriteProductionStep, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_PROD_STEPS));

	makeEvolPair(COOLDev, COOLDevWriteCategoryProductionStep, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORY_PROD_STEPS));

	makeEvolPair(COOLDev, COOLDevWritePretext, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_PRETEXTS));

	makeEvolPair(COOLDev, COOLDevWriteCategoryPretext, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORY_PRETEXTS));

	makeEvolPair(COOLDev, COOLDevWriteCoolText, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_TEXTS));

	makeEvolPair(COOLDev, COOLDevWriteShortlist, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_SHORTLIST_INFO));
#if 1	//	What is this ???
	makeEvolPair(COOLDev, COOLDevWriteShortlistText, &pair);
	deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_SHORTLIST_RECORD));	//	???
#endif

	makeEvolPair(SoftkeyDev, WriteSoftkeyGroup, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_GROUP));

	makeEvolPair(SoftkeyDev, WriteSoftkeyStyle, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_STYLE));

	makeEvolPair(SoftkeyDev, WriteSoftkeyFeature, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_FEATURE));

	makeEvolPair(SoftkeyDev, WriteSoftkey, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY));

	makeEvolPair(ProductDev, ProductWriteExpirationLabel, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_EXPIRATION_LABEL));


//-------------------------------------------------------------------------------

	makeEvolPair(SessionDev, SnWriteStatistics, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_STATISTICS));

	makeEvolPair(SessionDev, SnWriteScaleStatus, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SCALE_STATUS));

	makeEvolPair(SessionDev, SnCommStart, &pair);
	deviceEventWriterMap.insert(make_pair(pair, COMM_START));

	makeEvolPair(SessionDev, SnCommEnd, &pair);
	deviceEventWriterMap.insert(make_pair(pair, COMM_END));

	makeEvolPair(Wrapper, WrEvolBackupPackageTimings, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_WRAPPER_TIMINGS));


	makeEvolPair(RecallDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_RECALLS));

	makeEvolPair(DeviceDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_DEVICES));

	makeEvolPair(Printer, PrBackupPrinterSettings, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRINTER_SETTINGS));

	makeEvolPair(Weigher, WgRestoreWeigherSettings, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_WEIGHER_SETTINGS));
	
	makeEvolPair(ProductTextCfgDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_TEXT_CFGS));

	makeEvolPair(BarcodeScannerDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_BARCODE_SCANNERS));

	//	Start backwards compatibility for Ultima/Quantum
	makeEvolPair(COOLDev, COOLDevWriteAllCoolTextForUltima, &pair);
	deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_COOL_TEXT_RECORD));

	makeEvolPair(COOLDev, COOLDevWriteAllCategoryProdStepsForUltima, &pair);
	deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_CUSTOM_COOL_TEXT_RECORD));

	makeEvolPair(COOLDev, COOLDevWriteAllCategoryCountriesForUltima, &pair);
	deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_COUNTRY_TEXT_RECORD));

	makeEvolPair(COOLDev, COOLDevWriteAllShortlistsForUltima, &pair);
	deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_SHORTLIST_RECORD));

	makeEvolPair(COOLTotalDev, hfc::WriteAllCOOLTotalsForUltima, &pair);
	deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_COOL_TOTAL));
	//	End backwards compatibility for Ultima/Quantum

	makeEvolPair(CutTestDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CUTTING_TEST));

	makeEvolPair(CutTestTotalDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CUT_TEST_TOTAL));

	makeEvolPair(SessionDev, SnWriteDateTime, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_DATE_TIME));
}

void CMReaderWriterEvolTag::initDeviceEventReaderMap()
{
	UINT16 pair;

	makeEvolPair(DeptDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_DEPT_NAME, pair));

	makeEvolPair(ProductDev, ProductRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COMMODITIES, pair));

	makeEvolPair(ProductDefaultsDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_DEFAULTS, pair));

	makeEvolPair(ClassDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CLASSES, pair));

	makeEvolPair(NutrifactsDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_NU_DATA, pair));

	makeEvolPair(ExpandedTextDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_EXTEXT, pair));

	makeEvolPair(SpecialMessageDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SPECIAL_MSGS, pair));

	makeEvolPair(ProductNotesDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_NOTES, pair));

	makeEvolPair(MarqueeTextDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_MARQUEES, pair));

	makeEvolPair(ProductText5Dev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODTEXT5, pair));

	makeEvolPair(ProductText6Dev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODTEXT6, pair));

	makeEvolPair(ProductText7Dev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODTEXT7, pair));

	makeEvolPair(ProductText8Dev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODTEXT8, pair));

	makeEvolPair(ProductText9Dev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODTEXT9, pair));

	makeEvolPair(TransactionTypeDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_TOT_NAME, pair));

	makeEvolPair(OperatorDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_OP_IDS, pair));

	makeEvolPair(OperatorDev, OpRestoreNote, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_OPERATOR_NOTES, pair));

	makeEvolPair(ConfigurationDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CONFIG_ITEMS, pair));

	makeEvolPair(ConfigurationDev, CfgStoreInfoRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_STORE_NAME, pair));

	makeEvolPair(ConfigurationDev, CfgSynchronizationRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CONFIG_SYNCHRONIZATION, pair));

	makeEvolPair(GraphicDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_USER_GRAPHICS, pair));

	makeEvolPair(FontDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_USER_FONTS, pair));

	makeEvolPair(LabelTypeDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_LAB_FORM, pair));

	makeEvolPair(GenericTextDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_GEN_TEXT, pair));


#ifndef NOTSELFSERVICE
	makeEvolPair(CategoryDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CATEGORIES, pair));

	makeEvolPair(PresentationMethodDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRESENTATION_METHODS, pair));

	makeEvolPair(SpecialsDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SPECIALS, pair));

	makeEvolPair(StreamingGraphicDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_STREAMING_GRAPHIC_INFO, pair));

	makeEvolPair(ProductStatisticsDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_STATISTICS, pair));

	makeEvolPair(GraphicDev, RestoreProductGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_GRAPHICS, pair));

	makeEvolPair(GraphicDev, RestoreCategoryGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CATEGORY_GRAPHICS, pair));

	makeEvolPair(GraphicDev, RestoreSpecialsGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SPECIALS_GRAPHICS, pair));

	makeEvolPair(GraphicDev, RestoreStreamingGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_STREAMING_GRAPHICS, pair));
#endif

	makeEvolPair(GraphicDev, RestoreSoftkeyGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_GRAPHICS, pair));

	makeEvolPair(EmailAlertsDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_EMAIL_ALERTS, pair));


	//	Start backwards compatibility for Ultima/Quantum
	makeEvolPair(COOLDev, COOLDevRestoreCoolText, &pair);
	deviceEventReaderMap.insert(make_pair(SM_WRITE_COOL_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevRestoreProductionStep, &pair);
	deviceEventReaderMap.insert(make_pair(SM_WRITE_CUSTOM_COOL_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevRestoreCountry, &pair);
	deviceEventReaderMap.insert(make_pair(SM_WRITE_COUNTRY_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevRestoreShortlistForUltima, &pair);
	deviceEventReaderMap.insert(make_pair(SM_WRITE_SHORTLIST_RECORD, pair));


	makeEvolPair(COOLDev, COOLDevDeleteCoolText, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_COOL_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevDeleteProductionStep, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_CUSTOM_COOL_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevDeleteCountry, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_COUNTRY_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevShortlistDeleteText, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_SHORTLIST_RECORD, pair));
	//	End backwards compatibility for Ultima/Quantum

	makeEvolPair(COOLDev, COOLDevRestoreCategory, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORIES, pair));

	makeEvolPair(COOLDev, COOLDevRestoreCountry, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevRestoreCategoryCountry, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORY_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevRestoreProductionStep, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevRestoreCategoryProductionStep, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORY_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevRestorePretext, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevRestoreCategoryPretext, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORY_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevRestoreCoolText, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_TEXTS, pair));

	makeEvolPair(COOLDev, COOLDevRestoreShortlist, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_COOL_SHORTLIST_INFO, pair));

	makeEvolPair(SoftkeyDev, RestoreSoftkeyGroup, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_GROUP, pair));

	makeEvolPair(SoftkeyDev, RestoreSoftkeyStyle, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_STYLE, pair));

	makeEvolPair(SoftkeyDev, RestoreSoftkeyFeature, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_FEATURE, pair));

	makeEvolPair(SoftkeyDev, RestoreSoftkey, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY, pair));

	makeEvolPair(ProductDev, ProductRestoreExpirationLabel, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_EXPIRATION_LABEL, pair));

//-------------------------------------------------------------------------------

	makeEvolPair(DeptDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_DEPT_NAME, pair));

	makeEvolPair(ProductDev, ProductReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COMMODITIES, pair));

	makeEvolPair(ProductDefaultsDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODUCT_DEFAULTS, pair));

	makeEvolPair(ClassDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CLASSES, pair));

	makeEvolPair(NutrifactsDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_NU_DATA, pair));

	makeEvolPair(ExpandedTextDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_EXTEXT, pair));

	makeEvolPair(SpecialMessageDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SPECIAL_MSGS, pair));

	makeEvolPair(ProductNotesDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODUCT_NOTES, pair));

	makeEvolPair(MarqueeTextDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_MARQUEES, pair));

	makeEvolPair(ProductText5Dev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODTEXT5, pair));

	makeEvolPair(ProductText6Dev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODTEXT6, pair));

	makeEvolPair(ProductText7Dev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODTEXT7, pair));

	makeEvolPair(ProductText8Dev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODTEXT8, pair));

	makeEvolPair(ProductText9Dev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODTEXT9, pair));

	makeEvolPair(TransactionTypeDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_TOT_NAME, pair));

	makeEvolPair(OperatorDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_OP_IDS, pair));

	makeEvolPair(OperatorDev, OpReadNote, &pair);
	deviceEventReaderMap.insert(make_pair(READ_OPERATOR_NOTES, pair));

	makeEvolPair(OperatorChangesDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_OP_CHANGES, pair));

	// I think I want to make them all Operator Changes with the only difference the modechange flag.
    makeEvolPair(SupervisorChangesDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SUPER_OP_CHANGES, pair));

	makeEvolPair(ConfigurationDev, CfgStoreInfoRead, &pair);
	deviceEventReaderMap.insert(make_pair(READ_STORE_NAME, pair));

	makeEvolPair(ConfigurationDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CONFIG_ITEMS, pair));

	makeEvolPair(ConfigurationDev, CfgSynchronizationRead, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CONFIG_SYNCHRONIZATION, pair));

	makeEvolPair(GraphicDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_USER_GRAPHICS, pair));

	makeEvolPair(FontDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_USER_FONTS, pair));

	makeEvolPair(LabelTypeDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_LABEL_FORM, pair));  // currently 0x71, evol is

	makeEvolPair(GenericTextDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_GEN_TEXT, pair));

	makeEvolPair(ProductTotalDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COMMODITY_TOTALS, pair));

	makeEvolPair(ClassTotalDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CLASS_TOTALS, pair));

	makeEvolPair(OperatorTotalDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_OP_TOTALS, pair));

	makeEvolPair(HourlyTotalDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_HOURLY_TOTALS, pair));

	makeEvolPair(TransactionDev, TransactionReadAndClear, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CLEAR_ITEM_TRANS, pair));

#ifndef NOTSELFSERVICE
	makeEvolPair(CategoryDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CATEGORIES, pair));

	makeEvolPair(PresentationMethodDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRESENTATION_METHODS, pair));

	makeEvolPair(SpecialsDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SPECIALS, pair));

	makeEvolPair(StreamingGraphicDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_STREAMING_GRAPHIC_INFO, pair));

	makeEvolPair(ProductStatisticsDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODUCT_STATISTICS, pair));

	makeEvolPair(GraphicDev, ReadAllProductGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODUCT_GRAPHICS, pair));

	makeEvolPair(GraphicDev, ReadAllCategoryGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(READ_CATEGORY_GRAPHICS, pair));

	makeEvolPair(GraphicDev, ReadAllSpecialsGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SPECIALS_GRAPHICS, pair));

	makeEvolPair(GraphicDev, ReadAllStreamingGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(READ_STREAMING_GRAPHICS, pair));
#endif

	makeEvolPair(GraphicDev, ReadAllSoftkeyGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SOFTKEY_GRAPHICS, pair));

	makeEvolPair(EmailAlertsDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_EMAIL_ALERTS, pair));


	makeEvolPair(COOLDev, COOLDevReadAllCategories, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_CATEGORIES, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCountries, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCategoryCountries, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_CATEGORY_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevReadAllProductionSteps, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCategoryProductionSteps, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_CATEGORY_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevReadAllPretexts, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCategoryPretexts, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_CATEGORY_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCoolText, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_TEXTS, pair));

	makeEvolPair(COOLDev, COOLDevReadAllShortlists, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_SHORTLIST_INFO, pair));
#if 0	//	What is this ???
	makeEvolPair(COOLDev, COOLDevReadAllShortlistTexts, &pair);
	deviceEventReaderMap.insert(make_pair(READ_COOL_SHORTLIST_INFO, pair));
#endif

	makeEvolPair(SoftkeyDev, ReadAllSoftkeyGroups, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SOFTKEY_GROUPS, pair));

	makeEvolPair(SoftkeyDev, ReadAllSoftkeyStyles, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SOFTKEY_STYLES, pair));

	makeEvolPair(SoftkeyDev, ReadAllSoftkeyFeatures, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SOFTKEY_FEATURES, pair));

	makeEvolPair(SoftkeyDev, ReadAllSoftkeys, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SOFTKEYS, pair));

	makeEvolPair(ProductDev, ProductReadAllExpirationLabels, &pair);
	deviceEventReaderMap.insert(make_pair(READ_EXPIRATION_LABELS, pair));

//-------------------------------------------------------------------------------

	makeEvolPair(DeptDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_DEPT_NAMES, pair));

	makeEvolPair(ProductDev, ProductDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COMMODITIES, pair));

	makeEvolPair(ClassDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_CLASSES, pair));

	makeEvolPair(ProductDev, ProductDeleteAllCommodityClass, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_CDTY_CLASS, pair));

	makeEvolPair(NutrifactsDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_NU_DATA, pair));

	makeEvolPair(ExpandedTextDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_EXTEXT, pair));

	makeEvolPair(SpecialMessageDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SMSGS, pair));

	makeEvolPair(ProductNotesDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_NOTES, pair));

	makeEvolPair(MarqueeTextDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_MARQUEES, pair));

	makeEvolPair(ProductText5Dev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODTEXT5, pair));

	makeEvolPair(ProductText6Dev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODTEXT6, pair));

	makeEvolPair(ProductText7Dev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODTEXT7, pair));

	makeEvolPair(ProductText8Dev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODTEXT8, pair));

	makeEvolPair(ProductText9Dev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODTEXT9, pair));

	makeEvolPair(TransactionTypeDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_TOT_NAMES, pair));

	makeEvolPair(OperatorDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_OP_IDS, pair));

	makeEvolPair(OperatorDev, OpDeleteNote, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_OPERATOR_NOTES, pair));

	makeEvolPair(TransactionDev, ClearOperatorChanges, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_OP_CHANGES, pair));

	// I think I want to make them all Operator Changes with the only difference the modechange flag.
    makeEvolPair(TransactionDev, ClearSupervisorChanges, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SUPER_OP_CHANGES, pair));

	makeEvolPair(GraphicDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_USER_GRAPHICS, pair));

	makeEvolPair(FontDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_USER_FONTS, pair));

	makeEvolPair(LabelTypeDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_LAB_FORMS, pair));

	makeEvolPair(GenericTextDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_GEN_TEXT, pair));

	makeEvolPair(TransactionDev, ClearProductAndClassTotals, &pair);
	deviceEventReaderMap.insert(make_pair(CLEAR_PLU_CLASS_TOTALS, pair));

	makeEvolPair(TransactionDev, ClearOperatorTotals, &pair);
	deviceEventReaderMap.insert(make_pair(CLEAR_OP_TOTALS, pair));

	makeEvolPair(TransactionDev, ClearHourlyTotals, &pair);
	deviceEventReaderMap.insert(make_pair(CLEAR_HOURLY_TOTALS, pair));

	makeEvolPair(TransactionDev, ClearCOOLTotals, &pair);
	deviceEventReaderMap.insert(make_pair(SM_CLEAR_COOL_TOTALS, pair));

	makeEvolPair(TransactionDev, ClearAllTotals, &pair);
	deviceEventReaderMap.insert(make_pair(CLEAR_ALL_TOTALS, pair));

	makeEvolPair(TransactionDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_ITEM_TRANS, pair));

#ifndef NOTSELFSERVICE
	makeEvolPair(CategoryDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_CATEGORIES, pair));

	makeEvolPair(PresentationMethodDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRESENTATION_METHODS, pair));

	makeEvolPair(SpecialsDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SPECIALS, pair));

	makeEvolPair(StreamingGraphicDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_STREAMING_GRAPHIC_INFO, pair));

	makeEvolPair(ProductStatisticsDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_STATISTICS, pair));

	makeEvolPair(GraphicDev, DeleteAllProductGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_GRAPHICS, pair));

	makeEvolPair(GraphicDev, DeleteAllCategoryGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_CATEGORY_GRAPHICS, pair));

	makeEvolPair(GraphicDev, DeleteAllSpecialsGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SPECIALS_GRAPHICS, pair));

	makeEvolPair(GraphicDev, DeleteAllStreamingGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_STREAMING_GRAPHICS, pair));
#endif

	makeEvolPair(GraphicDev, DeleteAllSoftkeyGraphics, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_GRAPHICS, pair));


	makeEvolPair(EmailAlertsDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_EMAIL_ALERTS, pair));

	//	Start backwards compatibility for Ultima/Quantum
	makeEvolPair(COOLDev, COOLDevDeleteAllCountryTextForUltima, &pair);		//	Eventually change to COOLDevDeleteAllCoolTextForUltima
	deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_COOL_TEXT, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllProductionSteps, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_CUSTOM_COOL_TEXT, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllCountries, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_COUNTRY_TEXT, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllShortlists, &pair);
	deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_SHORTLIST, pair));
	//	End backwards compatibility for Ultima/Quantum


	makeEvolPair(COOLDev, COOLDevDeleteAllCategories, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORIES, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllCountries, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllCategoryCountries, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORY_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllProductionSteps, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllCategoryProductionSteps, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORY_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllPretexts, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllCategoryPretexts, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORY_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllCoolText, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_TEXTS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteAllShortlists, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_SHORTLIST_INFO, pair));


	makeEvolPair(SoftkeyDev, DeleteAllSoftkeyGroups, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_GROUPS, pair));

	makeEvolPair(SoftkeyDev, DeleteAllSoftkeyStyles, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_STYLES, pair));

	makeEvolPair(SoftkeyDev, DeleteAllSoftkeyFeatures, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_FEATURES, pair));

	makeEvolPair(SoftkeyDev, DeleteAllSoftkeys, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEYS, pair));

	makeEvolPair(ProductDev, ProductDeleteAllExpirationLabels, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_EXPIRATION_LABELS, pair));


	makeEvolPair(ProductDefaultsDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_DEFAULTS, pair));


//-------------------------------------------------------------------------------

	makeEvolPair(ProductDev, ProductDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COMMODITY, pair));

	makeEvolPair(ClassDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_CLASS, pair));

	makeEvolPair(NutrifactsDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_NU_DATA, pair));

	makeEvolPair(ExpandedTextDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_EXTEXT, pair));

	makeEvolPair(SpecialMessageDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SPCL_MSG, pair));

	makeEvolPair(ProductNotesDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODUCT_NOTE, pair));

	makeEvolPair(MarqueeTextDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_MARQUEE, pair));

	makeEvolPair(ProductText5Dev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODTEXT5, pair));

	makeEvolPair(ProductText6Dev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODTEXT6, pair));

	makeEvolPair(ProductText7Dev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODTEXT7, pair));

	makeEvolPair(ProductText8Dev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODTEXT8, pair));

	makeEvolPair(ProductText9Dev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODTEXT9, pair));

	makeEvolPair(OperatorDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_OP_ID, pair));

	makeEvolPair(GraphicDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_USER_GRAPHIC, pair));

	makeEvolPair(FontDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_USER_FONT, pair));

	makeEvolPair(LabelTypeDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_LABEL_FORM, pair));

#ifndef NOTSELFSERVICE
	makeEvolPair(CategoryDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_CATEGORY, pair));

	makeEvolPair(PresentationMethodDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRESENTATION_METHOD, pair));

	makeEvolPair(SpecialsDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SPECIAL, pair));

	makeEvolPair(StreamingGraphicDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_STREAMING_GRAPHIC_INFO, pair));

	makeEvolPair(ProductStatisticsDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODUCT_STATISTIC, pair));

	makeEvolPair(GraphicDev, DeleteProductGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODUCT_GRAPHIC, pair));

	makeEvolPair(GraphicDev, DeleteCategoryGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_CATEGORY_GRAPHIC, pair));

	makeEvolPair(GraphicDev, DeleteSpecialsGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SPECIALS_GRAPHIC, pair));

	makeEvolPair(GraphicDev, DeleteStreamingGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_STREAMING_GRAPHIC, pair));
#endif

	makeEvolPair(GraphicDev, DeleteSoftkeyGraphic, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_GRAPHIC, pair));

	makeEvolPair(EmailAlertsDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_EMAIL_ALERT, pair));


	makeEvolPair(COOLDev, COOLDevDeleteCategory, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORIES, pair));

	makeEvolPair(COOLDev, COOLDevDeleteCountry, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevDeleteCategoryCountry, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORY_COUNTRIES, pair));

	makeEvolPair(COOLDev, COOLDevDeleteProductionStep, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteCategoryProductionStep, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORY_PROD_STEPS, pair));

	makeEvolPair(COOLDev, COOLDevDeletePretext, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteCategoryPretext, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORY_PRETEXTS, pair));

	makeEvolPair(COOLDev, COOLDevDeleteCoolText, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_TEXTS, pair));

	makeEvolPair(COOLDev, COOLDevShortlistDeleteText, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_COOL_SHORTLIST_INFO, pair));


	makeEvolPair(SoftkeyDev, DeleteSoftkeyGroup, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_GROUP, pair));

	makeEvolPair(SoftkeyDev, DeleteSoftkeyStyle, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_STYLE, pair));

	makeEvolPair(SoftkeyDev, DeleteSoftkeyFeature, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_FEATURE, pair));

	makeEvolPair(SoftkeyDev, DeleteSoftkey, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY, pair));

//-------------------------------------------------------------------------------

	makeEvolPair(SessionDev, SnReadStatistics, &pair);
	deviceEventReaderMap.insert(make_pair(READ_STATISTICS, pair));

	makeEvolPair(SessionDev, SnReadScaleStatus, &pair);
	deviceEventReaderMap.insert(make_pair(READ_SCALE_STATUS, pair));

	makeEvolPair(SessionDev, SnCommStart, &pair);
	deviceEventReaderMap.insert(make_pair(COMM_START, pair));

	makeEvolPair(SessionDev, SnCommEnd, &pair);
	deviceEventReaderMap.insert(make_pair(COMM_END, pair));

	makeEvolPair(SessionDev, SnTFTPdbDate, &pair);
	deviceEventReaderMap.insert(make_pair(TFTP_DB_DATE, pair));

	makeEvolPair(SessionDev, SnDebugTrace, &pair);
	deviceEventReaderMap.insert(make_pair(0x5F44, pair));

	makeEvolPair(Wrapper, WrEvolRestorePackageTimings, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_WRAPPER_TIMINGS, pair));

	makeEvolPair(SessionDev, SnShutdownRequest, &pair);
	deviceEventReaderMap.insert(make_pair(RESET_SCALE, pair));

	makeEvolPair(VersionManagerDev, UpgradeAutomatic, &pair);
	deviceEventReaderMap.insert(make_pair(REMOTE_FLASH_UPGRADE, pair));

	makeEvolPair(SessionDev, SnKeyActivation, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_USB_KEYS_PRESENT, pair));


	makeEvolPair(RecallDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_RECALLS, pair));

	makeEvolPair(RecallDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_RECALLS, pair));

	makeEvolPair(RecallDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_RECALLS, pair));

	makeEvolPair(RecallDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_RECALLS, pair));


	makeEvolPair(DeviceDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_DEVICES, pair));

	makeEvolPair(DeviceDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_DEVICE, pair));

	makeEvolPair(DeviceDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_DEVICES, pair));

	makeEvolPair(DeviceDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_DEVICES, pair));


	makeEvolPair(ProductTextCfgDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRODUCT_TEXT_CFGS, pair));

	makeEvolPair(ProductTextCfgDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_PRODUCT_TEXT_CFG, pair));

	makeEvolPair(ProductTextCfgDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_TEXT_CFGS, pair));

	makeEvolPair(ProductTextCfgDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_TEXT_CFGS, pair));
	
	makeEvolPair(BarcodeScannerDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_BARCODE_SCANNERS, pair));

	makeEvolPair(BarcodeScannerDev, DtDelete, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_BARCODE_SCANNER, pair));

	makeEvolPair(BarcodeScannerDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_BARCODE_SCANNERS, pair));

	makeEvolPair(BarcodeScannerDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_BARCODE_SCANNERS, pair));

	makeEvolPair(SessionDev, SnExecuteTask, &pair);
	deviceEventReaderMap.insert(make_pair(EXECUTE_TASK, pair));


	//	Start backwards compatibility for Ultima/Quantum
	makeEvolPair(COOLDev, COOLDevReadAllCoolTextForUltima, &pair);
	deviceEventReaderMap.insert(make_pair(SM_READ_COOL_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCategoryProdStepsForUltima, &pair);
	deviceEventReaderMap.insert(make_pair(SM_READ_CUSTOM_COOL_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevReadAllCategoryCountriesForUltima, &pair);
	deviceEventReaderMap.insert(make_pair(SM_READ_COUNTRY_TEXT_RECORD, pair));

	makeEvolPair(COOLDev, COOLDevReadAllShortlistsForUltima, &pair);
	deviceEventReaderMap.insert(make_pair(SM_READ_SHORTLIST_RECORD, pair));

	makeEvolPair(COOLTotalDev, hfc::ReadAllCOOLTotalsForUltima, &pair);
	deviceEventReaderMap.insert(make_pair(SM_READ_COOL_TOTAL_RECORD, pair));
	//	End backwards compatibility for Ultima/Quantum

    makeEvolPair(ConfigurationDev, CfgDeleteAllFiles, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_FILES, pair));

	makeEvolPair(CutTestDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CUTTING_TEST, pair));

	makeEvolPair(CutTestDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_ALL_CUTTING_TESTS, pair));

	makeEvolPair(CutTestDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_CUTTING_TESTS, pair));


	makeEvolPair(CutTestTotalDev, DtWrite, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CUT_TEST_TOTAL, pair));

	makeEvolPair(CutTestTotalDev, DtReadAll, &pair);
	deviceEventReaderMap.insert(make_pair(READ_ALL_CUT_TEST_TOTALS, pair));

	makeEvolPair(CutTestTotalDev, DtDeleteAll, &pair);
	deviceEventReaderMap.insert(make_pair(DEL_ALL_CUT_TEST_TOTALS, pair));


	makeEvolPair(Printer, PrBackupPrinterSettings, &pair);
	deviceEventReaderMap.insert(make_pair(READ_PRINTER_SETTINGS, pair));

	makeEvolPair(Printer, PrRestorePrinterSettings, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_PRINTER_SETTINGS, pair));

	makeEvolPair(Weigher, WgBackupWeigherSettings, &pair);
	deviceEventReaderMap.insert(make_pair(READ_WEIGHER_SETTINGS, pair));

	makeEvolPair(Weigher, WgRestoreWeigherSettings, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_WEIGHER_SETTINGS, pair));

	makeEvolPair(SessionDev, SnBackendRemotePrintTemplateLabel, &pair);
	deviceEventReaderMap.insert(make_pair(REMOTE_PRINT_TEMPLATE, pair));

	makeEvolPair(SessionDev, SnBackendRemotePrint, &pair);
	deviceEventReaderMap.insert(make_pair(REMOTE_PRINT_LABEL, pair));



	/* Generate a reversed ReaderMap so that we can look up function codes from
	a device-event pair. */
	std::map< UINT16, UINT16 >::iterator readerMapIter;
	for (readerMapIter = deviceEventReaderMap.begin(); readerMapIter != deviceEventReaderMap.end(); readerMapIter++)
	{
		deviceEventReversedReaderMap.insert(make_pair(readerMapIter->second, readerMapIter->first));
	}
}
