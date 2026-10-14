#pragma once

#include <vector>
#include <string>

#if defined(WIN32)
#ifdef HOBARTCUSTOMERHOOK_EXPORTS
#define CUSTHOOKDLL __declspec(dllexport)
#else
#define CUSTHOOKDLL __declspec(dllimport)
#endif

#elif defined(__GNUC__)
#ifdef HOBARTCUSTOMERHOOK_EXPORTS
#define CUSTHOOKDLL __attribute__((visibility("default")))
#else
#define CUSTHOOKDLL
#endif

#endif

#define HOOK_SS_SCALE_BUILD_NUMBER                  0
#define HOOK_SS_SCALE_MODE                          1
#define HOOK_SS_SCALE_MODE_VALUE                    2
#define HOOK_SS_SCALE_MODE_VALUE_DESCRIPTION        3
#define HOOK_SS_SCALE_ITEM_ENTRY_TYPE               4
#define HOOK_SS_SCALE_LABEL_STOCK_1                 5
#define HOOK_SS_SCALE_LABEL_STOCK_2                 6
#define HOOK_SS_SCALE_LABEL_STOCK_3                 7
#define HOOK_SS_SCALE_WEIGHER_PRECISION             8
#define HOOK_SS_SCALE_WEIGHER_CALIB_STAMP           9
#define HOOK_SS_SCALE_WEIGHER_CONF_STAMP            10
#define HOOK_SS_SCALE_ALL_FILES_DELETED             11

#if defined ( __linux__ )

extern "C" CUSTHOOKDLL bool hobart_cust_hook_init();
extern "C" CUSTHOOKDLL void hobart_cust_hook_finalize();
extern "C" CUSTHOOKDLL void hobart_cust_hook_config_saved();
extern "C" CUSTHOOKDLL void hobart_cust_hook_before_open_database( const char *dbname );
extern "C" CUSTHOOKDLL void hobart_cust_hook_open_database();
extern "C" CUSTHOOKDLL bool hobart_cust_hook_before_plu_read( const char *plu, const char *deptNum );
extern "C" CUSTHOOKDLL bool hobart_cust_hook_before_plu_text_read( const char *number, const char *deptNum, int type );
extern "C" CUSTHOOKDLL bool hobart_cust_hook_before_print( const char *plu, const char *deptNum );
extern "C" CUSTHOOKDLL bool hobart_cust_hook_before_remote_print( const char *plu, const char *deptNum );
extern "C" CUSTHOOKDLL void hobart_cust_hook_after_print( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel );
extern "C" CUSTHOOKDLL void hobart_cust_hook_custom_action( const char *action, int *rc, char *rcex, unsigned int rcexLen );
extern "C" CUSTHOOKDLL void hobart_cust_hook_update_peripheral_version( int dev, int devex, int kind, const char *swVer, const char *hwVer );
extern "C" CUSTHOOKDLL void hobart_cust_hook_update_scale_state( int type, const char *value );
extern "C" CUSTHOOKDLL bool hobart_cust_hook_check( const char *check );

#endif

typedef bool (*hobart_cust_hook_init_ptr_t)();
typedef void (*hobart_cust_hook_finalize_ptr_t)();
typedef void (*hobart_cust_hook_config_saved_ptr_t)();
typedef void (*hobart_cust_hook_before_open_database_ptr_t)(const char *dbname);
typedef void (*hobart_cust_hook_open_database_ptr_t)();
typedef bool (*hobart_cust_hook_before_plu_read_ptr_t)(const char *plu, const char *deptNum);
typedef bool (*hobart_cust_hook_before_plu_text_read_ptr_t)(const char *number, const char *deptNum, int type);
typedef bool (*hobart_cust_hook_before_print_ptr_t)(const char *plu, const char *deptNum);
typedef bool (*hobart_cust_hook_before_remote_print_ptr_t)(const char *plu, const char *deptNum);
typedef void (*hobart_cust_hook_after_print_ptr_t)(bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel);
typedef void (*hobart_cust_hook_custom_action_ptr_t)(const char *action, int *rc, char *rcex, unsigned int rcexLen);
typedef void (*hobart_cust_hook_update_peripheral_version_ptr_t)(int dev, int devex, int kind, const char *swVer, const char *hwVer);
typedef void (*hobart_cust_hook_update_scale_state_ptr_t)(int type, const char *value);

// Define log levels
enum LogLevel {
    LOG_OFF = 0,
    LOG_NON_DETAILED = 1,
    LOG_DETAILED = 2
};

// Declare the current log level variable
extern LogLevel currentLogLevel;

struct Hook {
    void* handle;
    std::string soFilePath;
    hobart_cust_hook_init_ptr_t init;
    hobart_cust_hook_finalize_ptr_t finalize;
    hobart_cust_hook_config_saved_ptr_t config_saved;
    hobart_cust_hook_before_open_database_ptr_t before_open_database;
    hobart_cust_hook_open_database_ptr_t open_database;
    hobart_cust_hook_before_plu_read_ptr_t before_plu_read;
    hobart_cust_hook_before_plu_text_read_ptr_t before_plu_text_read;
    hobart_cust_hook_before_print_ptr_t before_print;
    hobart_cust_hook_before_remote_print_ptr_t before_remote_print;
    hobart_cust_hook_after_print_ptr_t after_print;
    hobart_cust_hook_custom_action_ptr_t custom_action;
    hobart_cust_hook_update_peripheral_version_ptr_t update_peripheral_version;
    hobart_cust_hook_update_scale_state_ptr_t update_scale_state;
};
