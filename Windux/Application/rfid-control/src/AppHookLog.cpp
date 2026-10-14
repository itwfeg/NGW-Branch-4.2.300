#include "AppHookLog.h"
#include "Config.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <iostream>

#ifdef USE_GHC_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace AppHookColors {
    const std::string RESET   = "\033[0m";
    const std::string BLACK   = "\033[30m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BOLD    = "\033[1m";
}

bool AppHookLog::isInitialized() const {
    return m_logger != nullptr;  // Check if logger is initialized
}

AppHookLog& AppHookLog::getInstance() {
    static AppHookLog instance;
    return instance;
}

// New method to parse log level string to spdlog level enum
spdlog::level::level_enum AppHookLog::parseLogLevel(const std::string& levelStr) {
    if (levelStr == "trace") return spdlog::level::trace;
    else if (levelStr == "debug") return spdlog::level::debug;
    else if (levelStr == "info") return spdlog::level::info;
    else if (levelStr == "warn") return spdlog::level::warn;
    else if (levelStr == "error") return spdlog::level::err;
    else if (levelStr == "critical") return spdlog::level::critical;
    else if (levelStr == "off") return spdlog::level::off;
    else return spdlog::level::info; // Default to info
}

bool AppHookLog::initialize() {
    std::string logLevelStr = Config::getInstance().getLogLevelFromConfig();
    spdlog::level::level_enum logLevel = parseLogLevel(logLevelStr);

    if (isInitialized()) {
        // Update log level if it has changed
        if (logLevel != m_currentLogLevel) {
            m_logger->set_level(logLevel);
            m_currentLogLevel = logLevel;
        }
        return true;
    }
    try {
        // Change log directory to a writable path
        std::string log_directory = "/usr/share/rfid-hook";

        if (!fs::exists(log_directory)) {
            fs::create_directories(log_directory);
        }

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            log_directory + "/rfid_log.txt", 
            true  // Append mode
        );

        // Set sink levels to accept all logs
        console_sink->set_level(spdlog::level::trace);
        file_sink->set_level(spdlog::level::trace);

        // Create a vector of sinks
        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};

        // Create a logger with multiple sinks
        m_logger = std::make_shared<spdlog::logger>("rfid_logger", sinks.begin(), sinks.end());

        // Set the log level from configuration
        m_logger->set_level(logLevel);
        m_currentLogLevel = logLevel;

        // Flush on every info message or higher
        m_logger->flush_on(spdlog::level::info);

        // Set as default logger
        spdlog::set_default_logger(m_logger);

        // Set a pattern for log messages
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        // Now safe to log messages
        m_logger->info("Logging initialized. File: {}", log_directory + "/rfid_log.txt");
        m_logger->info("Current log level: {}", spdlog::level::to_string_view(m_logger->level()));
        m_logger->info("Log level from config: {}", logLevelStr);

        return true;
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }
}

// Ensure the logger is initialized before logging
void AppHookLog::log(LogLevel level, const std::string& message) {
    try {
        std::string logLevelStr = Config::getInstance().getLogLevelFromConfig();
        spdlog::level::level_enum logLevel = parseLogLevel(logLevelStr);

        if (!isInitialized()) {
            if (!initialize()) {
                std::cerr << "Failed to initialize logger" << std::endl;
                return;
            }
        }
        
        if (logLevel != m_currentLogLevel) {
            m_logger->set_level(logLevel);
            m_currentLogLevel = logLevel;
        }

        switch (level) {
            case LogLevel::Info:
                m_logger->info(colorize(message, AppHookColors::YELLOW));
                break;
            case LogLevel::Error:
                m_logger->error(colorize(message, AppHookColors::RED));
                break;
            case LogLevel::Warn:
                m_logger->warn(colorize(message, AppHookColors::YELLOW));
                break;
            case LogLevel::Debug:
                m_logger->debug(colorize(message, AppHookColors::WHITE));
                break;
        }
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logging failed: " << ex.what() << std::endl;
    }
}

void AppHookLog::printSeparator() {
    try {
        if (!isInitialized()) {
            if (!initialize()) {
                std::cerr << "Failed to initialize logger" << std::endl;
                return;
            }
        }
        std::string separator = std::string(50, '=');
        m_logger->info(colorize(separator, AppHookColors::BOLD + AppHookColors::CYAN));
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logging separator failed: " << ex.what() << std::endl;
    }
}

void AppHookLog::printSectionHeader(const std::string& header) {
    try {
        if (!isInitialized()) {
            if (!initialize()) {
                std::cerr << "Failed to initialize logger" << std::endl;
                return;
            }
        }
        if (Config::getInstance().getColoredOutputLogFromConfig()) {
            std::string separator = std::string(50, '=');
            m_logger->info(colorize(separator, AppHookColors::BOLD + AppHookColors::CYAN));
            m_logger->info(colorize(" {} ", AppHookColors::BOLD + AppHookColors::CYAN), header);
            m_logger->info(colorize(separator, AppHookColors::BOLD + AppHookColors::CYAN));
        } else {
            std::string separator(50, '=');
            m_logger->info("{:^50}", separator);
            m_logger->info("{:^50}", header);
            m_logger->info("{:^50}", separator);
        }
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logging section header failed: " << ex.what() << std::endl;
    }
}

void AppHookLog::printKeyValue(const std::string& key, const std::string& value, int keyWidth) {
    try {
        if (!isInitialized()) {
            if (!initialize()) {
                std::cerr << "Failed to initialize logger" << std::endl;
                return;
            }
        }
        m_logger->info(colorize("{:<{}} : ", AppHookColors::YELLOW) + colorize("{}", AppHookColors::YELLOW), key, keyWidth, value);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logging key-value pair failed: " << ex.what() << std::endl;
    }
}

std::string AppHookLog::formatSuccess(const std::string& message) {
    return colorize(message, AppHookColors::GREEN + AppHookColors::BOLD);
}

std::string AppHookLog::formatFailure(const std::string& message) {
    return colorize(message, AppHookColors::RED + AppHookColors::BOLD);
}

std::string AppHookLog::colorize(const std::string& text, const std::string& color) {
    if (Config::getInstance().getColoredOutputLogFromConfig()) {
        return color + text + AppHookColors::RESET;
    }
    return text;
}

std::string AppHookLog::combineColors(const std::string& color1, const std::string& color2) {
    return color1 + color2;
}