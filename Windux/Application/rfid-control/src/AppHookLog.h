#pragma once

#include <memory>
#include <string>
#include <sstream>
#include "spdlog/spdlog.h"

class AppHookLog {
public:
    static AppHookLog& getInstance();

    bool initialize();

    bool isInitialized() const;

    // Method to update log level dynamically based on the configuration
    void updateLogLevel();

    // Logging methods
    void info(const std::string& message) {
        log(LogLevel::Info, message);
    }

    void error(const std::string& message) {
        log(LogLevel::Error, message);
    }

    void warn(const std::string& message) {
        log(LogLevel::Warn, message);
    }

    void debug(const std::string& message) {
        log(LogLevel::Debug, message);
    }

    // Formatted logging methods
    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        log(LogLevel::Info, formatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        log(LogLevel::Error, formatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void warn(const std::string& format, Args&&... args) {
        log(LogLevel::Warn, formatString(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        log(LogLevel::Debug, formatString(format, std::forward<Args>(args)...));
    }

    // Additional logging utilities
    void printSeparator();
    void printSectionHeader(const std::string& header);
    void printKeyValue(const std::string& key, const std::string& value, int keyWidth = 20);

    std::string formatSuccess(const std::string& message);
    std::string formatFailure(const std::string& message);

private:
    AppHookLog() = default;
    ~AppHookLog() = default;
    AppHookLog(const AppHookLog&) = delete;
    AppHookLog& operator=(const AppHookLog&) = delete;

    std::shared_ptr<spdlog::logger> m_logger;

    // Stores the current log level to detect changes
    spdlog::level::level_enum m_currentLogLevel = spdlog::level::info;

    // Helper function to parse the log level from configuration
    spdlog::level::level_enum parseLogLevel(const std::string& levelStr);

    std::string colorize(const std::string& text, const std::string& color);
    std::string combineColors(const std::string& color1, const std::string& color2);

    enum class LogLevel { Debug, Info, Warn, Error };
    void log(LogLevel level, const std::string& message);

    // String formatting helpers
    template<typename T>
    void formatHelper(std::ostringstream& oss, const std::string& format, size_t& pos, T&& value) {
        size_t open = format.find('{', pos);
        size_t close = format.find('}', open);
        if (open != std::string::npos && close != std::string::npos) {
            oss << format.substr(pos, open - pos);
            oss << value;
            pos = close + 1;
        }
    }

    template<typename T, typename... Args>
    void formatHelper(std::ostringstream& oss, const std::string& format, size_t& pos, T&& value, Args&&... args) {
        formatHelper(oss, format, pos, std::forward<T>(value));
        formatHelper(oss, format, pos, std::forward<Args>(args)...);
    }

    std::string formatString(const std::string& format) {
        return format;
    }

    template<typename... Args>
    std::string formatString(const std::string& format, Args&&... args) {
        std::ostringstream oss;
        size_t pos = 0;
        formatHelper(oss, format, pos, std::forward<Args>(args)...);
        oss << format.substr(pos);
        return oss.str();
    }
};

namespace Log {
    inline AppHookLog& logger() { return AppHookLog::getInstance(); }
}