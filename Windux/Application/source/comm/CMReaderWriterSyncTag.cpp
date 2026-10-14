#include "CMReaderWriterSyncTag.h"


#include "Messages/SynchronizationDevEvent.h"
#include "scalemaster.h"
#include "scalemasterEvol.h"

#include "DataTypeEvent.h"
#include "ProductDevEvent.h"
#include "OperatorEvent.h"
#include "SessionEvent.h"
#include "ConfigurationEvent.h"
#include "BackupRestoreEvent.h"
#include "GraphicEvent.h"
#include "COOLDevEvent.h"
#include "SoftkeyDevEvent.h"
#ifdef LINUX_FIX
#include "messages/COOLTotalEvent.h"
#else
#include "Messages/COOLTotalEvent.h"
#endif

#include "Hex.h"
#include "util/e_string/utilstring.h"



using namespace std;


// statics
CMReaderWriterSyncTag::functionCodesContainer_type CMReaderWriterSyncTag::deviceEventReaderMap;
std::map< UINT16, UINT16 > CMReaderWriterSyncTag::deviceEventWriterMap;
std::map< UINT16, UINT16 > CMReaderWriterSyncTag::deviceEventReversedReaderMap;

const char CMReaderWriterSyncTag::SYNC_TAGGED_IDENTIFIER[CMReaderWriterSyncTag::SYNC_TAGGED_IDENTIFIER_LENGTH + 1] = {'R', 'T', '\0'};
const char CMReaderWriterSyncTag::SYNC_TAGGED_SEPARATOR[CMReaderWriterSyncTag::SYNC_TAGGED_SEPARATOR_LENGTH + 1] = {0x1e, '\0'};
const char CMReaderWriterSyncTag::UNIT_SEPARATOR = 0x1F;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CMReaderWriterSyncTag::_dataType_ = 0;
const string CMReaderWriterSyncTag::_dataTypeName_("CMReaderWriterSyncTag");
#endif



CMReaderWriterSyncTag::CMReaderWriterSyncTag()
{
#ifdef USE_ACCOUNTANT
    if ( !_dataType_ )
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif


    readerWriterId = CMReaderWriter::SYNC_TAGGED_ID;
    recordIdentifierLength = 5;
    memset(previousRTData, 0, sizeof(previousRTData));
    previousRTDataStr = "";

    CMReaderWriterSyncTag::initDeviceEventMaps();
}


CMReaderWriterSyncTag::~CMReaderWriterSyncTag()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}


int CMReaderWriterSyncTag::getRecordHeaderLength() const
{
    return 0;
}


int CMReaderWriterSyncTag::getRecordFooterLength() const
{
    return SYNC_TAGGED_SEPARATOR_LENGTH;
}


int CMReaderWriterSyncTag::finalizeRecordForSending(char *buff, const char *msg, int length)
{
    memcpy(buff, msg, length);
    if (*(buff + length - 1) == UNIT_SEPARATOR)
        length -= 1;
    memcpy(buff + length, SYNC_TAGGED_SEPARATOR, SYNC_TAGGED_SEPARATOR_LENGTH);
    *(buff + length + SYNC_TAGGED_SEPARATOR_LENGTH) = (char)NULL;

    if (Debug::isTracing("CMReaderWriter.SyncTag"))
        Debug::trace("CMReaderWriter.SyncTag", HobString("finalizeRecordForSending - ") + HobString(Hex::toString(buff, length + SYNC_TAGGED_SEPARATOR_LENGTH)));
    return length + SYNC_TAGGED_SEPARATOR_LENGTH;
}


bool CMReaderWriterSyncTag::finishedReadingRecord(const char *msg, int length)
{
    return (*(msg + length - 1) == *SYNC_TAGGED_SEPARATOR) ? true : false;

    /* If the separator ever becomes more than 1 byte, do the following; otherwise, just do above.
       for (int i = 0; i < SYNC_TAGGED_SEPARATOR_LENGTH; i++)
       {
       if (*(msg + length - SYNC_TAGGED_SEPARATOR_LENGTH +i) != *(HOB_RECORD_SEPARATOR + i))
       return false;
       }
       return true;
       */
}


bool CMReaderWriterSyncTag::finishedReadingRecord(const HobString &msg)
{
    return ((INT32) msg.size() >= SYNC_TAGGED_SEPARATOR_LENGTH && msg.substr(msg.size() - SYNC_TAGGED_SEPARATOR_LENGTH, SYNC_TAGGED_SEPARATOR_LENGTH) == SYNC_TAGGED_SEPARATOR) ? true : false;
}


/*
 * Note: The msg buffer being passed in needs to be large enough to allow for this method
 * to prepend the previous RT tag and data onto the msg, if it doesn't contain it.
 */
int CMReaderWriterSyncTag::formatFromReceive(char *msg, int length, bool UNUSED(isRecordBeginningBuffer))
{
    UNREFERENCED_PARAMETER(isRecordBeginningBuffer);

    if (!msg || length <= 0)
        return 0;

    int finalLength = length;

    // First, remove the record separator from the received msg.
    // The following assumes the SYNC_TAGGED_SEPARATOR is only 1 byte!!!!!!!!!!!
    if (*(msg + length - 1) == *SYNC_TAGGED_SEPARATOR)
    {
        *(msg + length - 1) = (char)NULL;
        finalLength -= SYNC_TAGGED_SEPARATOR_LENGTH;
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

        Debug::trace("CMReaderWriter.SyncTag", "prepended the saved RT data onto the msg.");
        delete[] tempBuff;
        finalLength = len;
    }

    return finalLength;
}


int CMReaderWriterSyncTag::formatFromReceive(HobString &msg)
{
    if (!msg.size())
        return 0;

    int finalLength = (int)msg.size();

    // First, remove the record separator from the received msg, if it exists.
    if (msg.substr(msg.size() - SYNC_TAGGED_SEPARATOR_LENGTH, SYNC_TAGGED_SEPARATOR_LENGTH) == SYNC_TAGGED_SEPARATOR)
    {
        msg.erase(msg.size() - SYNC_TAGGED_SEPARATOR_LENGTH);
        finalLength -= SYNC_TAGGED_SEPARATOR_LENGTH;
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

        Debug::trace("CMReaderWriter.SyncTag", "prepended the saved RT data onto the msg.");
    }

    return finalLength;
}


//private, static
bool CMReaderWriterSyncTag::containsRTData(const char *msg)
{
    if (!msg)
        return false;

    for (int i = 0; i < SYNC_TAGGED_IDENTIFIER_LENGTH; i++)
    {
        if (*(msg + i) != SYNC_TAGGED_IDENTIFIER[i])
            return false;
    }
    return true;
}

//private, static
bool CMReaderWriterSyncTag::containsRTData(const HobString &msg)
{
    return (!msg.size() || msg.substr(0, SYNC_TAGGED_IDENTIFIER_LENGTH) != SYNC_TAGGED_IDENTIFIER) ? false : true;
}


//private
void CMReaderWriterSyncTag::saveRTData(const char *msg, char *whereToPutIt)
{
    if (!msg || !whereToPutIt)
        return;

    int i = 0;
    while (i < RT_DATA_MAXLENGTH)
    {
        *(whereToPutIt + i) = *(msg + i);
        if (*(msg + i) == UNIT_SEPARATOR || *(msg + i) == *SYNC_TAGGED_SEPARATOR)
            break;
        i++;
    }
    *(whereToPutIt + RT_DATA_MAXLENGTH) = (char)0;
}


void CMReaderWriterSyncTag::saveRTData(const HobString &msg, HobString &whereToPutIt)
{
    whereToPutIt = msg.substr(0, RT_DATA_MAXLENGTH);
}


void CMReaderWriterSyncTag::getRTData(const char *msg, char *rtData)
{
    if (!msg || !rtData)
        return;

    if (CMReaderWriterSyncTag::containsRTData(msg))
    {
        char data[RT_DATA_MAXLENGTH + 1];  // more than enough
        CMReaderWriterSyncTag::saveRTData(msg, data);
        unsigned int i = SYNC_TAGGED_IDENTIFIER_LENGTH;
        int j = 0;
        while (i < sizeof(data))
        {
            if (data[i] == UNIT_SEPARATOR || data[i] == *SYNC_TAGGED_SEPARATOR || data[i] == '\0')
                break;
            *(rtData + j++) = data[i++];
        }
        *(rtData + j) = (char)NULL;
    }
}


// static
bool CMReaderWriterSyncTag::canIdentifyBuffer(const char *msg)
{
    if (!msg)
        return false;

    for (int i = 0; i < SYNC_TAGGED_IDENTIFIER_LENGTH; i++)
    {
        if (*(msg+i) != SYNC_TAGGED_IDENTIFIER[i])
            return false;
    }
    return true;
}


/* By passing in the data associated with the RT tag, identify the representative
 * device and event (as set up in Event.h) by looking up tagData (the key) in the deviceEventReaderMap.
 */
// static
void CMReaderWriterSyncTag::determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event)
{
    *device = UnknownDevice;
    *event = 0;

    if ( !recordTypeData || strlen(recordTypeData) < 2 )
        return;

    if ( strlen(recordTypeData) != 2 )
    {
        Debug::trace("CMReaderWriterSyncTag", "recordTypeData length is " +
                util_string::inttostring(strlen(recordTypeData)) + " data is *" + std::string(recordTypeData) + "*");
    }


    UINT16 rtui16 = 0;
    UINT8 *pU8 = (UINT8 *) &rtui16;

    // because Intel is little endian, have to byte-reverse the two bytes in the UINT16
    *pU8 = *(recordTypeData + 1);
    *(pU8+1) = *recordTypeData;


    functionCodesContainer_type::iterator readerMapIter = deviceEventReaderMap.find(rtui16);
    if ( readerMapIter != deviceEventReaderMap.end() )
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
void CMReaderWriterSyncTag::determineRecordType(char *recordTypeData, EvolDeviceType device, int event)
{
    UINT16 pair;

    if ( !recordTypeData )
        return;

    makeSyncPair(device, (UINT8)event, &pair);

    functionCodesContainer_type::iterator writerMapIter = deviceEventWriterMap.find(pair);
    if ( writerMapIter != deviceEventWriterMap.end() )
    {
        UINT16 rtData = writerMapIter->second;
        UINT8 *pU8 = (UINT8 *) &rtData;

        // because Intel is little endian, have to byte-reverse the two bytes in the UINT16
        *(recordTypeData++) = *(pU8 + 1);
        *(recordTypeData++) = *pU8;
        *(recordTypeData)   = '\0';
    }
    else
        *recordTypeData = '\0';
}


/* By passing in the application's device and event (as set up in Event.h), identify the representative
 * RT tag value by looking up the device/event data (the key) in the deviceEventReaderMap.
 */
// static
void CMReaderWriterSyncTag::determineReaderRecordType(char *recordTypeData, EvolDeviceType device, int event)
{
    UINT16 pair;

    if ( !recordTypeData )
        return;

    makeSyncPair(device, (UINT8)event, &pair);

    functionCodesContainer_type::iterator readerMapIter = deviceEventReversedReaderMap.find(pair);
    if ( readerMapIter != deviceEventReversedReaderMap.end() )
    {
        UINT16 rtData = readerMapIter->second;
        UINT8 *pU8 = (UINT8 *) &rtData;

        // because Intel is little endian, have to byte-reverse the two bytes in the UINT16
        *(recordTypeData++) = *(pU8 + 1);
        *(recordTypeData++) = *pU8;
        *recordTypeData     = '\0';
    }
    else
        *recordTypeData = '\0';
}


void CMReaderWriterSyncTag::initDeviceEventMaps()
{
    if ( deviceEventReaderMap.empty() )
    {
        initDeviceEventReaderMap();
        initDeviceEventWriterMap();
    }
}


void CMReaderWriterSyncTag::initDeviceEventReaderMap()
{
    UINT16 pair;

    makeSyncPair(SessionDev, SnCommStart, &pair);
    deviceEventReaderMap.insert(make_pair(COMM_START, pair));

    makeSyncPair(SessionDev, SnCommEnd, &pair);
    deviceEventReaderMap.insert(make_pair(COMM_END, pair));

    makeSyncPair(SynchronizationDev, synchronization::RegisterSecondaryScale, &pair);
    deviceEventReaderMap.insert(make_pair(REGISTER_SECONDARY_SCALE, pair));

    makeSyncPair(SynchronizationDev, synchronization::NotifyLocalBatchesPresent, &pair);
    deviceEventReaderMap.insert(make_pair(NOTIFY_LOCAL_BATCHES, pair));

    makeSyncPair(SynchronizationDev, synchronization::NotifyOnline, &pair);
    deviceEventReaderMap.insert(make_pair(NOTIFY_ONLINE, pair));

    makeSyncPair(SynchronizationDev, synchronization::ReadBatches, &pair);
    deviceEventReaderMap.insert(make_pair(READ_BATCHES, pair));

    makeSyncPair(SynchronizationDev, synchronization::WriteBatch, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_BATCH, pair));

    makeSyncPair(SynchronizationDev, synchronization::WriteSynchronizationChange, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SYNCHRONIZATION_CHANGE, pair));

    makeSyncPair(Error, DtWrite, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_ERROR, pair));



    makeSyncPair(ProductDev, ProductRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COMMODITIES, pair));

    makeSyncPair(ProductDev, ProductRestoreExpirationLabel, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_EXPIRATION_LABEL, pair));

    makeSyncPair(ProductDefaultsDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_DEFAULTS, pair));

    makeSyncPair(ClassDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_CLASSES, pair));

    makeSyncPair(NutrifactsDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_NU_DATA, pair));

    makeSyncPair(ExpandedTextDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_EXTEXT, pair));

    makeSyncPair(SpecialMessageDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SPECIAL_MSGS, pair));

    makeSyncPair(ProductNotesDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_NOTES, pair));

    makeSyncPair(MarqueeTextDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_MARQUEES, pair));

    makeSyncPair(TransactionTypeDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_TOT_NAME, pair));

    makeSyncPair(OperatorDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_OP_IDS, pair));

    makeSyncPair(OperatorDev, OpRestoreNote, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_OPERATOR_NOTES, pair));

    /*  makeSyncPair(OperatorDev, OpRestoreOpChange, &pair);
        deviceEventReaderMap.insert(make_pair(WRITE_OP_CHANGES, pair));
        */
    makeSyncPair(ConfigurationDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_CONFIG_ITEMS, pair));

    makeSyncPair(ConfigurationDev, CfgStoreInfoRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_STORE_NAME, pair));

    makeSyncPair(GraphicDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_USER_GRAPHICS, pair));

    makeSyncPair(FontDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_USER_FONTS, pair));

    makeSyncPair(LabelTypeDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_LAB_FORM, pair));

    makeSyncPair(GenericTextDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_GEN_TEXT, pair));

    makeSyncPair(CategoryDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_CATEGORIES, pair));

    makeSyncPair(SpecialsDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SPECIALS, pair));

    makeSyncPair(TransactionDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_ITEM_TRANS, pair));

#ifndef NOTSELFSERVICE
    makeSyncPair(PresentationMethodDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_PRESENTATION_METHODS, pair));

    makeSyncPair(StreamingGraphicDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_STREAMING_GRAPHIC_INFO, pair));

    makeSyncPair(ProductStatisticsDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_STATISTICS, pair));
#endif

    makeSyncPair(GraphicDev, RestoreProductGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_PRODUCT_GRAPHICS, pair));

    makeSyncPair(GraphicDev, RestoreCategoryGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_CATEGORY_GRAPHICS, pair));

    makeSyncPair(GraphicDev, RestoreSpecialsGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SPECIALS_GRAPHICS, pair));

    makeSyncPair(GraphicDev, RestoreStreamingGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_STREAMING_GRAPHICS, pair));

    makeSyncPair(GraphicDev, RestoreSoftkeyGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_GRAPHICS, pair));

    makeSyncPair(EmailAlertsDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_EMAIL_ALERTS, pair));

    //  Start backwards compatibility for Ultima/Quantum
    makeSyncPair(COOLDev, COOLDevRestoreCoolText, &pair);
    deviceEventReaderMap.insert(make_pair(SM_WRITE_COOL_TEXT_RECORD, pair));

    makeSyncPair(COOLDev, COOLDevRestoreProductionStep, &pair);
    deviceEventReaderMap.insert(make_pair(SM_WRITE_CUSTOM_COOL_TEXT_RECORD, pair));

    makeSyncPair(COOLDev, COOLDevRestoreCountry, &pair);
    deviceEventReaderMap.insert(make_pair(SM_WRITE_COUNTRY_TEXT_RECORD, pair));

    makeSyncPair(COOLDev, COOLDevRestoreShortlistText, &pair);
    deviceEventReaderMap.insert(make_pair(SM_WRITE_SHORTLIST_RECORD, pair));


    makeSyncPair(COOLDev, COOLDevDeleteCoolText, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_COOL_TEXT_RECORD, pair));

    makeSyncPair(COOLDev, COOLDevDeleteProductionStep, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_CUSTOM_COOL_TEXT_RECORD, pair));

    makeSyncPair(COOLDev, COOLDevDeleteCountry, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_COUNTRY_TEXT_RECORD, pair));

    makeSyncPair(COOLDev, COOLDevDeleteShortlist, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_SHORTLIST_RECORD, pair));
    //  End backwards compatibility for Ultima/Quantum

    makeSyncPair(COOLDev, COOLDevRestoreCategory, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORIES, pair));

    makeSyncPair(COOLDev, COOLDevRestoreCountry, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_COUNTRIES, pair));

    makeSyncPair(COOLDev, COOLDevRestoreCategoryCountry, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORY_COUNTRIES, pair));

    makeSyncPair(COOLDev, COOLDevRestoreProductionStep, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_PROD_STEPS, pair));

    makeSyncPair(COOLDev, COOLDevRestoreCategoryProductionStep, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORY_PROD_STEPS, pair));

    makeSyncPair(COOLDev, COOLDevRestorePretext, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_PRETEXTS, pair));

    makeSyncPair(COOLDev, COOLDevRestoreCategoryPretext, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_CATEGORY_PRETEXTS, pair));

    makeSyncPair(COOLDev, COOLDevRestoreCoolText, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_TEXTS, pair));

    makeSyncPair(COOLDev, COOLDevRestoreShortlist, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_COOL_SHORTLIST_INFO, pair));

    makeSyncPair(SoftkeyDev, RestoreSoftkeyGroup, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_GROUP, pair));

    makeSyncPair(SoftkeyDev, RestoreSoftkeyStyle, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_STYLE, pair));

    makeSyncPair(SoftkeyDev, RestoreSoftkeyFeature, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY_FEATURE, pair));

    makeSyncPair(SoftkeyDev, RestoreSoftkey, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_SOFTKEY, pair));

    makeSyncPair(ProductDev, ProductRestoreExpirationLabel, &pair);
    deviceEventReaderMap.insert(make_pair(WRITE_EXPIRATION_LABEL, pair));

    //-------------------------------------------------------------------------------

    makeSyncPair(ProductDev, ProductDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COMMODITIES, pair));

    makeSyncPair(ClassDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_CLASSES, pair));

    makeSyncPair(ProductDev, ProductDeleteAllCommodityClass, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_CDTY_CLASS, pair));

    makeSyncPair(ExpandedTextDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_EXTEXT, pair));

    makeSyncPair(SpecialMessageDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SMSGS, pair));

    makeSyncPair(NutrifactsDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_NU_DATA, pair));

    makeSyncPair(ProductNotesDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_NOTES, pair));

    makeSyncPair(MarqueeTextDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_MARQUEES, pair));

    makeSyncPair(TransactionTypeDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_TOT_NAMES, pair));

    makeSyncPair(OperatorDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_OP_IDS, pair));

    makeSyncPair(OperatorDev, OpDeleteNote, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_OPERATOR_NOTES, pair));

    /*  makeSyncPair(OperatorDev, OpDeleteAllOpChanges, &pair);
        deviceEventReaderMap.insert(make_pair(DEL_ALL_OP_CHANGES, pair));
        */
    makeSyncPair(GraphicDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_USER_GRAPHICS, pair));

    makeSyncPair(FontDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_USER_FONTS, pair));

    makeSyncPair(LabelTypeDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_LAB_FORMS, pair));

    makeSyncPair(GenericTextDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_GEN_TEXT, pair));

    /*
        makeSyncPair(TransactionDev, ClearProductAndClassTotals, &pair);
        deviceEventReaderMap.insert(make_pair(CLEAR_PLU_CLASS_TOTALS, pair));

        makeSyncPair(TransactionDev, ClearOperatorTotals, &pair);
        deviceEventReaderMap.insert(make_pair(CLEAR_OP_TOTALS, pair));

        makeSyncPair(TransactionDev, ClearHourlyTotals, &pair);
        deviceEventReaderMap.insert(make_pair(CLEAR_HOURLY_TOTALS, pair));

        makeSyncPair(TransactionDev, ClearCOOLTotals, &pair);
        deviceEventReaderMap.insert(make_pair(SM_CLEAR_COOL_TOTALS, pair));

        makeEvolPair(TransactionDev, ClearAllTotals, &pair);
        deviceEventReaderMap.insert(make_pair(CLEAR_ALL_TOTALS, pair));
    */
    makeSyncPair(CategoryDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_CATEGORIES, pair));

    makeSyncPair(SpecialsDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SPECIALS, pair));

#ifndef NOTSELFSERVICE
    makeSyncPair(PresentationMethodDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_PRESENTATION_METHODS, pair));

    makeSyncPair(StreamingGraphicDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_STREAMING_GRAPHIC_INFO, pair));

    makeSyncPair(ProductStatisticsDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_STATISTICS, pair));
#endif

    makeSyncPair(GraphicDev, DeleteAllProductGraphics, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_GRAPHICS, pair));

    makeSyncPair(GraphicDev, DeleteAllCategoryGraphics, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_CATEGORY_GRAPHICS, pair));

    makeSyncPair(GraphicDev, DeleteAllSpecialsGraphics, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SPECIALS_GRAPHICS, pair));

    makeSyncPair(GraphicDev, DeleteAllStreamingGraphics, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_STREAMING_GRAPHICS, pair));

    makeSyncPair(GraphicDev, DeleteAllSoftkeyGraphics, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_GRAPHICS, pair));

    makeSyncPair(EmailAlertsDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_EMAIL_ALERTS, pair));

    //  Start backwards compatibility for Ultima/Quantum
    makeSyncPair(COOLDev, COOLDevDeleteAllCountryTextForUltima, &pair);         //      Eventually change to COOLDevDeleteAllCoolTextForUltima
    deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_COOL_TEXT, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllProductionSteps, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_CUSTOM_COOL_TEXT, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllCountries, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_COUNTRY_TEXT, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllShortlists, &pair);
    deviceEventReaderMap.insert(make_pair(SM_DEL_ALL_SHORTLIST, pair));
    //  End backwards compatibility for Ultima/Quantum


    makeSyncPair(COOLDev, COOLDevDeleteAllCategories, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORIES, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllCountries, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_COUNTRIES, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllCategoryCountries, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORY_COUNTRIES, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllProductionSteps, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_PROD_STEPS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllCategoryProductionSteps, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORY_PROD_STEPS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllPretexts, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_PRETEXTS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllCategoryPretexts, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_CATEGORY_PRETEXTS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllCoolText, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_TEXTS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllShortlists, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_SHORTLIST_INFO, pair));

    makeSyncPair(COOLDev, COOLDevDeleteAllShortlistTexts, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_COOL_LISTTEXTS, pair));


    makeSyncPair(SoftkeyDev, DeleteAllSoftkeyGroups, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_GROUPS, pair));

    makeSyncPair(SoftkeyDev, DeleteAllSoftkeyStyles, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_STYLES, pair));

    makeSyncPair(SoftkeyDev, DeleteAllSoftkeyFeatures, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEY_FEATURES, pair));

    makeSyncPair(SoftkeyDev, DeleteAllSoftkeys, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_SOFTKEYS, pair));

    makeSyncPair(ProductDev, ProductDeleteAllExpirationLabels, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_EXPIRATION_LABELS, pair));

    makeSyncPair(ProductDefaultsDev, DtDeleteAll, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_PRODUCT_DEFAULTS, pair));


    makeSyncPair(ConfigurationDev, CfgBatchApplyDeleteAllFiles, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_ALL_FILES, pair));


    //-------------------------------------------------------------------------------

    makeSyncPair(ProductDev, ProductBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COMMODITY, pair));

    makeSyncPair(ProductDev, ProductDeleteExpirationLabel, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_EXPIRATION_LABELS, pair));

    makeSyncPair(ClassDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_CLASS, pair));

    makeSyncPair(ExpandedTextDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_EXTEXT, pair));

    makeSyncPair(SpecialMessageDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SPCL_MSG, pair));

    makeSyncPair(NutrifactsDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_NU_DATA, pair));

    makeSyncPair(ProductNotesDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_PRODUCT_NOTE, pair));

    makeSyncPair(MarqueeTextDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_MARQUEE, pair));

    makeSyncPair(OperatorDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_OP_ID, pair));

    makeSyncPair(GraphicDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_USER_GRAPHIC, pair));

    makeSyncPair(LabelTypeDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_LABEL_FORM, pair));

    makeSyncPair(CategoryDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_CATEGORY, pair));

    makeSyncPair(SpecialsDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SPECIAL, pair));

#ifndef NOTSELFSERVICE
    makeSyncPair(StreamingGraphicDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_STREAMING_GRAPHIC_INFO, pair));

    makeSyncPair(GraphicDev, BatchApplyDeleteProductGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_PRODUCT_GRAPHIC, pair));

    makeSyncPair(GraphicDev, BatchApplyDeleteCategoryGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_CATEGORY_GRAPHIC, pair));

    makeSyncPair(GraphicDev, BatchApplyDeleteSpecialsGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SPECIALS_GRAPHIC, pair));

    makeSyncPair(GraphicDev, BatchApplyDeleteStreamingGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_STREAMING_GRAPHIC, pair));
#endif

    makeSyncPair(GraphicDev, BatchApplyDeleteSoftkeyGraphic, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_GRAPHIC, pair));

    makeSyncPair(EmailAlertsDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_EMAIL_ALERT, pair));


    makeSyncPair(COOLDev, COOLDevCategoryBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORIES, pair));

    makeSyncPair(COOLDev, COOLDevCountryBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_COUNTRIES, pair));

    makeSyncPair(COOLDev, COOLDevBatchApplyDeleteCategoryCountry, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORY_COUNTRIES, pair));

    makeSyncPair(COOLDev, COOLDevProductionStepBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_PROD_STEPS, pair));

    makeSyncPair(COOLDev, COOLDevBatchApplyDeleteCategoryProductionStep, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORY_PROD_STEPS, pair));

    makeSyncPair(COOLDev, COOLDevPretextBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_PRETEXTS, pair));

    makeSyncPair(COOLDev, COOLDevBatchApplyDeleteCategoryPretext, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_CATEGORY_PRETEXTS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteCoolText, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_TEXTS, pair));

    makeSyncPair(COOLDev, COOLDevDeleteShortlist, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_COOL_SHORTLIST_INFO, pair));


    makeSyncPair(SoftkeyDev, BatchApplyDeleteSoftkeyGroup, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_GROUP, pair));

    makeSyncPair(SoftkeyDev, BatchApplyDeleteSoftkeyStyle, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_STYLE, pair));

    makeSyncPair(SoftkeyDev, BatchApplyDeleteSoftkeyFeature, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY_FEATURE, pair));

    makeSyncPair(SoftkeyDev, BatchApplyDeleteSoftkey, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_SOFTKEY, pair));


    makeSyncPair(TransactionTypeDev, DtBatchApplyDelete, &pair);
    deviceEventReaderMap.insert(make_pair(DEL_TOT_NAME, pair));



    /* Generate a reversed ReaderMap so that we can look up function codes from
       a device-event pair. */
    functionCodesContainer_type::iterator readerMapIter;
    for ( readerMapIter = deviceEventReaderMap.begin(); readerMapIter != deviceEventReaderMap.end(); ++readerMapIter )
        deviceEventReversedReaderMap.insert(make_pair(readerMapIter->second, readerMapIter->first));
}


void CMReaderWriterSyncTag::initDeviceEventWriterMap()
{
    UINT16 pair;

    makeSyncPair(SessionDev, SnCommStart, &pair);
    deviceEventWriterMap.insert(make_pair(pair, COMM_START));

    makeSyncPair(SynchronizationDev, synchronization::RegisterSecondaryScale, &pair);
    deviceEventWriterMap.insert(make_pair(pair, REGISTER_SECONDARY_SCALE));

    makeSyncPair(SynchronizationDev, synchronization::NotifyLocalBatchesPresent, &pair);
    deviceEventWriterMap.insert(make_pair(pair, NOTIFY_LOCAL_BATCHES));

    makeSyncPair(SynchronizationDev, synchronization::NotifyOnline, &pair);
    deviceEventWriterMap.insert(make_pair(pair, NOTIFY_ONLINE));

    makeSyncPair(SynchronizationDev, synchronization::ReadBatches, &pair);
    deviceEventWriterMap.insert(make_pair(pair, READ_BATCHES));

    makeSyncPair(SynchronizationDev, synchronization::WriteBatch, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_BATCH));

    makeSyncPair(SynchronizationDev, synchronization::WriteSynchronizationChange, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SYNCHRONIZATION_CHANGE));

    makeSyncPair(TransactionDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_ITEM_TRANS));

    makeSyncPair(Error, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_ERROR));


    makeSyncPair(ProductDev, ProductWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COMMODITIES));

    makeSyncPair(ProductDev, ProductWriteExpirationLabel, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_EXPIRATION_LABEL));

    makeSyncPair(ProductDefaultsDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_DEFAULTS));

    makeSyncPair(ClassDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_CLASSES));

    makeSyncPair(ExpandedTextDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_EXTEXT));

    makeSyncPair(SpecialMessageDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SPECIAL_MSGS));

    makeSyncPair(NutrifactsDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_NU_DATA));

    makeSyncPair(MarqueeTextDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_MARQUEES));

    makeSyncPair(ProductNotesDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_NOTES));

    makeSyncPair(CategoryDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_CATEGORIES));

    makeSyncPair(SpecialsDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SPECIALS));

    makeSyncPair(TransactionTypeDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_TOT_NAME));


    makeSyncPair(OperatorDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_OP_IDS));

    makeSyncPair(OperatorDev, OpWriteNote, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_OPERATOR_NOTES));


    makeSyncPair(SoftkeyDev, WriteSoftkeyGroup, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_GROUP));

    makeSyncPair(SoftkeyDev, WriteSoftkeyStyle, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_STYLE));

    makeSyncPair(SoftkeyDev, WriteSoftkeyFeature, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_FEATURE));

    makeSyncPair(SoftkeyDev, WriteSoftkey, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY));


    makeSyncPair(COOLDev, COOLDevWriteCategory, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORIES));

    makeSyncPair(COOLDev, COOLDevWriteCountry, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_COUNTRIES));

    makeSyncPair(COOLDev, COOLDevWriteCategoryCountry, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORY_COUNTRIES));

    makeSyncPair(COOLDev, COOLDevWriteProductionStep, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_PROD_STEPS));

    makeSyncPair(COOLDev, COOLDevWriteCategoryProductionStep, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORY_PROD_STEPS));

    makeSyncPair(COOLDev, COOLDevWritePretext, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_PRETEXTS));

    makeSyncPair(COOLDev, COOLDevWriteCategoryPretext, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_CATEGORY_PRETEXTS));

    makeSyncPair(COOLDev, COOLDevWriteCoolText, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_TEXTS));

    makeSyncPair(COOLDev, COOLDevWriteShortlist, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_COOL_SHORTLIST_INFO));

    makeSyncPair(COOLDev, COOLDevWriteShortlistText, &pair);
    deviceEventWriterMap.insert(make_pair(pair, SM_WRITE_SHORTLIST_RECORD));


    makeSyncPair(EmailAlertsDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_EMAIL_ALERTS));

    makeSyncPair(GenericTextDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_GEN_TEXT));

    makeSyncPair(ConfigurationDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_CONFIG_ITEMS));

    makeSyncPair(ConfigurationDev, CfgStoreInfoWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_STORE_NAME));


    makeSyncPair(LabelTypeDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_LAB_FORM));


    makeSyncPair(GraphicDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_USER_GRAPHICS));

    makeSyncPair(GraphicDev, WriteProductGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_PRODUCT_GRAPHICS));

    makeSyncPair(GraphicDev, WriteCategoryGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_CATEGORY_GRAPHICS));

    makeSyncPair(GraphicDev, WriteSpecialsGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SPECIALS_GRAPHICS));

    makeSyncPair(GraphicDev, WriteStreamingGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_STREAMING_GRAPHICS));

    makeSyncPair(GraphicDev, WriteSoftkeyGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, WRITE_SOFTKEY_GRAPHICS));


    //---------------------------------------------------------------------------------

    makeSyncPair(ProductDev, ProductDeleteExpirationLabel, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_EXPIRATION_LABELS));

    makeSyncPair(ProductDev, ProductDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COMMODITY));

    makeSyncPair(ProductDev, ProductDeleteExpirationLabel, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_EXPIRATION_LABELS));

    makeSyncPair(ClassDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_CLASS));

    makeSyncPair(ExpandedTextDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_EXTEXT));

    makeSyncPair(SpecialMessageDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SPCL_MSG));

    makeSyncPair(NutrifactsDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_NU_DATA));

    makeSyncPair(ProductNotesDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_PRODUCT_NOTE));

    makeSyncPair(MarqueeTextDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_MARQUEE));

    makeSyncPair(OperatorDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_OP_ID));

    makeSyncPair(GraphicDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_USER_GRAPHIC));

    makeSyncPair(FontDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_USER_FONT));

    makeSyncPair(LabelTypeDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_LABEL_FORM));

    makeSyncPair(CategoryDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_CATEGORY));

    makeSyncPair(SpecialsDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SPECIAL));

#ifndef NOTSELFSERVICE
    makeSyncPair(PresentationMethodDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_PRESENTATION_METHOD));

    makeSyncPair(StreamingGraphicDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_STREAMING_GRAPHIC));

    makeSyncPair(ProductStatisticsDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_PRODUCT_STATISTIC));
#endif

    makeSyncPair(GraphicDev, DeleteProductGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_PRODUCT_GRAPHIC));

    makeSyncPair(GraphicDev, DeleteCategoryGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_CATEGORY_GRAPHIC));

    makeSyncPair(GraphicDev, DeleteSpecialsGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SPECIALS_GRAPHIC));

    makeSyncPair(GraphicDev, DeleteStreamingGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_STREAMING_GRAPHIC));

    makeSyncPair(GraphicDev, DeleteSoftkeyGraphic, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SOFTKEY_GRAPHIC));

    makeSyncPair(EmailAlertsDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_EMAIL_ALERT));


    makeSyncPair(COOLDev, COOLDevDeleteCategory, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_CATEGORIES));

    makeSyncPair(COOLDev, COOLDevDeleteCountry, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_COUNTRIES));

    makeSyncPair(COOLDev, COOLDevDeleteCategoryCountry, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_CATEGORY_COUNTRIES));

    makeSyncPair(COOLDev, COOLDevDeleteProductionStep, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_PROD_STEPS));

    makeSyncPair(COOLDev, COOLDevDeleteCategoryProductionStep, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_CATEGORY_PROD_STEPS));

    makeSyncPair(COOLDev, COOLDevDeletePretext, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_PRETEXTS));

    makeSyncPair(COOLDev, COOLDevDeleteCategoryPretext, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_CATEGORY_PRETEXTS));

    makeSyncPair(COOLDev, COOLDevDeleteCoolText, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_TEXTS));

    makeSyncPair(COOLDev, COOLDevDeleteShortlist, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_COOL_SHORTLIST_INFO));


    makeSyncPair(SoftkeyDev, DeleteSoftkeyGroup, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SOFTKEY_GROUP));

    makeSyncPair(SoftkeyDev, DeleteSoftkeyStyle, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SOFTKEY_STYLE));

    makeSyncPair(SoftkeyDev, DeleteSoftkeyFeature, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SOFTKEY_FEATURE));

    makeSyncPair(SoftkeyDev, DeleteSoftkey, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_SOFTKEY));


    makeSyncPair(TransactionTypeDev, DtDelete, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_TOT_NAME));


    //---------------------------------------------------------------------------------

    makeSyncPair(ProductDev, ProductDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COMMODITIES));

    makeSyncPair(ProductDev, ProductDeleteAllExpirationLabels, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_EXPIRATION_LABELS));

    makeSyncPair(ClassDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_CLASSES));

    makeSyncPair(ProductDev, ProductDeleteAllCommodityClass, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_CDTY_CLASS));

    makeSyncPair(ExpandedTextDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_EXTEXT));

    makeSyncPair(SpecialMessageDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SMSGS));

    makeSyncPair(NutrifactsDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_NU_DATA));

    makeSyncPair(MarqueeTextDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_MARQUEES));

    makeSyncPair(ProductNotesDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_PRODUCT_NOTES));

    makeSyncPair(CategoryDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_CATEGORIES));

    makeSyncPair(SpecialsDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SPECIALS));

    makeSyncPair(TransactionTypeDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_TOT_NAMES));


    makeSyncPair(OperatorDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_OP_IDS));

    makeSyncPair(OperatorDev, OpDeleteNote, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_OPERATOR_NOTES));


    makeSyncPair(SoftkeyDev, DeleteAllSoftkeyGroups, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SOFTKEY_GROUPS));

    makeSyncPair(SoftkeyDev, DeleteAllSoftkeyStyles, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SOFTKEY_STYLES));

    makeSyncPair(SoftkeyDev, DeleteAllSoftkeyFeatures, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SOFTKEY_FEATURES));

    makeSyncPair(SoftkeyDev, DeleteAllSoftkeys, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SOFTKEYS));


    makeSyncPair(COOLDev, COOLDevDeleteAllCategories, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_CATEGORIES));

    makeSyncPair(COOLDev, COOLDevDeleteAllCountries, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_COUNTRIES));

    makeSyncPair(COOLDev, COOLDevDeleteAllCategoryCountries, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_CATEGORY_COUNTRIES));

    makeSyncPair(COOLDev, COOLDevDeleteAllProductionSteps, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_PROD_STEPS));

    makeSyncPair(COOLDev, COOLDevDeleteAllCategoryProductionSteps, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_CATEGORY_PROD_STEPS));

    makeSyncPair(COOLDev, COOLDevDeleteAllPretexts, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_PRETEXTS));

    makeSyncPair(COOLDev, COOLDevDeleteAllCategoryPretexts, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_CATEGORY_PRETEXTS));

    makeSyncPair(COOLDev, COOLDevDeleteAllCoolText, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_TEXTS));

    makeSyncPair(COOLDev, COOLDevDeleteAllShortlists, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_SHORTLIST_INFO));

    makeSyncPair(COOLDev, COOLDevDeleteAllShortlistTexts, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_COOL_LISTTEXTS));


    makeSyncPair(ProductDefaultsDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_PRODUCT_DEFAULTS));

    makeSyncPair(EmailAlertsDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_EMAIL_ALERTS));


    makeSyncPair(GenericTextDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_GEN_TEXT));

    makeSyncPair(LabelTypeDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_LAB_FORMS));

    makeSyncPair(GraphicDev, DtDeleteAll, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_USER_GRAPHICS));


    makeSyncPair(GraphicDev, DeleteAllProductGraphics, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_PRODUCT_GRAPHICS));

    makeSyncPair(GraphicDev, DeleteAllCategoryGraphics, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_CATEGORY_GRAPHICS));

    makeSyncPair(GraphicDev, DeleteAllSpecialsGraphics, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SPECIALS_GRAPHICS));

    makeSyncPair(GraphicDev, DeleteAllStreamingGraphics, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_STREAMING_GRAPHICS));

    makeSyncPair(GraphicDev, DeleteAllSoftkeyGraphics, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_SOFTKEY_GRAPHICS));


    makeSyncPair(ConfigurationDev, CfgDeleteAllFiles, &pair);
    deviceEventWriterMap.insert(make_pair(pair, DEL_ALL_FILES));


    makeSyncPair(SessionDev, SnCommEnd, &pair);
    deviceEventWriterMap.insert(make_pair(pair, COMM_END));}


void CMReaderWriterSyncTag::makeSyncPair(EvolDeviceType dev, UINT8 event, UINT16 *buff)
{
    if ( buff )
    {
        UINT8 *pU8 = (UINT8 *)buff;

        pU8[0] = event;
        pU8[1] = (UINT8)dev;
    }
}
