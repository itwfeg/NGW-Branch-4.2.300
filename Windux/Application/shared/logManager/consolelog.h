/**
 * @file consolelog.h
 * Rolling log that displays in a dialog window.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup loggingCsu
 * @{
 */

#ifndef CONSOLELOG_H
#define CONSOLELOG_H

#include "log.h"

#include <QDialog>

class QPlainTextEdit;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class consoleLog
 * @brief Rolling log that displays in a dialog window. Very similiar to the Java Console.
 */
class consoleLog : public QDialog, public logger
{
    Q_OBJECT

    typedef consoleLog _Myt;
    typedef QDialog _Mybase;

public:

    /**
     * Destructor.
     */
    virtual ~consoleLog();

    /**
     * Factory function for @c consoleLog objects.
     *
     * @param[in] xml  xml stream reader sitting at start of log node
     * @returns  pointer to object
     */
    static _Myt *create( QXmlStreamReader& xml );

    /**
     * Factory function for @c consoleLog objects.
     *
     * @returns  pointer to object
     */
    static _Myt *create();

protected:

    /**
     * Constructor.
     */
    consoleLog();

    /**
     * Write string to log.
     *
     * @param[in] data  string data to write
     */
    virtual void writeString( const QString& data );

private Q_SLOTS:

    /**
     * Slot for when clear is pressed.
     */
    void onClearButtonClicked();

    /**
     * Slot for when copy is pressed.
     */
    void onCopyButtonClicked();

    /**
     * Slot for when close is pressed.
     */
    void onCloseButtonClicked();

private:

    QPlainTextEdit *log_;

    QPushButton *clearButton_;
    QPushButton *copyButton_;
    QPushButton *closeButton_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // CONSOLELOG_H
