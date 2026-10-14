/**
 * @file cooltextpanel.h
 * Panel for COOL text in operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef COOLTEXTPANEL_H
#define COOLTEXTPANEL_H

#include "operatemodepanelex.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for COOL text in operate mode.
class CoolTextPanel : public OperateModePanelEx
{
    Q_OBJECT

    typedef CoolTextPanel _Myt;
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
    explicit CoolTextPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~CoolTextPanel();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for clear button.
    void onClearButtonClicked();

    /// Slot for create button.
    void onCreateButtonClicked();

    /// Slot for list item clicked.
    void onListItemClicked( QListWidgetItem *item );

private:

    int coolTextNum_;

    QPushButton *create_;
    QPushButton *clear_;

    QLabel *textLabel_;
    QLineEdit *text_;

    QLabel *productionStepsLabel_;
    QListWidget *productionSteps_;

    QLabel *countriesLabel_;
    QListWidget *countries_;

    /// Initialize.
    void initialize();

    /// Update button properties.
    void updateButtons();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // COOLTEXTPANEL_H
