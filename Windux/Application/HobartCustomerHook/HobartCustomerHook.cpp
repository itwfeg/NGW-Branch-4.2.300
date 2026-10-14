// HobartCustomerHook.cpp : Defines the exported functions for the DLL application.
//

#if defined(WIN32)
#include "stdafx.h"
#endif

#include "HobartCustomerHook.h"
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <mutex>

#include <cstring>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <ctime>
#include <future>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

std::vector<Hook> hooks;
std::mutex logMutex;
LogLevel currentLogLevel = LOG_OFF;

bool isAlreadyInitialized = false;

const std::string hookListFilePath = "/usr/share/hook-manager/hook-list.txt";
const std::string hookPrefix = "libHobartCustomerHook";

///////////////////////////////////////////////////////////////////////////////////////////////////
void loadLogLevelFromFile(const std::string& filePath) {
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open log config file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Skip lines starting with #
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line.rfind("LogLevel=", 0) == 0) {
            int logLevel = std::stoi(line.substr(9));
            if (logLevel < LOG_OFF || logLevel > LOG_DETAILED) {
                std::cerr << "Invalid log level in config file: " << filePath << std::endl;
                return;
            }
            currentLogLevel = static_cast<LogLevel>(logLevel);
            break;
        }
    }
    configFile.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logMessage(const std::string& message, int level = LOG_NON_DETAILED) {
    if (currentLogLevel == LOG_OFF || level > currentLogLevel) {
        return;
    }

    std::stringstream ss;
    if (currentLogLevel == LOG_DETAILED) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time_t);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", now_tm);

        ss << "[" << buffer << "][Thread " << std::this_thread::get_id() << "] ";
    }
    ss << message;

    std::ofstream logFile("/var/log/manager-applog.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << ss.str() << std::endl;
        logFile.close();
    } else {
        std::cerr << "Failed to open log file for writing." << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void logMessage(const std::string& message, const std::vector<std::string>& varStatus, int level = LOG_NON_DETAILED) {
    if (currentLogLevel == LOG_OFF || level > currentLogLevel) {
        return;
    }

    std::stringstream ss;
    if (currentLogLevel == LOG_DETAILED) {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time_t);

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", now_tm);

        ss << "[" << buffer << "][Thread " << std::this_thread::get_id() << "] ";
    }
    ss << message;
    for (const auto& status : varStatus) {
        ss << " | " << status;
    }
    logMessage(ss.str(), level);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool loadHook(const std::string& path) {

    logMessage("Entering loadHook", {std::string("Path: ") + path}, LOG_NON_DETAILED);
    Hook hook;
    hook.handle = dlopen(path.c_str(), RTLD_LAZY);

    std::size_t lastSlashPos = path.find_last_of("/");
    if (lastSlashPos != std::string::npos) {
        hook.soFilePath = path.substr(lastSlashPos + 1);
    } else {
        hook.soFilePath = path;
    }

    if (!hook.handle) {
    logMessage("Failed to load library", {"Path: " + path, "Error: " + std::string(dlerror())}, LOG_NON_DETAILED);
    return false;
    } else {
    logMessage("Library loaded successfully", {"Path: " + path}, LOG_NON_DETAILED);
    }
    dlerror(); // Clearing existing errors

    // Dynamically load the function symbols and store them in the Hook struct
    *(void**)(&hook.init) = dlsym(hook.handle, "hobart_cust_hook_init");
    if (const char* dlsym_error = dlerror()) {
        logMessage("Failed to load: hobart_cust_hook_init", {"Error: " + std::string(dlsym_error)}, LOG_DETAILED);
    } else {
        logMessage("Loaded successfully: hobart_cust_hook_init", LOG_DETAILED);
    }
    *(void**)(&hook.finalize) = dlsym(hook.handle, "hobart_cust_hook_finalize");
    *(void**)(&hook.config_saved) = dlsym(hook.handle, "hobart_cust_hook_config_saved");
    *(void**)(&hook.before_open_database) = dlsym(hook.handle, "hobart_cust_hook_before_open_database");
     if (const char* dlsym_error = dlerror()) {
        logMessage("Failed to load: hobart_cust_hook_before_open_database", {"Error: " + std::string(dlsym_error)}, LOG_DETAILED);
    } else {
        logMessage("Loaded successfully: hobart_cust_hook_before_open_database", LOG_DETAILED);
    }
    *(void**)(&hook.open_database) = dlsym(hook.handle, "hobart_cust_hook_open_database");
    *(void**)(&hook.before_plu_read) = dlsym(hook.handle, "hobart_cust_hook_before_plu_read");
    *(void**)(&hook.before_plu_text_read) = dlsym(hook.handle, "hobart_cust_hook_before_plu_text_read");
    *(void**)(&hook.before_print) = dlsym(hook.handle, "hobart_cust_hook_before_print");
    *(void**)(&hook.before_remote_print) = dlsym(hook.handle, "hobart_cust_hook_before_remote_print");
    *(void**)(&hook.after_print) = dlsym(hook.handle, "hobart_cust_hook_after_print");
    *(void**)(&hook.custom_action) = dlsym(hook.handle, "hobart_cust_hook_custom_action");
    *(void**)(&hook.update_peripheral_version) = dlsym(hook.handle, "hobart_cust_hook_update_peripheral_version");
    *(void**)(&hook.update_scale_state) = dlsym(hook.handle, "hobart_cust_hook_update_scale_state");

    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Failed to load symbols from library: " << path << " due to error: " << dlsym_error << std::endl;
        dlclose(hook.handle);
        return false;
    }
    hooks.push_back(hook);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void unloadHooks() {
    for (auto& hook : hooks) {
        if (hook.finalize) {
            hook.finalize();
        }
        if (hook.handle) {
            dlclose(hook.handle);
        }
    }
    hooks.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hookUnload(const std::string& hookPath) {
    logMessage("Unloading hook: " + hookPath, LOG_NON_DETAILED);

    for (auto it = hooks.begin(); it != hooks.end(); ++it) {
        if (it->soFilePath == hookPath) {
            if (it->finalize) {
                it->finalize();
            }
            if (it->handle) {
                dlclose(it->handle);
            }
            hooks.erase(it);
            logMessage("Hook successfully unloaded: " + hookPath, LOG_NON_DETAILED);
            return;
        }
    }
    
    logMessage("Attempted to unload non-existent hook: " + hookPath, LOG_NON_DETAILED);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void updateHookListFile(const std::string& hookListFilePath) {
    // Function to search for .so files in /usr/lib and add them to the hook-list file
    const std::string directoryPath = "/usr/lib";
    const std::string soFilePrefix = "libHobartCustomerHook";
    std::regex validSoFilePattern(R"(^libHobartCustomerHook[^/]*\.so(\.\d+)?$)");

    std::vector<std::string> soFilePaths;
    std::vector<std::string> headerLines;
    std::vector<std::string> activeHooks;
    std::vector<std::string> commentedHooks;

    // Open the directory
    DIR* dir = opendir(directoryPath.c_str());
    if (dir == nullptr) {
        logMessage("Failed to open directory", {directoryPath}, LOG_NON_DETAILED);
        return;
    }

    // Read entries in the directory
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;

        // Ensure it matches the .so file pattern and does not contain "Manager"
        if (std::regex_match(filename, validSoFilePattern) && filename.find("Manager") == std::string::npos) {
            // Ensure there's an identifying name after "Hook" and before ".so"
            size_t hookPos = filename.find("Hook");
            size_t suffixPos = filename.find(".so");
            if (suffixPos == std::string::npos || suffixPos <= hookPos + 4) {
                continue;
            }

            // Ensure the part between "Hook" and ".so" is not empty
            if (suffixPos == hookPos + 4) {
                continue;
            }

            // Exclude "libHobartCustomerHookEmpty.so"
            if (filename == "libHobartCustomerHookEmpty.so") {
                continue;
            }

            // Add the valid file paths to the list
            soFilePaths.push_back(directoryPath + "/" + filename);
        }
    }

    closedir(dir);

    // Read the current hook-list file content
    std::ifstream hookListFile(hookListFilePath);
    if (hookListFile.is_open()) {
        std::string line;
        while (std::getline(hookListFile, line)) {
            if (!line.empty() && line[0] == '#') {
                commentedHooks.push_back(line); // Retain commented out hooks
            } else if (!line.empty()) {
                activeHooks.push_back(line); // Retain active hooks
            } else {
                headerLines.push_back(line); // Retain header comments
            }
        }
        hookListFile.close();
    }

    // Remove hooks from activeHooks if the corresponding .so file is no longer present
    auto it = activeHooks.begin();
    while (it != activeHooks.end()) {
        if (std::find(soFilePaths.begin(), soFilePaths.end(), *it) == soFilePaths.end()) {
            it = activeHooks.erase(it);
        } else {
            ++it;
        }
    }

    // Add the new valid .so file paths to the active hooks, avoiding duplicates
    for (const auto& soFilePath : soFilePaths) {
        if (std::find(activeHooks.begin(), activeHooks.end(), soFilePath) == activeHooks.end() &&
            std::find(commentedHooks.begin(), commentedHooks.end(), "#" + soFilePath) == commentedHooks.end()) {
            activeHooks.push_back(soFilePath);
        }
    }

    // Write the updated hook list back to the file, including header comments and commented hooks
    std::ofstream updatedHookListFile(hookListFilePath, std::ios::trunc);
    if (updatedHookListFile.is_open()) {
        for (const auto& line : headerLines) {
            updatedHookListFile << line << std::endl;
        }
        for (const auto& line : commentedHooks) {
            updatedHookListFile << line << std::endl;
        }
        for (const auto& line : activeHooks) {
            updatedHookListFile << line << std::endl;
        }
        updatedHookListFile.close();
    } else {
        logMessage("Failed to open hook list file for writing", {std::string("File Path: ") + hookListFilePath}, LOG_NON_DETAILED);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void loadHooksFromFile(const std::string& filePath) {
    // Function to load hooks from file with additional .so files from /usr/share
    logMessage("Loading hooks from", {std::string("File Path: ") + filePath}, LOG_DETAILED);

    // Update the hook-list file with .so files from /usr/lib
    updateHookListFile(filePath);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        logMessage("Failed to open hook list file", {std::string("File Path: ") + filePath}, LOG_NON_DETAILED);
        return;
    }

    std::string line;
    std::vector<std::future<bool>> futures; // Store futures of asynchronous hook loading operations

    while (std::getline(file, line)) {
        // Skip lines starting with #
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Launch a task for each hook loading
        auto future = std::async(std::launch::async, [line]() -> bool { return loadHook(line); });
        futures.push_back(std::move(future));
    }

    if (futures.empty()) {
        logMessage("No hooks listed in file to load.", LOG_NON_DETAILED);
        return;
    }

    // Wait for all hook loading tasks to complete
    for (auto& future : futures) {
        bool success = future.get(); 
        if (!success) {
            logMessage("Error loading hook", LOG_NON_DETAILED);
        } else {
            logMessage("Hook loaded successfully", LOG_DETAILED);
        }
    }

    file.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool loadOrUnloadHooks(const std::string& commandType, const std::string& hookName) {
    std::ifstream file(hookListFilePath);
    if (!file.is_open()) {
        logMessage("Failed to open hook-list.txt", LOG_NON_DETAILED);
        return false;
    }

    logMessage("Processing " + commandType + " for " + hookName, LOG_DETAILED);

    std::vector<std::string> lines;
    std::vector<std::string> headerLines;
    bool modified = false;
    bool found = (hookName == "ALL");  // No need to check for "ALL" hooks

    std::string targetHookPath = hookPrefix + hookName + ".so";
    std::string targetHookPathLower = targetHookPath;
    std::transform(targetHookPathLower.begin(), targetHookPathLower.end(), targetHookPathLower.begin(), ::tolower);

    std::string line;

    // Read and store the first 4 header lines
    for (int i = 0; i < 4 && std::getline(file, line); i++) {
        headerLines.push_back(line);
    }

    while (std::getline(file, line)) {
        logMessage("Reading line: " + line, LOG_DETAILED);

        if (hookName == "ALL") {
            if (commandType == "UNLOAD") {
                logMessage("Commenting out all hooks for UNLOAD|ALL", LOG_DETAILED);
                if (line[0] != '#') {
                    line = "#" + line;  // Comment out all hooks
                    modified = true;
                }
            } 
            else if (commandType == "LOAD" && line[0] == '#') {
                line = line.substr(1);  // Uncomment all hooks
                modified = true;
            }
        } 
        else {
            std::string lineLower = line;
            std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(), ::tolower);

            if (lineLower.find(targetHookPathLower) != std::string::npos) {
                found = true;
                if (commandType == "LOAD" && line[0] == '#') {
                    logMessage("Uncommenting hook: " + targetHookPath, LOG_DETAILED);
                    line = line.substr(1);  // Uncomment
                    modified = true;
                } 
                else if (commandType == "UNLOAD" && line[0] != '#') {
                    logMessage("Commenting out hook: " + targetHookPath, LOG_DETAILED);
                    line = "#" + line;  // Comment out
                    modified = true;
                }
            }
        }
        lines.push_back(line);
    }
    file.close();

    if (!found && hookName != "ALL") {
        logMessage("Target hook path NOT found: " + targetHookPath, LOG_NON_DETAILED);
        return false;
    }

    if (!modified) {
        logMessage("No action needed. Hooks already in the desired state.", LOG_DETAILED);
        return false;
    }

    logMessage("Writing updated hook-list.txt", LOG_DETAILED);
    std::ofstream outFile(hookListFilePath, std::ios::trunc);
    if (!outFile.is_open()) {
        logMessage("Failed to write to hook-list.txt", LOG_NON_DETAILED);
        return false;
    }

    for (const auto& header : headerLines) {
        outFile << header << std::endl;
    }
    for (const auto& l : lines) {
        outFile << l << std::endl;
    }
    outFile.close();

    logMessage("Successfully modified hook-list.txt", LOG_DETAILED);

    // When unloading all hooks comment them out in the file and unload them from memory
    if (commandType == "UNLOAD" && hookName == "ALL") {
        logMessage("Unloading all hooks from memory", LOG_NON_DETAILED);
        unloadHooks();
        return true;
    }

    // Otherwise, handle individual load/unload
    if (commandType == "LOAD") {
        for (const auto& l : lines) {
            if (l[0] != '#') {  // Load only uncommented lines
                logMessage("Loading hook: " + l, LOG_NON_DETAILED);
                loadHook(l);
            }
        }
    } 
    else if (commandType == "UNLOAD") {
        for (const auto& l : lines) {
            if (l[0] == '#') {  // Unload only commented lines
                std::string cleanHook = l.substr(1);  // Remove `#` to get actual hook path
                logMessage("Unloading hook: " + cleanHook, LOG_NON_DETAILED);
                hookUnload(cleanHook);
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool reloadHooks(const std::string& hookName) {
    std::ifstream fileCheck(hookListFilePath);
    if (!fileCheck.is_open()) {
        logMessage("Failed to re-open hook-list.txt for RELOAD", LOG_NON_DETAILED);
        return false;
    }

    std::vector<std::string> activeHooks;
    std::string hookFilePath;
    std::string line;

    // For doing a case insensitive comparison
    std::string targetHookPathLower = hookPrefix + hookName + ".so";
    std::transform(targetHookPathLower.begin(), targetHookPathLower.end(), targetHookPathLower.begin(), ::tolower);

    // Skip first 4 lines (header information)
    for (int i = 0; i < 4 && std::getline(fileCheck, line); i++);

    while (std::getline(fileCheck, line)) {
        if (hookName == "ALL") {
            if (line[0] != '#') { // Only reload enabled hooks
                activeHooks.push_back(line);
            }
        } else {
            std::string lineLower = line;
            std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(), ::tolower);

            if (lineLower.find(targetHookPathLower) != std::string::npos && line[0] != '#') {
                hookFilePath = line;
                break; // Stop searching when we find the hook
            }
        }
    }
    fileCheck.close();

    if (hookName == "ALL" && activeHooks.empty()) {
        logMessage("No enabled hooks found to reload: " + hookName, LOG_NON_DETAILED);
        return false;
    }
    if (hookName != "ALL" && hookFilePath.empty()) {
        logMessage("No enabled hook found for: " + hookName, LOG_NON_DETAILED);
        return false;
    }

    if (hookName == "ALL") {
        logMessage("Unloading all hooks before reloading", LOG_NON_DETAILED);
        unloadHooks();  // Unload all hooks
        logMessage("Reloading all hooks", LOG_NON_DETAILED);
        loadHooksFromFile(hookListFilePath);
    } else {
        logMessage("Unloading specific hook before reloading: " + hookFilePath, LOG_NON_DETAILED);
        hookUnload(hookFilePath);  // Unload only the specified hook
        logMessage("Reloading specific hook: " + hookFilePath, LOG_NON_DETAILED);
        if (!loadHook(hookFilePath)) {
            logMessage("Failed to reload hook: " + hookFilePath, LOG_NON_DETAILED);
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_init() {
    loadLogLevelFromFile("/usr/share/hook-manager/log-config.txt");

    if (isAlreadyInitialized) {
        //logMessage("hobart_cust_hook_init skipped (already initialized).", LOG_DETAILED);
        return true;
    }
    logMessage("Starting hobart_cust_hook_init", LOG_NON_DETAILED);
    loadHooksFromFile("/usr/share/hook-manager/hook-list.txt");

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_init()", LOG_DETAILED);
        return true;
    }

    for (auto& hook : hooks) {
        if (hook.init) {
            logMessage("Initializing Hook from: " + hook.soFilePath, LOG_NON_DETAILED);
            hook.init();
        }
    }
    isAlreadyInitialized = true;
    
    logMessage("Completed hobart_cust_hook_init", LOG_NON_DETAILED);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_finalize() {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_finalize()", LOG_DETAILED);
        return;
    }

    unloadHooks();
    logMessage("hobart_cust_hook_finalize complete", LOG_NON_DETAILED);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_config_saved() {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_config_saved()", LOG_DETAILED);
        return;
    }

    for (auto& hook : hooks) {
        if (hook.config_saved) {
            logMessage("config_saved for: " + hook.soFilePath, LOG_NON_DETAILED);
            hook.config_saved();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_before_open_database(const char *dbname) {

    if (!hobart_cust_hook_init()) {
        logMessage("hobart_cust_hook_init failed to initialize hooks.", LOG_DETAILED);
        return;
    }
    logMessage("Attempting to open database: " + std::string(dbname ? dbname : "NULL"), LOG_DETAILED);

    bool foundValidHook = false;
    for (auto& hook : hooks) {
        if (hook.before_open_database) {
            std::string logMsg = "before_open_database for: " + hook.soFilePath + 
                                     ", Database Name: " + std::string(dbname ? dbname : "NULL");
            logMessage(logMsg, LOG_NON_DETAILED);
            hook.before_open_database(dbname);

            foundValidHook = true;
        }
    }
    if (!foundValidHook) {
        logMessage("No valid hobart_cust_hook_before_open_database_ptr found for any hook.", LOG_DETAILED);
    } else {
        logMessage("Successfully called before_open_database on registered hooks.", LOG_DETAILED);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_open_database() {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_open_database()", LOG_DETAILED);
        return;
    }

    for (auto& hook : hooks) {
        if (hook.open_database) {
            logMessage("hobart_cust_hook_open_database for: " + hook.soFilePath, LOG_NON_DETAILED);
            hook.open_database();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_plu_read(const char *plu, const char *deptNum) {

    loadLogLevelFromFile("/usr/share/hook-manager/log-config.txt");
    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_before_plu_read()", LOG_DETAILED);
        return true;
    }

    bool allHooksPassed = true;
    for (auto& hook : hooks) {
        if (hook.before_plu_read) {
            std::string logMsg = "before_plu_read for: " + hook.soFilePath +
                                 ", PLU: " + (plu ? plu : "NULL") + ", DeptNum: " + (deptNum ? deptNum : "NULL");
            logMessage(logMsg, LOG_NON_DETAILED);
            bool hookResult = hook.before_plu_read(plu, deptNum);
            logMessage(hook.soFilePath + " before_plu_read response: " + (hookResult ? "true" : "false"), LOG_NON_DETAILED);
            if (!hookResult)
                allHooksPassed = false;
        }
    }
    return allHooksPassed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_plu_text_read(const char *number, const char *deptNum, int type) {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_before_plu_text_read()", LOG_DETAILED);
        return true;
    }

    bool allHooksPassed = true;
    for (auto& hook : hooks) {
        if (hook.before_plu_text_read) {
            std::string logMsg = "before_plu_text_read for: " + hook.soFilePath +
                                 ", Number: " + (number ? number : "NULL") + ", DeptNum: " + (deptNum ? deptNum : "NULL") +
                                 ", Type: " + std::to_string(type);
            logMessage(logMsg, LOG_NON_DETAILED);
            bool hookResult = hook.before_plu_text_read(number, deptNum, type);
            logMessage(hook.soFilePath + " before_plu_text_read response: " + (hookResult ? "true" : "false"), LOG_NON_DETAILED);
            if (!hookResult)
                allHooksPassed = false;
        }
    }
    return allHooksPassed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_print(const char *plu, const char *deptNum) {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_before_print()", LOG_DETAILED);
        return true;
    }

    bool allHooksPassed = true;
    for (auto& hook : hooks) {
        if (hook.before_print) {
            std::string logMsg = "before_print for: " + hook.soFilePath +
                                 ", PLU: " + std::string(plu ? plu : "NULL") + ", DeptNum: " + std::string(deptNum ? deptNum : "NULL");
            logMessage(logMsg, LOG_NON_DETAILED);
            bool hookResult = hook.before_print(plu, deptNum);
            logMessage(hook.soFilePath + " before_print response: " + (hookResult ? "true" : "false"), LOG_NON_DETAILED);
            if (!hookResult)
                allHooksPassed = false;
        }
    }
    return allHooksPassed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_remote_print(const char *plu, const char *deptNum) {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_before_remote_print()", LOG_DETAILED);
        return true;
    }

    bool allHooksPassed = true;
    for (auto& hook : hooks) {
        if (hook.before_remote_print) {
            std::string logMsg = "before_remote_print for: " + hook.soFilePath +
                                 ", PLU: " + std::string(plu ? plu : "NULL") + ", DeptNum: " + std::string(deptNum ? deptNum : "NULL");
            logMessage(logMsg, LOG_NON_DETAILED);
            bool hookResult = hook.before_remote_print(plu, deptNum);
            logMessage(hook.soFilePath + " before_remote_print response: " + (hookResult ? "true" : "false"), LOG_NON_DETAILED);
            if (!hookResult)
                allHooksPassed = false;
        }
    }
    return allHooksPassed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_after_print(bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel) {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_after_print()", LOG_DETAILED);
        return;
    }

    for (auto& hook : hooks) {
        if (hook.after_print) {
            std::string logMsg = "after_print for: " + hook.soFilePath + 
                ", success: " + (success ? "true" : "false") + 
                ", numLabelsToPrint: " + std::to_string(numLabelsToPrint) + 
                ", numLabelsPrinted: " + std::to_string(numLabelsPrinted) + 
                ", templateLabel: " + (templateLabel ? "true" : "false");
            logMessage(logMsg, LOG_NON_DETAILED);
            hook.after_print(success, numLabelsToPrint, numLabelsPrinted, templateLabel);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_custom_action(const char *action, int *rc, char *rcex, unsigned int rcexLen) {  

    std::string actionStr(action);
    size_t delimiterPos = actionStr.find("|");
    std::string commandType;
    std::string hookName;

    if(isAlreadyInitialized == false)
    {
        hobart_cust_hook_init();
        logMessage("Initializing customer hooks from custom action call", LOG_NON_DETAILED);
    }

    if (delimiterPos == std::string::npos) {
        // no '|' then treat the whole action as a custom hook action
        commandType.clear();
        hookName = actionStr;
    }
    else {
        commandType = actionStr.substr(0, delimiterPos); // RELOAD, UNLOAD, LOAD for which command
        hookName = actionStr.substr(delimiterPos + 1); // ALL, RFID, WM... for which hooks
    }

    if (hooks.empty()) {
        if (commandType != "LOAD") {
            logMessage("No hooks loaded, skipping " + commandType + " action", LOG_DETAILED);
            return;
        }
    }

    if (!commandType.empty()) {
        logMessage(commandType + " action detected for: " + hookName, LOG_NON_DETAILED);
    } else {
        logMessage("Custom action detected: " + hookName, LOG_NON_DETAILED);
    }

    if (commandType == "LOAD" || commandType == "UNLOAD") {
        if (!loadOrUnloadHooks(commandType, hookName)) {
            logMessage("loadOrUnload hooks call", LOG_DETAILED);
            //*rc = -1;
            return;
        }
    }
    else if (commandType == "RELOAD") {
        if (!reloadHooks(hookName)) {
            //*rc = -1;
            return;
        }
    }
    else {
        for (auto& hook : hooks) {
            if (hook.custom_action) {
                logMessage("Passing action to hook: " + hook.soFilePath, LOG_NON_DETAILED);

                logMessage("Return code before custom_action: " + std::to_string(*rc), LOG_DETAILED);

                hook.custom_action(action, rc, rcex, rcexLen);

                logMessage("Return code after custom_action: " + std::to_string(*rc), LOG_DETAILED);

                // This checks the return code after calling each hook
                // Hooks that return -1 do not recognize the command, so we can assume that there would only be one apphook giving
                // a return code other than -1. So as soon as a hook changes the return code from -1 we can stop there and leave the
                // return code as that value for it to be used elsewhere.
                if (*rc != -1) {
                    return;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_peripheral_version(int dev, int devex, int kind, const char *swVer, const char *hwVer) {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_update_peripheral_version()", LOG_DETAILED);
        return;
    }

    for (auto& hook : hooks) {
        if (hook.update_peripheral_version) {
            std::string logMsg = "update_peripheral_version for: " + hook.soFilePath +
                                 " with dev: " + std::to_string(dev) +
                                 ", devex: " + std::to_string(devex) +
                                 ", kind: " + std::to_string(kind) +
                                 ", swVer: " + swVer +
                                 ", hwVer: " + hwVer;
            logMessage(logMsg, LOG_NON_DETAILED);
            hook.update_peripheral_version(dev, devex, kind, swVer, hwVer);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_scale_state(int type, const char *value) {

    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_update_scale_state()", LOG_DETAILED);
        return;
    }

    for (auto& hook : hooks) {
        if (hook.update_scale_state) {
            std::string logMsg = "update_scale_state for: " + hook.soFilePath +
                                 " with type: " + std::to_string(type) +
                                 ", value: " + value;
            logMessage(logMsg, LOG_NON_DETAILED);
            hook.update_scale_state(type, value);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" CUSTHOOKDLL bool hobart_cust_hook_check(const char* check) {
    // This is used to check for if a certain hook is installed
    if (hooks.empty()) {
        logMessage("No hooks loaded, skipping hobart_cust_hook_check()", LOG_DETAILED);
        return false;
    }

    std::string checkStr = check;
    std::string prefix = "libHobartCustomerHook";

    logMessage("hobart_cust_hook_check called with value: " + checkStr, LOG_NON_DETAILED);
    for (auto& hook : hooks) {
        logMessage("Checking for hook: " + hook.soFilePath, LOG_DETAILED);

        // Check if the .so file name ends with the prefix + checkStr
        if (hook.soFilePath.rfind(prefix + checkStr) != std::string::npos) {
            logMessage("Found hook: " + hook.soFilePath, LOG_NON_DETAILED);
            return true;
        }
    }
    logMessage("No matching hooks found, returning false", LOG_DETAILED);
    return false;
}
