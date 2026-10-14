/**
 * @file printticketwidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PRINTTICKETWIDGET_H
#define PRINTTICKETWIDGET_H

#include <QMap>
#include <QString>
#include <QWidget>

class errorMessageWidget;
class serverComm;
class takeNumberWidget;
class ticketPrinter;

class QLabel;
class QStackedWidget;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for ticket printing.
class printTicketWidget : public QWidget
{
    Q_OBJECT

    typedef printTicketWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] params  configuration
     * @param[in] parent  parent widget
     */
    printTicketWidget( const QMap<QString, QString>& params, QWidget *parent = 0 );

    /// Destructor.
    virtual ~printTicketWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate widgets.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for heartbeat.
    void onHeartbeat();

    /// Slot for printing a ticket.
    void onPrintTicket();

    /// Slot to dismiss error message.
    void onDismissErrorMessage();

private:

    QMap<QString, QString> params_;

    QWidget *logo_;
    QLabel *dept_;

    QStackedWidget *screens_;
    takeNumberWidget *takeNumber_;
    errorMessageWidget *errorMessage_;

    serverComm *server_;

    ticketPrinter *printer_;

    QTimer *dismissErrorMessage_;
    QTimer *heartbeat_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PRINTTICKETWIDGET_H
