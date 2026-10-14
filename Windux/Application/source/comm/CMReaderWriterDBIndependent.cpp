/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "ProductDevEvent.h"
#include "OperatorEvent.h"
#include "SessionEvent.h"
#include "ConfigurationEvent.h"
#ifdef LINUX_FIX
#include "messages/TransactionDevEvent.h"
#else
#include "Messages/TransactionDevEvent.h"
#endif      // LINUX_FIX
#include "BackupRestoreEvent.h"
#include "GraphicEvent.h"
#include "COOLDevEvent.h"
#include "SoftkeyDevEvent.h"
#include "Messages/SynchronizationDevEvent.h"
#ifdef LINUX_FIX
#include "VersionManager/include/VersionManagerEvent.h"
#else
#include "VersionManager/Include/VersionManagerEvent.h"
#endif      // LINUX_FIX

#include "CMReaderWriterDBIndependent.h"
#include "Integer.h"
#include "Hex.h"

#include "scalemaster.h"
#include "scalemasterEvol.h"


#include "util/e_string/utilstring.h"


using namespace std;


// statics
std::map< UINT16, UINT16 > CMReaderWriterDBIndependent::deviceEventReaderMap;
std::map< UINT16, UINT16 > CMReaderWriterDBIndependent::deviceEventWriterMap;
std::map< UINT16, UINT16 > CMReaderWriterDBIndependent::deviceEventReversedReaderMap;

const char CMReaderWriterDBIndependent::EVOL_TAGGED_IDENTIFIER[CMReaderWriterDBIndependent::EVOL_TAGGED_IDENTIFIER_LENGTH+1] = {'R', 'T', '\0'};
const char CMReaderWriterDBIndependent::EVOL_TAGGED_SEPARATOR[CMReaderWriterDBIndependent::EVOL_TAGGED_SEPARATOR_LENGTH+1] = {0x1e, '\0'};
const char CMReaderWriterDBIndependent::UNIT_SEPARATOR = 0x1F;

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int CMReaderWriterDBIndependent::_dataType_ = 0;
	std::string CMReaderWriterDBIndependent::_dataTypeName_("CMReaderWriterDBIndependent"); 
#endif

// This is the reader/writer when receiving/sending records to the Evolution formatted backup/restore file,
// as well as the Comm format written to the socket when communicating with Data Gateweigh (ScaleComm).

CMReaderWriterDBIndependent::CMReaderWriterDBIndependent()
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	readerWriterId = CMReaderWriter::DB_INDEPENDENT_ID;
	recordIdentifierLength = 5;
	memset(previousRTData, 0, sizeof(previousRTData));
	previousRTDataStr = "";

	CMReaderWriterDBIndependent::initDeviceEventMaps();
}

CMReaderWriterDBIndependent::~CMReaderWriterDBIndependent()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
}

int CMReaderWriterDBIndependent::getRecordHeaderLength() const
{
	return 0;
}

int CMReaderWriterDBIndependent::getRecordFooterLength() const
{
	return EVOL_TAGGED_SEPARATOR_LENGTH;
}


int CMReaderWriterDBIndependent::finalizeRecordForSending(char *buff, const char *msg, int length)
{
	memcpy(buff, msg, length);
	if (*(buff + length - 1) == UNIT_SEPARATOR)
		length -= 1;
	memcpy(buff + length, EVOL_TAGGED_SEPARATOR, EVOL_TAGGED_SEPARATOR_LENGTH);
	*(buff + length + EVOL_TAGGED_SEPARATOR_LENGTH) = (char)NULL;
		
	if (Debug::isTracing("CMReaderWriter.DBIndependent"))
		Debug::trace("CMReaderWriter.DBIndependent", HobString("finalizeRecordForSending - ") + HobString(Hex::toString(buff, length + EVOL_TAGGED_SEPARATOR_LENGTH)));
	return length + EVOL_TAGGED_SEPARATOR_LENGTH;
}


bool CMReaderWriterDBIndependent::finishedReadingRecord(const char *msg, int length)
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

bool CMReaderWriterDBIndependent::finishedReadingRecord(const HobString &msg)
{
	return ((INT32) msg.size() >= EVOL_TAGGED_SEPARATOR_LENGTH && msg.substr(msg.size() - EVOL_TAGGED_SEPARATOR_LENGTH, EVOL_TAGGED_SEPARATOR_LENGTH) == EVOL_TAGGED_SEPARATOR) ? true : false;
}


/*
 * Note: The msg buffer being passed in needs to be large enough to allow for this method
 * to prepend the previous RT tag and data onto the msg, if it doesn't contain it.
 */
int CMReaderWriterDBIndependent::formatFromReceive(char *msg, int length, bool /*isRecordBeginningBuffer*/)
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

		Debug::trace("CMReaderWriter.DBIndependent", "prepended the saved RT data onto the msg.");
		delete[] tempBuff;
		finalLength = len;
	}

	return finalLength;
}

int CMReaderWriterDBIndependent::formatFromReceive(HobString &msg)
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

		Debug::trace("CMReaderWriter.DBIndependent", "prepended the saved RT data onto the msg.");
	}

	return finalLength;
}


//private, static
bool CMReaderWriterDBIndependent::containsRTData(const char *msg)
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
bool CMReaderWriterDBIndependent::containsRTData(const HobString &msg)
{
	return (!msg.size() || msg.substr(0, EVOL_TAGGED_IDENTIFIER_LENGTH) != EVOL_TAGGED_IDENTIFIER) ? false : true;
}


//private
void CMReaderWriterDBIndependent::saveRTData(const char *msg, char *whereToPutIt)
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

void CMReaderWriterDBIndependent::saveRTData(const HobString &msg, HobString &whereToPutIt)
{
	whereToPutIt = msg.substr(0, RT_DATA_MAXLENGTH);
}


void CMReaderWriterDBIndependent::getRTData(const char *msg, char *rtData)
{
	if (!msg || !rtData)
		return;

	if (CMReaderWriterDBIndependent::containsRTData(msg))
	{
		char data[RT_DATA_MAXLENGTH + 1];  // more than enough
		CMReaderWriterDBIndependent::saveRTData(msg, data);
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
bool CMReaderWriterDBIndependent::canIdentifyBuffer(const char *msg)
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
void CMReaderWriterDBIndependent::determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event)
{
	*device = UnknownDevice;
	*event = 0;

	if (!recordTypeData || strlen(recordTypeData) < 2)
		return;

	if (strlen(recordTypeData) != 2)
		Debug::trace("CMReaderWriter.DBIndependent", "recordTypeData length is " + util_string::inttostring(strlen(recordTypeData)) +
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
void CMReaderWriterDBIndependent::determineRecordType(char *recordTypeData, EvolDeviceType device, int event)
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
void CMReaderWriterDBIndependent::determineReaderRecordType(char *recordTypeData, EvolDeviceType device, int event)
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


void CMReaderWriterDBIndependent::makeEvolPair(EvolDeviceType dev, UINT8 event, UINT16 *buff)
{
	if (buff)
	{
		UINT8 *pU8 = (UINT8 *) buff;

		pU8[0] = event;
		pU8[1] = (UINT8)dev;
	}
}


void CMReaderWriterDBIndependent::initDeviceEventMaps()
{
	if (deviceEventReaderMap.empty())
	{
		initDeviceEventReaderMap();
		initDeviceEventWriterMap();
	}
}

void CMReaderWriterDBIndependent::initDeviceEventWriterMap()
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

	makeEvolPair(ConfigurationDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CONFIG_ITEMS));

	makeEvolPair(ConfigurationDev, CfgStoreInfoWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_STORE_NAME));

	makeEvolPair(ConfigurationDev, CfgSynchronizationWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_CONFIG_SYNCHRONIZATION));

	makeEvolPair(ScaleDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SCALE));

	makeEvolPair(BatchDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_BATCH));

	makeEvolPair(SynchronizationChangeDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_SYNCHRONIZATION_CHANGE));

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


	makeEvolPair(RecallDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_RECALLS));

	makeEvolPair(DeviceDev, DtWrite, &pair);
	deviceEventWriterMap.insert(make_pair(pair, WRITE_DEVICES));

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

	//	End backwards compatibility for Ultima/Quantum
}

void CMReaderWriterDBIndependent::initDeviceEventReaderMap()
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

	makeEvolPair(TransactionDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_ITEM_TRANS, pair));

	makeEvolPair(ScaleDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SCALE, pair));

	makeEvolPair(BatchDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_BATCH, pair));

	makeEvolPair(SynchronizationChangeDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_SYNCHRONIZATION_CHANGE, pair));

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

	makeEvolPair(CutTestDev, DtRestore, &pair);
	deviceEventReaderMap.insert(make_pair(WRITE_CUTTING_TEST, pair));

//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------

	makeEvolPair(SessionDev, SnCommStart, &pair);
	deviceEventReaderMap.insert(make_pair(COMM_START, pair));

	makeEvolPair(SessionDev, SnCommEnd, &pair);
	deviceEventReaderMap.insert(make_pair(COMM_END, pair));




	/* Generate a reversed ReaderMap so that we can look up function codes from
	a device-event pair. */
	std::map< UINT16, UINT16 >::iterator readerMapIter;
	for (readerMapIter = deviceEventReaderMap.begin(); readerMapIter != deviceEventReaderMap.end(); readerMapIter++)
	{
		deviceEventReversedReaderMap.insert(make_pair(readerMapIter->second, readerMapIter->first));
	}
}
