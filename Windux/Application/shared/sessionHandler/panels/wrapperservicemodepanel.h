/**
 * @file wrapperservicemodepanel.h
 * Panel for service mode on a prepack system.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERSERVICEMODEPANEL_H
#define WRAPPERSERVICEMODEPANEL_H

#include "modepanel.h"

class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for service mode on a prepack system.
class WrapperServiceModePanel : public ModePanel
{
    Q_OBJECT

    typedef WrapperServiceModePanel _Myt;
    typedef ModePanel _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit WrapperServiceModePanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~WrapperServiceModePanel();

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

    /// Slot for push button pressed.
    void onPushButtonPressed( QWidget *widget );

private:

    QSignalMapper *signalMapper_;

    pushButtonList peripherals_;
    pushButtonList miscellaneous_;
    pushButtonList supervisorSettings_;

    pushButtonList printer_;
    pushButtonList weigher_;
    pushButtonList labelApplier_;
    pushButtonList wrapper_;

    static const int buttonsPerRowLeft_;
    static const int buttonsPerRowRight_;

    static const int buttonsPerRowLeft_WrapOnly_;
    static const int buttonsPerRowRight_WrapOnly_;

    /// Initialize.
    void initialize();

    /// Initialize menu buttons.
    void initializeButtons();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WRAPPERSERVICEMODEPANEL_H
