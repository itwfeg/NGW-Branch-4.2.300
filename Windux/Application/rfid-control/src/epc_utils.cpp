#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <functional>
#include <cstdio>
#include <string>
#include <stdexcept>
#include "Hex.h"
#include "AppHookLog.h"

// Constants and Enums
enum GtinDateType
{
    ProductionDate = 0,     // 0 = (11) production date
    PackagingDate = 1,      // 1 = (13) packaging date
    BestBeforeDate = 2,     // 2 = (15) best before date
    SellByDate = 3,         // 3 = (16) sell by date
    ExpirationDate = 4,     // 4 = (17) expiration date
    FirstFreezeDate = 5,    // 5 = (7006) first freeze date
    HarvestDate = 6         // 6 = (7007) harvest date
};

union rfid_epc
{
    uint64_t value;
    unsigned char raw[8];
};

// Renaming and unifying constant definitions
static const uint64_t DSGTIN_HEADER_MASK = 0xff;
static const uint64_t DSGTIN_HEADER_SHIFT = 1;

static const uint64_t DSGTIN_AIDC_MASK = 0x01;
static const uint64_t DSGTIN_AIDC_SHIFT = 3;

static const uint64_t DSGTIN_FILTER_MASK = 0x07;
static const uint64_t DSGTIN_FILTER_SHIFT = 4;

static const uint64_t DSGTIN_DATE_TYPE_MASK = 0x0f;
static const uint64_t DSGTIN_DATE_TYPE_SHIFT = 16;

static const uint64_t DSGTIN_DATE_MASK = 0xffff;
static const uint64_t DSGTIN_DATE_SHIFT = 32;

static const uint64_t DSGTIN_GTIN_MASK = 0xffffffffffffff;
static const uint64_t DSGTIN_GTIN_SHIFT = 3;

static const uint64_t DSGTIN_ENCODING_MASK = 0x07;
static const uint64_t DSGTIN_ENCODING_SHIFT = 5;

static const uint64_t DSGTIN_LENGTH_MASK = 0x1f;
static const uint64_t DSGTIN_LENGTH_SHIFT = 32;

static const uint64_t DSGTIN_SERIAL_MASK = 0xffffffff;

static const uint64_t SGTIN_HEADER_MASK = 0xff;
static const uint64_t SGTIN_HEADER_SHIFT = 3;

static const uint64_t SGTIN_FILTER_MASK = 0x07;
static const uint64_t SGTIN_FILTER_SHIFT = 3;

static const uint64_t SGTIN_PARTITION_MASK = 0x07;
static const uint64_t SGTIN_PARTITION_SHIFT = 24;

static const uint64_t SGTIN_COMPANY_MASK = 0xffffff;
static const uint64_t SGTIN_COMPANY_SHIFT = 20;

static const uint64_t SGTIN_REFERENCE_MASK = 0xfffff;
static const uint64_t SGTIN_REFERENCE_SHIFT = 6;

static const uint64_t SGTIN_SERIAL_MASK = 0x3fffffffff;
static const uint64_t SGTIN_SERIAL_SHIFT = 24;

/**
 * @param[in] storeNum  store number
 * @param[in] scaleNum  scale number
 * @param[in] transNum  transaction number
 * @param[in] printId  print id (i.e. 3 of 56 production prints)
 * @return  64 bit serial number
 */
uint64_t generate_serial_number(int storeNum, int scaleNum, int transNum = 0, int printId = 0)
{
    const auto now(std::chrono::system_clock::now());
    const auto timeSinceEpoch(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()));

    std::hash<std::string> hashAlgo;
    uint64_t hash(0);

    char buffer[128];

    std::snprintf(buffer, sizeof(buffer), "%04d%02d%06d%04d%016ld",
                  storeNum,
                  scaleNum,
                  transNum,
                  printId,
                  timeSinceEpoch.count());

    if (8 == sizeof(std::size_t))
        hash = hashAlgo(buffer);
    else if (4 == sizeof(std::size_t))
    {
        hash = timeSinceEpoch.count();
        hash <<= 32;
        hash |= hashAlgo(buffer);
    }

    return hash;
}

// Helper to print buffer
void printbuffer(const unsigned char *buffer, int len)
{
    for (int i(0); i < len; ++i)
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)buffer[i]);
    std::cout << '\n';
}

// Function to check if a string contains only numeric characters
bool isNumeric(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

// Function to calculate check digit based on GS1 standard
char calculateCheckDigit(const std::string &digits) {
    //Log::logger().info("Calculating check digit for: {}", digits);

    int sum = 0;
    int length = digits.length();
    for (int i = 0; i < length; ++i)
    {
        int digit = digits[i] - '0';
        if ((i % 2) == 0)
        {
            // Even index i (positions N1, N3, N5, ...), multiply by 3
            sum += digit * 3;
        }
        else
        {
            // Odd index i (positions N2, N4, N6, ...), multiply by 1
            sum += digit * 1;
        }
    }
    int checkDigit = (10 - (sum % 10)) % 10;
    //Log::logger().info("Calculated check digit: {}", checkDigit);
    return '0' + checkDigit; // Convert integer to character
}

// Function to convert UPC-A or EAN-13 to GTIN-14
std::string convertToGTIN14(const std::string &code)
{
    //Log::logger().info("Converting code to GTIN-14: {}", code);

    // Validate input
    if (!isNumeric(code))
    {
        Log::logger().error("Input code must contain only numeric characters.");
        throw std::invalid_argument("Input code must contain only numeric characters.");
    }

    std::string coreCode;

    // Determine if input is UPC-A or EAN-13
    if (code.length() == 12)
    {
        // UPC-A detected
        //Log::logger().info("Detected UPC-A code");
        coreCode = code.substr(0, 11); // Remove the check digit
    }
    else if (code.length() == 13)
    {
        // EAN-13 detected
        //Log::logger().info("Detected EAN-13 code");
        coreCode = code.substr(0, 12); // Remove the check digit
    }
    else
    {
        Log::logger().error("Invalid code length. Expected UPC-A (12 digits) or EAN-13 (13 digits).");
        throw std::invalid_argument("Invalid code length. Expected UPC-A (12 digits) or EAN-13 (13 digits).");
    }

    // Prepend zeros to make the length 13
    while (coreCode.length() < 13)
    {
        coreCode = '0' + coreCode;
    }

    // Calculate the new check digit using the full 13-digit code
    char newCheckDigit = calculateCheckDigit(coreCode);

    // Append the new check digit to form GTIN-14
    std::string gtin14 = coreCode + newCheckDigit;

    //Log::logger().info("Converted GTIN-14: {}", gtin14);

    return gtin14;
}

// Encode DSGTIN-128
void encode_dsgtin128(uint64_t gtin, GtinDateType dateType, uint64_t date, uint64_t serial, unsigned char *buffer16)
{
    uint64_t gtinh(gtin / 1000000);
    uint64_t gtinl(gtin % 1000000);

    rfid_epc epch;
    std::memset(&epch, 0, sizeof(epch));
    epch.value |= (0xfb & DSGTIN_HEADER_MASK); // DSGTIN+
    epch.value <<= DSGTIN_HEADER_SHIFT;
    epch.value |= (0 & DSGTIN_AIDC_MASK); // 0 = No additional data exists after the 128 bits
    epch.value <<= DSGTIN_AIDC_SHIFT;
    epch.value |= (1 & DSGTIN_FILTER_MASK); // 1 = Point of Sale (POS) Trade Item
    epch.value <<= DSGTIN_FILTER_SHIFT;

    epch.value |= (dateType & DSGTIN_DATE_TYPE_MASK);

    epch.value <<= 7;
    epch.value |= (date / 10000) % 100; // year (7 bits)
    epch.value <<= 4;
    epch.value |= (date / 100) % 100;  // month (4 bits)
    epch.value <<= 5;
    epch.value |= date % 100;          // day (5 bits)

    uint64_t gtinh_mask = 10000000;
    for (int i(0); i < 8; i++)
    {
        epch.value <<= 4;
        epch.value |= (gtinh / gtinh_mask) % 10;
        gtinh_mask /= 10;
    }

    rfid_epc epcl;
    std::memset(&epcl, 0, sizeof(epcl));

    uint64_t gtinl_mask = 100000;
    for (int i(0); i < 6; i++)
    {
        epcl.value <<= 4;
        epcl.value |= (gtinl / gtinl_mask) % 10;
        gtinl_mask /= 10;
    }

    epcl.value <<= DSGTIN_GTIN_SHIFT;

    epcl.value |= (1 & DSGTIN_ENCODING_MASK); // 1 = upper case hex
    epcl.value <<= DSGTIN_ENCODING_SHIFT;
    epcl.value |= (8 & DSGTIN_LENGTH_MASK);   // 8 chars
    epcl.value <<= DSGTIN_LENGTH_SHIFT;
    epcl.value |= (serial & DSGTIN_SERIAL_MASK);

    const int check(1);
    if (1 == *(char *)&check)
        for (int i(4); i--;)
        {
            std::swap(epch.raw[i], epch.raw[7 - i]);
            std::swap(epcl.raw[i], epcl.raw[7 - i]);
        }

    memcpy(&buffer16[0], &epch, 8);
    memcpy(&buffer16[8], &epcl, 8);
}

// Encode SGTIN-96
void encode_sgtin96(uint64_t company, uint64_t reference, uint64_t serial, unsigned char *buffer12)
{
    rfid_epc epch;
    std::memset(&epch, 0, sizeof(epch));
    epch.value |= (48 & SGTIN_HEADER_MASK); // SGTIN-96
    epch.value <<= SGTIN_HEADER_SHIFT;
    epch.value |= (1 & SGTIN_FILTER_MASK);  // 1 = Point of Sale (POS) Trade Item
    epch.value <<= SGTIN_FILTER_SHIFT;
    epch.value |= (5 & SGTIN_PARTITION_MASK); // 5 = 24 bits company, 20 bits item/reference code
    epch.value <<= SGTIN_PARTITION_SHIFT;
    epch.value |= (company & SGTIN_COMPANY_MASK);
    epch.value <<= SGTIN_COMPANY_SHIFT;
    epch.value |= (reference & SGTIN_REFERENCE_MASK);
    epch.value <<= SGTIN_REFERENCE_SHIFT;

    rfid_epc epcl;
    memset(&epcl, 0, sizeof(epcl));
    epcl.value |= (serial & SGTIN_SERIAL_MASK);
    epcl.value <<= SGTIN_SERIAL_SHIFT;

    const int check(1);
    if (1 == *(char *)&check)
        for (int i(4); i--;)
        {
            std::swap(epch.raw[i], epch.raw[7 - i]);
            std::swap(epcl.raw[i], epcl.raw[7 - i]);
        }

    memcpy(&buffer12[0], &epch, 8);
    memcpy(&buffer12[7], &epcl, 5);
    buffer12[7] |= (epch.raw[7] & 0xC0);
}

// Hexadecimal to Decimal conversion (existing function from your code)
std::string hexadecimalToDecimal(const unsigned char *hexVal, int hexValLen)
{
    int base = 1;
    long long dec_val = 0;
    for (int i = hexValLen - 1; i >= 0; i--)
    {
        unsigned char cc = *(hexVal + i);
        if (*(hexVal + i) >= '0' && *(hexVal + i) <= '9')
        {
            dec_val += (int(*(hexVal + i)) - 48) * base;
            base = base * 16;
        }
        else if (*(hexVal + i) >= 'A' && *(hexVal + i) <= 'F')
        {
            dec_val += (int(*(hexVal + i)) - 55) * base;
            base = base * 16;
        }
    }
    return std::to_string(dec_val);
}

// Test function for GTIN conversion
void testGTINConversion()
{
    Log::logger().info("Starting GTIN conversion test");

    try
    {
        // Test UPC-A to GTIN-14
        std::string upcA = "012345678905";
        Log::logger().info("Testing UPC-A to GTIN-14 conversion with UPC-A: {}", upcA);

        std::string gtin14FromUPC = convertToGTIN14(upcA);
        Log::logger().info("GTIN-14 from UPC-A: {}", gtin14FromUPC);

        // Test EAN-13 to GTIN-14
        std::string ean13 = "0123456789012";
        Log::logger().info("Testing EAN-13 to GTIN-14 conversion with EAN-13: {}", ean13);

        std::string gtin14FromEAN = convertToGTIN14(ean13);
        Log::logger().info("GTIN-14 from EAN-13: {}", gtin14FromEAN);

        // Test with invalid inputs
        Log::logger().info("Testing with invalid inputs");

        try
        {
            convertToGTIN14("1234567890"); // Too short
        }
        catch (const std::exception &e)
        {
            Log::logger().error("Error (expected): {}", e.what());
        }

        try
        {
            convertToGTIN14("123456789012345"); // Too long
        }
        catch (const std::exception &e)
        {
            Log::logger().error("Error (expected): {}", e.what());
        }

    }
    catch (const std::exception &e)
    {
        Log::logger().error("Unexpected error: {}", e.what());
    }

    Log::logger().info("GTIN conversion test completed");
}
