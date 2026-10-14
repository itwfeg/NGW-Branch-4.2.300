/**
 * @file logline.h
 * Classes that contain details of log lines.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef LOGLINE_H
#define LOGLINE_H

#include "log.h"

#include <QString>
#include <QDateTime>

#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class logLine
 * @brief This class stores data regarding a log line.
 */
class logLine
{
    typedef logLine _Myt;

public:

    /**
     * Constructor.
     *
     * @param[in] level  log line level
     * @param[in] file  source file name (use the __FILE__ macro)
     * @param[in] line  source file line (use the __LINE__ macro)
     * @param[in] name  class name
     * @param[in] function  class function
     */
    logLine( logLevel::type level,
        const QString& file,
        unsigned int line,
        const QString& name,
        const QString& function );

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    logLine( const _Myt& rhs ) {copy( rhs );}

    /**
     * Destructor.
     */
    virtual ~logLine() {}

    /**
     * Assignment operator.
     *
     * @param[in] rhs  value to assign from
     * @returns  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {copy( rhs ); return *this;}

    /**
     * Retrieve log level.
     *
     * @returns  log level
     */
    virtual logLevel::type level() const {return level_;}

    /**
     * Set log level.
     *
     * @param[in] value  log level
     */
    virtual void setLevel( logLevel::type value ) {level_ = value;}

    /**
     * Retrieve source filename.
     *
     * @returns  source file name
     */
    virtual QString sourceFilename() const {return sourceFilename_;}

    /**
     * Set source filename.
     *
     * Best way to set this is to use the @c __FILE__ macro.
     *
     * @param[in] value  source file name
     */
    virtual void setSourceFilename( const QString& value ) {sourceFilename_ = value;}

    /**
     * Retrieve source file line number.
     *
     * @returns  source file line
     */
    virtual unsigned int sourceLine() const {return sourceLine_;}

    /**
     * Set source file line number
     *
     * Best way to set this is to use the @c __LINE__ macro.
     *
     * @param[in] value  source file line
     */
    virtual void setSourceLine( unsigned int value ) {sourceLine_ = value;}

    /**
     * Retrieve class name.
     *
     * @returns  source file class name
     */
    virtual QString className() const {return className_;}

    /**
     * Set class name.
     *
     * @param[in] value  class name
     */
    virtual void setClassName( const QString& value ) {className_ = value;}

    /**
     * Retrieve class function.
     *
     * @returns  source file class function name
     */
    virtual QString classFunction() const {return classFunction_;}

    /**
     * Set class function name.
     *
     * @param[in] value  class function name
     */
    virtual void setClassFunction( const QString& value ) {classFunction_ = value;}

    /**
     * Retrieve log text line.
     *
     * @returns  log text
     */
    virtual QString text() const {return text_;}

    /**
     * Set log text.
     *
     * @param[in] value  log text
     */
    virtual void setText( const QString& value ) {text_ = value;}

    /**
     * Set log text from format.
     *
     * This function will set text from a formatted string.
     *
     * @param[in] format  format string
     */
    virtual void setText( const char *format, ... );

    /**
     * Set log text to hexadecimal string from buffer.
     *
     * This function will write a buffer in hexadecimal to the log text.
     *
     * The @p width parameter will control how big the text line is.  For example, using a width
     * of 8 on a buffer of numbers 0-19 would look like:
     * 00000000  00 01 02 03 04 05 06 07 08  ........
     * 00000008  09 0A 0B 0C 0D 0E 0F 10 11  ........
     * 00000010  12 13 14 15                 ....
     *
     * @param[in] buffer  buffer to log out
     * @param[in] bufferLen  size of buffer
     * @param[in] width  width of hexadecimal lines to write out.
     */
    virtual void setTextHex( const void *buffer, unsigned int bufferLen, unsigned int width = DEFAULT_WIDTH );

    /**
     * Retrieve log line time stamp.
     *
     * @returns  time stamp of log line
     */
    virtual QDateTime timeStamp() const {return stamp_;}

    /**
     * Set log line time stamp.
     *
     * @param[in] value  time stamp
     */
    virtual void setTimeStamp( const QDateTime& value ) {stamp_ = value;}

    /**
     * Get log line thread id.
     *
     * This is the thread id of who logged the line.
     *
     * @returns  thread id
     */
    virtual Qt::HANDLE threadId() const {return threadId_;}

    /**
     * Set thread id.
     *
     * @param[in] value  thread id
     */
    virtual void setThreadId( Qt::HANDLE value ) {threadId_ = value;}

private:

    enum {DEFAULT_WIDTH = 16};

    enum {MIN_WIDTH = 8};
    enum {MAX_WIDTH = 64};

    logLevel::type level_;

    QString sourceFilename_;
    unsigned int sourceLine_;

    QString className_;
    QString classFunction_;

    QString text_;

    QDateTime stamp_;
    Qt::HANDLE threadId_;

    /**
     * Copy object.
     */
    void copy( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LOGLINE_H
