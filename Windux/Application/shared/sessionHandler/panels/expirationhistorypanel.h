/**
 * @file expirationhistorypanel.h
 * Panel for expiration history in operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef EXPIRATIONHISTORYPANEL_H
#define EXPIRATIONHISTORYPANEL_H

#include "operatemodepanelex.h"

class QLabel;
class QTreeWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for expiration history in operate mode.
class ExpirationHistoryPanel : public OperateModePanelEx
{
    Q_OBJECT

    typedef ExpirationHistoryPanel _Myt;
    typedef OperateModePanelEx _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit ExpirationHistoryPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~ExpirationHistoryPanel();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:

    QLabel *notes_;
    QTreeWidget *table_;

    /// Initialize.
    void initialize();

    /// Convert to days text.
    static QString convertDaysText( int days, QColor& c );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // EXPIRATIONHISTORYPANEL_H
