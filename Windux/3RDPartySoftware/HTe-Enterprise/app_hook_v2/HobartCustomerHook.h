#include <QtCore/qglobal.h>

#if defined(HOBARTCUSTOMERHOOK_TEST_APPLICATION)
#  define CUSTHOOKDLL
#elif defined(HOBARTCUSTOMERHOOK_EXPORTS)
#  define CUSTHOOKDLL Q_DECL_EXPORT
#else
#  define CUSTHOOKDLL Q_DECL_IMPORT
#endif

// Note:
// The contents and methods found here should match header in Application/HobartCustomerHook

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
