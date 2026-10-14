/**
 * @file selfservicedialogG2.h
 * Self service UI framework.
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Joe Crew
 * @inspired by Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEDIALOGG2_H
#define SELFSERVICEDIALOGG2_H

#include "selfservicehook.h"

#include <QDialog>

namespace hobart {
    class selfserveStartG2;
    class selfserveInstructG2;
    class selfserveItemLookupG2;
    class selfserveImageLookupG2;
    class selfserveImageLookupViewAllG2;
    class selfserveWeightsMeasuresG2;
    class selfserveAuditLogG2;
    class selfserveRandomWeightG2;
    class selfserveByCountG2;
    class selfserveDialogG2;

    //class selfserveImageLookupG2;
    //class selfserveFinishG2;
    //class selfserveAuditLogG2;
}

class Session;

class DomLoadSoftkeyGroup;
class MadpNetworkConnectivity;
class MadpNetworkDeviceType;
class MadpScaleConfig;
class MadpInternalConfig;
class ScaleConfiguration;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service UI framework.
class selfServiceDialogG2 : public QDialog
{
    Q_OBJECT

    typedef selfServiceDialogG2 _Myt;
    typedef QDialog _Mybase;

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] h  pointer to hook
     * @param[in] parent  parent widget
     */
    selfServiceDialogG2( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceDialogG2();

    // ************************************************************************
    // Methods
    // ************************************************************************

    enum mode
    {
        weightOnly,                                 ///< Show only weight.
        weightAndPrice,                             ///< Show weight and pricing.
        byCountAndPrice,                            ///< Show by count and pricing.
        capacityOnly,                               ///< Show weigher capacity only.
        loginWeightOnly
    };

    /// Possible buttons types.
    enum showButtonTypes
    {
        none                    = 0,                ///< No buttons.
        showItemEntryKeyed      = 0x0001,           ///< Item lookup button.
        showItemEntryImages     = 0x0002,           ///< Item search button.
        showToggleLanguage      = 0x0004,           ///< Toggle language button.
        showRezero              = 0x0008,           ///< Rezero weigher button.
        showStartOver           = 0x0010,           ///< Start over button.
        showEnter               = 0x0020,           ///< Enter button. - NOT USED
        showPrintLabel          = 0x0040,           ///< Print label button.
        showCancel              = 0x0080,           ///< Cancel button.
        showOkay                = 0x0100,           ///< Okay button.
        showKeypad              = 0x0200,           ///< keypad
        showBack                = 0x0400,           ///< Back Button

        // max types
        MAX_TYPES               = 0x0800            ///< Not a type! Maximum number of types.
    };

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    virtual void loadSoftkeyGroup( const DomLoadSoftkeyGroup *m );

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    virtual void renderBarcodeOnlyLabel();

    /**
     * @brief  Network connectivity message.
     * @param[in] m  pointer to message
     */
    virtual void networkConnectivity( const MadpNetworkConnectivity *m );

    /**
     * @brief  Network device type message.
     * @param[in] m  pointer to message
     */
    virtual void networkDeviceType( const MadpNetworkDeviceType *m );

    /// Refresh item information.
    virtual void refreshItem();

    /// Reset the number pad to start state
    virtual void resetByCountNumpad();

    /// Emitted when barcodes are present and we entered a bycount.
    void onShowBackButton();

    /// Reset the number pad to start state
    virtual void resetByCountNumpadDisplay();

    /// what is the stack index of the keypad
    virtual int getByCountStackState();

    /// Refresh settings.
    virtual void refreshSettings();

    /// Refresh weight information.
    virtual void refreshWeight();

    /// Translate to current language.
    virtual void translate();

    /// Refresh playlist
    virtual void refreshPlaylist( const QString playlist );

	// resets number pad entry
    virtual void resetNumPadEntry();

    /// Pop an error dialog
    virtual void refreshDialog(const QString code, int buttonstyle);

    /// Hide an existing error dialog
    virtual void showErrorDialog();

    /// Pop an existing error dialog
    virtual void hideErrorDialog();

    /// Dismiss an error dialog
    virtual void dismissErrorDialog(const QString code, int buttonstyle);

    /// Dismiss an error dialog with no code
    virtual void dismissErrorDialogNoCode();

    ///  dims the screen
    virtual void dimBackground();

    ///  resets the screen
    virtual void resetBackground();

    /// returns the error code
    virtual QString getDialogBoxCode();

    /// returns the error code
    virtual int getDialogBoxStyle();

protected:

    // ************************************************************************
    // Events
    // ************************************************************************

    /**
     * @brief  Resize event.
     * @param[in] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /**
     * @brief  Show event.
     * @param[in] e  event
     */
    virtual void showEvent( QShowEvent *e );

private Q_SLOTS:

    /// Slot to handle button bar cancel button.
    void onButtonBarCancel();

    /// Slot to handle button bar enter button.
    void onButtonBarEnter();

    /// Slot to handle button bar okay button.
    void onButtonBarOkay();

    /// Slot to handle a request to print from the keypad (on ByCount Screen)
    void onPrintPressed();

    /// Slot to trigger an error when trying to print (on ByCount Screen)
    void onPrintError(int err);

    /// Need to set a config value from the keypad
    void onSendConfigPressed();

    /// Slot to handle hook state change.
    void onHookCurrentLookOfStateChange();

    /// Slot to handle hook state change.
    void onHookCurrentStateChanged();

    /// Slot to handle hook images changing.
    void onHookReloadImages();

    /// Slot to handle weight bar pressed.
    void onWeightBarPressed();

    /// Slot to handle when the OK/Rezero button is clicked on a dialog
    void onDialogDone();

    /// Pop the Quantity keypad
    void onPopQuantity();

private:

    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;
    
    //MadpScaleConfig *scaleConfigData;
    //MadpInternalConfig *internalConfigData;

    hobart::selfserveStartG2 *attract_;                     // green go button
    hobart::selfserveInstructG2 *instruct_;                 // weight required
    hobart::selfserveItemLookupG2 *itemEntryKeyed_;         // Main Flashkey and Numpad Lookup
    hobart::selfserveImageLookupG2 *itemEntryImages_;       // search char by char flashkey
    hobart::selfserveImageLookupViewAllG2 *itemEntryImagesViewAll_;
    hobart::selfserveWeightsMeasuresG2 *weightsMeasures_;
    hobart::selfserveAuditLogG2 *auditLog_;
    hobart::selfserveByCountG2 *byCountItem_;
    hobart::selfserveRandomWeightG2 *randomWeightItem_;

    hobart::selfserveDialogG2 *errorDialog_;

    QPixmap background_;

    bool online_;

    int code_;
    bool bgflag_;
    bool networkFlipped_;

    static const QColor offlineBackgroundColor_;
    static const QColor onlineBackgroundColor_;

    // not implemented
    selfServiceDialogG2( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// Create layout.
    void createLayout( QWidget *w, bool showBar = false, int mode = 0, int buttons = 0 );

    /// Hide all child widgets.
    void hideAllChildren();

    /// Update background color.
    void updateBackground( bool online );

    /// Turn off the ability to display a label to the screen and scan.
    bool turnOffPrintToScreen();

    /// Turn on the ability to display a label to the screen and scan.
    bool turnOnPrintToScreen();

    /// decide if we automatically close a dialog that deals with negative weight
    bool autoCloseDialogOnWeight(int code);

    /// Find out what the diminestions of the screen are
    int checkScreenSize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEDIALOGG2_H
