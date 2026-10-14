/**
 * @file qtlog.h
 * Logger class that uses the Qt logging system.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef QTLOG_H
#define QTLOG_H

#include "log.h"
#include "logline.h"

#include <QDebug>
#include <QXmlStreamReader>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class qtLog
 * @brief Qt log class wrapper object.
 */
class qtLog : public logger
{
    typedef qtLog _Myt;
    typedef logger _Mybase;

public:

    /**
     * Destructor.
     */
    virtual ~qtLog() {}

    /**
     * Write to log.
     *
     * @param[in] line  line to write
     */
    virtual void write( const logLine& line );

    /**
     * Factory function for @c fileLog objects.
     *
     * @param[in] xml  xml stream reader sitting at start of log node
     * @returns  pointer to object
     */
    static _Myt *create( QXmlStreamReader& xml );

protected:

    /**
     * Constructor.
     */
    qtLog() : _Mybase() {}

    /**
     * Write string to log.
     *
     * @param[in] data  string data to write
     */
    virtual void writeString( const QString& data );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // QTLOG_H
