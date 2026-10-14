/**
 * @file wrapperservicemodediagnosticspanel.h
 * Panel for service mode diagnostics on a prepack system.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERSERVICEMODEDIAGNOSTICSPANEL_H
#define WRAPPERSERVICEMODEDIAGNOSTICSPANEL_H

#include "modepanel.h"

class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for service mode diagnostics on a prepack system.
class WrapperServiceModeDiagnosticsPanel : public ModePanel
{
    Q_OBJECT

    typedef WrapperServiceModeDiagnosticsPanel _Myt;
    typedef ModePanel _Mybase;

public:

    /// Which test suite panel should be shown.
    enum TestSuite
    {
        PANEL_ID = 200,                             // do not modify, add enums after
        WrapperDiagnostics
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
    explicit WrapperServiceModeDiagnosticsPanel( int testSuite, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~WrapperServiceModeDiagnosticsPanel();

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

    pushButtonList diagnostics_;

    static const int buttonsPerRow_;

    /// Initialize.
    void initialize();

    /// Initialize menu buttons.
    void initializeButtons();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WRAPPERSERVICEMODEDIAGNOSTICSPANEL_H
