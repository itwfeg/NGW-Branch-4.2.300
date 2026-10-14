
#include "IntelHexParser.h"
#include "HobTokenizer.h"

#include "Debug.h"
#include "Hex.h"
#include "HobString.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int IntelHexParser::dataType = 0;
std::string IntelHexParser::dataTypeName("IntelHexParser"); 
#endif

IntelHexParser::IntelHexParser() :	filename("")
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    recordIter = hexRecords.begin();
    dataIter = hexData.begin();

}

IntelHexParser::IntelHexParser(HobString &filename)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    this->filename = filename;

    parseFile();

    recordIter = hexRecords.begin();
    dataIter = hexData.begin();

}

IntelHexParser::IntelHexParser(const char *filename)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    this->filename = HobString(filename);

    parseFile();

    recordIter = hexRecords.begin();
    dataIter = hexData.begin();

}

IntelHexParser::~IntelHexParser()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

    recordIter = hexRecords.begin();
    while (recordIter != hexRecords.end())
    {
        IntelHexRecord *pIHR = *recordIter;
        recordIter = hexRecords.erase(recordIter);

        delete pIHR;
    }

}

void IntelHexParser::setFilename(HobString &filename)
{
    this->filename = filename;

    parseFile();

    recordIter = hexRecords.begin();
    dataIter = hexData.begin();
}

UINT16 IntelHexParser::getNumRecords() const
{
    return (UINT16)hexRecords.size();
}

bool IntelHexParser::getFirstRecord(INTEL_HEX_RECORD &hexRecord)
{
    bool rv = true;

    if (hexRecords.size() > 0)
    {
        recordIter = hexRecords.begin();
        dataIter = hexData.begin();

        hexRecord.Length = (*recordIter)->Length;
        hexRecord.Address = (*recordIter)->Address;
        hexRecord.Type = (*recordIter)->Type;

        (*dataIter).copy((char *) hexRecord.Data, sizeof(hexRecord.Data));

        recordIter++;
        dataIter++;
    }
    else
    {
        rv = false;
    }

    return rv;
}

bool IntelHexParser::getNextRecord(INTEL_HEX_RECORD &hexRecord)
{
    bool rv = true;

    if ((hexRecords.size() > 0) && (recordIter != hexRecords.end()))
    {
        hexRecord.Length = (*recordIter)->Length;
        hexRecord.Address = (*recordIter)->Address;
        hexRecord.Type = (*recordIter)->Type;

        (*dataIter).copy((char *) hexRecord.Data, sizeof(hexRecord.Data));

        recordIter++;
        dataIter++;
    }
    else
    {
        rv = false;
    }

    return rv;
}






void IntelHexParser::parseFile()
{
#if defined(LINUX_FIX)
    // Alamy: Rewrite with C++ standard
    // Fetch filesize
    ifstream::pos_type  file_size = 0;
    ifstream myFile(const_cast<char *> (filename.c_str()), ios::in|ios::ate);   // Open and seek to end.
    if ( !(myFile.is_open()) ) 
	{
        return;
    }
    file_size = myFile.tellg();
    myFile.seekg(0, ios::beg);

    // Read in whole file
    CHAR *fileBuffer = new char [file_size];
    myFile.read( fileBuffer, file_size );

    // Parse the file
    //if (myFile.gcount() == file_size)
    //{
        HobString h1( fileBuffer, file_size );

        UINT16 recordNum = 0;
        HobTokenizer asciiRecords(const_cast<const HobString *> (&h1), ':');
        HobString oneRecord;

        while (asciiRecords.next(&oneRecord) != -1)
        {
            if ((recordNum % 100) == 0)
            {
                PAL_absclass::PAL_sleep( 0 );
            }

            if (parseRecord(oneRecord))
			{
				break;
			}
			
            recordNum++;
        }
    //}

    // Release resource
    delete [] fileBuffer;
    myFile.close();

#else
    WIN32_FILE_ATTRIBUTE_DATA attr_data;
    if ( GetFileAttributesExA(  filename.c_str(), GetFileExInfoStandard, &attr_data ) > 0 && attr_data.nFileSizeHigh == 0  )
    {
		Debug::trace("IntelHexParser.parseFile", HobString("IntelHexParser:opened file"));
        Debug::trace("IntelHexParser.parseFile", filename.c_str());
        // The size of the file in bytes is in results.st_size
        ifstream myFile(const_cast<char *> (filename.c_str()), ios::in);
        UINT8 *fileBuffer = new UINT8[attr_data.nFileSizeLow];

		memset(fileBuffer, 0, attr_data.nFileSizeLow);
		myFile.seekg(0, ios::beg);
        myFile.read( (char * ) fileBuffer, attr_data.nFileSizeLow);
/*not sure why the file size never agrees? removed look into later! -- ats
		if (myFile.gcount() == attr_data.nFileSizeLow)
*/	
//		{

            HobString h1((const char *) fileBuffer, attr_data.nFileSizeLow);

            UINT16 recordNum = 0;
            HobTokenizer asciiRecords(const_cast<const HobString *> (&h1), ':');
            HobString oneRecord;

            while (asciiRecords.next(&oneRecord) != -1)
            {
                if ((recordNum % 100) == 0)
                {
                    Sleep(0);
                }

				if(parseRecord(oneRecord))
				{
					break;
				}
                recordNum++;
            }
//		}
		Debug::trace("IntelHexParser.parseFile", HobString("IntelHexParser:close file"));

        delete [] fileBuffer;
        myFile.close();
    }
	else
	{
		Debug::trace("IntelHexParser.parseFile", "IntelHexParser: failed to open file");
	}

#endif  // LINUX_FIX
}



bool IntelHexParser::parseRecord(HobString &hexRecord)
{
    UINT8 checksum = 0;
	bool eofRecord = false;

    if (hexRecord.size() > 0)
    {
        IntelHexRecord *pIHR = new IntelHexRecord;

        int i = 0;
        for (i = 0; i < 8; i+=2)
        {
            HobString oneByte(hexRecord.substr(i,2));
            unsigned char val = (unsigned char)Hex::valueOf(oneByte.c_str());
            checksum = checksum + val;

            switch (i)
            {
                case 0: pIHR->Length = val;
                        break;
                case 2: pIHR->Address = (val << 8);
                        break;
                case 4: pIHR->Address = pIHR->Address + val;
                        break;
                case 6: pIHR->Type = val;
                        break;
                default:
                        break;
            }
		}
		//are we at the end of the file?
		if(pIHR->Type == 0x01)	
		{
			eofRecord = true;
		}

        HobString stringData = "";
        for (i = 8; i < (8 + 2*pIHR->Length); i += 2)
        {
            HobString oneByte(hexRecord.substr(i,2));
            unsigned char val = (unsigned char)Hex::valueOf(oneByte.c_str());
            checksum = checksum + val;

            stringData.append(1,val);
        }

        Debug::trace("IntelHexParser.parseRecord", HobString("stringData: ") + HobString(Hex::toString(stringData.c_str())));

        HobString oneByte(hexRecord.substr(i,2));
        unsigned char val = (unsigned char)Hex::valueOf(oneByte.c_str());

        checksum = (~checksum) + 1; // 2's complement

        if (val == checksum)
        {
            hexRecords.push_back(pIHR);
            hexData.push_back(stringData);
        }
        else
        {
            Debug::trace("IntelHexParser.Error", "Checksum does not match!!");
            Debug::trace("IntelHexParser.Error", HobString("recorded   checksum: 0x") + HobString(Hex::toString(val)));
            Debug::trace("IntelHexParser.Error", HobString("calculated checksum: 0x") + HobString(Hex::toString(checksum)));
        }
    }
	return(eofRecord);
}
