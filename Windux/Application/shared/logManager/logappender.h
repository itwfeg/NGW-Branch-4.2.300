/**
 * @file logappender.h
 * Appender classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef LOGAPPENDER_H
#define LOGAPPENDER_H

#include "log.h"

#include <QString>
#include <QList>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class logAppender
 * @brief This class stores a list of logs to append to.
 *
 * When writing log data to this object all logs within the object will be appended to.
 */
class logAppender : public QList<logSharedPointer>
{
    typedef logAppender _Myt;
    typedef QList<logSharedPointer> _Mybase;

public:

    /**
     * Constructor.
     */
    logAppender() : _Mybase(), level_( (logLevel::type) DEFAULT_LOG_LEVEL ) {}

    /**
     * Constructor.
     *
     * @param[in] name  appender name
     * @param[in] level  appender log level
     */
    logAppender(
        const QString& name,
        logLevel::type level = (logLevel::type) DEFAULT_LOG_LEVEL ) : _Mybase(), name_( name ), level_( level ) {}

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    logAppender( const _Myt& rhs ) : _Mybase() {copy( rhs );}

    /**
     * Destructor.
     */
    virtual ~logAppender() {}

    /**
     * Assignment operator.
     *
     * @param[in] rhs  value to assign from
     * @returns  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {copy( rhs ); return *this;}

    /**
     * Retrieve appender name.
     *
     * @returns  appender name
     */
    virtual QString name() const {return name_;}

    /**
     * Set appender name.
     *
     * @param[in] value  appender name
     */
    virtual void setName( const QString& value ) {name_ = value;}

    /**
     * Retrieve appender log level.
     *
     * @returns  log level of appender
     */
    virtual logLevel::type level() const {return level_;}

    /**
     * Set appender log level.
     *
     * @param[in] value  log level
     */
    virtual void setLevel( logLevel::type value ) {level_ = value;}

    /**
     * Write to log.
     *
     * This function will write a formatted string to the log file if the log level is greater
     * or equal to the level passed in.
     *
     * Using this function directly is possible, but using the macros within logmanager is suggested.
     *
     * Example usage:
     * @code
     *   log::instance()->write(
     *       logLevel::Info,
     *       __FILE__,
     *       __LINE__,
     *       "myClass",
     *       "myFunction",
     *       "the value is %d",
     *       42 );
     * @endcode
     *
     * @param[in] level  log line level
     * @param[in] file  source file name (use the __FILE__ macro)
     * @param[in] line  source file line (use the __LINE__ macro)
     * @param[in] name  class name
     * @param[in] function  class function
     * @param[in] format  format string
     */
    virtual void write(
        logLevel::type level,
        const char *file,
        unsigned int line,
        const char *name,
        const char *function,
        const char *format,
        ... );

    /**
     * Write hex dump to log.
     *
     * This function will write a buffer in hexadecimal to the log file if the log level is greater
     * or equal to the level passed in.
     *
     * Using this function directly is possible, but using the macros within logmanager is suggested.
     *
     * Example usage:
     * @code
     *   char buffer[128];
     *   log::instance()->write(
     *       logLevel::Info,
     *       __FILE__,
     *       __LINE__,
     *       "myClass",
     *       "myFunction",
     *       buffer,
     *       sizeof(buffer) );
     * @endcode
     *
     * @param[in] level  log line level
     * @param[in] file  source file name (use the __FILE__ macro)
     * @param[in] line  source file line (use the __LINE__ macro)
     * @param[in] name  class name
     * @param[in] function  class function
     * @param[in] buffer  buffer to log out
     * @param[in] bufferLen  size of buffer
     */
    virtual void writeHex(
        logLevel::type level,
        const char *file,
        unsigned int line,
        const char *name,
        const char *function,
        const void *buffer,
        unsigned int bufferLen );

protected:

    /**
     * Check if logging enabled at specific level.
     *
     * @param[in] level  log level to check
     * @returns  @c true if enabled, @c false otherwise
     */
    virtual bool enabled( logLevel::type level ) const;

private:

    enum {DEFAULT_LOG_LEVEL = logLevel::Warning};

    QString name_;
    logLevel::type level_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

    /**
     * Append data to logs.
     */
    void append( const logLine& data );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LOGAPPENDER_H
