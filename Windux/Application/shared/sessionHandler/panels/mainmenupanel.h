/**
 * @file mainmenupanel.h
 * Main menu panel.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MAINMENUPANEL_H
#define MAINMENUPANEL_H

#include "panelwidget.h"

class ScaleConfiguration;

namespace hobart {
    class toolButton;
}

class QString;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main menu panel.
/**
 * This panel contains the mode buttons, shutdown button, etc. for controlling the session.
 */
class MainMenuPanel : public PanelWidget
{
    Q_OBJECT

    typedef MainMenuPanel _Myt;
    typedef PanelWidget _Mybase;

public:

    /// Which panel button should be selected.
    enum SelectedButton
    {
        NoButtonSelected,                           ///< Open panel with no button selected
        MenuButtonSelected,                         ///< Open panel with the menu button selected
        SearchButtonSelected,                       ///< Open panel with the search button selected
        LanguageButtonSelected,                     ///< Open panel with the language button selected
        HelpButtonSelected,                         ///< Open panel with the help button selected
        ShutdownButtonSelected                      ///< Open panel with the shutdown button selected
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     * @param[in] selected  which button to select/highlight
     */
    explicit MainMenuPanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0, SelectedButton selected = MenuButtonSelected );

    /// Destructor.
    virtual ~MainMenuPanel();

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

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for when menu is clicked.
    void onMenuClicked();

    /// Slot for when search is clicked.
    void onSearchClicked();

    /// Slot for when language is clicked.
    void onLanguageClicked();

    /// Slot for when help is clicked.
    void onHelpClicked();

    /// Slot for when shutdown is clicked.
    void onShutdownClicked();

private:

    ScaleConfiguration *scaleConfig_;

    SelectedButton selected_;

    hobart::toolButton *menuButton_;
    hobart::toolButton *searchButton_;
    hobart::toolButton *languageButton_;
    hobart::toolButton *helpButton_;
    hobart::toolButton *shutdownButton_;

    static const QSize iconSize_;

    bool searchEnabled_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MAINMENUPANEL_H
