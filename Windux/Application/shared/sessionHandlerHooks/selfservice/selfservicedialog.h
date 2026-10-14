/**
 * @file selfservicedialog.h
 * Self service UI framework.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEDIALOG_H
#define SELFSERVICEDIALOG_H

#include "selfservicehook.h"

#include <QDialog>

namespace hobart {
    class selfserveStart;
    class selfserveInstruct;
	class selfservePlaylist;
    class selfserveLogin;
    class selfserveItemLookup;
    class selfserveImageLookup;
    class selfserveByCount;
    class selfserveRandomWeight;
    class selfserveFinish;
    class selfserveWeightsMeasures;
    class selfserveAuditLog;
}

class Session;

class selfServiceButtonBar;
class selfServiceWeightBar;

class DomLoadSoftkeyGroup;
class MadpNetworkConnectivity;
class MadpNetworkDeviceType;
class MadpScaleConfig;
class MadpInternalConfig;
class ScaleConfiguration;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service UI framework.
class selfServiceDialog : public QDialog
{
    Q_OBJECT

    typedef selfServiceDialog _Myt;
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
    selfServiceDialog( selfServiceHook *h, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceDialog();

    // ************************************************************************
    // Methods
    // ************************************************************************

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

private:

    selfServiceHook *hook_;
    ScaleConfiguration *scaleConfig_;
    
    //MadpScaleConfig *scaleConfigData;
    //MadpInternalConfig *internalConfigData;

    hobart::selfserveStart *attract_;                   // green go button
    hobart::selfserveInstruct *instruct_;               // weight required
    hobart::selfservePlaylist *playlist_;               // playlists
    hobart::selfserveLogin *login_;                     // operator any user
    hobart::selfserveItemLookup *itemEntryKeyed_;       // keypad entry with red apple
    hobart::selfserveImageLookup *itemEntryImages_;     // flashkeys
    hobart::selfserveByCount *byCountItem_;
    hobart::selfserveRandomWeight *randomWeightItem_;
    hobart::selfserveFinish *takeLabelAndStartOver_;    // finished printing video/image

    hobart::selfserveWeightsMeasures *weightsMeasures_;
    hobart::selfserveAuditLog *auditLog_;

    selfServiceButtonBar *buttonBar_;
    selfServiceWeightBar *weightBar_;

    QPixmap background_;

    bool online_;

    static const QColor offlineBackgroundColor_;
    static const QColor onlineBackgroundColor_;

    // not implemented
    selfServiceDialog( const _Myt& rhs );

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

    /// Find out what the diminestions of the screen are
    int checkScreenSize();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEDIALOG_H
