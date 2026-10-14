/**
 * @file meatspecsheetwidget.h
 * Widget for meat specification sheet.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Joe E. Crew
 * @inspired by Randy Blankley
 */

#ifndef MEATSPECSHEETWIDGET_H
#define MEATSPECSHEETWIDGET_H

#include <QWidget>

#include <Session.h>

class MadpCurrencySettings;
class OperateMode;
class ScaleConfiguration;
class SessionMainWindow;

class QLabel;
class QTreeWidget;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for meat specification sheet.
class meatSheetWidget : public QWidget
{
    Q_OBJECT

    typedef meatSheetWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit meatSheetWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~meatSheetWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

public Q_SLOTS:

    /// Update list of items.
    virtual void updateList();

protected:

    Session *session_;                                  ///< Pointer to session.
    SessionMainWindow *mainWindow_;                     ///< Pointer to session main window.

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot to handle selection changing.
    void onTableItemSelectionChanged();

    /// Slot to handle session messages.
    void onSessionMessageReceiveInternal( const SessionMessage& msg );

private:

    QLabel *numberOfSidesRemainingLabel_;
    QLabel *numberOfSidesRemaining_;

    QTreeWidget *table_;

    QLabel *totalLabel_;
    QLabel *total_;

    hobart::pushButton *removeAllItems_;
    hobart::pushButton *removeItem_;
    hobart::pushButton *print_;

    OperateMode *operateMode_;

    ScaleConfiguration *scaleConfig_;
    MadpCurrencySettings *currencySettings_;

    int accumulatedItemsCount_;
    int sidesRemaining_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update button states and text.
    void updateButtons();

    /// Update mode.
    void updateMode();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart namespace

#endif // MEATSPECSHEETWIDGET_H
