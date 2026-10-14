/**
 * @file serviceNewWrapperPkgHistory.h
 * @brief Service screen for viewing wrapper package history.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERPKGHISTORY_H
#define SERVICENEWWRAPPERPKGHISTORY_H

#include "screenwidget.h"

class QTreeWidget;
class QTreeWidgetItem;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for viewing wrapper package history.
class serviceNewWrapperPkgHistory : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperPkgHistory _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceNewWrapperPkgHistory( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperPkgHistory();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:

    QTreeWidget *table_;

    QFont tableFont_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Resize table column widths.
    void calcTableColumnWidths();

    /// Set font point sizes for item in table.
    void calcTableFontSizes( QTreeWidgetItem *item ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERPKGHISTORY_H
