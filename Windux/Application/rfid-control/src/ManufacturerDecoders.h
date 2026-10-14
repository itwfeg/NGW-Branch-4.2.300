#pragma once
#include "RFIDDecoder.h"
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <vector>
#include <memory>
#include <unordered_map>

// ManufacturerDecoders.h

class ImpinjDecoder : public RFIDDecoder {
public:
    std::string decode(const std::string& hexData) const override {
        validate(hexData, getExpectedLength());

        unsigned char buffer[18];
        unsigned char result[5];
        hexStringToBuffer(hexData, buffer, sizeof(buffer));
        serialize(buffer, result);

        std::stringstream ss;
        for (size_t i = 0; i < sizeof(result); ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(result[i]);
        }
        return ss.str();
    }

    size_t getExpectedLength() const override { return 18; } // 36 hex characters = 18 bytes

private:
    static uint16_t byteswap16(unsigned char *b) {
        const int check(1);
        if (1 == *(reinterpret_cast<const char*>(&check)))
            for (int i(1); i--;)
                std::swap(b[i], b[1 - i]);
        return *reinterpret_cast<uint16_t*>(b);
    }

    static uint64_t byteswap64(unsigned char *b) {
        const int check(1);
        if (1 == *(reinterpret_cast<const char*>(&check)))
            for (int i(4); i--;)
                std::swap(b[i], b[7 - i]);
        return *reinterpret_cast<uint64_t*>(b);
    }

    static void serialize(unsigned char *b, unsigned char *result) {
        uint64_t s = 0;
        uint16_t w;
        w = byteswap16(&b[10]);
        w &= 0x1ff;
        s |= w;
        s <<= 16;
        w = byteswap16(&b[8]);
        s |= w;
        s <<= 13;
        w = byteswap16(&b[6]);
        w &= 0x1fff;
        s |= w;
        s = byteswap64(reinterpret_cast<unsigned char*>(&s));
        memcpy(&result[0], &reinterpret_cast<unsigned char*>(&s)[3], 5);
        // 38 bits of serial data in result!
    }

    static void hexStringToBuffer(const std::string& hexString, unsigned char* buffer, size_t bufferSize) {
        for (size_t i = 0; i < bufferSize; ++i) {
            std::stringstream ss;
            ss << std::hex << hexString.substr(i * 2, 2);
            int byte;
            ss >> byte;
            buffer[i] = static_cast<unsigned char>(byte);
        }
    }

    static bool registered;
};

bool ImpinjDecoder::registered = RFIDDecoder::registerDecoder<ImpinjDecoder>("Impinj");

class Manufacturer2Decoder : public RFIDDecoder {
public:
    std::string decode(const std::string& hexData) const override {
        validate(hexData, getExpectedLength());
        // Placeholder for Manufacturer2 specific decoding algorithm
        return "Manufacturer2: " + hexData;
    }

    size_t getExpectedLength() const override { return 16; } // 32 hex characters = 16 bytes

private:
    static bool registered;
};

bool Manufacturer2Decoder::registered = RFIDDecoder::registerDecoder<Manufacturer2Decoder>("Manufacturer2");

class Manufacturer3Decoder : public RFIDDecoder {
public:
    std::string decode(const std::string& hexData) const override {
        validate(hexData, getExpectedLength());
        // Placeholder for Manufacturer3 specific decoding algorithm
        return "Manufacturer3: " + hexData;
    }

    size_t getExpectedLength() const override { return 12; } // 24 hex characters = 12 bytes

private:
    static bool registered;
};

bool Manufacturer3Decoder::registered = RFIDDecoder::registerDecoder<Manufacturer3Decoder>("Manufacturer3");