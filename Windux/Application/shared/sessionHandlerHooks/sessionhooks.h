/**
 * @file sessionhooks.h
 * Hooks API for session handler.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SESSIONHOOKS_H
#define SESSIONHOOKS_H

#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>

class sessionHook;

class Session;
class DomLoadSoftkeyGroup;
class MadpNetworkConnectivity;
class MadpNetworkDeviceType;
class DomAuditLogInformation;
class DomLoadSoftkeyGroup;

template <typename T>
class QVector;

typedef QVector<QString> SessionMessage;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class  sessionHooks
 * @brief  Hooks API for session handler.
 */
class sessionHooks : public QObject
{
    Q_OBJECT

    typedef sessionHooks _Myt;
    typedef QObject _Mybase;

public:

    // ************************************************************************
    // DTOR
    // ************************************************************************

    /// DTOR
    ~sessionHooks();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /**
     * @brief  Set the session pointer.
     * @param[in] value  pointer to session
     */
    void setSession( Session *value );

    /**
     * @brief  Retrieve session pointer.
     * @return  pointer to session
     */
    Session *session() const;

    // ************************************************************************
    // Methods
    // ************************************************************************

    /**
     * @brief  Install hook object into system.
     * @param[in] hook  pointer to hook
     * @return  @c true upon success, @c false otherwise
     */
    bool installHook( sessionHook *hook );

    /**
     * @brief  Remove hook object from system.
     * @param[in] name  hook name
     */
    void removeHook( const QString& name );

    /**
     * @brief  Remove hook object from system.
     * @param[in] hook  pointer to hook
     */
    void removeHook( sessionHook *hook );

    /**
     * @brief  Retrieve hook object.
     * @param[in] name  hook name
     * @return  pointer to hook or @c NULL on failure
     */
    sessionHook *findHook( const QString& name ) const;

    // ************************************************************************
    // Static Methods
    // ************************************************************************

    /// Destroy hook layer and all hooks.
    static void destroy();

    /**
     * @brief  Retrieve instance of @c sessionHooks object.
     * @return  pointer to @c sessionHooks
     */
    static sessionHooks *instance();

private Q_SLOTS:

    /// Slot for handling session messages.
    void onSessionReceive( const SessionMessage& msg );

private:

    mutable QMutex m_;

    Session *session_;
    QList<sessionHook*> hooks_;

    /// CTOR
    sessionHooks( Session *s = NULL );

    // not implemented
    sessionHooks( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    // /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/-
    // ========================================================================
    //
    // A P I   L A Y E R
    //
    // Below is the hook API layer broken down into sections.
    //
    // Fields: This layer processes item and on screen user interface fields.
    // Dialog: This layer handle showing and removing error dialogs.
    // Language: This layer handles language changes.
    // Item and Printing: This layer handles item changes and label printing.
    // Messages: This layer handles specific messages.
    //
    // ========================================================================
    // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

    // ************************************************************************
    // Fields API
    // ************************************************************************

public:

    /// Field types.
    enum fieldType
    {
        autoLookupNumOfDigits,                      ///< Number of digits for auto lookup.
        autoLookupOverrideDigit,                    ///< Special prefix digit for auto lookup.
        autoLookupProduct,                          ///< Auto lookup enabled on/off.
        attractInstructionalScreen,                 ///< Use the instructional attract screen.
        attractMessage,                             ///< Welcome message on the attract screen.
        attractRequiresStableWeight,                ///< Attract screen requires stable weight to exit.
        attractSlideshowSpeed,                      ///< Speed of attract slideshow in seconds.
        attractWhenZeroWeight,                      ///< Show attract screen when zero weight.
        capacityIndicator,                          ///< Capacity indicator (dual high range) on/off.
        capacityIndicatorString,                    ///< Capacity indicator (dual high range) string.
        capacityString,                             ///< 10 Capacity string.
        dualCapacityIndicator,                      ///< Capacity indicator (dual low range) on/off.
        dualCapacityIndicatorString,                ///< Capacity indicator (dual low range) string.
        itemByCount,                                ///< Item by count value.
        itemCode,                                   ///< Item code.
        itemDescription,                            ///< Item description.
        itemDiscountedSavings,                      ///< Item total savings for frequent shopper.
        itemDiscountedTotalPrice,                   ///< Item total price for frequent shopper.
        itemDiscountedUnitPrice,                    ///< Item unit price for frequent shopper.
        itemExceptionPrice,                         ///< Item exception price.
        itemExpandedText,                           ///< 20 Item expanded text.
        itemHasExceptionPricing,                    ///< Flag for when item has exception pricing.
        itemHasFreqShopperPricing,                  ///< Flag for item frequent shopper pricing.
        itemImage,                                  ///< Item image.
        itemNutrifactsPanel,                        ///< Item nutrifacts panel.
        itemTare,                                   ///< Item tare.
        itemTotalPrice,                             ///< Item total price.
        itemUnitPrice,                              ///< Item unit price.
        itemUnitPriceCountFor,                      ///< Item unit price count for amount.
        itemUnitPriceFractional,                    ///< Item unit price fractional amount.
        itemUnitPricePoundsFor,                     ///< 30 Item unit price pounds for amount.
        minWeightToPrint,                           ///< Minimum weight to print.
        primaryLanguage,                            ///< Primary language.
        metricScale,                                ///< Metric scale yes/no.
        netWeight,                                  ///< Net weight.
        netWeightString,                            ///< Net weight string.
        numberPadStyle,                             ///< Number pad style ("123" telephone style -or "789" keyboard style).
        secondaryLanguage,                          ///< Secondary language.
        selfServiceBarcodeFile,                     ///< Barcode to display on screen (PJR-82: Amazon pickers).
        selfServiceDisableAttract,                  ///< Disable attract screen in self service mode.
        selfServiceKey,                             ///< 40 Self service key present.
        selfServicePassword,                        ///< Self service mode password.
        selfServiceShowBestSellers,                 ///< Show best sellers instead of apple on the "keyed" entry screen.
        selfServiceStartScreen,                     ///< Start Screen in self service mode ("keyed" entry -or- "softkey" entry).
        selfServicePrintBarcodeToScreen,            ///< Print barcode to screen instead of printer (PJR-82: Amazon pickers).
        serviceKey,                                 ///< Service key present.
        supervisorKey,                              ///< Supervisor key present.
        weightsAndMeasuresInfo,                     ///< Weights and measures information.
        zeroIndicator,                              ///< Zero indicator on/off.
        zeroIndicatorString,                        ///< Zero indicator string.
        itemPropTare,                               ///< 50 Item proportianl tare on self serv random weight screen
        selfServiceTareFlashkeys,                   ///< Tare flashkeys on the self serv random weight screen
        attractPlaylistScreen,                      ///< Play a playlist in self service mode
        selfServiceKeyEntryStyle,                   ///< For BigY flashkeys(0) or Numpad(1) on itemLookup Screen
        selfServicePrintDisplayingLabel,            ///< For BigY Option3 Print while displaying the on screen label
        selfServiceBarcodeOnlyFile,                 ///< Display this barcode file
        selfServiceDisplayBarcodeLabel,             ///< Enable the print barcode only to screen feature
        selfServiceOperatorLogin,                   ///< Enable an Operator (Picker in this case) to log into self service mode
        selfServiceModeActive,                       ///< Indicates we are actively in self service mode, not Super, Service, or Operate
        selfServiceByCountPrompt,                    ///< 'Enter' button text frequently changes; this value keeps track of that - used by session error dialog
        selfServiceRestrictedDialog                  ///< When true, this is the kind of dialog that HAS to be resolved before continuing. PREVENTS SCREEN ACTIVITY
    };

    /// Mapping of field types to values.
    typedef QMap<fieldType, QString> fieldTypeMap;

    /// List of field types.
    typedef QList<fieldType> fieldTypeList;

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void updateCurrentState();

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void updateFieldValue( fieldType type, bool value ) {updateFieldValue( type, QString( value ? "1" : "0" ) );}

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void updateFieldValue( fieldType type, int value ) {updateFieldValue( type, QString::number( value ) );}

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void updateFieldValue( fieldType type, const char *value ) {updateFieldValue( type, QString( value ) );}

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void updateFieldValue( fieldType type, const QString& value );

    /**
     * @brief  Update field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void updateFieldValue( fieldType type, const QStringList& value );

    /**
     * @brief  Update field values.
     * @param[in] values  field values
     */
    void updateFieldValue( const fieldTypeMap& values );

    /**
     * @brief  Retrieve field value.
     * @param[in] type  field type
     * @return  field value
     */
    QString getFieldValue( fieldType type ) const;

Q_SIGNALS:

    /**
     * @brief  Emitted to change a field value.
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void changeFieldValue( int type, const QString& value );

private:

    fieldTypeMap fields_;

    // ************************************************************************
    // Dialog API
    // ************************************************************************

public:

    /// Mapping of button role to button text.
    typedef QMap<int, QString> buttonMap;

    /// Remove camera predictions dialog.
    void removeCameraPredictionsDialog();

    /// Show dialog on screen.
    /**
     * @param[in] title  dialog title
     * @param[in] message  dialog message
     * @param[in] buttons  dialog buttons
     */
    void showDialog( const QString& title, const QString& message, const buttonMap& buttons );

    /// Show camera predictions dialog on screen.
    /**
     * @param[in] m  predictions
     */
    void showCameraPredictionsDialog( const DomLoadSoftkeyGroup *m );

    /// Dismiss active dialog.
    void dismissDialog();

Q_SIGNALS:

    /**
     * @brief  Emitted when dialog button pressed.
     * @param[in] role  button role
     */
    void dialogButtonPress( int role );

    // ************************************************************************
    // Language API
    // ************************************************************************

public:

    /**
     * @brief Update current language.
     * @param[in] language  new language
     */
    void updateCurrentLanguage( const QString& language );

    /**
     * @brief Update current playlist.
     * @param[in] playlist  new playlist
     */
    void updateCurrentPlaylist( const QString& playlist );

Q_SIGNALS:

    /// Emitted when language changed.
    void changeLanguage();

    /// Emitted when the language is reset to the primary language
    void resetPrimaryLanguage();

    // ************************************************************************
    // Item and Printing API
    // ************************************************************************

public:

    /// Item types.
    enum itemType
    {
        none,                                       ///< No item.
        randomWeightItem,                           ///< Item that is random weight.
        forcedByCount                               ///< Item with forced by count.
    };

    /// Update current item.
    /**
     * @param[in] type  item type
     */
    void updateCurrentItem( itemType type );

    /// Update current error dialog.
    /**
     * @param[in] code  error code
     */
    void updateCurrentErrDialog( const QString& code, int buttonstyle );

    /// Update current error dialog.
    /**
     * @param[in] code  error code
     */
    void dismissCurrentErrDialog(const QString& code , int style);

    /// Update current start state after we log in an operator.
    /**
     * @param[in] none
     */
    void updateStartState();

    /// Printing of current item has begun.
    /**
     * @param[in] filename  image filename
     */
    void beginPrinting( const QString &filename );

    /// Printing of current item has begun.
    /**
     * @param[in] filename  image filename
     */
    void beginBarcodeOnlyPrinting( const QString &filename );

    /// Printing of current item has ended.
    void endPrinting();

Q_SIGNALS:

    /// Emitted when print key pressed.
    void print();

    /// Emitted when we want to dismiss numpads and keypads.
    void shutdownKeyboards();

    /// Emitted when zero scale key pressed.
    void zeroScale();

    /// Emitted when Audit Log key pressed.
    void auditLog();

    // ************************************************************************
    // Messages API
    // ************************************************************************

public:

    /**
     * @brief  Load softkey group.
     * @param[in] m  pointer to message
     */
    void loadSoftkeyGroup( const DomLoadSoftkeyGroup *m );

    /**
     * @brief Load the Current State on key inserted message
     * @param[in] none
     */
    void loadCurrentState();

    /**
     * @brief  Update the audit log screen with records.
     * @param[in] m  pointer to message
     */
    void callUpdateAuditLogList( const DomAuditLogInformation *m );

    /**
     * @brief Update the screen message when the audit log is saved
     * @param[in] none
     */
    void callUpdateSaveMessage();


    /**
     * @brief  Network connectivity message.
     * @param[in] m  pointer to message
     */
    void networkConnectivity( const MadpNetworkConnectivity *m );

    /**
     * @brief  Network device type message.
     * @param[in] m  pointer to message
     */
    void networkDeviceType( const MadpNetworkDeviceType *m );

Q_SIGNALS:

    /**
     * @brief  Emitted when softkey clicked.
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    void softkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONHOOKS_H
