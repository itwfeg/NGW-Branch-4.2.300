/**
  * @file superCustomerDisplay.h
  * @brief Allows supervisor to modify settings for Graphical Customer Display.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup superCustomerDisplayCsu
  * @{
  */

#ifndef SUPERCUSTOMERDISPLAY_H
#define SUPERCUSTOMERDISPLAY_H

#include "screenwidget.h"

class python;

class DomGcdSettings;

class QIntValidator;

namespace Ui {
class superCustomerDisplay;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Allows supervisor to modify settings for Customer Display.
/**
 * Settings that can be modified are:
 * - Number of seconds to wait before slideshow or video begins
 * - Language to use for the Customer Display
 * - Volume control
 */
class superCustomerDisplay : public ScreenWidget
{
    Q_OBJECT
    
    typedef superCustomerDisplay _Myt;
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
    explicit superCustomerDisplay( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~superCustomerDisplay();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

public slots:

    /// Slot for when a field on the screen has changed.
    /**
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    void SetupChanged();

    /// Slot for when the Playback Wait Time has changed.
    /**
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    void PlaybackWaitTimeChanged();

    /// Slot for when the Dimmer Wait Time has changed.
    /**
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    void DimmerTimeoutWaitTimeChanged( const QString& );

    /// Slot to update Volume percentage when slider is moved.
    void UpdateVolumeDisplay( int );

    /// Slot to edit playlist file.
    void EditPlaylistFile();

protected:

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:

    Ui::superCustomerDisplay *ui;

    // Save off initial values that are not used but need
    // to be sent when saving the updated settings
    DomGcdSettings *gcdSettings_;

    bool bSetupChanged;
    bool bPlaybackWaitTimeChanged;
    bool bDimmerWaitTimeChanged;

    QIntValidator *playbackWaitTimeValidator_;
    QIntValidator *dimmerWaitTimeValidator_;

    QString databaseFileLocation;

    /// Builds the parameter string and saves the setup.
    void SaveDataOnExit();

    /// Save the Playback Wait Time.
    void SavePlaybackWaitTime();

    /// Save the Dimmer Timeout minutes.
    void SaveDimmerWaitTime();

    /// Initialize dropdown.
    void initialize();

    /// Get the Dimmer Timeout minutes.
    QString getDimmerTimeoutValue();

    /// Update the Dimmer Timeout minutes.
    int getCustomerVolumeValue();

    bool updateDB( const QString& key, const QString& value );

    void setCustomerVolume();

    void UpdateUserInterface();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SUPERCUSTOMERDISPLAY_H
