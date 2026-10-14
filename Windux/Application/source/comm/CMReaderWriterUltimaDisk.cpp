/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductDevEvent.h"
#include "ConfigurationEvent.h"
#include "UltimaDiskEvent.h"
#include "COOLDevEvent.h"
#include "SoftkeyDevEvent.h"

#include "CMReaderWriterUltimaDisk.h"
#include "Integer.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CMReaderWriterUltimaDisk::_dataType_ = 0;
std::string CMReaderWriterUltimaDisk::_dataTypeName_("CMReaderWriterUltimaDisk");
#endif


using namespace std;


// statics
std::map< UINT16, UINT16 > CMReaderWriterUltimaDisk::deviceEventReaderMap;
std::map< UINT16, UINT16 > CMReaderWriterUltimaDisk::deviceEventWriterMap;


// private
CMReaderWriterUltimaDisk::CMReaderWriterUltimaDisk()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

    readerWriterId = CMReaderWriter::ULTIMA_DISK_ID;
    recordIdentifierLength = 2;

    CMReaderWriterUltimaDisk::initDeviceEventMaps();
}

CMReaderWriterUltimaDisk::~CMReaderWriterUltimaDisk()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif
}


int CMReaderWriterUltimaDisk::finalizeRecordForSending(char *buff, const char *msg, int length)
{
    memcpy(buff, msg, length);
    *(buff + length) = ULTIMA_DISK_RECORD_SEPARATOR;
    *(buff + length + 1) = (char)NULL;

    if (Debug::isTracing("CMReaderWriter.UltimaDisk"))
        Debug::trace("CMReaderWriter.UltimaDisk", HobString("finalizeRecordForSending - ") + HobString(Hex::toString(buff, length + 1)));
    return length + 1;
}


bool CMReaderWriterUltimaDisk::finishedReadingRecord(const char *msg, int length)
{
    return (length > 0 &&
            (*(msg + length - 1) == ULTIMA_DISK_RECORD_SEPARATOR || *(msg + length - 1) == ULTIMA_DISK_FILE_TERMINATOR))
        ? true : false;
}

bool CMReaderWriterUltimaDisk::finishedReadingRecord(const HobString &msg)
{
    return ((INT32) msg.size() > 0 &&
            (msg.at(msg.size() - 1) == ULTIMA_DISK_RECORD_SEPARATOR || msg.at(msg.size() - 1) == ULTIMA_DISK_FILE_TERMINATOR))
        ? true : false;
}

/*
 * Note: The msg buffer being passed in needs to be large enough to allow for this method
 * to prepend the previous RT tag and data onto the msg, if it doesn't contain it.
 */
int CMReaderWriterUltimaDisk::formatFromReceive(char *msg, int length, bool UNUSED(isRecordBeginningBuffer))
{
    UNREFERENCED_PARAMETER(isRecordBeginningBuffer);

    if (!msg || length <= 0)
        return 0;

    int finalLength = length;

    // First, remove the record separator from the received msg.
    if (*(msg + length - 1) == ULTIMA_DISK_RECORD_SEPARATOR || *(msg + length - 1) == ULTIMA_DISK_FILE_TERMINATOR)
    {
        *(msg + length - 1) = (char)NULL;
        finalLength -= 1;
    }

    return finalLength;
}

int CMReaderWriterUltimaDisk::formatFromReceive(HobString &msg)
{
    if (!msg.size())
        return 0;

    int finalLength = (int) msg.size();

    // First, remove the record separator from the received msg.
    if (msg.at(msg.size() - 1) == ULTIMA_DISK_RECORD_SEPARATOR || msg.at(msg.size() - 1) == ULTIMA_DISK_FILE_TERMINATOR)
    {
        msg.erase(msg.size() - 1);
        finalLength -= 1;
    }

    return finalLength;
}


void CMReaderWriterUltimaDisk::getRTData(const char *msg, char *rtData)
{
    if (!msg || !rtData)
        return;

    *rtData = *msg;
    *(rtData+1) = *(msg+1);
    *(rtData+2) = (char)NULL;
}


// static
bool CMReaderWriterUltimaDisk::canIdentifyBuffer(const char *msg)
{
    if (!msg || *msg != '3' || *(msg+1) != '0')
        return false;
    return true;
}


/* By passing in the data associated with the RT tag, identify the representative
 * device and event (as set up in Event.h) by looking up tagData (the key) in the deviceEventReaderMap.
 */
// static
void CMReaderWriterUltimaDisk::determineDeviceAndEvent(const char *recordTypeData, EvolDeviceType *device, UINT8 *event)
{
    *device = UnknownDevice;
    *event = 0;

    if (!recordTypeData || strlen(recordTypeData) != 2)
        return;

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
void CMReaderWriterUltimaDisk::determineRecordType(char *recordTypeData, EvolDeviceType device, int event)
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


void CMReaderWriterUltimaDisk::makeEvolPair(EvolDeviceType dev, UINT8 event, UINT16 *buff)
{
    if (buff)
    {
        UINT8 *pU8 = (UINT8 *) buff;

        pU8[0] = event;
        pU8[1] = (UINT8)dev;
    }
}


void CMReaderWriterUltimaDisk::initDeviceEventMaps()
{
    if (deviceEventReaderMap.empty())
    {
        initDeviceEventReaderMap();
        initDeviceEventWriterMap();
    }
}

void CMReaderWriterUltimaDisk::initDeviceEventWriterMap()
{
    /*
       std::map< UINT16, UINT16 >::iterator readerMapIter;
       for (readerMapIter = deviceEventReaderMap.begin(); readerMapIter != deviceEventReaderMap.end(); readerMapIter++)
       {
       deviceEventWriterMap.insert(make_pair(readerMapIter->second, readerMapIter->first));
       }
       */
    UINT16 pair;

    makeEvolPair(ProductDev, ProductWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3332));  // 32

    makeEvolPair(ClassDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3331));  // 31

    makeEvolPair(ExpandedTextDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3337));  // 37

    makeEvolPair(SpecialMessageDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3338));  // 38

    // We aren't supporting reading from/writing to Ultima/Quantum for Nutrifacts
    //makeEvolPair(NutrifactsDev, DtWrite, &pair);
    //deviceEventWriterMap.insert(make_pair(pair, 0x3334));  // 34

    makeEvolPair(MarqueeTextDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3333));  // 33

    makeEvolPair(TransactionTypeDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3439));  // 49

    makeEvolPair(OperatorDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3336));  // 36

    makeEvolPair(ConfigurationDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3632));  // 62

    makeEvolPair(ConfigurationDev, CfgStoreInfoWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3438));  // 48

    makeEvolPair(GenericTextDev, DtWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3530));  // 50

    makeEvolPair(UltimaDiskDev, HeaderWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3330));  // 30

    makeEvolPair(UltimaDiskDev, FooterWrite, &pair);
    deviceEventWriterMap.insert(make_pair(pair, 0x3430));  // 40

    // Graphics (FC 46), Fonts (FC 45), and LabelTypes (FC 47) in Ultima Disk format
    // are not supported for the Evolution.

    // COOLTextRecord (FC 65), COOLCountryTextRecord (FC 66), COOLCustomTextRecord (FC 67),
    // and COOLShortListRecord (FC 68) are not supported when writing data out in HobDisk
    // format.  We only support reading these function codes into the Evolution.

    // Flashkeys (FC 51) is not supported when writing data out in HobDisk
    // format.  We only support reading this function code into the Evolution.
}

void CMReaderWriterUltimaDisk::initDeviceEventReaderMap()
{
    UINT16 pair;

    makeEvolPair(ProductDev, ProductRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3332, pair));  // 32

    makeEvolPair(ClassDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3331, pair));  // 31

    makeEvolPair(ExpandedTextDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3337, pair));  // 37

    makeEvolPair(SpecialMessageDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3338, pair));  // 38

    // We aren't supporting reading from/writing to Ultima/Quantum for Nutrifacts
    //makeEvolPair(NutrifactsDev, DtRestore, &pair);
    //deviceEventReaderMap.insert(make_pair(0x3334, pair));  // 34

    makeEvolPair(MarqueeTextDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3333, pair));  // 33

    makeEvolPair(TransactionTypeDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3439, pair));  // 49

    makeEvolPair(OperatorDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3336, pair));  // 36

    makeEvolPair(ConfigurationDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3632, pair));  // 62

    makeEvolPair(ConfigurationDev, CfgStoreInfoRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3438, pair));  // 48

    makeEvolPair(GenericTextDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3530, pair));  // 50

    makeEvolPair(LabelTypeDev, DtRestore, &pair);
    deviceEventReaderMap.insert(make_pair(0x3437, pair));  // 47


    // Note: Both CoolText and CoolPretext are included with this function code.
    // The textType field (tag cq) indicates which type the record is.
    // We will treat this record as CoolText, and the CoolManager will check the
    // textType field of the CoolText HFC object to determine which restore method to call.
    makeEvolPair(COOLDev, COOLDevRestoreCoolText, &pair);
    deviceEventReaderMap.insert(make_pair(0x3635, pair));  // 65

    makeEvolPair(COOLDev, COOLDevRestoreCountry, &pair);
    deviceEventReaderMap.insert(make_pair(0x3636, pair));  // 66

    makeEvolPair(COOLDev, COOLDevRestoreProductionStep, &pair);
    deviceEventReaderMap.insert(make_pair(0x3637, pair));  // 67

    makeEvolPair(COOLDev, COOLDevRestoreShortlistForUltima, &pair);
    deviceEventReaderMap.insert(make_pair(0x3638, pair));  // 68


    makeEvolPair(SoftkeyDev, RestoreFlashkeyAsSoftkey, &pair);
    deviceEventReaderMap.insert(make_pair(0x3531, pair));  // 51


    makeEvolPair(UltimaDiskDev, HeaderWrite, &pair);
    deviceEventReaderMap.insert(make_pair(0x3330, pair));  // 30

    makeEvolPair(UltimaDiskDev, FooterWrite, &pair);
    deviceEventReaderMap.insert(make_pair(0x3430, pair));  // 40

    // Graphics (FC 46) and Fonts (FC 45) in Ultima Disk format
    // are not supported for the Evolution.

}

/*  defined in Quantum/Ultima ninclude\com_headers.h
    .Class                                              31
    Configuration (Block)       42
    .Expanded text                              37
    xSelf Service Text          54
    .Flashkey                                   51
    xFont                                                       45
    xGraphic                                            46
    .Item                                                       32
    xLabel                                              47
    Label Strings                               50
    .Marquee                                            33
    xMarquee time                               52
    .Nutrifacts                                 34
    .Operator                                   36
    .Special message                    38
    xSpeedkey                                   39
    Store                                                       48
    .Total Name                                 49
    .Header                                             30
    .Footer                                             40
    */
