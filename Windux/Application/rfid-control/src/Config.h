#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

#include "inih/cpp/INIReader.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifdef USE_GHC_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

enum class EncodingProtocol {
    SGTIN_96,
    DSGTIN_128
};

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    bool initializeLogger() {
        try {
            std::string log_directory = "/usr/share/rfid-hook";

            if (!fs::exists(log_directory)) {
                fs::create_directories(log_directory);
            }

            std::string log_file_path = log_directory + "/config.log";

            auto console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                    log_file_path, true);

            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

            m_logger = std::make_shared<spdlog::logger>(
                "config_logger", sinks.begin(), sinks.end());

            m_logger->set_level(spdlog::level::debug);
            m_logger->flush_on(spdlog::level::info);
            m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

            spdlog::set_default_logger(m_logger);

            m_logger->info(
                "Config logger initialized. Log file: {}",
                log_file_path);

            return true;
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Log initialization failed: "
                      << ex.what() << std::endl;
            return false;
        }
    }

    std::shared_ptr<spdlog::logger> getLogger() const {
        return m_logger;
    }

    bool loadFromFile(const std::string& filename) {
        if (!fs::exists(filename)) {
            return false;
        }

        return loadFromFiles({filename});
    }

    bool loadFromFiles(const std::vector<std::string>& filenames) {
        auto start = std::chrono::high_resolution_clock::now();

        for (const auto& filename : filenames) {
            INIReader reader(filename);

            if (reader.ParseError() < 0) {
                continue;
            }

            loadSettings(reader);

            {
                std::lock_guard<std::mutex> lock(mutex_);
                configFilePath_ = filename;
            }

            auto end = std::chrono::high_resolution_clock::now();

            loadTime_ =
                std::chrono::duration_cast<
                    std::chrono::duration<double>>(
                    end - start);

            return true;
        }

        std::string defaultConfigPath =
            filenames.empty()
                ? "/tmp/default_config.ini"
                : filenames[0];

        if (generateDefaultConfig(defaultConfigPath)) {
            return loadFromFiles({defaultConfigPath});
        }

        useDefaultSettings();

        loadTime_ = std::chrono::duration<double>(0);
        return false;
    }

    bool generateDefaultConfig(const std::string& filename) {
        fs::path configPath(filename);
        fs::path configDir = configPath.parent_path();

        try {
            if (configDir != "." && !fs::exists(configDir)) {
                if (!fs::create_directories(configDir)) {
                    if (m_logger) {
                        m_logger->warn(
                            "Failed to create directory: {}. "
                            "Using current directory.",
                            configDir.string());
                    }

                    configPath = fs::path(configPath.filename());
                }
            }

            std::ofstream configFile(configPath);

            if (!configFile.is_open()) {
                if (m_logger) {
                    m_logger->error(
                        "Failed to create configuration file: {}",
                        configPath.string());
                }

                return false;
            }

            writeDefaultConfig(configFile);
            configFile.close();

            if (m_logger) {
                m_logger->debug(
                    "Generated default configuration file: {}",
                    configPath.string());
            }

            return true;
        }
        catch (const std::exception& e) {
            if (m_logger) {
                m_logger->error(
                    "Error generating default config: {}",
                    e.what());
            }

            return false;
        }
    }

    void createDefaultRFIDConfig(const std::string& directoryPath) {
        std::string filePath =
            directoryPath + "/rfid_config.ini";

        if (!fs::exists(directoryPath)) {
            if (!fs::create_directories(directoryPath)) {
                return;
            }
        }

        if (!fs::exists(filePath)) {
            std::ofstream configFile(filePath);

            if (!configFile) {
                return;
            }

            writeDefaultConfig(configFile);
            configFile.close();
        }
    }

    // -------------------------------------------------------------------------
    // Getters
    // -------------------------------------------------------------------------

    std::string getReaderURIFromConfig() const {
        return getSettingValue("URI", "tmr:///dev/ttyACM0");
    }

    int getDefaultReadPowerFromConfig() const {
        return getIntSetting("DefaultReadPower", 250);
    }

    int getDefaultWritePowerFromConfig() const {
        return getIntSetting("DefaultWritePower", 2400);
    }

    int getReadTIDRetryCountFromConfig() const {
        return getIntSetting("ReadTIDRetryCount", 4);
    }

    int getReadTIDRetryPowerIncrementFromConfig() const {
        return getIntSetting("ReadTIDRetryPowerIncrement", 500);
    }

    int getWriteEPCRetryCountFromConfig() const {
        return getIntSetting("WriteEPCRetryCount", 4);
    }

    int getWriteEPCRetryPowerIncrementFromConfig() const {
        return getIntSetting("WriteEPCRetryPowerIncrement", 500);
    }

    bool getLogTransportTrafficFromConfig() const {
        return getBoolSetting("TransportTraffic", false);
    }

    std::string getLogLevelFromConfig() const {
        return getSettingValue("Level", "INFO");
    }

    bool getColoredOutputLogFromConfig() const {
        return getBoolSetting("ColoredOutput", true);
    }

    bool getAutoCalibrateFromConfig() const {
        return getBoolSetting("AutoCalibrate", false);
    }

    bool getReadAutoCalibrateFromConfig() const {
        return getBoolSetting("ReadAutoCalibrate", false);
    }

    bool getWriteAutoCalibrateFromConfig() const {
        return getBoolSetting("WriteAutoCalibrate", false);
    }

    bool getAutoCalibrateOnSizingFromConfig() const {
        return getBoolSetting("AutoCalibrateOnSizing", false);
    }

    bool getAutoCalibrateOnInitFromConfig() const {
        return getBoolSetting("AutoCalibrateOnInit", false);
    }

    bool getReadAutoCalibrateOnSizingFromConfig() const {
        return getBoolSetting("ReadAutoCalibrateOnSizing", false);
    }

    bool getReadAutoCalibrateOnInitFromConfig() const {
        return getBoolSetting("ReadAutoCalibrateOnInit", false);
    }

    bool getWriteAutoCalibrateOnSizingFromConfig() const {
        return getBoolSetting("WriteAutoCalibrateOnSizing", false);
    }

    bool getWriteAutoCalibrateOnInitFromConfig() const {
        return getBoolSetting("WriteAutoCalibrateOnInit", false);
    }

    int getRegionFromConfig() const {
        return getIntSetting("Region", 1);
    }

    int getReadTimeFromConfig() const {
        return getIntSetting("ReadTime", 500);
    }

    int getCommandTimeoutFromConfig() const {
        return getIntSetting("CommandTimeout", 1000);
    }

    int getReadTimeoutFromConfig() const {
        return getIntSetting("ReadTimeout", 1000);
    }

    double getLoadTime() const {
        return loadTime_.count();
    }

    EncodingProtocol getEncodingProtocol() const {
        std::string protocolStr =
            getSettingValue("EncodingProtocol", "SGTIN_96");

        if (protocolStr == "SGTIN_96") {
            return EncodingProtocol::SGTIN_96;
        }

        if (protocolStr == "DSGTIN_128") {
            return EncodingProtocol::DSGTIN_128;
        }

        if (m_logger) {
            m_logger->warn(
                "Unknown EncodingProtocol '{}', "
                "defaulting to SGTIN_96",
                protocolStr);
        }

        return EncodingProtocol::SGTIN_96;
    }

    // -------------------------------------------------------------------------
    // Generic setter
    //
    // Changes the in-memory setting. It does NOT write to disk until save()
    // or saveToFile() is called.
    // -------------------------------------------------------------------------

    bool setSettingValue(
        const std::string& key,
        const std::string& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        settings_[key] = value;
        return true;
    }

    // -------------------------------------------------------------------------
    // Typed setters
    // -------------------------------------------------------------------------

    bool setReaderURI(const std::string& value) {
        return setSettingValue("URI", value);
    }

    bool setDefaultReadPower(int value) {
        return setSettingValue(
            "DefaultReadPower", std::to_string(value));
    }

    bool setDefaultWritePower(int value) {
        return setSettingValue(
            "DefaultWritePower", std::to_string(value));
    }

    bool setReadTIDRetryCount(int value) {
        return setSettingValue(
            "ReadTIDRetryCount", std::to_string(value));
    }

    bool setReadTIDRetryPowerIncrement(int value) {
        return setSettingValue(
            "ReadTIDRetryPowerIncrement",
            std::to_string(value));
    }

    bool setWriteEPCRetryCount(int value) {
        return setSettingValue(
            "WriteEPCRetryCount", std::to_string(value));
    }

    bool setWriteEPCRetryPowerIncrement(int value) {
        return setSettingValue(
            "WriteEPCRetryPowerIncrement",
            std::to_string(value));
    }

    bool setLogTransportTraffic(bool value) {
        return setSettingValue(
            "TransportTraffic",
            value ? "true" : "false");
    }

    bool setLogLevel(const std::string& value) {
        return setSettingValue("Level", value);
    }

    bool setColoredOutputLog(bool value) {
        return setSettingValue(
            "ColoredOutput",
            value ? "true" : "false");
    }

    bool setAutoCalibrate(bool value) {
        return setSettingValue(
            "AutoCalibrate",
            value ? "true" : "false");
    }

    bool setReadAutoCalibrate(bool value) {
        return setSettingValue(
            "ReadAutoCalibrate",
            value ? "true" : "false");
    }

    bool setWriteAutoCalibrate(bool value) {
        return setSettingValue(
            "WriteAutoCalibrate",
            value ? "true" : "false");
    }

    bool setAutoCalibrateOnSizing(bool value) {
        return setSettingValue(
            "AutoCalibrateOnSizing",
            value ? "true" : "false");
    }

    bool setAutoCalibrateOnInit(bool value) {
        return setSettingValue(
            "AutoCalibrateOnInit",
            value ? "true" : "false");
    }

    bool setReadAutoCalibrateOnSizing(bool value) {
        return setSettingValue(
            "ReadAutoCalibrateOnSizing",
            value ? "true" : "false");
    }

    bool setReadAutoCalibrateOnInit(bool value) {
        return setSettingValue(
            "ReadAutoCalibrateOnInit",
            value ? "true" : "false");
    }

    bool setWriteAutoCalibrateOnSizing(bool value) {
        return setSettingValue(
            "WriteAutoCalibrateOnSizing",
            value ? "true" : "false");
    }

    bool setWriteAutoCalibrateOnInit(bool value) {
        return setSettingValue(
            "WriteAutoCalibrateOnInit",
            value ? "true" : "false");
    }

    bool setRegion(int value) {
        return setSettingValue(
            "Region", std::to_string(value));
    }

    bool setEncodingProtocol(EncodingProtocol value) {
        switch (value) {
            case EncodingProtocol::SGTIN_96:
                return setSettingValue(
                    "EncodingProtocol", "SGTIN_96");

            case EncodingProtocol::DSGTIN_128:
                return setSettingValue(
                    "EncodingProtocol", "DSGTIN_128");

            default:
                return false;
        }
    }

    bool setReadTime(int value) {
        return setSettingValue(
            "ReadTime", std::to_string(value));
    }

    bool setCommandTimeout(int value) {
        return setSettingValue(
            "CommandTimeout", std::to_string(value));
    }

    bool setReadTimeout(int value) {
        return setSettingValue(
            "ReadTimeout", std::to_string(value));
    }

    // -------------------------------------------------------------------------
    // Save to the file that was loaded
    // -------------------------------------------------------------------------

    bool save() {
        std::string filename;

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (configFilePath_.empty()) {
                if (m_logger) {
                    m_logger->error(
                        "Cannot save configuration: "
                        "no configuration file has been loaded.");
                }

                return false;
            }

            filename = configFilePath_;
        }

        return saveToFile(filename);
    }

    // -------------------------------------------------------------------------
    // Save to a specific file
    // -------------------------------------------------------------------------

    bool saveToFile(const std::string& filename) {
        // Copy the settings while locked. This keeps the mutex held for only
        // the in-memory copy and not for the potentially slow file operation.
        std::unordered_map<std::string, std::string> settingsCopy;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            settingsCopy = settings_;
        }

        try {
            fs::path path(filename);
            fs::path parent = path.parent_path();

            if (parent != "." &&
                !parent.empty() &&
                !fs::exists(parent)) {
                if (!fs::create_directories(parent)) {
                    if (m_logger) {
                        m_logger->error(
                            "Failed to create configuration "
                            "directory: {}",
                            parent.string());
                    }

                    return false;
                }
            }

            // Write to a temporary file first so a failed write does not
            // leave the main configuration file partially written.
            std::string tempFilename = filename + ".tmp";

            {
                std::ofstream configFile(tempFilename);

                if (!configFile.is_open()) {
                    if (m_logger) {
                        m_logger->error(
                            "Failed to open temporary configuration "
                            "file for writing: {}",
                            tempFilename);
                    }

                    return false;
                }

                writeConfig(configFile, settingsCopy);

                configFile.flush();

                if (!configFile.good()) {
                    configFile.close();
                    fs::remove(tempFilename);

                    if (m_logger) {
                        m_logger->error(
                            "Error writing configuration file: {}",
                            tempFilename);
                    }

                    return false;
                }

                configFile.close();
            }

            // Replace the existing file.
            if (fs::exists(filename)) {
                fs::remove(filename);
            }

            fs::rename(tempFilename, filename);

            if (m_logger) {
                m_logger->info(
                    "Configuration saved to {}",
                    filename);
            }

            return true;
        }
        catch (const std::exception& e) {
            if (m_logger) {
                m_logger->error(
                    "Error saving configuration: {}",
                    e.what());
            }

            return false;
        }
    }

    // Returns the path of the configuration file currently associated
    // with this Config object.
    std::string getConfigFilePath() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return configFilePath_;
    }

private:
    Config() = default;
    ~Config() = default;

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // -------------------------------------------------------------------------
    // Load settings from INIReader
    // -------------------------------------------------------------------------

    void loadSettings(const INIReader& reader) {
        std::lock_guard<std::mutex> lock(mutex_);

        settings_["URI"] =
            reader.Get(
                "Reader",
                "URI",
                "tmr:///dev/ttyACM0");

        settings_["DefaultReadPower"] =
            std::to_string(
                reader.GetInteger(
                    "Reader",
                    "DefaultReadPower",
                    250));

        settings_["DefaultWritePower"] =
            std::to_string(
                reader.GetInteger(
                    "Reader",
                    "DefaultWritePower",
                    2400));

        settings_["ReadTIDRetryCount"] =
            std::to_string(
                reader.GetInteger(
                    "ReadTID",
                    "RetryCount",
                    4));

        settings_["ReadTIDRetryPowerIncrement"] =
            std::to_string(
                reader.GetInteger(
                    "ReadTID",
                    "RetryPowerIncrement",
                    500));

        settings_["WriteEPCRetryCount"] =
            std::to_string(
                reader.GetInteger(
                    "WriteEPC",
                    "RetryCount",
                    4));

        settings_["WriteEPCRetryPowerIncrement"] =
            std::to_string(
                reader.GetInteger(
                    "WriteEPC",
                    "RetryPowerIncrement",
                    500));

        settings_["TransportTraffic"] =
            reader.GetBoolean(
                "Logging",
                "TransportTraffic",
                false)
                ? "true"
                : "false";

        settings_["Level"] =
            reader.Get(
                "Logging",
                "Level",
                "INFO");

        settings_["ColoredOutput"] =
            reader.GetBoolean(
                "Logging",
                "ColoredOutput",
                true)
                ? "true"
                : "false";

        settings_["AutoCalibrate"] =
            reader.GetBoolean(
                "Operation",
                "AutoCalibrate",
                false)
                ? "true"
                : "false";

        settings_["ReadAutoCalibrate"] =
            reader.GetBoolean(
                "Operation",
                "ReadAutoCalibrate",
                false)
                ? "true"
                : "false";

        settings_["WriteAutoCalibrate"] =
            reader.GetBoolean(
                "Operation",
                "WriteAutoCalibrate",
                false)
                ? "true"
                : "false";

        settings_["AutoCalibrateOnSizing"] =
            reader.GetBoolean(
                "Operation",
                "AutoCalibrateOnSizing",
                false)
                ? "true"
                : "false";

        settings_["AutoCalibrateOnInit"] =
            reader.GetBoolean(
                "Operation",
                "AutoCalibrateOnInit",
                false)
                ? "true"
                : "false";

        settings_["ReadAutoCalibrateOnSizing"] =
            reader.GetBoolean(
                "Operation",
                "ReadAutoCalibrateOnSizing",
                false)
                ? "true"
                : "false";

        settings_["ReadAutoCalibrateOnInit"] =
            reader.GetBoolean(
                "Operation",
                "ReadAutoCalibrateOnInit",
                false)
                ? "true"
                : "false";

        settings_["WriteAutoCalibrateOnSizing"] =
            reader.GetBoolean(
                "Operation",
                "WriteAutoCalibrateOnSizing",
                false)
                ? "true"
                : "false";

        settings_["WriteAutoCalibrateOnInit"] =
            reader.GetBoolean(
                "Operation",
                "WriteAutoCalibrateOnInit",
                false)
                ? "true"
                : "false";

        settings_["Region"] =
            std::to_string(
                reader.GetInteger(
                    "Operation",
                    "Region",
                    1));

        settings_["EncodingProtocol"] =
            reader.Get(
                "Operation",
                "EncodingProtocol",
                "SGTIN_96");

        settings_["ReadTime"] =
            std::to_string(
                reader.GetInteger(
                    "Operation",
                    "ReadTime",
                    500));

        settings_["CommandTimeout"] =
            std::to_string(
                reader.GetInteger(
                    "Operation",
                    "CommandTimeout",
                    1000));

        settings_["ReadTimeout"] =
            std::to_string(
                reader.GetInteger(
                    "Operation",
                    "ReadTimeout",
                    1000));

        if (m_logger) {
            m_logger->debug(
                "Loaded log level: {}",
                settings_["Level"]);
        }
    }

    // -------------------------------------------------------------------------
    // Defaults
    // -------------------------------------------------------------------------

    void useDefaultSettings() {
        std::lock_guard<std::mutex> lock(mutex_);

        settings_["URI"] = "tmr:///dev/ttyACM0";
        settings_["DefaultReadPower"] = "250";
        settings_["DefaultWritePower"] = "2400";
        settings_["ReadTIDRetryCount"] = "4";
        settings_["ReadTIDRetryPowerIncrement"] = "500";
        settings_["WriteEPCRetryCount"] = "4";
        settings_["WriteEPCRetryPowerIncrement"] = "500";
        settings_["TransportTraffic"] = "false";
        settings_["Level"] = "INFO";
        settings_["ColoredOutput"] = "true";
        settings_["AutoCalibrate"] = "false";
        settings_["ReadAutoCalibrate"] = "false";
        settings_["WriteAutoCalibrate"] = "false";
        settings_["AutoCalibrateOnSizing"] = "false";
        settings_["AutoCalibrateOnInit"] = "false";
        settings_["ReadAutoCalibrateOnSizing"] = "false";
        settings_["ReadAutoCalibrateOnInit"] = "false";
        settings_["WriteAutoCalibrateOnSizing"] = "false";
        settings_["WriteAutoCalibrateOnInit"] = "false";
        settings_["Region"] = "1";
        settings_["EncodingProtocol"] = "DSGTIN_128";
        settings_["ReadTime"] = "500";
        settings_["CommandTimeout"] = "1000";
        settings_["ReadTimeout"] = "1000";
    }

    // -------------------------------------------------------------------------
    // Write the complete INI configuration
    // -------------------------------------------------------------------------

    void writeDefaultConfig(std::ofstream& configFile) {
        std::unordered_map<std::string, std::string> defaults;

        defaults["URI"] = "tmr:///dev/ttyACM0";
        defaults["DefaultReadPower"] = "250";
        defaults["DefaultWritePower"] = "2400";
        defaults["ReadTIDRetryCount"] = "4";
        defaults["ReadTIDRetryPowerIncrement"] = "500";
        defaults["WriteEPCRetryCount"] = "4";
        defaults["WriteEPCRetryPowerIncrement"] = "500";
        defaults["TransportTraffic"] = "false";
        defaults["Level"] = "INFO";
        defaults["ColoredOutput"] = "true";
        defaults["AutoCalibrate"] = "false";
        defaults["ReadAutoCalibrate"] = "false";
        defaults["WriteAutoCalibrate"] = "false";
        defaults["AutoCalibrateOnSizing"] = "false";
        defaults["AutoCalibrateOnInit"] = "false";
        defaults["ReadAutoCalibrateOnSizing"] = "false";
        defaults["ReadAutoCalibrateOnInit"] = "false";
        defaults["WriteAutoCalibrateOnSizing"] = "false";
        defaults["WriteAutoCalibrateOnInit"] = "false";
        defaults["Region"] = "1";
        defaults["EncodingProtocol"] = "DSGTIN_128";
        defaults["ReadTime"] = "500";
        defaults["CommandTimeout"] = "1000";
        defaults["ReadTimeout"] = "1000";

        writeConfig(configFile, defaults);
    }

    void writeConfig(
        std::ofstream& configFile,
        const std::unordered_map<std::string, std::string>& settings)
    {
        configFile
            << "; RFID Reader configuration\n\n"

            << "[Reader]\n"
            << "URI = " << getMapValue(
                settings, "URI", "tmr:///dev/ttyACM0") << "\n"
            << "DefaultReadPower = " << getMapValue(
                settings, "DefaultReadPower", "250") << "\n"
            << "DefaultWritePower = " << getMapValue(
                settings, "DefaultWritePower", "2400") << "\n\n"

            << "[ReadTID]\n"
            << "RetryCount = " << getMapValue(
                settings, "ReadTIDRetryCount", "4") << "\n"
            << "RetryPowerIncrement = " << getMapValue(
                settings, "ReadTIDRetryPowerIncrement", "500") << "\n\n"

            << "[WriteEPC]\n"
            << "RetryCount = " << getMapValue(
                settings, "WriteEPCRetryCount", "4") << "\n"
            << "RetryPowerIncrement = " << getMapValue(
                settings, "WriteEPCRetryPowerIncrement", "500") << "\n\n"

            << "[Logging]\n"
            << "TransportTraffic = " << getMapValue(
                settings, "TransportTraffic", "false") << "\n"
            << "Level = " << getMapValue(
                settings, "Level", "INFO") << "\n"
            << "ColoredOutput = " << getMapValue(
                settings, "ColoredOutput", "true") << "\n\n"

            << "[Operation]\n"
            << "AutoCalibrate = " << getMapValue(
                settings, "AutoCalibrate", "false") << "\n"
            << "ReadAutoCalibrate = " << getMapValue(
                settings, "ReadAutoCalibrate", "false") << "\n"
            << "WriteAutoCalibrate = " << getMapValue(
                settings, "WriteAutoCalibrate", "false") << "\n"
            << "AutoCalibrateOnSizing = " << getMapValue(
                settings, "AutoCalibrateOnSizing", "false") << "\n"
            << "AutoCalibrateOnInit = " << getMapValue(
                settings, "AutoCalibrateOnInit", "false") << "\n"
            << "ReadAutoCalibrateOnSizing = " << getMapValue(
                settings, "ReadAutoCalibrateOnSizing", "false") << "\n"
            << "ReadAutoCalibrateOnInit = " << getMapValue(
                settings, "ReadAutoCalibrateOnInit", "false") << "\n"
            << "WriteAutoCalibrateOnSizing = " << getMapValue(
                settings, "WriteAutoCalibrateOnSizing", "false") << "\n"
            << "WriteAutoCalibrateOnInit = " << getMapValue(
                settings, "WriteAutoCalibrateOnInit", "false") << "\n"
            << "Region = " << getMapValue(
                settings, "Region", "1") << "\n"
            << "EncodingProtocol = " << getMapValue(
                settings, "EncodingProtocol", "DSGTIN_128") << "\n"
            << "ReadTime = " << getMapValue(
                settings, "ReadTime", "500") << "\n"
            << "CommandTimeout = " << getMapValue(
                settings, "CommandTimeout", "1000") << "\n"
            << "ReadTimeout = " << getMapValue(
                settings, "ReadTimeout", "1000") << "\n";
    }

    static std::string getMapValue(
        const std::unordered_map<std::string, std::string>& settings,
        const std::string& key,
        const std::string& defaultValue)
    {
        auto it = settings.find(key);

        if (it != settings.end()) {
            return it->second;
        }

        return defaultValue;
    }

    // -------------------------------------------------------------------------
    // Internal getters
    // -------------------------------------------------------------------------

    std::string getSettingValue(
        const std::string& key,
        const std::string& defaultValue) const
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = settings_.find(key);

        if (it != settings_.end()) {
            return it->second;
        }

        return defaultValue;
    }

    int getIntSetting(
        const std::string& key,
        int defaultValue) const
    {
        std::string value =
            getSettingValue(key, std::to_string(defaultValue));

        try {
            return std::stoi(value);
        }
        catch (const std::exception&) {
            if (m_logger) {
                m_logger->warn(
                    "Invalid integer setting '{}': '{}'. "
                    "Using default {}.",
                    key,
                    value,
                    defaultValue);
            }

            return defaultValue;
        }
    }

    bool getBoolSetting(
        const std::string& key,
        bool defaultValue) const
    {
        std::string value =
            getSettingValue(
                key,
                defaultValue ? "true" : "false");

        if (value == "true" ||
            value == "TRUE" ||
            value == "True" ||
            value == "1") {
            return true;
        }

        if (value == "false" ||
            value == "FALSE" ||
            value == "False" ||
            value == "0") {
            return false;
        }

        if (m_logger) {
            m_logger->warn(
                "Invalid boolean setting '{}': '{}'. "
                "Using default {}.",
                key,
                value,
                defaultValue);
        }

        return defaultValue;
    }

    std::shared_ptr<spdlog::logger> m_logger;

    mutable std::mutex mutex_;

    std::unordered_map<std::string, std::string> settings_;

    std::chrono::duration<double> loadTime_;

    // Path of the INI file that was most recently loaded successfully.
    std::string configFilePath_;
};

#endif // SETTINGS_H