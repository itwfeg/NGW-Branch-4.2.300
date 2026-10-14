/**
 * @file nowservingdialog.h
 * Dialog for managing now serving printer and customer queues.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef NOWSERVINGDIALOG_H
#define NOWSERVINGDIALOG_H

#include "../sessiondialog.h"

class NowServingQueueManagement;

class QLabel;
class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for managing now serving printer and customer queues.
class NowServingDialog : public SessionDialog
{
    Q_OBJECT

    typedef NowServingDialog _Myt;
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
    explicit NowServingDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~NowServingDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if dialog can be shown in dynamic content area.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowDynamicContentArea() const {return true;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for handling button clicked.
    void onButtonClicked( QWidget *widget );

private:

    enum takeNumberType
    {
        Electronic = 1,
        PrePrintedPaper = 2
    };

    QSignalMapper *buttonSignalMapper_;

    bool error_;
    int waitingForResponse_;

    takeNumberType numberIssuer_;

    // title

    QLabel *titleLabel_;

    // customer queues

    QLabel *custQueueNextLabel_;
    QLabel *custQueueNext_;

    QLabel *custQueueSizeLabel_;
    QLabel *custQueueSize_;

    QPushButton *serveNextCustomer_;
    QPushButton *serveSpecificCustomer_;
    QPushButton *deleteCustomerQueue_;

    // printer queues

    QLabel *printerQueueNextLabel_;
    QLabel *printerQueueNext_;

    QPushButton *printCustomerLabel_;
    QPushButton *setNextCustomerNumber_;

    // queue management

    QPushButton *resetEverything_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Request queue status.
    void requestQueueStatus( bool cust, bool printer );

    /// Validate message.
    bool validateMsg( const NowServingQueueManagement& m );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NOWSERVINGDIALOG_H
