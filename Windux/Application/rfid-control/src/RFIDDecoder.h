#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <functional>

// RFIDDecoder.h

class RFIDDecoder {
public:
    virtual ~RFIDDecoder() = default;
    virtual std::string decode(const std::string& hexData) const = 0;
    virtual size_t getExpectedLength() const = 0;

    // New static function for direct decoding
    static std::string decodeTID(const std::string& hexData, const std::string& manufacturer) {
        auto decoder = createDecoder(manufacturer);
        return decoder->decode(hexData);
    }

    static std::string formatWithSpaces(const std::string& serial) {
        std::stringstream ss;
        for (size_t i = 0; i < serial.length(); i++) {
            ss << serial[i];
            if ((i + 1) % 2 == 0 && i < serial.length() - 1) {
                ss << " ";
            }
        }
        return ss.str();
    }

protected:
    static void validate(const std::string& hexData, size_t expectedLength) {
        if (hexData.length() != expectedLength * 2) {
            throw std::invalid_argument("Invalid input length: expected " + 
                                        std::to_string(expectedLength * 2) + 
                                        " characters, got " + std::to_string(hexData.length()));
        }
        if (!std::all_of(hexData.begin(), hexData.end(), ::isxdigit)) {
            throw std::invalid_argument("Invalid input: must contain only hexadecimal characters");
        }
    }

    static std::string bytesToHexString(const std::vector<unsigned char>& bytes) {
        std::stringstream ss;
        for (unsigned char byte : bytes) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return ss.str();
    }

    static std::vector<unsigned char> hexStringToBytes(const std::string& hexString) {
        std::vector<unsigned char> bytes;
        for (size_t i = 0; i < hexString.length(); i += 2) {
            bytes.push_back(static_cast<unsigned char>(std::stoi(hexString.substr(i, 2), nullptr, 16)));
        }
        return bytes;
    }

    template<typename T>
    static bool registerDecoder(const std::string& manufacturer) {
        getFactory()[manufacturer] = []() { return std::make_unique<T>(); };
        return true;
    }

private:
    static std::unique_ptr<RFIDDecoder> createDecoder(const std::string& manufacturer) {
        auto it = getFactory().find(manufacturer);
        if (it == getFactory().end()) {
            throw std::runtime_error("Unsupported manufacturer: " + manufacturer);
        }
        return it->second();
    }

    static std::unordered_map<std::string, std::function<std::unique_ptr<RFIDDecoder>()>>& getFactory() {
        static std::unordered_map<std::string, std::function<std::unique_ptr<RFIDDecoder>()>> factory;
        return factory;
    }
};