/**
 * @file log.h
 * Log base class definition.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef LOG_H
#define LOG_H

#include <QMutex>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QWeakPointer>

class logLine;

class QXmlStreamReader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class logLevel
 * @brief Logging levels.
 *
 * Enumeration class of log levels.
 */
class logLevel
{
public:

    /**
     * Log levels.
     */
    enum type
    {
        Disabled            = 0,                            ///< no logging
        Error               = 1,                            ///< bad pointers, exceptions
        Warning             = 2,                            ///< bad configuration, recoverable errors
        Info                = 3,                            ///< configuration, state changes
        Debug               = 4,                            ///< stuff a developer would use to troubleshoot
        Trace               = 5                             ///< developer stuff but much more detail
    };

    /**
     * Convert from string.
     *
     * @param[in] value  log level in string form (see above)
     * @returns  log level
     */
    static type fromString( const QString& value ) {
        if ( toString( Error ) == value )
            return Error;
        else if ( toString( Warning ) == value )
            return Warning;
        else if ( toString( Info ) == value )
            return Info;
        else if ( toString( Debug ) == value )
            return Debug;
        else if ( toString( Trace ) == value )
            return Trace;
        return Disabled;
    }

    /**
     * Convert to string.
     *
     * @param[in] value  log level
     * @returns  log level in string form (see above)
     */
    static QString toString( type value ) {
        if ( Error == value )
            return "ERROR";
        else if ( Warning == value )
            return "WARN";
        else if ( Info == value )
            return "INFO";
        else if ( Debug == value )
            return "DEBUG";
        else if ( Trace == value )
            return "TRACE";
        return "DISABLED";
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class logger
 * @brief Log class base object.
 *
 * This is the base class for all log objects.  Each derived class needs to implement the
 * writeString() function.
 */
class logger
{
    typedef logger _Myt;

public:

    /**
     * Destructor.
     */
    virtual ~logger() {}

    /**
     * Retrieve line format.
     *
     * @returns  format of log line
     */
    virtual QString lineFormat() const {return lineFormat_;}

    /**
     * Set line format.
     *
     * Supported formats are as follows:
     * @arg %date - date and time stamp of log line
     * @arg %thread - thread id of who logged the line
     * @arg %level - log line level (i.e. ERROR, INFO, ...)
     * @arg %levelnum - log line level value
     * @arg %class - class who logged the line
     * @arg %method - method that logged the line
     * @arg %message - message that was logged
     * @arg %file - source file name
     * @arg %line - source file line number
     *
     * @param[in] value  format of log line
     */
    virtual void setLineFormat( const QString& value ) {lineFormat_ = value;}

    /**
     * Write to log.
     *
     * @param[in] line  line to write
     */
    virtual void write( const logLine& line );

protected:

    /**
     * Constructor.
     */
    logger();

    /**
     * Retrieve formatted log line text suitable for writing.
     *
     * @param[in] line  log line
     * @returns  text string representing a log line in proper format
     */
    virtual QString formatted( const logLine& line ) const;

    /**
     * Write string to log.
     *
     * @param[in] data  string data to write
     */
    virtual void writeString( const QString& data ) = 0;

private:

    QString lineFormat_;

    QStringList formats_;

    QMutex m_;

    /**
     * Find format in string.
     */
    bool findFormat( const QString& line, QString& name, QString& format ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @typedef logSharedPointer
 * @brief Shared @c log pointer class.
 */
typedef QSharedPointer<logger> logSharedPointer;

/**
 * @typedef logWeakPointer
 * @brief Weak @c log pointer class.
 */
typedef QWeakPointer<logger> logWeakPointer;

/** @} */

#endif // LOG_H
