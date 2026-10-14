#include <string>
#include <cstring>
#include "Hex.h"

#define EPC_HEADER_MASK 0xff
#define EPC_HEADER_SHIFT 3

#define EPC_FILTER_MASK 0x07
#define EPC_FILTER_SHIFT 3

#define EPC_PARTITION_MASK 0x07
#define EPC_PARTITION_SHIFT 24

#define EPC_COMPANY_MASK 0xffffff
#define EPC_COMPANY_SHIFT 20

#define EPC_REFERENCE_MASK 0xfffff
#define EPC_REFERENCE_SHIFT 6

#define EPC_SERIAL_MASK 0x3fffffffff
#define EPC_SERIAL_SHIFT 24

union rfid_epc
{
    uint64_t value;
    unsigned char raw[8];
};

using namespace std;

// Function to convert hexadecimal 
// to decimal 
string hexadecimalToDecimal(const unsigned char *hexVal, int hexValLen)
{
	// Initializing base value to 1, 
	// i.e 16^0 
	int base = 1;

	long long dec_val = 0;

	// Extracting characters as digits 
	// from last character 
	for (int i = hexValLen - 1; i >= 0; i--) {
		// If character lies in '0'-'9', 
		// converting it to integral 0-9 
		// by subtracting 48 from ASCII value 
		unsigned char cc = *(hexVal + i);
		int ccval = (int)cc;

		if ( *(hexVal+i) >= '0' && *(hexVal + i) <= '9' )
		{
			int newVal = int(*(hexVal + i));
			int newVal2 = newVal * base;
			dec_val += (int( *(hexVal + i) ) - 48) * base;

			// incrementing base by power 
			base = base * 16;
		}

		// If character lies in 'A'-'F' , converting 
		// it to integral 10 - 15 by subtracting 55 
		// from ASCII value 
		else if ( *(hexVal + i) >= 'A' && *(hexVal + i) <= 'F' )
		{
			int newVal = int(*(hexVal + i));
			int newVal2 = newVal * base;
			dec_val += (int( *(hexVal + i) ) - 55) * base;

			// Incrementing base by power 
			base = base * 16;
		}
	}
	return to_string(dec_val);
}

// companyData = 
void generateEpcRfid(const string barcodeStr, const string serialNumberStr, unsigned char *result)
{
	string epcBarcode("00000000000000");
	epcBarcode.append(barcodeStr);
	epcBarcode = epcBarcode.substr( epcBarcode.length() - 14, 14 );

	string companyDataStr = epcBarcode.substr(1, 7);
	string itemReferenceStr = epcBarcode.substr(0, 1) + epcBarcode.substr(8, 5);

	uint64_t companyData = stoi(companyDataStr);
	uint64_t itemReference = stoi(itemReferenceStr);
	uint64_t serialNumber = stoull(serialNumberStr);

    rfid_epc epch;
    memset( &epch, 0, sizeof(epch) );
    epch.value |= (48 & EPC_HEADER_MASK);
    epch.value <<= EPC_HEADER_SHIFT;
    epch.value |= (1 & EPC_FILTER_MASK);
    epch.value <<= EPC_FILTER_SHIFT;
    epch.value |= (5 & EPC_PARTITION_MASK);
    epch.value <<= EPC_PARTITION_SHIFT;
    epch.value |= (companyData & EPC_COMPANY_MASK);
    epch.value <<= EPC_COMPANY_SHIFT;
    epch.value |= (itemReference & EPC_REFERENCE_MASK);
    epch.value <<= EPC_REFERENCE_SHIFT;

    rfid_epc epcl;
    memset( &epcl, 0, sizeof(epcl) );
    epcl.value |= (serialNumber & EPC_SERIAL_MASK);
    epcl.value <<= EPC_SERIAL_SHIFT;

    // byteswap
    const int check( 1 );

    if ( 1 == *(char *)&check )
        for ( int i( 4 ); i--; )
        {
            std::swap( epch.raw[i], epch.raw[7-i] );
            std::swap( epcl.raw[i], epcl.raw[7-i] );
        }

	// Just a local var for checking the result of the above algorithm
    unsigned char buffer[12];
    memcpy( &buffer[0], &epch, 8 );
    memcpy( &buffer[7], &epcl, 5 );
    buffer[7] |= (epch.raw[7] & 0xC0);

	memcpy(result, &epch, 8);
	memcpy(result+7, &epcl, 5);
	*(result+7) |= (epch.raw[7] & 0xC0);

    // 96 bits of EPC data in buffer!

	string hexResultString;
	for (int i = 0; i++; i < 12)
		hexResultString.append( Hex::toString( (const unsigned char)buffer[i] ) );

	//return hexResultString;
    //return hexadecimalToDecimal( &buffer[0], 12 );
}

