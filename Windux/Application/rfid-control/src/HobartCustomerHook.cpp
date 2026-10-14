// HobartCustomerHook.cpp : Defines the exported functions for the DLL application.
#include "inih/ini.h"
#include "inih/cpp/INIReader.h"
#include "Config.h"
#include "epc_utils.h"
#include "Hex.h"

#include "AppHookLog.h"

#ifdef USE_GHC_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#define MAX_RFID_POWER      2400
#define MIN_RFID_POWER      0
#define POWER_SAVE_MODE_OFF 0
#define POWER_SAVE_MODE_ON  1
#define OUTPUT              1
#define INPUT               0
#define GPIO_PIN_COUNT      4

#if defined(WIN32)
#include "stdafx.h"
#endif

// Standard imports
#include <algorithm>
#include <array>
#include <cinttypes>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <limits.h>
#include <linux/usbdevice_fs.h>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <string.h>
#include <string>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <thread>
#include <utility>

// MercuryAPI imports
#include <tm_reader.h>
#include <tmr_utils.h>
#include <serial_reader_imp.h>

// Project imports
#include "ManufacturerDecoders.h"
#include "manufacturer_names.h"
#include "RFIDDecoder.h"
#include "epc_utils.h"

#include "HobartCustomerHook.h"


class FunctionProfiler
{
public:
    using Clock = std::chrono::steady_clock;

    FunctionProfiler()
        : m_callCount(0),
          m_lastTimeNs(0),
          m_minTimeNs(std::numeric_limits<uint64_t>::max()),
          m_maxTimeNs(0),
          m_totalTimeNs(0)
    {
    }

    void Start()
    {
        m_startTime = Clock::now();
    }

    void Stop()
    {
        auto end = Clock::now();

        uint64_t elapsed =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                end - m_startTime).count();

        m_lastTimeNs = elapsed;

        if (elapsed < m_minTimeNs)
            m_minTimeNs = elapsed;

        if (elapsed > m_maxTimeNs)
            m_maxTimeNs = elapsed;

        m_totalTimeNs += elapsed;
        ++m_callCount;
    }

    uint64_t LastNs() const
    {
        return m_lastTimeNs;
    }

    uint64_t MinNs() const
    {
        return (m_callCount == 0) ? 0 : m_minTimeNs;
    }

    uint64_t MaxNs() const
    {
        return m_maxTimeNs;
    }

    uint64_t AverageNs() const
    {
        return (m_callCount == 0)
            ? 0
            : m_totalTimeNs / m_callCount;
    }

    double LastUs() const
    {
        return m_lastTimeNs / 1000.0;
    }

    double LastMs() const
    {
        return LastUs() / 1000.0;
    }

    double LastS() const
    {
        return LastMs() / 1000.0;
    }

    double AverageUs() const
    {
        return AverageNs() / 1000.0;
    }

    double MinUs() const
    {
        return MinNs() / 1000.0;
    }

    double MaxUs() const
    {
        return MaxNs() / 1000.0;
    }

    uint64_t CallCount() const
    {
        return m_callCount;
    }

private:
    Clock::time_point m_startTime;

    uint64_t m_callCount;

    uint64_t m_lastTimeNs;
    uint64_t m_minTimeNs;
    uint64_t m_maxTimeNs;
    uint64_t m_totalTimeNs;
};

#define PRINT_TAG_METADATA 0
#define numberof(x) (sizeof((x))/sizeof((x)[0]))

constexpr auto UPGRADE_ACTION = "UPG";
constexpr auto WRITE_ACTION = "EPC";
constexpr auto CAL_ACTION = "CAL";
constexpr auto CAL_READ_ACTION = "CRP";
constexpr auto CAL_WRITE_ACTION = "CWP";
constexpr auto STATUS_ACTION = "STS";
constexpr auto QUERY_ACTION = "QRY";

constexpr auto FIRMWARE_PATH = "/usr/share/rfid-hook/";
constexpr auto FIRMWARE_EXT = ".sim";
constexpr auto RESULT_FAIL  = 1; /* For the RFID apphook, anything greater than 0 indicates a failure */
constexpr auto RESULT_SUCCESS = 0;

bool GPIOFlag = false;
bool GPIOInit = false;

int RSSIAvg = 0;
int readCountAvg = 0;
int writeFailureCount = 0;
int readFailureCount = 0;
int RFIDTransactionFailureCount = 0;
int RFIDTransactions = 0;
float RFIDFailureRate = 0.0;
bool readPowerCalCheckFlag = false;
std::thread::id heartbeatLEDThreadID;
std::atomic<bool> heartbeatStopFlag(false);
std::thread::id calibrateThreadID;
std::atomic<bool> calibrateStopFlag(false);
std::thread::id upgradeThreadID;
std::atomic<bool> upgradeStopFlag(false);
char initFailedCount = 0;


std::string readerURI = ""; 

class RFIDException : public std::runtime_error 
{
    public:
        RFIDException(const std::string& message) : std::runtime_error("RFID exception caught: " + message) {}

        const char* what() const noexcept override 
        {
            return std::runtime_error::what();
        }
};

struct Tag 
{
    std::string idStr;
    std::string timeStr;
    int readCount;
    int antenna;
    int rssi;
    int frequency;
    int phase;
    int protocol;
};

struct ReaderInfo
{
    std::string hardwareVersion;
    std::string serialNumber;
    std::string model;
    std::string firmwareVersion;
};

//CK cont read -
struct TagInfo
{
    std::string epc;
    int rssi;
    uint32_t readCount;
};

static std::vector<Tag> g_tagList;
bool tagReadEnoughTimes = false;
bool readErrorOccurred = false;

TMR_Reader RFIDReader;
ReaderInfo RFIDReaderInfo = {};
TMR_TransportListenerBlock RFIDTransportListenerBlock;
TMR_uint8List response;
TMR_ReadListenerBlock RFIDReadListenerBlock;
TMR_ReadExceptionListenerBlock RFIDReadExceptionListenerBlock;
TMR_TagOp writeOp, readOp;
FunctionProfiler runtimeProfiler;
FunctionProfiler calibrationProfiler;
FunctionProfiler calibrationTimingProfiler;
FunctionProfiler readCalibrationTimingProfiler;
FunctionProfiler writeCalibrationTimingProfiler;
bool calibrationInProgress = false;
bool upgradeInProgress = false;
//std::string calString = "FB1331370027874225800728E20FF988";
std::string calString               = "00000000000000000000000000000000";
std::string currentEPCDataToWrite   = "00000000000000000000000000000000";
bool isInitialized = false;
bool isCalibrated = false;
bool isReadCalibrated = false;
bool isWriteCalibrated = false;
bool calSuccess = true;
bool upgSuccess = true;
bool calStarted = false;
bool upgStarted = false;
bool moduleConnected = false;

std::string getStatusMessage(uint16_t status) {
    switch (status) {
        case 0x0000: return "TMR_SUCCESS";
        // Bootloader & Flash Errors
        case 0x0100: return "TMR_ERROR_MSG_INVALID_OPCODE";
        case 0x0101: return "TMR_ERROR_MSG_INVALID_PARAMETER";
        case 0x0102: return "TMR_ERROR_MSG_INVALID_DATA_LENGTH";
        
        // Hardware & Performance Faults
        case 0x0500: return "TMR_ERROR_HIGH_TEMPERATURE";
        case 0x0504: return "TMR_ERROR_UNIMPLEMENTED_FEATURE";
        
        // Protocol & Air Interface Faults
        case 0x0400: return "TMR_ERROR_NO_TAG_FOUND";
        case 0x0401: return "TMR_ERROR_TAG_ALREADY_EXISTS";
        case 0x0402: return "TMR_ERROR_TAG_ID_BUFFER_FULL";
        case 0x0403: return "TMR_ERROR_BLOCK_NOT_AVAILABLE";
        case 0x0404: return "TMR_ERROR_TAG_UNSUPPORTED";
        
        // Cryptographic & Security Faults
        case 0x0405: return "TMR_ERROR_TAG_LOCKED";
        case 0x0406: return "TMR_ERROR_TAG_PASSWORD_INVALID";
        
        default: {
            char hexStr[32];
            Log::logger().printSectionHeader("get status message default");
            return std::string(hexStr);
        }
    }
}

std::string getCommandName(uint8_t opcode) {
    switch (opcode) {
        case 0x03: return "TMR_SR_OP_CMD_GET_VERSION";
        case 0x04: return "TMR_SR_OP_CMD_SET_BAUD_RATE";
        case 0x0D: return "TMR_SR_OPCODE_BOOT_BOOTLOADER";
        case 0x21: return "TMR_SR_OP_CMD_EXECUTE_TAG_OP";
        case 0x22: return "TMR_SR_OP_CMD_READ_TAG_MULTIPLE";
        case 0x23: return "TMR_SR_OP_CMD_WRITE_TAG_DATA";
        case 0x28: return "TMR_SR_OP_CMD_READ_TAG_DATA";
        case 0x29: return "TMR_SR_OP_CMD_WRITE_TAG_SPECIFIC_DATA";
        case 0x2A: return "TMR_SR_OP_CMD_CLEAR_TAG_ID_BUFFER";
        case 0x2F: return "TMR_SR_OP_CMD_HEX_PASSTHROUGH";
        case 0x61: return "TMR_SR_OP_CMD_GET_CURRENT_TAG_PROTOCOL";
        case 0x62: return "TMR_SR_OP_CMD_SET_TAG_PROTOCOL";
        case 0x63: return "TMR_SR_OP_CMD_GET_TAG_PROTOCOL";
        case 0x91: return "TMR_SR_OP_CMD_GET_ANALOG_PARAM";
        case 0x92: return "TMR_SR_OP_CMD_SET_ANALOG_PARAM";
        case 0x93: return "TMR_SR_OP_CMD_SET_TAG_PROTOCOL"; 
        case 0x96: return "TMR_SR_OP_CMD_SET_USER_GPIO_OUTPUTS";

        default: {
            char hexStr[32];
            snprintf(hexStr, sizeof(hexStr), "UNKNOWN (0x%02X)", opcode);
            Log::logger().printSectionHeader("get command name default");
            Log::logger().printKeyValue("opCode", std::to_string(opcode));
            return std::string(hexStr);
        }
    }
}

void RFIDTransportListenerCallback(bool tx, uint32_t dataLen, const uint8_t data[], uint32_t timeoutMs, void *cookie) {
    // 1. Parse Host Transmissions (TX) going out to the reader
    if (tx) {
        if (dataLen >= 3 && data[0] == 0xFF) {
            uint8_t opcode = data[2];
            if (opcode != 0x96) 
            {
                Log::logger().printSectionHeader("RFID Module command");
                Log::logger().printKeyValue("Outbound Command", getCommandName(opcode));
            } 
        }
    } 
    // 2. Parse Reader Responses (RX) incoming from the COM Port
    else {
        // Ensure the packet has basic serial frame structures
        if (dataLen >= 5 && data[0] == 0xFF) {
            uint8_t opcode = data[2];
            uint16_t statusCode = (uint16_t)((data[3] << 8) | data[4]);

            // Process payload ONLY if the serial operation executed successfully
            if (statusCode == 0x0000) 
            {
                if (opcode != 0x96) 
                {
                    Log::logger().printKeyValue("[READER] <- Response for", getCommandName(opcode));
                    Log::logger().printKeyValue("Status Code", getStatusMessage(statusCode));
                }
            } 
            // Handle error codes returned by the device firmware
            else {
                Log::logger().printSectionHeader("RFID module command failed");
                Log::logger().printKeyValue("Failed Command", getCommandName(opcode));
                Log::logger().printKeyValue("Error Reason", getStatusMessage(statusCode));
            }
        }
    }
}

void RFIDReadListenerCallback(TMR_Reader *reader, const TMR_TagReadData *t, void *cookie)
{
    char epcStr[128];

    // Convert EPC bytes to string
    TMR_bytesToHex(t->tag.epc, t->tag.epcByteCount, epcStr);

    std::string epc = epcStr;

    // Search for existing EPC
    auto it = std::find_if(g_tagList.begin(), g_tagList.end(), [&](const Tag& tag) { return tag.idStr == epc; });

    // Existing tag
    if (it != g_tagList.end())
    {
        it->readCount++;

        // Update latest values
        it->rssi = t->rssi;
        it->antenna = t->antenna;
        it->frequency = t->frequency;
        it->phase = t->phase;
        it->protocol = t->tag.protocol;

        // Optional timestamp update
        it->timeStr = std::to_string(t->timestampHigh) + ":" + std::to_string(t->timestampLow);
    }
    else // New tag
    {
        Tag newTag;

        newTag.idStr = epc;

        newTag.timeStr = std::to_string(t->timestampHigh) + ":" + std::to_string(t->timestampLow);

        newTag.readCount = 1;

        newTag.antenna = t->antenna;
        newTag.rssi = t->rssi;
        newTag.frequency = t->frequency;
        newTag.phase = t->phase;
        newTag.protocol = t->tag.protocol;

        g_tagList.push_back(newTag);
    }

    // Check threshold
    for (const auto& tag : g_tagList)
    {
        if (tag.readCount >= 6)
        {
            tagReadEnoughTimes = true;  
                   
            //TMR_stopReading(reader);

            break;
        }
    }
}

void RFIDReadExceptionListenerCallback(TMR_Reader *reader, TMR_Status error, void *cookie)
{
    // This function will be called if there is an error during a background read
    Log::logger().error("Background read error: {}", TMR_strerr(reader, error));
    Log::logger().printSectionHeader("Read exception occurred");

    //TMR_stopReading(reader);

    readErrorOccurred = true;
    tagReadEnoughTimes = true;
}

TMR_Status clearTagBuffer(TMR_Reader *reader)
{
    TMR_Status ret;
    
    // Save the current read plan
    TMR_ReadPlan originalPlan;
    ret = TMR_paramGet(reader, TMR_PARAM_READ_PLAN, &originalPlan);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().error("Error getting original read plan: {}", TMR_strerr(reader, ret));
        return ret;
    }
    uint8_t antennaList[] = {1};
    // Create a simple read plan with a very short read time
    TMR_ReadPlan plan;
    TMR_RP_init_simple(&plan, 1, antennaList, TMR_TAG_PROTOCOL_GEN2, 1000);

    // Set the new read plan
    ret = TMR_paramSet(reader, TMR_PARAM_READ_PLAN, &plan);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().error("Error setting temporary read plan: {}", TMR_strerr(reader, ret));
        return ret;
    }

    // Perform a very short read operation to clear the buffer
    uint32_t onTime = 1; // 1 millisecond
    ret = TMR_paramSet(reader, TMR_PARAM_READ_ASYNCONTIME, &onTime);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().error("Error setting asyncontime: {}", TMR_strerr(reader, ret));
        return ret;
    }

    ret = TMR_read(reader, 1, NULL);



    if (TMR_SUCCESS != ret)
    {
        Log::logger().error("Error performing read to clear buffer: {}", TMR_strerr(reader, ret));
        return ret;
    }

    // Restore the original read plan
    ret = TMR_paramSet(reader, TMR_PARAM_READ_PLAN, &originalPlan);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().error("Error restoring original read plan: {}", TMR_strerr(reader, ret));
        return ret;
    }

    Log::logger().info("Tag buffer cleared successfully.");
    return TMR_SUCCESS;
}

void errx(int exitval, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);
    throw RFIDException(buffer);
}

void checkerr(TMR_Reader* rp, TMR_Status ret, int exitval, const char *msg)
{
    if (TMR_SUCCESS != ret)
    {
        throw RFIDException(std::string(msg) + ": " + TMR_strerr(rp, ret));
    }
}

uint64_t hexStringToDecimal(const std::string& hexStr) 
{
    // Convert the hex string to a decimal number
    return std::stoull(hexStr, nullptr, 16);
}

void hexStringToByteArray(const char* hexStr, uint8_t* byteArray, size_t byteArraySize)
{
    if (!hexStr || !byteArray) {
        throw RFIDException("Invalid input parameters for hexStringToByteArray");
    }

    size_t hexStrLen = strlen(hexStr);
    if (hexStrLen % 2 != 0)
    {
        throw RFIDException("Invalid hex string length");
    }

    for (size_t i = 0; i < hexStrLen / 2 && i < byteArraySize; ++i)
    {
        if (sscanf(&hexStr[2 * i], "%2hhx", &byteArray[i]) != 1) {
            throw RFIDException("Invalid hex character in string");
        }
    }
}

void serialPrinter(bool tx, uint32_t dataLen, const uint8_t data[],
                   uint32_t timeout, void *cookie)
{
  std::stringstream ss;
  ss << (tx ? "Sending:  " : "Received: ");
  for (uint32_t i = 0; i < dataLen; i++)
  {
    if (i > 0 && i % 16 == 0)
    {
      Log::logger().info("{}", ss.str());
      ss.str("");
      ss << "         ";
    }
    ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(data[i]) << " ";
  }
  Log::logger().info("{}", ss.str());
}

void stringPrinter(bool tx, uint32_t dataLen, const uint8_t data[], uint32_t timeout, void *cookie)
{
  Log::logger().info("{} {:.{}s}", tx ? "Sending:" : "Received:", reinterpret_cast<const char*>(data), dataLen);
}

void initializeReader(TMR_Reader *rp, const char *uri)
{
    Log::logger().debug("Initializing Reader");
    if (!rp || !uri) {
        throw RFIDException("Invalid parameters for initializeReader");
    }

    TMR_Status ret = TMR_create(rp, uri);
    checkerr(rp, ret, 1, "creating reader");
    Log::logger().debug("Reader initialized");
}

void setupTransportListener(TMR_Reader *rp)
{
    Log::logger().debug("Setting up transport listener");
    if (!rp) {
        throw RFIDException("Invalid reader for setupTransportListener");
    }

    if (TMR_READER_TYPE_SERIAL == rp->readerType)
    {
        RFIDTransportListenerBlock.listener = serialPrinter;
    }
    else
    {
        RFIDTransportListenerBlock.listener = stringPrinter;
    }
    RFIDTransportListenerBlock.cookie = nullptr;  // We don't need cookie anymore as we're using appPrint

    TMR_Status ret = TMR_addTransportListener(rp, &RFIDTransportListenerBlock);
    checkerr(rp, ret, 1, "adding transport listener");
}

void connectReader(TMR_Reader *rp)
{
    if (!rp) 
    {
        throw RFIDException("Invalid reader for connectReader");
    }

    TMR_Status ret = TMR_connect(rp);

    if(ret != TMR_SUCCESS)
    {
        throw RFIDException("Error connecting to reader");
    }  
}

void setReadPower(TMR_Reader* rp, int readpower) {
    if (!rp) {
        throw RFIDException("Invalid reader for setReadPower");
    }

    if (readpower < 0 || readpower > MAX_RFID_POWER)
    {
        readpower = MAX_RFID_POWER;
    }

    if (readpower < 0 || readpower > MAX_RFID_POWER) 
    {
        throw RFIDException("Read power out of valid range (0-2400)");
    }
    int cur_readpower = 0;
    TMR_Status ret;
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_READPOWER, &cur_readpower);
    checkerr(rp, ret, 1, "getting read power");
    ret = TMR_paramSet(rp, TMR_PARAM_RADIO_READPOWER, &readpower);
    checkerr(rp, ret, 1, "setting read power");
    Log::logger().info("Read power: {} -> {} dBm", cur_readpower, readpower);
}

int getReadPower(TMR_Reader* rp) {
    if (!rp) {
        throw RFIDException("Invalid reader for getReadPower");
    }
    TMR_Status ret;
    int cur_readpower = 0;
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_READPOWER, &cur_readpower);
    checkerr(rp, ret, 1, "getting read power");
    return cur_readpower;
}

void setWritePower(TMR_Reader* rp, int writepower) {
    if (!rp) {
        throw RFIDException("Invalid reader for setWritePower");
    }
    
    if(writepower < 0 || writepower > MAX_RFID_POWER) 
    {
        writepower = MAX_RFID_POWER;
    }
    
    if (writepower < 0 || writepower > MAX_RFID_POWER) 
    {
        throw RFIDException("Write power out of valid range (0-2400)");
    }
    int cur_writepower = 0;
    TMR_Status ret;
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_WRITEPOWER, &cur_writepower);
    checkerr(rp, ret, 1, "getting write power");
    ret = TMR_paramSet(rp, TMR_PARAM_RADIO_WRITEPOWER, &writepower);
    checkerr(rp, ret, 1, "setting write power");
    Log::logger().info("Write power: {} -> {} dBm", cur_writepower, writepower);
}

int getWritePower(TMR_Reader* rp) {
    if (!rp) {
        throw RFIDException("Invalid reader for getWritePower");
    }
    TMR_Status ret;
    int cur_writepower = 0;
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_WRITEPOWER, &cur_writepower);
    checkerr(rp, ret, 1, "getting write power");
    return cur_writepower;
}

void setUserMode(TMR_Reader* rp, int userMode) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for setUserMode()");
    }
    if (userMode < 0 || userMode > 1) 
    {
        throw RFIDException("userMode out of range");
    }
 
    TMR_Status ret;

    ret = TMR_paramSet(rp, TMR_PARAM_USERMODE, &userMode);
    checkerr(rp, ret, 1, "setting userMode");
}

int getUserMode(TMR_Reader* rp) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for getUserMode()");
    }

    TMR_Status ret;
    int userMode = 0;

    ret = TMR_paramGet(rp, TMR_PARAM_USERMODE, &userMode);
    checkerr(rp, ret, 1, "getting user mode");

    Log::logger().printKeyValue("user mode", std::to_string(userMode));
    
    return userMode;
}

void setPowerMode(TMR_Reader* rp, int powerMode) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for setPowerMode()");
    }
    
    if (powerMode < 0 || powerMode > 1) 
    {
        throw RFIDException("powerMode out of range");
    }

    TMR_Status ret;

    ret = TMR_paramSet(rp, TMR_PARAM_POWERMODE, &powerMode);
    checkerr(rp, ret, 1, "setting power mode");
}

int getPowerMode(TMR_Reader* rp) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for getPowerMode()");
    }

    TMR_Status ret;
    int powerMode = 0;

    ret = TMR_paramGet(rp, TMR_PARAM_POWERMODE, &powerMode);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().printKeyValue("Power mode setting failed", std::to_string(powerMode));
    }
    else
    {
        Log::logger().printKeyValue("Power mode setting", std::to_string(powerMode));
    }
    
    checkerr(rp, ret, 1, "getting power mode");
    
    return powerMode;
}

void setCommandTimeout(TMR_Reader* rp, int timeout) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for setCommandTimeout()");
    }
    
    TMR_Status ret;
    Config& settings = Config::getInstance();
    uint32_t commandTimeout = settings.getCommandTimeoutFromConfig();

    //set command timeout
    ret = TMR_paramSet(rp, TMR_PARAM_COMMANDTIMEOUT, &commandTimeout);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().printKeyValue("Error setting command timeout", std::to_string(commandTimeout));
    }
    checkerr(rp, ret, 1, "Error setting command timeout");
    //Log::logger().printKeyValue("Command timeout", std::to_string(commandTimeout));
}

int getCommandTimeout(TMR_Reader* rp) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for getCommandTimeout()");
    }

    TMR_Status ret;
    int returnValue;

    //get commandtimeout
    ret = TMR_paramGet(rp, TMR_PARAM_COMMANDTIMEOUT, &returnValue);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().printKeyValue("Error getting command timeout", std::to_string(returnValue));
    }
    checkerr(rp, ret, 1, "Error getting command timeout");
    Log::logger().printKeyValue("Command timeout", std::to_string(returnValue));

    return returnValue;
}

void setGPIOPin(TMR_Reader* rp, int pin, int output) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for setGPIOPin()");
    }

    TMR_GpioPin state;
    TMR_Status ret;
    uint8_t stateCount;
    stateCount = 1;

    state.id = pin;
    state.high = output;
    state.output = true;
    
    moduleConnected = true;

    if(!heartbeatStopFlag.load())
    {
        ret = TMR_gpoSet(rp, stateCount, &state);
    }
    
    if (TMR_SUCCESS != ret)
    {
        Log::logger().printKeyValue("GPIO error setting pin", std::to_string(pin));

        moduleConnected = false;
        //isInitialized = false;
    }

    checkerr(rp, ret, 1, "Error setting GPIO pin");
}

int getGPIOPin(TMR_Reader* rp, int pin) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for getGPIOPin()");
    }

    TMR_Status ret;
    TMR_GpioPin state[16];
    uint8_t i, stateCount = numberof(state);

    ret = TMR_gpiGet(rp, &stateCount, state);
    if (TMR_SUCCESS != ret)
    {
        Log::logger().printKeyValue("GPIO error getting state of pin", std::to_string(pin));
    }
    checkerr(rp, ret, 1, "Error getting GPIO pin");

    for (i = 0 ; i < stateCount ; i++)
    {
        if(state[i].id == pin)
        {
            //Log::logger().printKeyValue("GPIO id", std::to_string(state[i].id));
            //Log::logger().printKeyValue("GPIO high", std::to_string(state[i].high));

            return state[i].high;
        }        
    }

    return 0;
}

void setGPIOPinDirection(TMR_Reader* rp, char pin, bool direction)
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for setGPIOPinsDirection()");
    }

    TMR_Param param;
    TMR_Status ret;
    
    if(direction == OUTPUT)
    {
        uint8_t out[] = {pin};
        uint8_t out_list[10];
        TMR_uint8List out_key;
        TMR_uint8List out_val;

        out_key.list = out;
        out_key.len = 1;
        out_val.list = out_list;
        out_val.max = 10;

        param = TMR_PARAM_GPIO_OUTPUTLIST;
        ret = TMR_paramSet(rp, param, &out_key);
        if (TMR_SUCCESS != ret)
        {
            Log::logger().printKeyValue("Set GPIO pin to output error", std::to_string(pin));
        }
        checkerr(rp, ret, 1, "Error setting GPIO pin direction to OUTPUT");
    }
    else
    {
        uint8_t in[] = {pin};
        uint8_t in_list[10];
        TMR_uint8List in_key;
        TMR_uint8List in_val;

        in_key.list = in;
        in_key.len = 1;
        in_val.list = in_list;
        in_val.max = 10;

        param = TMR_PARAM_GPIO_INPUTLIST;
        ret = TMR_paramSet(rp, param, &in_key);
        if (TMR_SUCCESS != ret)
        {
            Log::logger().printKeyValue("Set GPIO pin to input error", std::to_string(pin));
        }
        checkerr(rp, ret, 1, "Error setting GPIO pin direction TO INPUT");
    }
}

void blinkStatusLEDCodeHeartbeat( std::atomic<bool> &heartbeatStopFlag )
{
    while(true)
    {
        if(!heartbeatStopFlag.load())
        {
            //status LED on
            try
            {
                if(isInitialized == true)
                {
                    setGPIOPin(&RFIDReader, 4, 1);
                }
            }
            catch(...)
            {
                Log::logger().info("Exception thrown in heartbeat 1");
                isInitialized = false;
                moduleConnected = false;

                //stopHeartbeatLEDThread();
                heartbeatStopFlag.store(true);
            }

            for (int i = 0; i < 50 && !heartbeatStopFlag.load(); ++i) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        if(!heartbeatStopFlag.load())
        {
            //status LED off
            try
            {
                if(isInitialized == true)
                {
                    setGPIOPin(&RFIDReader, 4, 0); 
                }
            }
            catch(...)
            {
                Log::logger().info("Exception thrown in heartbeat 2");
                isInitialized = false;
                moduleConnected = false;

                heartbeatStopFlag.store(true);
                //stopHeartbeatLEDThread();
            }
            
            for (int i = 0; i < 50 && !heartbeatStopFlag.load(); ++i) 
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}

void createHeartbeatLEDThread()
{
    heartbeatStopFlag.store(false);
    std::thread heartbeatThread(blinkStatusLEDCodeHeartbeat, std::ref(heartbeatStopFlag));
    heartbeatLEDThreadID = heartbeatThread.get_id();
    heartbeatThread.detach();
}

void startHeartbeatLEDThread()
{
    heartbeatStopFlag.store(false);
}

void stopHeartbeatLEDThread()
{
    heartbeatStopFlag.store(true);
}

int getAntennaReturnLoss(TMR_Reader* rp) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for getAntennaReturnLoss()");
    }

    return 0;
}

int getRadioTemperature(TMR_Reader* rp) 
{
    if(!rp) 
    {
        throw RFIDException("Invalid reader for getRadioTemperature()");
    }

    TMR_Status ret;
    int cur_temperature = 0;
    
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_TEMPERATURE, &cur_temperature);
    checkerr(rp, ret, 1, "getting radio temperature");
    
    return cur_temperature;
}

void configureReader(TMR_Reader *rp, int readpower, int writepower)
{
    Log::logger().debug("Configuring reader");

    if(!rp) 
    {
        throw RFIDException("Invalid reader for configureReader");
    }

    //get settings
    Config& settings = Config::getInstance();

    TMR_Status ret;
    TMR_Region region;
    TMR_String model;
    char string[100];
    TMR_String regValue;
    char regString[256];  // Increased buffer size to accommodate longer strings
    regValue.value = regString;
    model.value = string;
    model.max   = sizeof(string);
    uint32_t commandTimeout = settings.getCommandTimeoutFromConfig();
    
    ret = TMR_paramGet(rp, TMR_PARAM_VERSION_MODEL, &model);
    checkerr(rp, ret, 1, "Getting version model");
    
    
    region = TMR_REGION_NONE;
    ret = TMR_paramGet(rp, TMR_PARAM_REGION_ID, &region);
    checkerr(rp, ret, 1, "getting region");
    
    if (region == TMR_REGION_NONE)
    {
        // Get model
        ret = TMR_paramGet(rp, TMR_PARAM_VERSION_MODEL, &regValue);
        if (TMR_SUCCESS == ret) {
            Log::logger().info("regValue=\"{}\"", regValue.value);
            if (strcmp(regValue.value, "M6e Nano") == 0) {
                Log::logger().info("Set region to NA2");
                region = TMR_REGION_NA2;
            } else {
                Log::logger().info("Set region to NA");
                region = TMR_REGION_NA;
            }
        } else {
            Log::logger().error("Error getting model: {}", TMR_strerr(rp, ret));
        }
        
        ret = TMR_paramSet(rp, TMR_PARAM_REGION_ID, &region);
        checkerr(rp, ret, 1, "setting region");
    }

    int value;

    Log::logger().info("Region set");

    setCommandTimeout(rp, settings.getCommandTimeoutFromConfig());

    Log::logger().info("Command timeout set");

    setReadPower(rp, settings.getDefaultReadPowerFromConfig());

    Log::logger().info("Set default read power");

    setWritePower(rp, settings.getDefaultWritePowerFromConfig());
    
    Log::logger().info("Set default write power");
    
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_READPOWER, &value);
    checkerr(rp, ret, 1, "getting read power");

    Log::logger().info("configureReader() done");
    
    //Log::logger().info("Read power = {} dBm", value);
}

void setMetadataFlags(TMR_Reader *rp)
{
    //Log::logger().debug("Setting metadata flags");
    if (!rp) {
        throw RFIDException("Invalid reader for setMetadataFlags");
    }

    TMR_Status ret;
    
    TMR_TRD_MetadataFlag metadataFlags = TMR_TRD_METADATA_FLAG_ALL;

    /*
    TMR_TRD_MetadataFlag metadataFlags = (TMR_TRD_MetadataFlag)(
        TMR_TRD_METADATA_FLAG_RSSI | 
        TMR_TRD_METADATA_FLAG_READCOUNT | 
        TMR_TRD_METADATA_FLAG_ANTENNAID
    );
    */

    ret = TMR_paramSet(rp, TMR_PARAM_METADATAFLAG, &metadataFlags);
    checkerr(rp, ret, 1, "Setting Metadata Flags");
}

void configureReadPlan(TMR_Reader *rp)
{
    //Log::logger().debug("Configuring read plan");

    if (!rp) {
        throw RFIDException("Invalid reader for configureReadPlan");
    }

    TMR_Status ret;
    TMR_ReadPlan plan;
    uint8_t antennaList[1] = {1};
    uint8_t antennaCount = 0x01;

    ret = TMR_RP_init_simple(&plan, antennaCount, antennaList, TMR_TAG_PROTOCOL_GEN2, 1000);
    checkerr(rp, ret, 1, "initializing the read plan");

    ret = TMR_paramSet(rp, TMR_PARAM_READ_PLAN, &plan);
    checkerr(rp, ret, 1, "setting read plan");
    //Log::logger().debug("Reader configuration completed successfully");
}

std::vector<Tag> readTags(TMR_Reader *rp)
{
    Log::logger().debug("Inventory RFID tags");
    Log::logger().printSectionHeader("Reading Tags");
    if (!rp) 
    {
        throw RFIDException("Invalid reader for readTags");
    }

    TMR_Status ret;
    std::vector<Tag> tags;
    
    Config& settings = Config::getInstance();

    int readTime = settings.getReadTimeFromConfig();
    int commandTimeout = settings.getCommandTimeoutFromConfig();
    int readTimeout = settings.getReadTimeoutFromConfig();

    if(readPowerCalCheckFlag == true)
    {
        ret = TMR_read(rp, 5000, NULL);
    }
    else
    {
        ret = TMR_read(rp, readTime, NULL);
    }
    
    if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret)
    {
        Log::logger().warn("Reading tags: {}", TMR_strerr(rp, ret));
        Log::logger().printSectionHeader("TMR_ERROR_TAG_ID_BUFFER_FULL");
    }
    else
    {
        checkerr(rp, ret, 1, "reading tags");
    }

    time_t startTime = time(NULL);

    while (TMR_SUCCESS == TMR_hasMoreTags(rp))
    {
        if (difftime(time(NULL), startTime) > readTimeout) {
            Log::logger().printKeyValue("Timeout while reading tags", std::to_string(readTimeout));
            //throw RFIDException("Timeout while reading tags");
            return tags;
        }

        TMR_TagReadData trd;

        memset(&trd, 0, sizeof(TMR_TagReadData)); 

        ret = TMR_getNextTag(rp, &trd);
        checkerr(rp, ret, 1, "fetching tag");

        Tag tag;
        char idStr[128];
        char timeStr[128];

        TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, idStr);
        tag.idStr = idStr;
        TMR_getTimeStamp(rp, &trd, timeStr);
        tag.timeStr = timeStr;

        tag.readCount = trd.readCount;
        tag.antenna = trd.antenna;
        tag.rssi = trd.rssi;
        tag.frequency = trd.frequency;
        tag.phase = trd.phase;
        tag.protocol = trd.tag.protocol;

        tags.push_back(tag);
    }

    // Sort tags by readCount, if there is a tie sort by RSSI
    std::sort(tags.begin(), tags.end(), [](const Tag& a, const Tag& b) 
    {
        if (a.readCount != b.readCount) 
        {
            return a.readCount > b.readCount; // Higher readCount first
        } 
        else 
        {
            return a.rssi > b.rssi; // If readCount is equal, lower rssi first
        }
    });

    for(unsigned int i = 0; i < tags.size(); i++)
    {
        if(tags[i].readCount != 0)
        {
            Log::logger().printKeyValue("Tag",std::to_string(i + 1));
            Log::logger().printKeyValue("Read Count", std::to_string(tags[i].readCount));
            Log::logger().printKeyValue("RSSI", std::to_string(tags[i].rssi));
        }
    }

    return tags;
}

int writeTag(TMR_Reader *rp, std::vector<Tag>& tags, const std::string& newEpcStrRef)
{
    const char* newEpcStr = newEpcStrRef.c_str();

    TMR_Status ret;

    // Convert EPC string to byte array
    TMR_TagData newEpc;
    uint32_t epcByteCount;
    ret = TMR_hexToBytes(newEpcStr, newEpc.epc, sizeof(newEpc.epc), &epcByteCount);
    if (ret != TMR_SUCCESS) {
        throw RFIDException("Failed to convert EPC hex string to bytes: " + std::string(TMR_strerr(rp, ret)));
    }
    newEpc.epcByteCount = epcByteCount;

    writeOp.u.gen2.u.writeTag.epcptr = &newEpc;

    // Create a list of operations
    TMR_TagOp* tagopArray[2] = {&writeOp, &readOp};
    TMR_TagOp_List tagopList = {tagopArray, 2};
    TMR_TagOp listop = {TMR_TAGOP_LIST};
    listop.u.list = tagopList;

    uint8_t responseData[256];  // Increased size to accommodate longer EPCs
    response.list = responseData;
    response.max = sizeof(responseData);
    response.len = 0;

    ret = TMR_executeTagOp(rp, &writeOp, NULL, NULL);

    ret = TMR_executeTagOp(rp, &readOp, NULL, &response);
    
    if (ret == TMR_SUCCESS && response.len > 0) // Verify written data
    {
        char readDataStr[128];
        TMR_bytesToHex(response.list, response.len, readDataStr);
        
        Log::logger().printKeyValue("readDataStr", readDataStr);

        // Extract EPC from read data (skip first 4 bytes which are PC and CRC)
        std::string readEpcStr(readDataStr + 8, response.len * 2 - 8);
        
        char expectedEpcStr[512];  // Increased size
        
        if(calibrationInProgress == true)
        {
            // 1. Ensure the new string fits (leave 1 byte for the null terminator '\0')
            if (calString.length() < sizeof(expectedEpcStr)) 
            {
                // 2. Copy the contents
                std::copy(calString.begin(), calString.end(), expectedEpcStr);
                
                // 3. Manually add the null terminator at the end of the copied text
                expectedEpcStr[calString.length()] = '\0';
            }
        }


        TMR_bytesToHex(newEpc.epc, newEpc.epcByteCount, expectedEpcStr);
        
        Log::logger().printSectionHeader("RFID write verification");

        if (readEpcStr == expectedEpcStr)
        {
            Log::logger().printKeyValue("Status", Log::logger().formatSuccess("Success"));
            Log::logger().printKeyValue("Verification", Log::logger().formatSuccess("Passed"));
            Log::logger().printKeyValue("ExpectedEPC", expectedEpcStr);
            Log::logger().printKeyValue("readEPC", readEpcStr);            
            return 0;  // Success, exit the function
        }
        else
        {
            Log::logger().warn("Status: {}", Log::logger().formatFailure("Verification Failed"));
            Log::logger().printKeyValue("ExpectedEPC", expectedEpcStr);
            Log::logger().printKeyValue("readEPC", readEpcStr);
        }
    }
    else
    {
        Log::logger().error("Status: {}", Log::logger().formatFailure("Failed"));
        Log::logger().error("Error: {}", TMR_strerr(rp, ret));
    }
    
    return 1;
}

TMR_Status readTidByEpc(TMR_Reader *rp, const char *epcStr, char *tidStr, size_t tidStrSize, Config& settings)
{
    if (!rp || !epcStr || !tidStr || tidStrSize == 0) {
        throw RFIDException("Invalid parameters for readTidByEpc");
    }

    TMR_Status ret;
    TMR_TagOp tagop;
    TMR_TagFilter filter;
    TMR_uint8List response;
    uint8_t responseData[258];
    TMR_TagData epcData;
    uint32_t epcByteCount;

    TMR_hexToBytes(epcStr, epcData.epc, sizeof(epcData.epc), &epcByteCount);
    epcData.epcByteCount = epcByteCount;

    ret = TMR_TF_init_tag(&filter, &epcData);
    if (TMR_SUCCESS != ret)
    {
        throw RFIDException("Failed to initialize tag filter: " + std::string(TMR_strerr(rp, ret)));
    }

    ret = TMR_TagOp_init_GEN2_ReadData(&tagop, TMR_GEN2_BANK_TID, 0, 0);
    if (TMR_SUCCESS != ret)
    {
        throw RFIDException("Failed to initialize read operation: " + std::string(TMR_strerr(rp, ret)));
    }

    response.list = responseData;
    response.max = sizeof(responseData) / sizeof(responseData[0]);
    response.len = 0;

    int maxRetries = settings.getReadTIDRetryCountFromConfig();
    int powerIncrement = settings.getReadTIDRetryPowerIncrementFromConfig();  // 1 dBm increment in centi-dBm
    int initialReadPower = getReadPower(rp);

    for (int retry = 0; retry < maxRetries; retry++)
    {
        ret = TMR_executeTagOp(rp, &tagop, &filter, &response);
        if (TMR_SUCCESS == ret && response.len > 0)
        {
            TMR_bytesToHex(response.list, response.len, tidStr);
            // Reset read power to initial value before returning
            setReadPower(rp, initialReadPower);
            return TMR_SUCCESS;
        }
        
        Log::logger().info("TID read attempt {} failed. Increasing power and retrying...", retry + 1);
        
        // Increase read power for the next attempt
        int currentReadPower = getReadPower(rp);
        if ((currentReadPower + powerIncrement) <= MAX_RFID_POWER)  // 2400 is max power in centi-dBm (27 dBm)
        {
            setReadPower(rp, currentReadPower + powerIncrement);
        }
        
        // Short delay before next attempt
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Reset read power to initial value before returning
    setReadPower(rp, initialReadPower);

    strncpy(tidStr, "No TID data", tidStrSize);
    return TMR_ERROR_NO_TAGS_FOUND;
}

const char* getVendorNameFromTid(const char *tidStr)
{
    if (!tidStr || strlen(tidStr) < 5) {
        throw RFIDException("Invalid TID string");
    }

    char vendorCodeStr[4] = { tidStr[2], tidStr[3], tidStr[4], '\0' };

    int vendorIndex = static_cast<int>(strtol(vendorCodeStr, nullptr, 16)) - 0x800;

    if (vendorIndex >= 0 && vendorIndex < vendor::manufacturer_names.size())
    {
        return vendor::manufacturer_names[vendorIndex];
    }
    else
    {
        return "Unknown Mfr";
    }
}

void getReaderInfo(TMR_Reader *rp, ReaderInfo *infoHandle) {
    //Log::logger().debug("Getting reader info");
    TMR_Status ret;
    TMR_String value;
    char string[256];
    value.value = string;
    value.max = sizeof(string);

    //Log::logger().printSectionHeader("Reader Information");

    // Get hardware version
    ret = TMR_paramGet(rp, TMR_PARAM_VERSION_HARDWARE, &value);
    if (TMR_SUCCESS == ret) {
        //Log::logger().printKeyValue("Hardware Version", value.value);
        infoHandle->hardwareVersion = value.value;
    } else {
        Log::logger().error("Error getting hardware version: {}", TMR_strerr(rp, ret));
    }

    // Get serial number
    ret = TMR_paramGet(rp, TMR_PARAM_VERSION_SERIAL, &value);
    if (TMR_SUCCESS == ret) {
        //Log::logger().printKeyValue("Serial Number", value.value);
        infoHandle->serialNumber = value.value;
    } else {
        Log::logger().error("Error getting serial number: {}", TMR_strerr(rp, ret));
    }

    // Get model
    ret = TMR_paramGet(rp, TMR_PARAM_VERSION_MODEL, &value);
    if (TMR_SUCCESS == ret) {
        //Log::logger().printKeyValue("Model", value.value);
        infoHandle->model = value.value;
    } else {
        Log::logger().error("Error getting model: {}", TMR_strerr(rp, ret));
    }

    // Get software version (firmware)
    ret = TMR_paramGet(rp, TMR_PARAM_VERSION_SOFTWARE, &value);
    if (TMR_SUCCESS == ret) {
        //Log::logger().printKeyValue("Firmware Version", value.value);
        infoHandle->firmwareVersion = value.value;
    } else {
        Log::logger().error("Error getting firmware version: {}", TMR_strerr(rp, ret));
    }
}

int upgradeFirmware(TMR_Reader* rp, std::atomic<bool> &upgradeStopFlag)
{
    try
    {
        ReaderInfo handle;
        std::string uri;
        FILE* cookie = nullptr;
        TMR_Status ret = 0;
        std::string old_firmware_version = "";
        std::string firmware_file = "";
        upgSuccess = true;

        handle = ReaderInfo{};
        //uri =  Config::getInstance().getReaderURIFromConfig();

        /* Initialize and connect reader */
        //initializeReader(rp, uri.c_str());
        //connectReader(rp);

        getReaderInfo(rp, &handle);
        old_firmware_version = handle.firmwareVersion;

        Log::logger().info("Firmware version on RFID board " + handle.firmwareVersion);

        /* Find the firmware file - will have .sim extension... hopefully? */
        try
        {
            for(const auto& entry : fs::directory_iterator(FIRMWARE_PATH))
            {
                if(entry.path().extension() == FIRMWARE_EXT)
                {
                    firmware_file = entry.path().filename();
                }
            }
        }
        catch(const fs::filesystem_error& e)
        {
            Log::logger().error("Filesytem error when upgrading firmware");
            upgSuccess = false;
            return -1;
        }

        /* Check if firmware file was found */
        if(!firmware_file.length())
        {
            Log::logger().error("No RFID firmware file found, returning");
            upgSuccess = false;
            return -1;
        }

        Log::logger().info("Attempting to open firmware file " + (FIRMWARE_PATH + firmware_file));

        /* Initialize file "cookie" for API firmware loading */
        cookie = fopen((FIRMWARE_PATH + firmware_file).c_str(), "rb");
        if(!cookie)
        {
            Log::logger().error("Unable to open RFID firmware file!");
            upgSuccess = false;
            return -1;
        }

        ret = TMR_firmwareLoad(rp, cookie, TMR_fileProvider);
        if(ret != TMR_SUCCESS)
        {
            Log::logger().error("RFID firmware load failure " + std::string(TMR_strerr(rp, ret)));
            upgSuccess = false;
            return -1;
        }

        /* Check to see if the firmware version has changed */
        getReaderInfo(rp, &handle);
        if(old_firmware_version == std::string(handle.firmwareVersion))
        {
            Log::logger().warn("Old firmware version is the same as the new firmware version ");
        }

        /* Cleanup reader object */
        ret = TMR_destroy(rp);
        if(ret != TMR_SUCCESS)
        {
            Log::logger().warn("Unable to destroy TMR object " + std::string(TMR_strerr(rp, ret)));
            upgSuccess = false;
        }

        
        TMR_TagData newEpc;
        Config& settings = Config::getInstance();
        EncodingProtocol protocol = settings.getEncodingProtocol();

        switch (protocol) 
        {
            case EncodingProtocol::SGTIN_96: 
            {
                Log::logger().info("Using SGTIN-96 encoding");
                newEpc.epcByteCount = 12;
                break;
            }
            case EncodingProtocol::DSGTIN_128: 
            {
                Log::logger().info("Using DSGTIN-128 encoding");
                newEpc.epcByteCount = 16;
                break;
            }
            default:
                Log::logger().warn("Unsupported encoding protocol");
                break;
        }

        
        TMR_Status readerReturnStatus;
        TMR_GEN2_WriteMode writeMode = TMR_GEN2_BLOCK_ONLY;

        initializeReader(&RFIDReader, readerURI.c_str());
        connectReader(&RFIDReader);
        getReaderInfo(&RFIDReader, &RFIDReaderInfo);
        configureReader(&RFIDReader, settings.getDefaultReadPowerFromConfig(), settings.getDefaultWritePowerFromConfig());
        setMetadataFlags(&RFIDReader);
        configureReadPlan(&RFIDReader);

        // Init transport listener
        RFIDTransportListenerBlock.listener  = RFIDTransportListenerCallback;
        RFIDTransportListenerBlock.cookie    = NULL; 
        readerReturnStatus                   = TMR_addTransportListener(&RFIDReader, &RFIDTransportListenerBlock);
        if (readerReturnStatus != TMR_SUCCESS) 
        {
            throw RFIDException("Failed to initialize transport listener");
            upgSuccess = false;
        }

        // Set block write mode
        readerReturnStatus  = TMR_paramSet(&RFIDReader, TMR_PARAM_GEN2_WRITEMODE, &writeMode);
        if (readerReturnStatus != TMR_SUCCESS)
        {
            throw RFIDException("Failed to set block write mode");
            upgSuccess = false;
        }

        // Initialize write operation
        readerReturnStatus = TMR_TagOp_init_GEN2_WriteTag(&writeOp, &newEpc);
        if (readerReturnStatus != TMR_SUCCESS)
        {
            throw RFIDException("Failed to initialize write operation");
            upgSuccess = false;
        }

        // Initialize read operation
        readerReturnStatus = TMR_TagOp_init_GEN2_ReadData(&readOp, TMR_GEN2_BANK_EPC, 0, newEpc.epcByteCount / 2 + 2);
        if (readerReturnStatus != TMR_SUCCESS)
        {
            throw RFIDException("Failed to initialize read operation");
            upgSuccess = false;
        }

        setGPIOPinDirection(&RFIDReader, 1, OUTPUT);
        setGPIOPinDirection(&RFIDReader, 2, OUTPUT);
        setGPIOPinDirection(&RFIDReader, 3, OUTPUT);
        setGPIOPinDirection(&RFIDReader, 4, OUTPUT);

        //startHeartbeatLEDThread();

        upgradeInProgress = false;
        

        return 1;    
    }
    catch(...)
    {
        Log::logger().info("Exception thrown in upgradeFirmware()");
        isInitialized = false;
        moduleConnected = false;
        upgradeInProgress = false;
        upgSuccess = false;
    }
    
    
}

void fetchAndPrintSupportedRegions(TMR_Reader* rp) {
    Log::logger().info("inside fetch and print support");
    TMR_Status ret;
    TMR_RegionList regions;
    TMR_Region _regionStore[32];
    regions.list = _regionStore;
    regions.max = sizeof(_regionStore) / sizeof(_regionStore[0]);
    regions.len = 0;

    ret = TMR_paramGet(rp, TMR_PARAM_REGION_SUPPORTEDREGIONS, &regions);
    if (TMR_SUCCESS != ret) {
        Log::logger().error("Error fetching supported regions: {}", TMR_strerr(rp, ret));
        return;
    }

    if (regions.len < 1) {   
        Log::logger().info("No supported regions found.");
        return;
    }

    Log::logger().info("Supported Regions:");
    for (uint32_t i = 0; i < regions.len; i++) {
        Log::logger().info("{:2}: TMR_REGION_{:02}", i + 1, static_cast<int>(regions.list[i]));
    }
}


enum CalibrationState 
{
    CAL_INIT = 0,
    CAL_READ_POWER,
    CAL_CALCULATE_READ_POWER,
    CAL_WRITE_POWER,
    CAL_CALCULATE_WRITE_POWER,
    CAL_FINALIZE,
    CAL_EXIT
};

void incrementEPCData(std::string& epc) 
{
    // Loop backwards starting from the very last character
    for (int i = epc.length() - 1; i >= 0; --i) {
        if (epc[i] == 'f') {
            epc[i] = '0'; // Rollover 'f' to '0' and continue loop to carry the 1
        } else if (epc[i] == 'F') {
            epc[i] = '0'; // Handle uppercase if needed
        } else if (epc[i] == '9') {
            epc[i] = 'a'; // Move from numbers to hex letters (change to 'A' if uppercase)
            break;        // No carry needed, stop here
        } else {
            epc[i]++;     // Increment 0-8 or a-e normally
            break;        // No carry needed, stop here
        }
    }
}

int autoCalibrateReadPower(TMR_Reader *rp, std::atomic<bool> &calibrateStopFlag)
{
    try
    {
        readCalibrationTimingProfiler.Start();

        Config& settings = Config::getInstance();
        int readPower = MIN_RFID_POWER;
        uint32_t readPowerIncrement = settings.getReadTIDRetryPowerIncrementFromConfig();
        unsigned char calState = 0;
        constexpr bool READ_SUCCESS = true;
        constexpr bool READ_FAILURE = false;
        std::vector<int> numberOfTagsRead;
        std::vector<bool> readResults;
        uint32_t calibratedReadPower = 0;
        calSuccess = true;

        while(calState != CAL_EXIT)
        {
            //stopHeartbeatLEDThread();

            switch (calState) 
            {
                case CAL_INIT: 
                {
                    Log::logger().printSectionHeader("CAL_INIT");

                    Log::logger().printSectionHeader("setMetaDataFlags()");
        
                    setMetadataFlags(&RFIDReader);

                    Log::logger().printSectionHeader("configureReadPlan()");

                    configureReadPlan(&RFIDReader);

                    //set initial read and write power
                    setReadPower(rp, readPower);

                    calibrationInProgress = true;                

                    //set calState to CAL_READ_POWER to start read power calibration
                    calState = CAL_READ_POWER; 

                    break;
                }
                case CAL_READ_POWER:
                {
                    Log::logger().printSectionHeader("CAL_READ_POWER");

                    for (unsigned int power = MIN_RFID_POWER; power <= MAX_RFID_POWER; power += readPowerIncrement) 
                    {
                        //set read power
                        setReadPower(rp, power);

                        //read tags
                        std::vector<Tag> tags = readTags(rp);
                        
                        if(tags.size() == 1)
                        {
                            //read was a success, only one tag was read during read timeout window
                            readResults.push_back(READ_SUCCESS);
                            numberOfTagsRead.push_back(tags.size());
                        }
                        else
                        {
                            //read was a failure, either no tags were read or more than one tag was read
                            readResults.push_back(READ_FAILURE);
                            numberOfTagsRead.push_back(tags.size());
                        }
                        
                        //Log::logger().printSeparator();
                        Log::logger().printKeyValue("Read Power Increment", std::to_string(readPowerIncrement));
                        Log::logger().printKeyValue("Read Power", std::to_string(power));
                        

                        if(tags.size() > 0)
                        {
                            if(tags[0].readCount != 0)
                            {
                                Log::logger().printKeyValue("Number of Tags Read", std::to_string(tags.size()));
                                Log::logger().printKeyValue("Read count of strongest tag", std::to_string(tags[0].readCount));
                            }
                            else
                            {
                                Log::logger().printKeyValue("Number of Tags Read", std::to_string(0));
                            }
                        }
                    }

                    //print success count
                    unsigned int successCount = 0;
                    for(unsigned int i = 0; i < readResults.size(); i++)
                    {
                        if(readResults[i] == READ_SUCCESS)
                        {
                            successCount++;
                        }
                    }
                    
                    Log::logger().printSeparator();
                    Log::logger().printKeyValue("Success Count", std::to_string(successCount));    
                    Log::logger().printSeparator();

                    //set calState to CAL_CALCULATE_READ_POWER to calculate the optimal read power based on the results
                    calState = CAL_CALCULATE_READ_POWER;
                    
                    break;
                }
                case CAL_CALCULATE_READ_POWER:
                {
                    Log::logger().printSectionHeader("CAL_CALCULATE_READ_POWER");

                    if(readResults.size() > 1)
                    {
                        uint32_t highestSuccessfulRead = MIN_RFID_POWER;
                        uint32_t lowestSuccessfulRead = MAX_RFID_POWER;

                        for(int i = 0; i < (readResults.size()); i++)
                        {
                            if(readResults[i] == READ_SUCCESS)
                            {
                                if(i > highestSuccessfulRead)
                                {
                                    highestSuccessfulRead = i;
                                }
                            }
                        }

                        for(int i = 1; i < (readResults.size()); i++)
                        {
                            if(readResults[i] == READ_SUCCESS)
                            {
                                if(i < lowestSuccessfulRead)
                                {
                                    lowestSuccessfulRead = i;
                                }
                            }
                        }

                        calibratedReadPower = ((highestSuccessfulRead + lowestSuccessfulRead) / 2) * readPowerIncrement;
                        setReadPower(rp, calibratedReadPower);

                        readPowerCalCheckFlag = true;
                        bool calFinalizeFlag = false;
                        bool calFinalizeFailureFlag = false;

                        while(calFinalizeFlag == false)
                        {
                            //read tags
                            std::vector<Tag> tags = readTags(rp);

                            if(tags.size() == 1)
                            {
                                if(tags[0].readCount != 0)
                                {
                                    Log::logger().printKeyValue("Number of Tags Read", std::to_string(tags.size()));
                                    Log::logger().printKeyValue("Read count of strongest tag", std::to_string(tags[0].readCount));
                                    Log::logger().printKeyValue("RSSI of strongest tag", std::to_string(tags[0].rssi));

                                    calFinalizeFlag = true;
                                }
                                else
                                {
                                    Log::logger().info("Failed to read tag - read cal FAILURE");

                                    calFinalizeFlag = true;
                                    calFinalizeFailureFlag = true;
                                    calSuccess = false;
                                }
                            }
                            else
                            {
                                if(calibratedReadPower - readPowerIncrement >= MIN_RFID_POWER)
                                {
                                    calibratedReadPower = calibratedReadPower - readPowerIncrement;
                                    setReadPower(rp, calibratedReadPower);
                                }
                                else
                                {
                                    Log::logger().info("minimum read power reached - read cal FAILURE");

                                    calFinalizeFlag = true;
                                    calFinalizeFailureFlag = true;
                                }  
                            }
                        }
                        
                        readPowerCalCheckFlag = false;



                        Log::logger().info("Calculate read power cal succeeded");
                    }
                    else
                    {
                        calibratedReadPower = settings.getDefaultReadPowerFromConfig();
                        setReadPower(rp, settings.getDefaultReadPowerFromConfig());
                        Log::logger().info("Calculate read power cal failed");
                        calSuccess = false;
                    }

                    //set calState to CAL_FINALIZE
                    calState = CAL_FINALIZE;
                    
                    break;
                }
                case CAL_FINALIZE:
                {
                    Log::logger().printSectionHeader("CAL_FINALIZE");

                    Log::logger().printKeyValue("Calibrated read power", std::to_string(calibratedReadPower));
        
                    setMetadataFlags(&RFIDReader);

                    configureReadPlan(&RFIDReader);

                    //set initial read and write power
                    setReadPower(rp, calibratedReadPower);

                    calibrationInProgress = false;

                    isReadCalibrated = true;

                    if(isReadCalibrated == true && isWriteCalibrated == true)
                    {
                        isCalibrated = true;
                    }

                    Config& config = Config::getInstance();

                    config.setDefaultReadPower(calibratedReadPower);

                    config.save();

                    readCalibrationTimingProfiler.Stop();

                    Log::logger().printKeyValue("RFID Read Calibration Call count", std::to_string(readCalibrationTimingProfiler.CallCount()));
                    Log::logger().printKeyValue("RFID Read Calibration Last Call Duration In Seconds", std::to_string(readCalibrationTimingProfiler.LastS()));
                    Log::logger().printKeyValue("RFID Read Calibration Last Call Duration In mS", std::to_string(readCalibrationTimingProfiler.LastMs()));
                    Log::logger().printKeyValue("RFID Read Calibration Last Call Duration In uS", std::to_string(readCalibrationTimingProfiler.LastUs()));
                    Log::logger().printKeyValue("RFID Read Calibration Average Call Duration uS", std::to_string(readCalibrationTimingProfiler.AverageUs()));
                    Log::logger().printKeyValue("RFID Read Calibration Minimum Call Duration uS", std::to_string(readCalibrationTimingProfiler.MinUs()));
                    Log::logger().printKeyValue("RFID Read Calibration Maximum Call Duration uS", std::to_string(readCalibrationTimingProfiler.MaxUs()));

                    //startHeartbeatLEDThread();

                    return 0;
                }
                case CAL_EXIT:
                {
                    Log::logger().printSectionHeader("CAL_EXIT");

                    return 0;
                }
                default:
                {
                    Log::logger().printSectionHeader("Invalid calState");

                    return 0;
                }
            }
        }   
    }
    catch(...)
    {
        Log::logger().info("Exception thrown in autoCalibrateReadPower()");
        isInitialized = false;
        moduleConnected = false;
        calSuccess = false;
        calibrationInProgress = false;
    }
}

void createReadCalibrateThread()
{
    calibrateStopFlag.store(false);
    std::thread calibrateThread(autoCalibrateReadPower, &RFIDReader, std::ref(calibrateStopFlag));
    calibrateThreadID = calibrateThread.get_id();
    calibrateThread.detach();
}

void startReadCalibrateThread()
{
    calibrateStopFlag.store(false);
}

void stopReadCalibrateThread()
{
    calibrateStopFlag.store(true);
}

void createUpgradeThread()
{
    upgradeStopFlag.store(false);
    std::thread upgradeThread(upgradeFirmware, &RFIDReader, std::ref(upgradeStopFlag));
    upgradeThreadID = upgradeThread.get_id();
    upgradeThread.detach();
}

void startUpgradeThread()
{
    upgradeStopFlag.store(false);
}

void stopUpgradeThread()
{
    upgradeStopFlag.store(true);
}

int autoCalibrateWritePower(TMR_Reader *rp)
{
    writeCalibrationTimingProfiler.Start();

    Config& settings = Config::getInstance();
    //int readPower = MIN_RFID_POWER;
    int writePower = MAX_RFID_POWER;
    //uint32_t readPowerIncrement = settings.getReadTIDRetryPowerIncrementFromConfig();
    uint32_t writePowerIncrement = settings.getWriteEPCRetryPowerIncrementFromConfig();
    unsigned char calState = 0;
    //constexpr bool READ_SUCCESS = true;
    //constexpr bool READ_FAILURE = false;
    //std::vector<int> numberOfTagsRead;
    //std::vector<bool> readResults;
    constexpr bool WRITE_SUCCESS = true;
    constexpr bool WRITE_FAILURE = false;
    std::vector<double> writeTimes;
    std::vector<bool> writeResults;
    //uint32_t calibratedReadPower = 0;
    uint32_t calibratedWritePower = 2400;

    while(calState != CAL_EXIT)
    {
        switch (calState) 
        {
            case CAL_INIT: 
            {
                Log::logger().printSectionHeader("CAL_INIT");

                //set initial read and write power
                //setReadPower(rp, readPower);
                setWritePower(rp, writePower);

                calibrationInProgress = true;
                
                //set calState to CAL_WRITE_POWER to start write power calibration
                calState = CAL_WRITE_POWER; 

                break;
            }
            case CAL_WRITE_POWER:
            {
                Log::logger().printSectionHeader("CAL_WRITE_POWER");

                uint32_t tempWriteTime = 0;

                for (unsigned int power = MAX_RFID_POWER; power > MIN_RFID_POWER; power -= writePowerIncrement) 
                {
                    int writeResult = 0;

                    //set write power
                    setWritePower(rp, power);
                    tempWriteTime = 0;

                    for(int i = 0; i < settings.getWriteEPCRetryCountFromConfig(); i++)
                    {
                        //start profiler to measure the amount of time taken to succesfully write to tag
                        calibrationProfiler.Start();
                        
                        //attempt a tag write
                        writeResult = writeTag(&RFIDReader, g_tagList, currentEPCDataToWrite);

                        incrementEPCData(currentEPCDataToWrite);
                        incrementEPCData(calString);
                        
                        //stop profiler
                        calibrationProfiler.Stop();

                        tempWriteTime += calibrationProfiler.LastUs();
                    }

                    tempWriteTime = tempWriteTime / settings.getWriteEPCRetryCountFromConfig();
                    
                    if(writeResult == RESULT_SUCCESS)
                    {
                        //tag was successfully written, record the success result and time taken
                        writeResults.push_back(WRITE_SUCCESS);
                        writeTimes.push_back(tempWriteTime);
                    }
                    else
                    {
                        //tag write was a failure, record the failure result and set time taken to 0
                        writeResults.push_back(WRITE_FAILURE);
                        writeTimes.push_back(0);
                    }

                    Log::logger().printSeparator();
                    Log::logger().printKeyValue("Write Power Increment", std::to_string(writePowerIncrement));
                    Log::logger().printKeyValue("Write Power", std::to_string(power));
                    Log::logger().printKeyValue("Time taken", std::to_string(calibrationProfiler.LastUs()));
                    Log::logger().printSeparator();
                }

                unsigned int successCount = 0;
                for(unsigned int i = 0; i < writeResults.size(); i++)
                {
                    if(writeResults[i] == WRITE_SUCCESS)
                    {
                        if(writeTimes[i] < 500000) //if time taken to write to tag is less than the limit, consider it a success
                        {
                            successCount++;
                        }
                    }
                }

                Log::logger().printSeparator();
                Log::logger().printKeyValue("Power levels with writes faster than 500000 μs", std::to_string(successCount));
                Log::logger().printSeparator();

                //set calState to CAL_CALCULATE_WRITE_POWER to calculate the optimal write power based on the results
                calState = CAL_CALCULATE_WRITE_POWER;

                break;
            }
            case CAL_CALCULATE_WRITE_POWER:
            {
                Log::logger().printSectionHeader("CAL_CALCULATE_WRITE_POWER");

                if(writeResults.size() > 0)
                {
                    uint32_t fastestWriteTime = 0xFFFFFFFF;
                    uint32_t fastestWritePower = 0;
                    
                    for(int i = 1; i < (writeResults.size()); i++)
                    {
                        if(writeResults[i] == WRITE_SUCCESS)
                        {
                            if(writeTimes[i] < fastestWriteTime)
                            {
                                fastestWriteTime = writeTimes[i];
                                fastestWritePower = (i * writePowerIncrement);
                            }
                        }
                    }

                    calibratedWritePower = fastestWritePower;
                    setWritePower(rp, calibratedWritePower);
                    Log::logger().info("Calculate write power cal succeeded");
                }
                else
                {
                    calibratedWritePower = settings.getDefaultWritePowerFromConfig();
                    setWritePower(rp, calibratedWritePower);
                    Log::logger().info("Calculate write power cal failed");
                }

                //set calState to CAL_FINALIZE to finalize the calibration process
                calState = CAL_FINALIZE;
                
                break;
            }
            case CAL_FINALIZE:
            {
                Log::logger().printSectionHeader("CAL_FINALIZE");

                //Log::logger().printKeyValue("Calibrated read power", std::to_string(calibratedReadPower));
                Log::logger().printKeyValue("Calibrated write power", std::to_string(calibratedWritePower));

                calibrationInProgress = false;

                isWriteCalibrated = true;

                if(isReadCalibrated == true && isWriteCalibrated == true)
                {
                    isCalibrated = true;
                }

                writeCalibrationTimingProfiler.Stop();

                Log::logger().printKeyValue("RFID Write Calibration Call count", std::to_string(writeCalibrationTimingProfiler.CallCount()));
                Log::logger().printKeyValue("RFID Write Calibration Last Call Duration In Seconds", std::to_string(writeCalibrationTimingProfiler.LastS()));
                Log::logger().printKeyValue("RFID Write Calibration Last Call Duration In mS", std::to_string(writeCalibrationTimingProfiler.LastMs()));
                Log::logger().printKeyValue("RFID Write Calibration Last Call Duration In uS", std::to_string(writeCalibrationTimingProfiler.LastUs()));
                Log::logger().printKeyValue("RFID Write Calibration Average Call Duration uS", std::to_string(writeCalibrationTimingProfiler.AverageUs()));
                Log::logger().printKeyValue("RFID Write Calibration Minimum Call Duration uS", std::to_string(writeCalibrationTimingProfiler.MinUs()));
                Log::logger().printKeyValue("RFID Write Calibration Maximum Call Duration uS", std::to_string(writeCalibrationTimingProfiler.MaxUs()));

                return 0;
            }
            case CAL_EXIT:
            {
                Log::logger().printSectionHeader("CAL_EXIT");

                return 0;
            }
            default:
            {
                Log::logger().printSectionHeader("Invalid calState");

                return 0;
            }
        }
    }
}

int autoCalibrate(TMR_Reader *rp) 
{
    calibrationTimingProfiler.Start();

    Config& settings = Config::getInstance();
    int readPower = MIN_RFID_POWER;
    int writePower = MAX_RFID_POWER;
    uint32_t readPowerIncrement = settings.getReadTIDRetryPowerIncrementFromConfig();
    uint32_t writePowerIncrement = settings.getWriteEPCRetryPowerIncrementFromConfig();
    unsigned char calState = 0;
    constexpr bool READ_SUCCESS = true;
    constexpr bool READ_FAILURE = false;
    std::vector<int> numberOfTagsRead;
    std::vector<bool> readResults;
    constexpr bool WRITE_SUCCESS = true;
    constexpr bool WRITE_FAILURE = false;
    std::vector<double> writeTimes;
    std::vector<bool> writeResults;
    uint32_t calibratedReadPower = 0;
    uint32_t calibratedWritePower = 2400;
    //std::string currentEPCDataToWrite = "FB1331370027874225800728E20FF988";
    //std::string currentEPCDataToWrite = "00000000000000000000000000000000";

    while(calState != CAL_EXIT)
    {
        switch (calState) 
        {
            case CAL_INIT: 
            {
                Log::logger().printSectionHeader("CAL_INIT");

                //set initial read and write power
                setReadPower(rp, readPower);
                setWritePower(rp, writePower);

                calibrationInProgress = true;
                
                //set calState to CAL_READ_POWER to start read power calibration
                calState = CAL_READ_POWER; 

                break;
            }
            case CAL_READ_POWER:
            {
                Log::logger().printSectionHeader("CAL_READ_POWER");

                for (unsigned int power = MIN_RFID_POWER; power <= MAX_RFID_POWER; power += readPowerIncrement) 
                {
                    //set read power
                    setReadPower(rp, power);

                    //read tags
                    std::vector<Tag> tags = readTags(rp);
                    
                    if(tags.size() == 1)
                    {
                        //read was a success, only one tag was read during read timeout window
                        readResults.push_back(READ_SUCCESS);
                        numberOfTagsRead.push_back(tags.size());
                    }
                    else
                    {
                        //read was a failure, either no tags were read or more than one tag was read
                        readResults.push_back(READ_FAILURE);
                        numberOfTagsRead.push_back(tags.size());
                    }
                    
                    Log::logger().printSeparator();
                    Log::logger().printKeyValue("Read Power Increment", std::to_string(readPowerIncrement));
                    Log::logger().printKeyValue("Read Power", std::to_string(power));
                    Log::logger().printKeyValue("Number of Tags Read", std::to_string(tags.size()));
                    if(tags.size() > 0)
                    {
                        Log::logger().printKeyValue("Read count of strongest tag", std::to_string(tags[0].readCount));
                    }
                    Log::logger().printSeparator();
                }

                //print success count
                unsigned int successCount = 0;
                for(unsigned int i = 0; i < readResults.size(); i++)
                {
                    if(readResults[i] == READ_SUCCESS)
                    {
                        successCount++;
                    }
                }
                
                Log::logger().printSeparator();
                Log::logger().printKeyValue("Success Count", std::to_string(successCount));    
                Log::logger().printSeparator();

                //set calState to CAL_CALCULATE_READ_POWER to calculate the optimal read power based on the results
                calState = CAL_CALCULATE_READ_POWER;
                
                break;
            }
            case CAL_CALCULATE_READ_POWER:
            {
                Log::logger().printSectionHeader("CAL_CALCULATE_READ_POWER");

                if(readResults.size() > 1)
                {
                    uint32_t highestSuccessfulRead = MIN_RFID_POWER;
                    uint32_t lowestSuccessfulRead = MAX_RFID_POWER;

                    for(int i = 0; i < (readResults.size()); i++)
                    {
                        if(readResults[i] == READ_SUCCESS)
                        {
                            if(i > highestSuccessfulRead)
                            {
                                highestSuccessfulRead = i;
                            }
                        }
                    }

                    for(int i = 1; i < (readResults.size()); i++)
                    {
                        if(readResults[i] == READ_SUCCESS)
                        {
                            if(i < lowestSuccessfulRead)
                            {
                                lowestSuccessfulRead = i;
                            }
                        }
                    }

                    calibratedReadPower = ((highestSuccessfulRead + lowestSuccessfulRead) / 2) * readPowerIncrement;
                    setReadPower(rp, calibratedReadPower);
                    Log::logger().info("Calculate read power cal succeeded");
                }
                else
                {
                    calibratedReadPower = settings.getDefaultReadPowerFromConfig();
                    setReadPower(rp, settings.getDefaultReadPowerFromConfig());
                    Log::logger().info("Calculate read power cal failed");
                }

                //set calState to CAL_WRITE_POWER to start the write power calibration
                calState = CAL_WRITE_POWER;
                
                break;
            }
            case CAL_WRITE_POWER:
            {
                Log::logger().printSectionHeader("CAL_WRITE_POWER");

                uint32_t tempWriteTime = 0;

                for (unsigned int power = MAX_RFID_POWER; power > MIN_RFID_POWER; power -= writePowerIncrement) 
                {
                    int writeResult = 0;

                    //set write power
                    setWritePower(rp, power);
                    tempWriteTime = 0;

                    for(int i = 0; i < settings.getWriteEPCRetryCountFromConfig(); i++)
                    {
                        //start profiler to measure the amount of time taken to succesfully write to tag
                        calibrationProfiler.Start();
                        
                        //attempt a tag write
                        writeResult = writeTag(&RFIDReader, g_tagList, currentEPCDataToWrite);

                        incrementEPCData(currentEPCDataToWrite);
                        incrementEPCData(calString);
                        
                        //stop profiler
                        calibrationProfiler.Stop();

                        tempWriteTime += calibrationProfiler.LastUs();
                    }

                    tempWriteTime = tempWriteTime / settings.getWriteEPCRetryCountFromConfig();
                    
                    if(writeResult == RESULT_SUCCESS)
                    {
                        //tag was successfully written, record the success result and time taken
                        writeResults.push_back(WRITE_SUCCESS);
                        writeTimes.push_back(tempWriteTime);
                    }
                    else
                    {
                        //tag write was a failure, record the failure result and set time taken to 0
                        writeResults.push_back(WRITE_FAILURE);
                        writeTimes.push_back(0);
                    }

                    Log::logger().printSeparator();
                    Log::logger().printKeyValue("Write Power Increment", std::to_string(writePowerIncrement));
                    Log::logger().printKeyValue("Write Power", std::to_string(power));
                    Log::logger().printKeyValue("Time taken", std::to_string(calibrationProfiler.LastUs()));
                    Log::logger().printSeparator();
                }

                unsigned int successCount = 0;
                for(unsigned int i = 0; i < writeResults.size(); i++)
                {
                    if(writeResults[i] == WRITE_SUCCESS)
                    {
                        if(writeTimes[i] < 500000) //if time taken to write to tag is less than the limit, consider it a success
                        {
                            successCount++;
                        }
                    }
                }

                Log::logger().printSeparator();
                Log::logger().printKeyValue("Power levels with writes faster than 500000 μs", std::to_string(successCount));
                Log::logger().printSeparator();

                //set calState to CAL_CALCULATE_WRITE_POWER to calculate the optimal write power based on the results
                calState = CAL_CALCULATE_WRITE_POWER;

                break;
            }
            case CAL_CALCULATE_WRITE_POWER:
            {
                Log::logger().printSectionHeader("CAL_CALCULATE_WRITE_POWER");

                if(writeResults.size() > 0)
                {
                    uint32_t fastestWriteTime = 0xFFFFFFFF;
                    uint32_t fastestWritePower = 0;
                    
                    for(int i = 1; i < (writeResults.size()); i++)
                    {
                        if(writeResults[i] == WRITE_SUCCESS)
                        {
                            if(writeTimes[i] < fastestWriteTime)
                            {
                                fastestWriteTime = writeTimes[i];
                                fastestWritePower = (i * writePowerIncrement);
                            }
                        }
                    }

                    calibratedWritePower = fastestWritePower;
                    setWritePower(rp, calibratedWritePower);
                    Log::logger().info("Calculate write power cal succeeded");
                }
                else
                {
                    calibratedWritePower = settings.getDefaultWritePowerFromConfig();
                    setWritePower(rp, calibratedWritePower);
                    Log::logger().info("Calculate write power cal failed");
                }

                //set calState to CAL_FINALIZE to finalize the calibration process
                calState = CAL_FINALIZE;
                
                break;
            }
            case CAL_FINALIZE:
            {
                Log::logger().printSectionHeader("CAL_FINALIZE");

                Log::logger().printKeyValue("Calibrated read power", std::to_string(calibratedReadPower));
                Log::logger().printKeyValue("Calibrated write power", std::to_string(calibratedWritePower));

                calibrationInProgress = false;

                isCalibrated = true;
                isWriteCalibrated = true;
                isReadCalibrated = true;

                calibrationTimingProfiler.Stop();

                Log::logger().printKeyValue("RFID Calibration Call count", std::to_string(calibrationTimingProfiler.CallCount()));
                Log::logger().printKeyValue("RFID Calibration Last Call Duration In Seconds", std::to_string(calibrationTimingProfiler.LastS()));
                Log::logger().printKeyValue("RFID Calibration Last Call Duration In mS", std::to_string(calibrationTimingProfiler.LastMs()));
                Log::logger().printKeyValue("RFID Calibration Last Call Duration In uS", std::to_string(calibrationTimingProfiler.LastUs()));
                Log::logger().printKeyValue("RFID Calibration Average Call Duration uS", std::to_string(calibrationTimingProfiler.AverageUs()));
                Log::logger().printKeyValue("RFID Calibration Minimum Call Duration uS", std::to_string(calibrationTimingProfiler.MinUs()));
                Log::logger().printKeyValue("RFID Calibration Maximum Call Duration uS", std::to_string(calibrationTimingProfiler.MaxUs()));

                return 0;
            }
            case CAL_EXIT:
            {
                Log::logger().printSectionHeader("CAL_EXIT");

                return 0;
            }
            default:
            {
                Log::logger().printSectionHeader("Invalid calState");

                return 0;
            }
        }
    }
}

void manageConfiguration(Config& settings) {
    std::string config_path;

#ifdef COMPILE_AS_EXECUTABLE
    config_path = "rfid-hook/rfid_config.ini";
#else
    config_path = "/usr/share/rfid-hook/rfid_config.ini";
#endif
    
    // Log::logger().debug("Attempting to load configuration from: {}", config_path);

    // Ensure the configuration file exists, creating it with default values if necessary
    Config::getInstance().createDefaultRFIDConfig(fs::path(config_path).parent_path().string());

    // Load the configuration
    if (!settings.loadFromFile(config_path)) {
        // Log::logger().initialize();
        // Log::logger().error("Failed to load configuration file.");
        throw RFIDException("Failed to load configuration file");
    }

    // Log::logger().info("Configuration successfully loaded from {}", config_path);
}

TMR_Status configureReaderParameters(TMR_Reader *rp, const Config& settings)
{
    TMR_Status ret;

    // Set Gen2 Q value (static Q of 4 in this example, adjust based on tag population)
    TMR_GEN2_Q q = {TMR_SR_GEN2_Q_STATIC, 4};
    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_Q, &q);
    checkerr(rp, ret, 1, "setting Gen2 Q");
    Log::logger().info("setting Gen2 Q");
    
    // Set Gen2 BLF (BackscatterLinkFrequency)
    // Try different frequencies: 250KHZ, 320KHZ, 640KHZ
    TMR_GEN2_LinkFrequency blf = TMR_GEN2_LINKFREQUENCY_250KHZ;
    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_BLF, &blf);
    checkerr(rp, ret, 1, "setting Gen2 BLF");
    Log::logger().info("setting Gen2 BLF");

    // Set Gen2 Tari (try different values: 6.25μs, 12.5μs, 25μs)
    TMR_GEN2_Tari tari = TMR_GEN2_TARI_25US;
    ret = TMR_paramSet(rp, TMR_PARAM_GEN2_TARI, &tari);
    checkerr(rp, ret, 1, "setting Gen2 Tari");
    Log::logger().info("setting Gen2 Tari");

    return TMR_SUCCESS;
}
struct PowerRSSI {
    int power;
    double avgRssi;
    std::string epc;
};

std::vector<PowerRSSI> findIdealReadPower(TMR_Reader *rp) {
    std::vector<PowerRSSI> topPowerLevels;
    const int testsPerPower = 5;
    Log::logger().printSectionHeader("Read Power Calibration");

    for (int power = 100; power <= 2400; power += 50) {
        try {
            setReadPower(rp, power);
            
            double totalRssi = 0;
            int validReadings = 0;
            int maxRssi = -100;
            std::string strongestEpc;

            for (int test = 0; test < testsPerPower; ++test) {
                auto tags = readTags(rp);
                if (!tags.empty()) {
                    const Tag& strongestTag = *std::max_element(tags.begin(), tags.end(),
                        [](const Tag& a, const Tag& b) { return a.rssi < b.rssi; });

                    totalRssi += strongestTag.rssi;
                    validReadings++;

                    if (strongestTag.rssi > maxRssi) {
                        maxRssi = strongestTag.rssi;
                        strongestEpc = strongestTag.idStr;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (validReadings > 0) {
                double avgRssi = totalRssi / validReadings;

                Log::logger().printKeyValue("Power", std::to_string(power));
                Log::logger().printKeyValue("Avg RSSI", std::to_string(avgRssi));
                Log::logger().printKeyValue("Max RSSI", std::to_string(maxRssi));
                Log::logger().printKeyValue("Strongest Tag EPC", strongestEpc);

                topPowerLevels.push_back({power, avgRssi, strongestEpc});
                std::sort(topPowerLevels.begin(), topPowerLevels.end(),
                    [](const PowerRSSI& a, const PowerRSSI& b) { return a.avgRssi > b.avgRssi; });

                if (topPowerLevels.size() > 5) {
                    topPowerLevels.resize(5);
                }
            } else {
                Log::logger().printKeyValue("Power", std::to_string(power));
                Log::logger().info("No tags read at this power level");
            }
        } catch (const std::exception& e) {
            Log::logger().error("Error at power level {}: {}", power, e.what());
        }
    }

    Log::logger().printSectionHeader("Calibration Results");
    Log::logger().info("Top 5 Power Levels:");
    for (size_t i = 0; i < topPowerLevels.size(); ++i) {
        Log::logger().info("Rank {}: Power = {} dBm, Avg RSSI = {:.2f}, EPC = {}", 
                           i+1, topPowerLevels[i].power, topPowerLevels[i].avgRssi, topPowerLevels[i].epc);
    }

    return topPowerLevels;
}

// Metrics struct to hold calculated values
struct Metrics {
    double avgRSSI;
    double avgReadCount;
    int failedReads;
    int failedWrites;
    bool writeSuccess;
};

// Static variables to track cumulative metrics for strongest tag across runs
static int cumulativeRSSI = 0;
static int cumulativeReadCount = 0;
static int cumulativeFailedReads = 0;
static int cumulativeFailedWrites = 0;
static int runCount = 0;

// Helper function to update cumulative metrics with strongest tag and calculate averages
Metrics calculateMetrics(const Tag* strongestTag, int failedReads, int failedWrites, bool writeSuccess) {
    // Update cumulative totals if we have a strongest tag
    if (strongestTag != nullptr) {
        cumulativeRSSI += strongestTag->rssi;
        cumulativeReadCount += strongestTag->readCount;
        runCount++;
    }

    cumulativeFailedReads += failedReads;
    cumulativeFailedWrites += failedWrites;
    
    // Calculate averages from cumulative data
    double avgRSSI = (runCount > 0) ? (double)cumulativeRSSI / runCount : 0.0;
    double avgReadCount = (runCount > 0) ? (double)cumulativeReadCount / runCount : 0.0;
    
    return {avgRSSI, avgReadCount, cumulativeFailedReads, cumulativeFailedWrites, writeSuccess};
}

std::pair<int, std::string> run(std::string barcode) 
{
    std::string writtenEpc = "";  // This will hold the EPC data to return
    std::string newEpcStr = "";
    int writeResult = 0;

    writeResult = writeTag(&RFIDReader, g_tagList, barcode);

    if(writeResult != 0)
    {
        Log::logger().error("Failed to write to tag with error code {}", writeResult);
        return {1, writtenEpc};  // Return 0 only if everything succeeds
    }
    else
    {
        writtenEpc = barcode;  // Set the written EPC to return
        return {0, writtenEpc};  // Return 0 only if everything succeeds
    }
}

// Function to convert a decimal uint64_t to a string
std::string decimalToString(uint64_t decimalValue) {
    return std::to_string(decimalValue);
}

#if 0
void testGenerateEpcRfid()
{
    // Test SGTIN-96 encoding
    unsigned char sgtin[12] = {0};
    
    encode_sgtin96(676280, 2708, 258014083222, sgtin);
    std::cout << "SGTIN-96 Test:" << std::endl;
    std::cout << "Expected: 30342946e002a53c12d67896" << std::endl;
    std::cout << "Result:   ";
    printbuffer(sgtin, sizeof(sgtin));

    // Test DSGTIN-128 encoding
    unsigned char dsgtin[16] = {0};
    encode_dsgtin128(614141999996, 230630, 316045462, dsgtin);
    std::cout << "\nDSGTIN-128 Test:" << std::endl;
    std::cout << "Expected: fb132ede006141419999962812d67896" << std::endl;
    std::cout << "Result:   ";
    printbuffer(dsgtin, sizeof(dsgtin));


    // Test generateEpcRfid with multiple inputs
    struct LogEntry
    {
        std::string barcode;
        std::string serialHex;
    };

    LogEntry logs[] = {
        {"278742102980", "0293c03c16c"},
        {"278742102980", "0293c03c14c"},
        {"278742102980", "0293c03c15c"}
    };

    std::cout << "\nGenerateEpcRfid Tests:" << std::endl;
    for (const auto& log : logs)
    {
        uint64_t serialDecimal = std::stoull(log.serialHex, nullptr, 16);
        std::string serialDecimalStr = std::to_string(serialDecimal);

        unsigned char result[12] = {0};
        generateEPCByBarcode(log.barcode, serialDecimalStr, result);

        std::cout << "Barcode: " << log.barcode << ", Serial (Hex): " << log.serialHex << " -> SGTIN: ";
        printbuffer(result, sizeof(result));
    }
}
#endif

#ifdef COMPILE_AS_EXECUTABLE
// Main only exists when compiled as an executable. As a .so, this will not exist.
int main(int argc, char* argv[])
{
    // auto result = run("02633948138238", "30340C3500000E00000F5820");

    // // Log the EPC string if available
    // if (!result.second.empty()) {
    //     Log::logger().info("EPC Written: {}", result.second);
    // } else {
    //     Log::logger().warn("No EPC was written or an error occurred.");
    // }

    // // Return the result code
    // return result.first;

    // testGenerateEpcRfid();

    return 0;
}
#endif

struct CustomActionData {
    std::string hookType;
    std::string fieldOffsetCfgRfidEncodeMethod;
    std::string barcode;
    std::string sellByDate;
    std::string storeId;
    std::string scaleId;
    std::string transId;
    std::string transLabelNum;
    std::vector<std::string> additionalFields;

    CustomActionData() = default;
};

CustomActionData parseCustomAction(const std::string& actionStr) {
    CustomActionData data;
    std::vector<std::string> tokens;
    std::stringstream ss(actionStr);
    std::string token;

    while (std::getline(ss, token, '|')) {
        if (token.empty())
            token = "0";
        tokens.push_back(token);
    }

    if (tokens.size() >= 8) {
        data.hookType = tokens[0];
        data.fieldOffsetCfgRfidEncodeMethod = tokens[1];
        data.barcode = tokens[2];
        data.sellByDate = tokens[3];
        data.storeId = tokens[4];
        data.scaleId = tokens[5];
        data.transId = tokens[6];
        data.transLabelNum = tokens[7];

        // Capture any additional fields beyond the 8th token
        if (tokens.size() > 8) {
            data.additionalFields.assign(tokens.begin() + 8, tokens.end());
        }
    }

    return data;
}

void logActionData(const CustomActionData& actionData) {
    Log::logger().info("Parsed Action Data:");
    Log::logger().info("-------------------");
    Log::logger().info("hookType:                       {}", actionData.hookType);
    Log::logger().info("fieldOffsetCfgRfidEncodeMethod: {}", actionData.fieldOffsetCfgRfidEncodeMethod);
    Log::logger().info("barcode:                        {}", actionData.barcode);
    Log::logger().info("sellByDate:                     {}", actionData.sellByDate);
    Log::logger().info("storeId:                        {}", actionData.storeId);
    Log::logger().info("scaleId:                        {}", actionData.scaleId);
    Log::logger().info("transId:                        {}", actionData.transId);
    Log::logger().info("transLabelNum:                  {}", actionData.transLabelNum);

    if (!actionData.additionalFields.empty()) {
        Log::logger().info("Additional Fields:");
        for (size_t i = 0; i < actionData.additionalFields.size(); ++i) {
            Log::logger().info("  Field {}: {}", i + 1, actionData.additionalFields[i]);
        }
    } else {
        Log::logger().info("No Additional Fields");
    }
}

uint64_t calculateSerialNumber(const CustomActionData& actionData) {
    int storeId = actionData.storeId.size() ? std::stoi(actionData.storeId) : 0;
    int scaleId = actionData.scaleId.size() ? std::stoi(actionData.scaleId) : 0;
    int transId = actionData.transId.size() ? std::stoi(actionData.transId) : 0;
    int transLabelNum = actionData.transLabelNum.size() ? std::stoi(actionData.transLabelNum) : 0;

    //Log::logger().info("storeId (int): {}", storeId);
    //Log::logger().info("scaleId (int): {}", scaleId);
    //Log::logger().info("transId (int): {}", transId);
    //Log::logger().info("transLabelNum (int): {}", transLabelNum);

    uint64_t serialNumber = generate_serial_number(storeId, scaleId, transId, transLabelNum);
    //Log::logger().info("Generated Serial Number: {}", std::to_string(serialNumber));

    return serialNumber;
}
std::string calculateSGTIN(const CustomActionData& actionData, uint64_t serialNumber) {

    // Convert barcode to GTIN-14
    std::string GTIN_14 = convertToGTIN14(actionData.barcode);
    //Log::logger().info("GTIN-14: {}", GTIN_14);

    // Extract company prefix and item reference
    std::string companyDataStr = GTIN_14.substr(1, 7);
    std::string itemReferenceStr = GTIN_14.substr(8, 5);
    uint64_t companyData = std::stoull(companyDataStr);
    uint64_t itemReference = std::stoull(itemReferenceStr);

    unsigned char result[12] = {0};
    encode_sgtin96(companyData, itemReference, serialNumber, result);

    // Convert the result to a hex string
    std::ostringstream hexResultStream;
    for (int i = 0; i < 12; i++) {
        hexResultStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(result[i]);
    }
    std::string hexResultString = hexResultStream.str();
    //Log::logger().info("Encoded SGTIN-96 (Hex): {}", hexResultString);

    return hexResultString;
}

std::string calculateDSGTIN(const CustomActionData& actionData, uint64_t serialNumber) {
    std::string dsgtinStr;
    try {
        unsigned char dsgtin[16] = {0};
        std::string GTIN_14 = convertToGTIN14(actionData.barcode);
        encode_dsgtin128((uint64_t)std::stoull(GTIN_14), SellByDate, (uint64_t)std::stoull(actionData.sellByDate), serialNumber, dsgtin);

        for (size_t i = 0; i < sizeof(dsgtin); ++i) {
            char buffer[3];
            std::snprintf(buffer, sizeof(buffer), "%02x", dsgtin[i]);
            dsgtinStr += buffer;
        }

        //Log::logger().info("Encoded DSGTIN (Hex): {}", dsgtinStr);
    } catch (const std::invalid_argument& e) {
        Log::logger().error("Error: {}", e.what());
    }

    return dsgtinStr;
}

void rfidHookInit(void)
{
    Config& settings = Config::getInstance();
    TMR_Status readerReturnStatus;
    TMR_GEN2_WriteMode writeMode = TMR_GEN2_BLOCK_ONLY;
    std::string config_path;
#ifdef COMPILE_AS_EXECUTABLE
    config_path = "rfid-hook/rfid_config.ini";
#else
    config_path = "/usr/share/rfid-hook/rfid_config.ini";
#endif
    TMR_TagData newEpc;

    if(!AppHookLog::getInstance().initialize()) 
    {
        throw RFIDException("Failed to initialize logger");
    }
    
    settings.initializeLogger();
    settings.loadFromFile(config_path);
    EncodingProtocol protocol = settings.getEncodingProtocol();

    switch (protocol) 
    {
        case EncodingProtocol::SGTIN_96: 
        {
            Log::logger().info("Using SGTIN-96 encoding");
            newEpc.epcByteCount = 12;
            break;
        }
        case EncodingProtocol::DSGTIN_128: 
        {
            Log::logger().info("Using DSGTIN-128 encoding");
            newEpc.epcByteCount = 16;
            break;
        }
        default:
            Log::logger().warn("Unsupported encoding protocol");
            break;
    }
    
    manageConfiguration(settings);

    Log::logger().printKeyValue("Reader URI from settings", settings.getReaderURIFromConfig());
    Log::logger().printKeyValue("Reader URI from actionData", readerURI);

    initializeReader(&RFIDReader, readerURI.c_str());
    connectReader(&RFIDReader);
    getReaderInfo(&RFIDReader, &RFIDReaderInfo);
    configureReader(&RFIDReader, settings.getDefaultReadPowerFromConfig(), settings.getDefaultWritePowerFromConfig());
    setMetadataFlags(&RFIDReader);
    configureReadPlan(&RFIDReader);

    // Init transport listener
    RFIDTransportListenerBlock.listener  = RFIDTransportListenerCallback;
    RFIDTransportListenerBlock.cookie    = NULL; 
    readerReturnStatus                   = TMR_addTransportListener(&RFIDReader, &RFIDTransportListenerBlock);
    if (readerReturnStatus != TMR_SUCCESS) 
    {
        throw RFIDException("Failed to initialize transport listener");
    }

    // Set block write mode
    readerReturnStatus  = TMR_paramSet(&RFIDReader, TMR_PARAM_GEN2_WRITEMODE, &writeMode);
    if (readerReturnStatus != TMR_SUCCESS)
    {
        throw RFIDException("Failed to set block write mode");
    }

    // Initialize write operation
    readerReturnStatus = TMR_TagOp_init_GEN2_WriteTag(&writeOp, &newEpc);
    if (readerReturnStatus != TMR_SUCCESS)
    {
        throw RFIDException("Failed to initialize write operation");
    }

    // Initialize read operation
    readerReturnStatus = TMR_TagOp_init_GEN2_ReadData(&readOp, TMR_GEN2_BANK_EPC, 0, newEpc.epcByteCount / 2 + 2);
    if (readerReturnStatus != TMR_SUCCESS)
    {
        throw RFIDException("Failed to initialize read operation");
    }

    setGPIOPinDirection(&RFIDReader, 1, OUTPUT);
    setGPIOPinDirection(&RFIDReader, 2, OUTPUT);
    setGPIOPinDirection(&RFIDReader, 3, OUTPUT);
    setGPIOPinDirection(&RFIDReader, 4, OUTPUT);
    
    //createHeartbeatLEDThread();
    //startHeartbeatLEDThread();

    isInitialized = true;
    moduleConnected = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_init()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_finalize()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_config_saved()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_before_open_database(const char* dbname)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_open_database()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_plu_read(const char* plu, const char* deptNum)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_plu_text_read(const char* number, const char* deptNum, int type)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_print(const char* plu, const char* deptNum)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_remote_print(const char* plu, const char* deptNum)
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_after_print(bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel)
{
}

extern "C" void CUSTHOOKDLL hobart_cust_hook_custom_action(const char* action, int* rc, char* rcex, unsigned int rcexLen) 
{
    //Log::logger().info("custom action command detected.");

    if (action == nullptr) 
    {
        Log::logger().info("Null pointer to action string received");
        *rc = 1; // Failure
        return;
    }

    std::string actionStr(action);
    CustomActionData actionData = parseCustomAction(actionStr); // No logging should happen

    if (actionData.hookType != UPGRADE_ACTION && actionData.hookType != WRITE_ACTION && actionData.hookType != CAL_READ_ACTION && actionData.hookType != STATUS_ACTION && actionData.hookType != QUERY_ACTION) 
    {
        //Log::logger().printKeyValue("Invalid RFID appHook command", actionStr);

        // -1 is what apphooks should return if the incoming command is not recognized. This happens when the command is not meant for this apphook
        // and by returning the -1 it tells the apphook manager to continue to call this command on other existing app hooks.
        *rc = -1;
        return;
    }

    
    if(isInitialized == false)
    {
        if(initFailedCount == 0)
        {
            Log::logger().info("RFID appHook not initialized, reader URI set to ACM0");
            readerURI = "tmr:///dev/ttyACM0";

            try
            {
                rfidHookInit();
            }
            catch (const RFIDException& e)
            {
                Log::logger().error(e.what());
                isInitialized = false;
                initFailedCount += 1;
            }
        }
        if(initFailedCount == 1)
        {
            Log::logger().info("RFID appHook not initialized, reader URI set to ACM1");
            readerURI = "tmr:///dev/ttyACM1";

            try
            {
                rfidHookInit();
            }
            catch (const RFIDException& e)
            {
                Log::logger().error(e.what());
                isInitialized = false;
                initFailedCount += 1;
            }
        }
        if(initFailedCount == 2)
        {
            Log::logger().info("RFID appHook not initialized, reader URI set to ACM2");
            readerURI = "tmr:///dev/ttyACM2";

            try
            {
                rfidHookInit();
            }
            catch (const RFIDException& e)
            {
                Log::logger().error(e.what());
                isInitialized = false;
                initFailedCount += 1;
            }
        }
        if(initFailedCount == 3)
        {
            Log::logger().info("RFID appHook not initialized, reader URI set to ACM3");
            readerURI = "tmr:///dev/ttyACM3";

            try
            {
                rfidHookInit();
            }
            catch (const RFIDException& e)
            {
                Log::logger().error(e.what());
                isInitialized = false;
                initFailedCount += 1;
            }
        }
        if(initFailedCount == 4)
        {
            Log::logger().info("RFID appHook not initialized, reader URI set to ACM4");
            readerURI = "tmr:///dev/ttyACM4";

            try
            {
                rfidHookInit();
            }
            catch (const RFIDException& e)
            {
                Log::logger().error(e.what());
                isInitialized = false;
                initFailedCount = 0;
            }
        }
        if(initFailedCount == 4)
        {
            initFailedCount = 0;
        }

        if(isInitialized == false)
        {
            *rc = 1;
            return;
        }        
    }
    

    /*
    if (!isInitialized) 
    {
        int maxACM = 10; // Define the maximum ACM value to try
        for (int i = 0; i <= maxACM; ++i) 
        {
            std::string readerURI = "tmr:///dev/ttyACM" + std::to_string(i);
            Log::logger().info("RFID appHook not initialized, reader URI set to " + readerURI);
            
            // Trim any potential leading/trailing whitespace
            readerURI.erase(std::remove_if(readerURI.begin(), readerURI.end(), isspace), readerURI.end());
            
            try 
            {
                rfidHookInit(); 
                isInitialized = true;
                break; // Exit the loop if initialization is successful
            } 
            catch (const RFIDException& e) 
            {
                Log::logger().error(e.what());
            }
        }
        if (!isInitialized) 
        {
            *rc = 1;
            return;
        }
    }
    */

    if(actionData.hookType == QUERY_ACTION)
    {
        if(calibrationInProgress == true || upgradeInProgress == true)
        {

            if(isInitialized == false || moduleConnected == false || upgSuccess == false || calSuccess == false )
            {
                *rc = 0;
            }
            else
            {
                *rc = 1;
            }   

            return;
        }

        static bool ledFlag = false;

        try
        {
            if(ledFlag == false)
            {
                ledFlag = true;

                if(moduleConnected = true)
                {
                    setGPIOPin(&RFIDReader, 4, 1);
                } 
            }
            else
            {
                ledFlag = false;

                if(moduleConnected = true)
                {
                    setGPIOPin(&RFIDReader, 4, 0);
                } 
            }
        }
        catch(...)
        {
            moduleConnected = false;
            isInitialized = false;
        }


        if(moduleConnected == true)
        {
            *rc = 2;
        }
        else
        {
            *rc = 3;

            isInitialized = false;
        }

        return;
    }

    if(actionData.hookType == STATUS_ACTION)
    {
        if(calibrationInProgress == true || upgradeInProgress == true)
        {
            //Log::logger().info("RFID appHook status call while hook busy");
            *rc = 1;
            return;
        }
        else
        {
            //Log::logger().info("RFID appHook status call while hook idle");

            if(calStarted == true)
            {
                calStarted = false;

                if(calSuccess == true)
                {
                    *rc = 2;
                }
                else
                {
                    *rc = 3;
                }
            }
            else if(upgStarted == true)
            {
                upgStarted = false;

                if(upgSuccess == true)
                {
                    *rc = 2;
                }
                else
                {
                    *rc = 3;
                }
            }

            return;
        }
    }

    *rc = 1; // Default to failure
    std::string EPC_Data;
    uint64_t serialNumber = calculateSerialNumber(actionData);
    int ret;

    if(actionData.hookType == CAL_READ_ACTION)
    {
        Log::logger().printSectionHeader("Calibrate read power command detected");

        calStarted = true;
        calibrationInProgress = true;

        //stopHeartbeatLEDThread();

        createReadCalibrateThread();
        startReadCalibrateThread();

        *rc = 1;

        return;
    }

    if(actionData.hookType == UPGRADE_ACTION)
    {
        Log::logger().printSectionHeader("Upgrade firmware command detected");
    
        upgStarted = true;
        upgradeInProgress = true;

        //stopHeartbeatLEDThread();

        createUpgradeThread();
        startUpgradeThread();

        *rc = static_cast<int>(RESULT_SUCCESS);
        
        return;
    }

    if(actionData.hookType == WRITE_ACTION)
    {
        Log::logger().printSectionHeader("Write command detected");
        
        try
        {
            runtimeProfiler.Start();

            //stopHeartbeatLEDThread();

            //barcode info
            logActionData(actionData);

            // Retrieve the encoding protocol from the configuration
            EncodingProtocol protocol = Config::getInstance().getEncodingProtocol();

            switch (protocol) {
                case EncodingProtocol::SGTIN_96: {
                    Log::logger().info("Using SGTIN-96 encoding");
                    std::string sgtinStr = calculateSGTIN(actionData, serialNumber);
                    EPC_Data = sgtinStr;
                    break;
                }
                case EncodingProtocol::DSGTIN_128: {
                    Log::logger().info("Using DSGTIN-128 encoding");
                    std::string dsgtinStr = calculateDSGTIN(actionData, serialNumber);
                    EPC_Data = dsgtinStr;
                    break;
                }
                default:
                    Log::logger().warn("Unsupported encoding protocol");
                    return;
            }

            // Call the run function and log the result
            if (EPC_Data.empty()) {
                Log::logger().error("EPC data is empty. Encoding failed.");
                *rc = 3;
                return;
            } 
            
            std::pair<int, std::string> result = run(EPC_Data);

            *rc = result.first;  // Assign the return code from run()    

            if(result.first != 0)
            {
                ++RFIDTransactionFailureCount;
            }

            Log::logger().info("Run result: Return code: {}, EPC: {}", result.first, result.second);

            // Copy EPC result into rcex if the buffer is valid
            if (rcex && rcexLen > 0) {
                strncpy(rcex, result.second.c_str(), rcexLen - 1);
                rcex[rcexLen - 1] = '\0'; // Ensure null-termination
            }

            runtimeProfiler.Stop();

            static int tagsOverLimit1 = 0;
            static int tagsOverLimit2 = 0;
            static int tagsOverLimit3 = 0;
            static int tagsOverLimit4 = 0;
            static int tagsOverLimit5 = 0;
            static int tagsOverLimit6 = 0;
            static int tagsOverLimit7 = 0;
            static int tagsOverLimit8 = 0;
            static int tagsOverLimit9 = 0;

            if(runtimeProfiler.LastUs() >= 900000)
            {
                tagsOverLimit9++;
            }

            if(runtimeProfiler.LastUs() >= 800000 && runtimeProfiler.LastUs() < 900000)
            {
                tagsOverLimit8++;
            }

            if(runtimeProfiler.LastUs() >= 700000 && runtimeProfiler.LastUs() < 800000)
            {
                tagsOverLimit7++;
            }

            if(runtimeProfiler.LastUs() >= 600000 && runtimeProfiler.LastUs() < 700000)
            {
                tagsOverLimit6++;
            }

            if(runtimeProfiler.LastUs() >= 500000 && runtimeProfiler.LastUs() < 600000)
            {
                tagsOverLimit5++;
            }

            if(runtimeProfiler.LastUs() >= 400000 && runtimeProfiler.LastUs() < 500000)
            {
                tagsOverLimit4++;
            }

            if(runtimeProfiler.LastUs() >= 300000 && runtimeProfiler.LastUs() < 400000)
            {
                tagsOverLimit3++;
            }

            if(runtimeProfiler.LastUs() >= 200000 && runtimeProfiler.LastUs() < 300000)
            {
                tagsOverLimit2++;
            }

            if(runtimeProfiler.LastUs() >= 100000 && runtimeProfiler.LastUs() < 200000)
            {
                tagsOverLimit1++;
            }

            
            

            Log::logger().printKeyValue("RFID AppHook Call count", std::to_string(runtimeProfiler.CallCount()));
            Log::logger().printKeyValue("RFID AppHook Last Call Duration uS", std::to_string(runtimeProfiler.LastUs()));
            Log::logger().printKeyValue("RFID AppHook Average Call Duration uS", std::to_string(runtimeProfiler.AverageUs()));
            Log::logger().printKeyValue("RFID AppHook Minimum Call Duration uS", std::to_string(runtimeProfiler.MinUs()));
            Log::logger().printKeyValue("RFID AppHook Maximum Call Duration uS", std::to_string(runtimeProfiler.MaxUs()));
            Log::logger().printKeyValue("Voided tags", std::to_string(RFIDTransactionFailureCount));
            Log::logger().printKeyValue("Tags > 900 mS", std::to_string(tagsOverLimit9));
            Log::logger().printKeyValue("Tags > 800 mS", std::to_string(tagsOverLimit8));
            Log::logger().printKeyValue("Tags > 700 mS", std::to_string(tagsOverLimit7));
            Log::logger().printKeyValue("Tags > 600 mS", std::to_string(tagsOverLimit6));
            Log::logger().printKeyValue("Tags > 500 mS", std::to_string(tagsOverLimit5));
            Log::logger().printKeyValue("Tags > 400 mS", std::to_string(tagsOverLimit4));
            Log::logger().printKeyValue("Tags > 300 mS", std::to_string(tagsOverLimit3));
            Log::logger().printKeyValue("Tags > 200 mS", std::to_string(tagsOverLimit2));
            Log::logger().printKeyValue("Tags > 100 mS", std::to_string(tagsOverLimit1));
        }
        catch(RFIDException &e)
        {
            Log::logger().error(e.what());
            *rc = 3;

            //moduleConnected = false;
            //isInitialized = false;

            return;
        }
    }

    //startHeartbeatLEDThread();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_peripheral_version(int dev, int devex, int kind, const char* swVer, const char* hwVer)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_scale_state(int type, const char* value)
{
}

#pragma GCC diagnostic pop 
