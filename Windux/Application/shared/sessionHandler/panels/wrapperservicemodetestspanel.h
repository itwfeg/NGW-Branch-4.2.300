/**
 * @file wrapperservicemodetestspanel.h
 * Panel for service mode tests on a prepack system.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERSERVICEMODETESTSPANEL_H
#define WRAPPERSERVICEMODETESTSPANEL_H

#include "modepanel.h"

class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for service mode tests on a prepack system.
class WrapperServiceModeTestsPanel : public ModePanel
{
    Q_OBJECT

    typedef WrapperServiceModeTestsPanel _Myt;
    typedef ModePanel _Mybase;

public:

    /// Which test suite panel should be shown.
    enum TestSuite
    {
        PANEL_ID = 100,                             // do not modify, add enums after
        WrapperTests
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] testSuite  test to display
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit WrapperServiceModeTestsPanel( int testSuite, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~WrapperServiceModeTestsPanel();

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
    // Properties
    // ========================================================================

    /// Retrieve renderer traits.
    /**
     * @return  traits object
     */
    virtual hobart::backgroundWidgetBase *rendererTraits() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create named button.
    /**
     * Creates a push button with button text @p buttonText. The button is added to the back of
     * list @p buttons.
     *
     * @param[in] buttons  list of buttons to add new button
     * @param[in] buttonText  text of button
     * @param[in] buttonObjectName  button object name
     * @param[in] requiredAccessLevel  access level required to use function
     * @param[in] selected  @c true for a selected button, @c false otherwise
     * @returns  pointer to QPushButton object
     */
    virtual QPushButton *createNamedButton( pushButtonList& buttons, const QString& buttonText, const QString& buttonObjectName, int requiredAccessLevel = 0, bool selected = false );

private Q_SLOTS:

    /// Slot for back button button pressed.
    void onBackClicked();

    /// Slot for push button pressed.
    void onPushButtonPressed( QWidget *widget );

private:

    QSignalMapper *signalMapper_;

    TestSuite mode_;

    pushButtonList tests_;

    static const int buttonsPerRow_;

    /// Initialize.
    void initialize();

    /// Initialize menu buttons.
    void initializeButtons();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WRAPPERSERVICEMODETESTSPANEL_H
