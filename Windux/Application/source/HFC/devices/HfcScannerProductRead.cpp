/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcScannerProductRead.h"
#include "BarcodeScannerTags.h"
#include "Configuration.h"
#include "Device.h"
#include "Printer.h"
#include "CMReaderWriterEvolTag.h"
#include "CMReaderWriterDBIndependent.h"


#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "util/e_string/utilstring.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBBarcodeScannersRM.h"


using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcScannerProductRead::dataType = 0;
string HfcScannerProductRead::dataTypeName("HfcScannerProductRead"); 
#endif

list<BarcodeScannerFieldOffsets> HfcScannerProductRead::fieldList;

HfcScannerProductRead::HfcScannerProductRead(const HfcScannerProductRead& obj): ScannerProductRead(obj), HfcObject(obj),
    codeDisplay(obj.codeDisplay)
{


#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

	Debug::trace("HfcScannerProductRead", "Constructor 1: " );
}

HfcScannerProductRead::HfcScannerProductRead() : ScannerProductRead()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();
	Debug::trace("HfcScannerProductRead", "Constructor 2: " );
}


HfcScannerProductRead::HfcScannerProductRead(const char *data) : ScannerProductRead(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	// WHAT IS (data) HERE?
	//serialize(InternalFormat);
	
	std::string strbuf(data, (MAX_SCANNED_HEADER_STRING + MAX_SCANNED_CODE_STRING) );
	std::string scancode_ = strbuf.substr(MAX_SCANNED_HEADER_STRING, MAX_SCANNED_CODE_STRING);	// get the code off the tail
	setScannerCode(scancode_);
	Debug::trace("HfcScannerProductRead", "Constructor 3: " );
}

// When I get here, I already have a code passed up from the RTC layer.  If I extend ScannerProductRead, it creates a new instance of it
// and I lose my scancode_ because in the new instance, it just has the header file value, [blank] and has lost the actual scanned code

//HfcScannerProductRead::HfcScannerProductRead(const char *data, int len, int formatType) : ScannerProductRead()

HfcScannerProductRead::HfcScannerProductRead(const char *data, int len, int formatType) : ScannerProductRead()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// This constructor gets called by the if else conditional where if (INTERNAL) do the data route, else go this route
	
	init();
	
	int offset = 0;

	switch (formatType)
	{
		case InternalFormat:
			
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetBCScannerCode:
						setScannerCode(token);
						break;
					case fieldOffsetBCScannerProductNum:
						setScannerProductNum((HobRecordNum)stringtolong(token));
						break;
					//case fieldOffsetBCScannerDescription:
					//	setScannerDescription(token);
					//	break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			BarcodeScannerTags BarcodeScannerTags;
			
			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
			  	 	temp = token->getTag();
					offset = (temp.size()) ? BarcodeScannerTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetBCScannerCode:
							  	setScannerCode(temp);
								break;
							case fieldOffsetBCScannerProductNum:
							  	setScannerProductNum((HobRecordNum)stringtolong(temp));
								break;
							//case fieldOffsetBCScannerDescription:
							//	setScannerDescription(temp);
							//	break;
							default:
								break;
						}
					}
				}	
				delete token;
			}
			break;
		}
		default:
			// todo - throw exception("invalid format");
			break;
	}

	
	Debug::trace("HfcScannerProductRead", "Constructor 4: scancode_" + scancode_);
}

HfcScannerProductRead::HfcScannerProductRead(const HfcScannerProductRead *hspr) : ScannerProductRead()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();

	// what is the scancode here?
    setScannerCode( hspr->getScannerCode() ); 

	Debug::trace("HfcScannerProductRead", "Constructor 5: scancode_" + scancode_);
}

HfcScannerProductRead::HfcScannerProductRead(HobString code) : ScannerProductRead()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// dont know where this constructor comes from. RFID?
	init();
	scancode_ = code;
	Debug::trace("HfcScannerProductRead", "Constructor 6: scancode_" + scancode_);
}

HfcScannerProductRead::HfcScannerProductRead(edb::DBBarcodeScanner *dbBarcodeScannerRec, DBAccess *access) : ScannerProductRead()
{
	// Is this method even ever being used?

	init();
	dbAccess_ = access;

	if (dbBarcodeScannerRec)
	{
		myDbRecord_ = dbBarcodeScannerRec;

		scancode_ = dbBarcodeScannerRec->getScannerCode();
		scanProductNum_ = (HobRecordNum) stringtoint(dbBarcodeScannerRec->getScannerProductNum());

		errorCode_ = RECORD_OK;
	}
	else
		Debug::trace("HfcScannerProductRead", HobString("Constructor passing in a db - dbrec IS NULL"));

	// WHO calls this?
	Debug::trace("HfcScannerProductRead", "Constructor 7: scancode_" + scancode_);
}

HfcScannerProductRead::~HfcScannerProductRead()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord_)
		delete myDbRecord_;
}

void HfcScannerProductRead::init()
{
	//scancode_ = "";
	scanProductNum_ = RECORD_NOT_FOUND;
	scanDescription_ = "";

	dbAccess_ = 0;
	myDbRecord_ = 0;

	errorCode_ = RECORD_NOT_FOUND;

	initFieldList();
}


HfcScannerProductRead *HfcScannerProductRead::findBarcodeScanner(HobString scancode, DBAccess *dbAccess)
{
	HfcScannerProductRead *foundRec = 0;

	if (scancode.length() <= 0 || !dbAccess)
		return foundRec;

	DBBarcodeScannersRM	*myRM = dbAccess->getDBBarcodeScannersRM();

	if (!myRM)
		return foundRec;

	try
	{
		// Use this to determine if we need to trim the string which is possible with codes < 16 digits
		const char * char_code = scancode.c_str();
		size_t code_len = strlen(char_code); 
		if(code_len < 16)
			scancode = scancode.substr(0, code_len);

		DBBarcodeScanner *dbRec = myRM->findNumber(scancode);
		if (dbRec)
		{
			if (Debug::isTracing("HfcScannerProductRead"))
				Debug::trace("HfcScannerProductRead", "findBarcodeScanner() - read the record from the db, scancode:" + dbRec->getScannerCode());
			foundRec = new HfcScannerProductRead(dbRec, dbAccess);
			if (!foundRec)
				delete dbRec;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("HfcScannerProductRead::findBarcodeScanner() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return foundRec;
}


void HfcScannerProductRead::saveRecord()
{
	if (scancode_.length() > 0 && dbAccess_)   //No need to save to the Database if the number is 0
	{
		DBBarcodeScannersRM *myRM = dbAccess_->getDBBarcodeScannersRM();
		if (!myRM)
			return;

		if (!myDbRecord_)
		{
			//	This must be a new record for the Database
			Debug::trace("HfcScannerProductRead", HobString("saveRecord() - don't have a db record, creating it...  Scanner Code:") + scancode_ );
			myDbRecord_ = myRM->createRecord(scancode_);
		}
		
		if (myDbRecord_)
		{
			myDbRecord_->setScannerCode(scancode_);
			myDbRecord_->setScannerProductNum(longtostring(scanProductNum_));

			Debug::trace("HfcScannerProductRead", HobString("Saving  Scanner Code:") + scancode_);

			try
			{
				myRM->saveRecord(myDbRecord_);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "HfcScannerProductRead::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + inttostring(t) + HobString("  info:") + dbe.info());
			}
		}
	}
}


int HfcScannerProductRead::deleteRecord()
{
	int result = 0;
	if (myDbRecord_ && dbAccess_)   //No need to delete if there is no record
	{
		DBBarcodeScannersRM *myRM = dbAccess_->getDBBarcodeScannersRM();
		if (myRM)
		{
			Debug::trace("HfcScannerProductRead", HobString("Deleting HfcScannerProductRead: ") + ptrtostring(this) + string("  scancode_:") + scancode_);
			result = myRM->deleteRecord(myDbRecord_);
		}
	}
	return result;
}


void HfcScannerProductRead::updateBarcodeScanner(HfcScannerProductRead *newRec)
{
	if (!newRec)
		return;

	short loopCnt = 1;
	while (loopCnt < endBarcodeScannerFieldOffsets)
	{
		if (newRec->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetBCScannerCode:
					setScannerCode( newRec->getScannerCode() );
					break;
				case fieldOffsetBCScannerProductNum:
					setScannerProductNum( newRec->getScannerProductNum() );
					break;
				//case fieldOffsetBCScannerDescription:
				//	setScannerDescription( newRec->getScannerDescription() );
				//	break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *HfcScannerProductRead::serialize(int format) const
{
	HfcObject::serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case InternalFormat:
		{
			const char *data = ScannerProductRead::serialize();
            int datalen = ScannerProductRead::getSerializedLength();
            HfcObject::serializedData.assign(data, datalen);

			std::string strbuf_a(data, MAX_SCANNED_HEADER_STRING );
			std::string strbuf_b(data, (MAX_SCANNED_HEADER_STRING + MAX_SCANNED_CODE_STRING) );
			//std::string scancode_ = strbuf.substr(MAX_SCANNED_HEADER_STRING, MAX_SCANNED_CODE_STRING);	// get the code off the tail
            break;
		}
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)BarcodeScannerDev);
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)BarcodeReadScannerCode));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			
			HfcObject::serializedData.append(scancode_);
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(longtostring(scanProductNum_));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HobString stringForQuoting(scanDescription_);
			stringForQuoting.quoteForCSV();
			HfcObject::serializedData.append(stringForQuoting);	
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			BarcodeScannerTags BarcodeScannerTags;
			HobString tag = "";
			HobString fieldData = "";
			HobString header = "";

			if( format == EvolutionTaggedFormat )
			{
				header = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;									// Gets RT
				char rtData[10];																		// Create var
				CMReaderWriterEvolTag::determineRecordType(rtData, BarcodeScannerDev, DtWrite );		// Gets D4
				header.append(rtData);																	// Buikds RTD4
				header.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));						// Adds the unprintable separator
			}
			else if( format == DBIndependentFormatFormat )
			{
				header = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;									// Gets RT
				char rtData[10];																		// Create var
				CMReaderWriterDBIndependent::determineRecordType(rtData, BarcodeScannerDev, DtWrite );	// Gets D4
				header.append(rtData);																	// Buikds RTD4
				header.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));						// Adds the unprintable separator
			}

            // SOF-4663 Self-Serve: RTD4 records have data that shouldn't exist and are not deleted with Restore
            // just add the tag to the beginning the first time
			HfcObject::serializedData.append(header);
            
			std::list<BarcodeScannerFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = BarcodeScannerTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetBCScannerCode:
						fieldData = scancode_;
						break;
					case fieldOffsetBCScannerProductNum:
						fieldData = longtostring(scanProductNum_);
						break;
					default:
					{
						header = "";
						tag = "";
						fieldData = "";
						break;
					}
				}

				if (tag.size())
				{
                    // SOF-4663 Self-Serve: RTD4 records have data that shouldn't exist and are not deleted with Restore
                    // dont add RT tag between each field in the whole record.
					HfcObject::serializedData.append(tag);
					HfcObject::serializedData.append(fieldData);
					HfcObject::serializedData.append(UnitSeparator);
				}

				fieldIter++;
			}
			break;	
		}

		default:
			HfcObject::serializedData = HobString("HfcScannerProductRead::serialize - unknown format:") + inttostring(format);
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}


// static
void HfcScannerProductRead::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetBCScannerCode);
		fieldList.push_back(fieldOffsetBCScannerProductNum);	

		BarcodeScannerTags BarcodeScannerTags((FieldOffsetsList *)&fieldList);
	}
}

// We dont currently have default records so I am clearing this method out.  But keeping it in case we do
HobErrorCode HfcScannerProductRead::getErrorCode() const
{
	return errorCode_;
}

void HfcScannerProductRead::setErrorCode(HobErrorCode code)
{
	errorCode_ = code;
}

// came from the header - I hate these over there.
HobString HfcScannerProductRead::getScannerCode() const 
{ 
	return scancode_; 
}

void HfcScannerProductRead::setScannerCode(HobString code)
{ 
	scancode_ = code; 
}

HobRecordNum HfcScannerProductRead::getScannerProductNum() const
{
	return scanProductNum_;
}

void HfcScannerProductRead::setScannerProductNum(HobRecordNum pluNum)
{
	scanProductNum_ = pluNum;
}

HobString HfcScannerProductRead::getScannerDescription() const
{
	return scanDescription_;
}

void HfcScannerProductRead::setScannerDescription(HobString desc)
{
	scanDescription_ = desc;
}