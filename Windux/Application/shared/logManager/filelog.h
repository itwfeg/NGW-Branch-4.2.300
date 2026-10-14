/**
 * @file filelog.h
 * File based logger class.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef FILELOG_H
#define FILELOG_H

#include "log.h"

#include <QFile>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class fileLog
 * @brief File based log class object.
 */
class fileLog : public logger
{
    typedef fileLog _Myt;
    typedef logger _Mybase;

public:

    /**
     * Destructor.
     */
    virtual ~fileLog();

    /**
     * Get rollover size.
     *
     * @returns  log rollover size
     */
    virtual unsigned int rolloverSize() const {return rolloverSize_;}

    /**
     * Set rollover size.
     *
     * When the log size (in bytes) reaches the rollover size it will be backed up and a new log
     * file will begin.
     *
     * @param[in] rolloverSize  rollover size to set
     */
    virtual void setRolloverSize( unsigned int rolloverSize ) {rolloverSize_ = rolloverSize;}

    /**
     * Factory function for @c fileLog objects.
     *
     * @param[in] xml  xml stream reader sitting at start of log node
     * @returns  pointer to object
     */
    static _Myt *create( QXmlStreamReader& xml );

    /**
     * Factory function for @c fileLog objects.
     *
     * @param[in] filename  log filename
     * @returns  pointer to object
     */
    static _Myt *create( const QString& filename );

protected:

    /**
     * Constructor.
     *
     * @param[in] filename  log filename
     */
    fileLog( const QString& filename );

    /**
     * Write string to log.
     *
     * @param[in] data  string data to write
     */
    virtual void writeString( const QString& data );

private:

    enum {DEFAULT_ROLLOVER_SIZE = 10485760}; // 10 MB

    QFile file_;

    unsigned int rolloverSize_;

    /**
     * Open log file.
     */
    bool open();

    /**
     * Rollover log file.
     */
    void rollover( bool force = false );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FILELOG_H
