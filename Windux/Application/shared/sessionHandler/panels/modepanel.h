/**
 * @file modepanel.h
 * Base class for mode panels (i.e. operate, service, supervisor).
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MODEPANEL_H
#define MODEPANEL_H

#include "panelwidget.h"

#include <QList>

class QPushButton;
class QVBoxLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for mode panels (i.e. operate, service, supervisor).
class ModePanel : public PanelWidget
{
    Q_OBJECT

    typedef ModePanel _Myt;
    typedef PanelWidget _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~ModePanel();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create an extended menu panel object.
    /**
     * @return  pointer to panel
     */
    virtual PanelWidget *createExtendedPanel();

protected:

    /// Which panel button should be selected.
    enum SelectedButton
    {
        NoButtonSelected,                           ///< Open panel with no button selected
        CurrentModeButtonSelected,                  ///< Open panel and selected button for current mode
        OperateButtonSelected,                      ///< Open panel with the operate button selected
        SupervisorButtonSelected,                   ///< Open panel with the supervisor button selected
        ServiceButtonSelected                       ///< Open panel with the service button selected
    };

    /// List of push buttons widgets.
    typedef QList<QPushButton*> pushButtonList;

    /// Access level of currently logged in operator.
    int accessLevel_;

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit ModePanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse release event.
    /**
     * @param[in,out] e  event
     */
    virtual void mouseReleaseEvent( QMouseEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create button.
    /**
     * Creates a push button with button text @p buttonText. The button is added to the back of
     * list @p buttons.
     *
     * @param[in] buttons  list of buttons to add new button
     * @param[in] buttonText  text of button
     * @param[in] requiredAccessLevel  access level required to use function
     * @param[in] selected  @c true for a selected button, @c false otherwise
     * @return  pointer to QPushButton object
     */
    virtual QPushButton *createButton( pushButtonList &buttons, const QString &buttonText, int requiredAccessLevel, bool selected = false );

    /// Create layout for buttons.
    /**
     * Creates a layout of buttons using the list of buttons @p buttons passed in. The layout will
     * be @p buttonRowSize buttons wide and have a title of @p title.
     *
     * @param[in] buttons  list of buttons to create layout from
     * @param[in] buttonRowSize  how many buttons wide the layout should be
     * @param[in] title  the layout title
     * @return  pointer to QVBoxLayout object
     */
    virtual QVBoxLayout *createButtonLayout( const pushButtonList &buttons, int buttonRowSize, const QString &title = QString() );

    /// Create layout for menu buttons.
    /**
     * Creates a layout of menu buttons. These buttons control which panel is shown (i.e. operate,
     * supervisor, service).
     *
     * @param[in] selected  which button is to be in the selected state
     * @returns  pointer to QVBoxLayout layout
     */
    virtual QVBoxLayout *createMenuButtonLayout( SelectedButton selected );

private Q_SLOTS:

    /// Slot for when dismiss button clicked.
    void onDismissClicked();

    /// Slot for when operate pages is clicked.
    void onOperatePagesClicked();

    /// Slot for when supervisor pages is clicked.
    void onSupervisorPagesClicked();

    /// Slot for when service pages is clicked.
    void onServicePagesClicked();

private:

    pushButtonList modeButtons_;

    QSize buttonSize_;
    QSize modeButtonSize_;

    static const int menuButtonFontPointSize_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MODEPANEL_H
