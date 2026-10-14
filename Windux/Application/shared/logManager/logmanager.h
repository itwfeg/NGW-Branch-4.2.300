/**
 * @file logmanager.h
 * Log management class.
 *
 * For the xml configuration a sample tree appears as follows:
 * @code
 * <logconfig>
 *   <logs>
 *   </logs>
 *   <root>
 *   </root>
 *   <appenders>
 *   </appenders>
 * </logconfig>
 * @endcode
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "log.h"
#include "logappender.h"

#include <QString>
#include <QMap>

#include <QMutex>
#include <QScopedPointer>

class QXmlStreamReader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct create_from_config_t
 * @brief Create @c logManager from config file.
 */
struct create_from_config_t {};

/**
 * @struct create_root_file_appender_t
 * @brief Create @c logManager with a file based log.
 */
struct create_root_file_appender_t {};

/**
 * Used to construct @c logManager from a config file.
 */
static const create_from_config_t create_from_config = {};

/**
 * Used to construct @c logManager with a file based log.
 */
static const create_root_file_appender_t create_root_file_appender = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class logManager
 * @brief Log management class. Provides top level access to logging.
 *
 * Allows multiple log files to be configured and maintained.
 *
 * You need to initialize the manager before use.  There are two ways of doing this, by using an
 * xml configuration file (for multiple logs and multiple appenders) or by passing in a log
 * filename (for a single log and single appender).
 *
 * Examples of each:
 * @code
 *   int main( int argc, char *argv[] ) {
 *       // init logging using xml config
 *       logManager::initialize( "appname.xml", create_from_config );
 *       // init logging for a single log file
 *       logManager::initialize( "appname.txt", create_root_file_appender );
 *   }
 * @endcode
 *
 * You may also set the log level of each appender directly (though its easier to do so in a
 * config file.
 *
 * Example:
 * @code
 *   logManager::instance()->rootAppender().setLevel( logLevel::Debug );
 *   logManager::instance()->appender( "myAppender" ).setLevel( logLevel::Warning );
 * @endcode
 *
 * If you do not initialize the log manager it will attempt to initialize itself.  First it will
 * look for a "config.xml" in the current path. If this file exists it will configure based on
 * that file.  If this file does not exist no logger will be generated.
 *
 * The easiest way to use logging is through the macros at the bottom of this file. For example:
 * @code
 *   void myClass::myFunction() {
 *       LOG_CLASS_INFO( "myClass", "myFunction" );
 *       LOG_INFO( "my pointer is %d", this );
 *       LOG_ERROR( "oh noes!", NULL );
 *       // log out a binary buffer
 *       char buffer[128];
 *       LOG_TRACE_DUMP( buffer, sizeof(buffer) );
 *       LOG_WARN( "danger %s!", "will robinson" );
 *   }
 * @endcode
 *
 * For advanced logging you will need to setup a config file.  The below example creates a root
 * appender that will log to a file and to qt log at the same time.  Another appender named randy
 * is also created that logs to the randy.txt file.
 * @code
 *   <logconfig>
 *       <logs>
 *           <log name="app" type="file">
 *               <format>%date{MM/dd/yyyy hh:mm:ss.zzz} [%thread{%08x},%levelnum] %class{%-15.15s} %method{%-15.15s} %message\n</format>
 *               <filename>app.txt</filename>
 *               <rolloversize>10</rolloversize>
 *           </log>
 *           <log name="qt" type="qt">
 *               <format>%date{hh:mm:ss.zzz} %class{%-15.15s} %method{%-15.15s} %message</format>
 *           </log>
 *           <log name="randy" type="file">
 *               <format>%level{%-5.5s} %message\n</format>
 *               <filename>randy.txt</filename>
 *           </log>
 *       </logs>
 *       <root>
 *           <level>DEBUG</level>
 *           <log-ref>app</log-ref>
 *           <log-ref>qt</log-ref>
 *       </root>
 *       <appenders>
 *           <appender name="randy">
 *               <level>WARN</level>
 *               <log-ref>randy</log-ref>
 *           </appender>
 *       </appenders>
 *   </logconfig>
 * @endcode
 */
class logManager
{
    typedef logManager _Myt;

public:

    /**
     * Destructor.
     */
    virtual ~logManager();

    /**
     * Initialize log manager.
     *
     * This function will create logs and appenders using the passed in xml configuration file.
     *
     * @param[in] filename  xml configuration filename
     */
    static void initialize( const QString& filename, create_from_config_t );

    /**
     * Initialize log manager.
     *
     * This function will create a file based log and root appender pointing to it.
     *
     * This version of initialize() is intended for simpler applications that require only a single
     * log file to be generated.
     *
     * @param[in] filename  log filename to create
     */
    static void initialize( const QString& filename, create_root_file_appender_t );

    /**
     * Finalize log manager.
     */
    static void finalize();

    /**
     * Retrieve root for logging purposes.
     *
     * @returns  reference to appender
     */
    virtual logAppender& rootAppender() {return root_;}

    /**
     * Retrieve appender for logging purposes.
     *
     * If appender does not exist, the root appender will be returned instead.
     *
     * @param[in] name  appender name
     * @returns  reference to appender
     */
    virtual logAppender& appender( const QString& name );

    /**
     * Get log instance.
     *
     * Note:
     * Please initialize the logger before calling this function!  If you fail to do this an empty
     * logger will be created and no logging will occur.
     *
     * @sa initialize
     *
     * @returns  pointer to log manager instance
     */
    static logManager *instance();

private:

    typedef QMap<QString, logSharedPointer> logMap;
    typedef QMap<QString, logAppender> logAppenderMap;

    static QMutex mutex_;
    static QScopedPointer<logManager> instance_;

    logMap logs_;
    logAppenderMap appenders_;

    logAppender root_;

    /**
     * Constructor.
     */
    logManager();

    /**
     * Constructor.
     */
    logManager( const QString& filename, create_from_config_t );

    /**
     * Constructor.
     */
    logManager( const QString& filename, create_root_file_appender_t );

    // not implemented
    logManager( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /**
     * Parse 'logconfig' node.
     */
    void parseLogConfig( QXmlStreamReader& xml );

    /**
     * Parse 'logs' node.
     */
    void parseLogs( QXmlStreamReader& xml );

    /**
     * Parse 'log' node.
     */
    void parseLog( QXmlStreamReader& xml, const QString& name, const QString& type );

    /**
     * Parse 'root' node.
     */
    void parseRootNode( QXmlStreamReader& xml );

    /**
     * Parse 'appenders' node.
     */
    void parseAppenders( QXmlStreamReader& xml );

    /**
     * Parse 'appender' node.
     */
    void parseAppender( QXmlStreamReader& xml, const QString& name );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @def LOG_CLASS_INFO( name, function )
 * Declares a class and member function name for logging.
 *
 * This macro is required in order to use the logging macros.
 *
 * For classes please use class name as @p name and the member function name as @p function. For
 * example:
 * @code
 *   void myClass::myFunction() {
 *       LOG_CLASS_INFO( "myClass", "myFunction" );
 *   }
 * @endcode
 *
 * For functions you can leave the @p name blank.  For example:
 * @code
 *   void function() {
 *       LOG_CLASS_INFO( "", "function" );
 *   }
 * @endcode
 */
#define LOG_CLASS_INFO( name, function ) static const char __CLASSNAME__[] = name; static const char __CLASSFUNC__[] = function

/**
 * @def LOG_ERROR( format, ... )
 * Logs an error to the error log.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_ERROR( format, ... ) logManager::instance()->appender( __CLASSNAME__ ).write( logLevel::Error, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, format, __VA_ARGS__ )

/**
 * @def LOG_WARN( format, ... )
 * Logs a warning to the error log.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_WARN( format, ... ) logManager::instance()->appender( __CLASSNAME__ ).write( logLevel::Warning, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, format, __VA_ARGS__ )

/**
 * @def LOG_INFO( format, ... )
 * Logs information to the error log.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_INFO( format, ... ) logManager::instance()->appender( __CLASSNAME__ ).write( logLevel::Info, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, format, __VA_ARGS__ )

/**
 * @def LOG_DEBUG( format, ... )
 * Logs debug statements to the error log.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_DEBUG( format, ... ) logManager::instance()->appender( __CLASSNAME__ ).write( logLevel::Debug, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, format, __VA_ARGS__ )

/**
 * @def LOG_TRACE( format, ... )
 * Logs trace information to the error log.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_TRACE( format, ... ) logManager::instance()->appender( __CLASSNAME__ ).write( logLevel::Trace, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, format, __VA_ARGS__ )

/**
 * @def LOG_ERROR_DUMP( buffer, len )
 * Log a buffer of data in hex format to the log at error level.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_ERROR_DUMP( buffer, len ) logManager::instance()->appender( __CLASSNAME__ ).writeHex( logLevel::Error, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, buffer, len )

/**
 * @def LOG_WARN_DUMP( buffer, len )
 * Log a buffer of data in hex format to the log at warning level.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_WARN_DUMP( buffer, len ) logManager::instance()->appender( __CLASSNAME__ ).writeHex( logLevel::Warning, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, buffer, len )

/**
 * @def LOG_INFO_DUMP( buffer, len )
 * Log a buffer of data in hex format to the log at info level.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_INFO_DUMP( buffer, len ) logManager::instance()->appender( __CLASSNAME__ ).writeHex( logLevel::Info, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, buffer, len )

/**
 * @def LOG_DEBUG_DUMP( buffer, len )
 * Log a buffer of data in hex format to the log at debug level.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_DEBUG_DUMP( buffer, len ) logManager::instance()->appender( __CLASSNAME__ ).writeHex( logLevel::Debug, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, buffer, len )

/**
 * @def LOG_TRACE_DUMP( buffer, len )
 * Log a buffer of data in hex format to the log at trace level.
 *
 * Note:
 * You will need to define LOG_CLASS_INFO before using this macro.
 *
 * @sa LOG_CLASS_INFO
 */
#define LOG_TRACE_DUMP( buffer, len ) logManager::instance()->appender( __CLASSNAME__ ).writeHex( logLevel::Trace, __FILE__, __LINE__, __CLASSNAME__, __CLASSFUNC__, buffer, len )

/** @} */

#endif // LOGMANAGER_H
