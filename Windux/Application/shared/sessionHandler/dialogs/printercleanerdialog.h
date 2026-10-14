#ifndef PRINTERCLEANERDIALOG_H
#define PRINTERCLEANERDIALOG_H

/**
 * @file printercleanerdialog.h
 * Dialog for cleaning labels.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Joseph DiCarlantonio
 *
 * @addtogroup sessionHandler
 * @{
 */

#include "../sessiondialog.h"

#include <Messages.h>

class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Ui {
class PrinterCleanerDialog;
}

/// Dialog for adjusting printer settings.
class PrinterCleanerDialog : public SessionDialog
{
    Q_OBJECT

    typedef PrinterCleanerDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit PrinterCleanerDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~PrinterCleanerDialog();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for button click.
    void onCleanButtonClicked( );

private:
    Ui::PrinterCleanerDialog *ui;

    /// Initialize.
    void initialize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PRINTERCLEANERDIALOG_H
