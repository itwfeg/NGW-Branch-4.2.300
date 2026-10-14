/**
 * @file takenumberwidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TAKENUMBERWIDGET_H
#define TAKENUMBERWIDGET_H

#include <QWidget>
#include <QString>

class QLabel;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget to initiate ticket printing.
class takeNumberWidget : public QWidget
{
    Q_OBJECT

    typedef takeNumberWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Signal emitted for printing a ticket.
    void printTicket();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    takeNumberWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~takeNumberWidget();

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

    /// Slot for disabling print ticket.
    void onDisablePrintTicket();

    /// Slot for enabling print ticket.
    void onEnablePrintTicket();

private:

    QTimer *enablePrintTicketTimer_;

    QLabel *printTicketLabel_;
    QPushButton *printTicket_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TAKENUMBERWIDGET_H
