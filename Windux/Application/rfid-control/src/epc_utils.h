#ifndef EPC_UTILS_H
#define EPC_UTILS_H

#include <string>
#include <cstdint>
#include <chrono>
#include <functional>
#include <stdexcept>

// Common macros
#define EPC_HEADER_MASK 0xff
#define EPC_FILTER_MASK 0x07
#define EPC_FILTER_SHIFT 3
#define EPC_HEADER_SHIFT 3
#define EPC_SERIAL_SHIFT 24

// DSGTIN128 specific macros
#define EPC_DSGTIN_HEADER_SHIFT 1
#define EPC_DSGTIN_FILTER_SHIFT 4
#define EPC_AIDC_MASK 0x01
#define EPC_AIDC_SHIFT 3
#define EPC_DATE_TYPE_MASK 0x0f
#define EPC_DATE_TYPE_SHIFT 16
#define EPC_DATE_MASK 0xffff
#define EPC_DATE_SHIFT 32
#define EPC_GTIN_MASK 0xffffffffffffff
#define EPC_GTIN_SHIFT 3
#define EPC_ENCODING_MASK 0x07
#define EPC_ENCODING_SHIFT 5
#define EPC_LENGTH_MASK 0x1f
#define EPC_LENGTH_SHIFT 32
#define EPC_SERIAL_MASK 0xffffffff

// SGTIN96 specific macros
#define EPC_SGTIN_HEADER_SHIFT 3
#define EPC_SGTIN_FILTER_SHIFT 3
#define EPC_PARTITION_MASK 0x07
#define EPC_PARTITION_SHIFT 24
#define EPC_COMPANY_MASK 0xffffff
#define EPC_COMPANY_SHIFT 20
#define EPC_REFERENCE_MASK 0xfffff
#define EPC_REFERENCE_SHIFT 6
#define EPC_SGTIN_SERIAL_MASK 0x3fffffffff
#define EPC_SGTIN_SERIAL_SHIFT 24

// Enum for GTIN date types
enum GtinDateType
{
    ProductionDate = 0,
    PackagingDate = 1,
    BestBeforeDate = 2,
    SellByDate = 3,
    ExpirationDate = 4,
    FirstFreezeDate = 5,
    HarvestDate = 6
};

// Union for EPC storage
union rfid_epc
{
    uint64_t value;
    unsigned char raw[8];
};

// Function declarations
uint64_t generate_serial_number(int storeNum, int scaleNum, int transNum = 0, int printId = 0);
void printbuffer(const unsigned char *buffer, int len);
bool isNumeric(const std::string &s);
char calculateCheckDigit(const std::string &digits);
std::string convertToGTIN14(const std::string &code);
void encode_dsgtin128(uint64_t gtin, GtinDateType dateType, uint64_t date, uint64_t serial, unsigned char *buffer16);
void encode_sgtin96(uint64_t company, uint64_t reference, uint64_t serial, unsigned char *buffer12);
std::string hexadecimalToDecimal(const unsigned char *hexVal, int hexValLen);
void testGTINConversion();

#endif // EPC_UTILS_H